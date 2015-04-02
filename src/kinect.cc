#include <sys/time.h>

#include <node.h>
#include <node_buffer.h>

#include <libfreenect.hpp>

#include "kinect.h"


using namespace node;
using namespace v8;


namespace
{
    v8::Persistent<v8::String> depthCallbackSymbol;
    v8::Persistent<v8::String> videoCallbackSymbol;

    void process_events_forever(void *);

    void video_callback(freenect_device *, void *, uint32_t);
    void async_video_callback(uv_async_t *, int);

    void depth_callback(freenect_device *, void *, uint32_t);
    void async_depth_callback(uv_async_t *, int);

    kinect::Context *get_kinect_context(uv_async_t *);
    kinect::Context *get_kinect_context(freenect_device *);

    void throw_message(char const *);
}


namespace kinect
{
    Handle<Value> Context::New(Arguments const &args)
    {
        assert(args.IsConstructCall());
        HandleScope scope;
        Context *const context = new Context();
        context->Wrap(args.This());
        return scope.Close(args.This());
    }

    Context::Context() : ObjectWrap(),
                         running_(false),
                         context_(nullptr),
                         device_(nullptr)
    {
        // Empty
    }

    Context::~Context()
    {
        // Empty
    }

    Handle<Value> Context::CallEnable(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->Enable(args);
        return scope.Close(Undefined());
    }

    void Context::Enable(Arguments const &args)
    {
        HandleScope scope;

        int user_device_number;
        int const argc = args.Length();

        if (argc == 0)
        {
            user_device_number = 0;
        }
        else if (argc == 1)
        {
            if (!args[0]->IsInt32())
            {
                throw_message("userDeviceNumber must be an integer");
                return;
            }

            user_device_number = args[0]->ToInt32()->Value();
        }
        else
        {
            throw_message("Excepted 1 argument");
            return;
        }

        if (freenect_init(&context_, nullptr /* usb_ctx */) < 0)
        {
            throw_message("Error initializing freenect context");
            return;
        }

        freenect_set_log_level(context_, FREENECT_LOG_DEBUG);

        freenect_select_subdevices(context_,
                (freenect_device_flags)
                (FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

        if (freenect_open_device(context_, &device_, user_device_number) < 0)
        {
            throw_message("Could not open device number");
            return;
        }

        freenect_set_user(device_, this);

        // Initialize video mode
        videoMode_ = freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM,
                FREENECT_VIDEO_RGB);
        assert(videoMode_.is_valid);

        // Initialize depth mode
        depthMode_ = freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM,
                FREENECT_DEPTH_11BIT);
        assert(depthMode_.is_valid);

        // LibUV stuff
        uv_loop_t *const loop = uv_default_loop();
        uv_async_init(loop, &uv_async_video_callback_, async_video_callback);
        uv_async_init(loop, &uv_async_depth_callback_, async_depth_callback);
    }

    Handle<Value> Context::CallDisable(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->Disable();
        return scope.Close(Undefined());
    }

    void Context::Disable()
    {
        std::string error_message;

        if (device_ != nullptr)
        {

            if (freenect_close_device(device_) != 0)
            {
                error_message += "Could not close device";
            }
            device_ = nullptr;
        }

        if (context_ != nullptr)
        {
            if (freenect_shutdown(context_) != 0)
            {
                if (error_message.length() > 0)
                {
                    error_message += ". ";
                }
                error_message += "Could not shutdown context";
            }
            context_ = nullptr;
        }

        if (error_message.length() > 0)
        {
            throw_message(error_message.c_str());
        }
    }


    // =====================================================================
    // = libfreenect event processing                                      =
    // =====================================================================

    Handle<Value> Context::Resume(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->Resume();
        return scope.Close(Undefined());
    }

    void Context::Resume()
    {
        if (!running_)
        {
            running_ = true;
            uv_thread_create(&event_thread_, process_events_forever, this);
        }
    }

    Handle<Value> Context::Pause(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->Pause();
        return scope.Close(Undefined());
    }

    void Context::Pause()
    {
        if (running_)
        {
            running_ = false;
            uv_thread_join(&event_thread_);
        }
    }


    // =====================================================================
    // = Video                                                             =
    // =====================================================================

    // = Start =============================================================

    Handle<Value> Context::StartVideo(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->StartVideo();
        return scope.Close(Undefined());
    }

    void Context::StartVideo()
    {
        freenect_set_video_callback(device_, video_callback);

        if (freenect_set_video_mode(device_, videoMode_) != 0)
        {
            throw_message("Could not set video mode");
            return;
        }

        videoBuffer_ = Buffer::New(videoMode_.bytes);

        videoBufferPersistentHandle_ =
            Persistent<Value>::New(videoBuffer_->handle_);

        if (freenect_set_video_buffer(device_, Buffer::Data(videoBuffer_)) != 0)
        {
            throw_message("Could not set video buffer");
            return;
        }

        if (freenect_start_video(device_) != 0)
        {
            throw_message("Could not start video");
            return;
        }
    }


    // = Stop ==============================================================

    Handle<Value> Context::StopVideo(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->StopVideo();
        return scope.Close(Undefined());
    }

    void Context::StopVideo()
    {
        freenect_stop_video(device_);
        freenect_set_video_buffer(device_, nullptr);
        videoBufferPersistentHandle_.Dispose();
        videoBufferPersistentHandle_.Clear();
        videoBuffer_ = nullptr;
        freenect_set_video_callback(device_, nullptr);
    }


    // = Callback ==========================================================

    Handle<Value> Context::CallSetVideoCallback(Arguments const& args)
    {
        HandleScope scope;
        GetContext(args)->SetVideoCallback(args);
        return scope.Close(Undefined());
    }

    void Context::SetVideoCallback(Arguments const &args)
    {
        if (args.Length() != 1 || !args[0]->IsFunction())
        {
            throw_message("Expected 1 function as arguments");
            return;
        }

        video_callback_ = Persistent<Function>::New(
                Local<Function>::Cast(args[0]));
    }

    Handle<Value> Context::CallUnsetVideoCallback(Arguments const& args)
    {
        HandleScope scope;
        GetContext(args)->UnsetVideoCallback();
        return scope.Close(Undefined());
    }

    void Context::UnsetVideoCallback()
    {
        video_callback_.Dispose();
        video_callback_.Clear();
    }

    void Context::VideoCallback()
    {
        if (videoBuffer_ != nullptr && !video_callback_.IsEmpty())
        {
            unsigned const argc = 1;
            Handle<Value> argv[1] = { videoBuffer_->handle_ };
            video_callback_->Call(handle_, argc, argv);
        }
    }


    // =====================================================================
    // = Depth                                                             =
    // =====================================================================

    // = Start =============================================================

    Handle<Value> Context::StartDepth(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->StartDepth();
        return scope.Close(Undefined());
    }

    void Context::StartDepth()
    {
        freenect_set_depth_callback(device_, depth_callback);

        if (freenect_set_depth_mode(device_, depthMode_) != 0)
        {
            throw_message("Could not set depth mode");
            return;
        }

        depthBuffer_ = Buffer::New(depthMode_.bytes);
        depthBufferPersistentHandle_ =
                Persistent<Value>::New(depthBuffer_->handle_);

        if (freenect_set_depth_buffer(device_, Buffer::Data(depthBuffer_)) != 0)
        {
            throw_message("Could not set depth buffer");
            return;
        }

        if (freenect_start_depth(device_) != 0)
        {
            throw_message("Could not start depth");
            return;
        }
    }


    // = Stop ==============================================================

    Handle<Value> Context::StopDepth(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->StopDepth();
        return scope.Close(Undefined());
    }

    void Context::StopDepth()
    {
        freenect_stop_depth(device_);
        freenect_set_depth_buffer(device_, nullptr);
        depthBufferPersistentHandle_.Dispose();
        depthBufferPersistentHandle_.Clear();
        depthBuffer_ = nullptr;
        freenect_set_depth_callback(device_, nullptr);
    }


    // = Callback ==========================================================

    Handle<Value> Context::CallSetDepthCallback(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->SetDepthCallback(args);
        return Undefined();
    }

    void Context::SetDepthCallback(Arguments const &args)
    {
        if (args.Length() != 1 || !args[0]->IsFunction())
        {
            throw_message("Expected 1 functions as arguments");
            return;
        }

        depth_callback_ = Persistent<Function>::New(
                Local<Function>::Cast(args[0]));
    }

    Handle<Value> Context::CallUnsetDepthCallback(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->UnsetDepthCallback();
        return scope.Close(Undefined());
    }

    void Context::UnsetDepthCallback()
    {
        depth_callback_.Dispose();
        depth_callback_.Clear();
    }

    void Context::DepthCallback()
    {
        if (depthBuffer_ != nullptr && !depth_callback_.IsEmpty())
        {
            unsigned const argc = 1;
            Handle<Value> argv[1] = { depthBuffer_->handle_ };
            depth_callback_->Call(handle_, argc, argv);
        }
    }


    // =====================================================================
    // = LED                                                               =
    // =====================================================================

    Handle<Value> Context::CallSetLEDOption(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->SetLEDOption(args);
        return scope.Close(Undefined());
    }

    void Context::SetLEDOption(Arguments const &args)
    {
        if (args.Length() != 1 || !args[0]->IsInt32())
        {
            throw_message("Expected 1 number");
            return;
        }

        auto const option = static_cast<freenect_led_options>(
                args[0]->ToInt32()->NumberValue());

        if (freenect_set_led(device_, option) != 0)
        {
            throw_message("Could not set LED option");
        }
    }


    // =====================================================================
    // = Tilt                                                              =
    // =====================================================================

    Handle<Value> Context::CallTilt(Arguments const &args)
    {
        HandleScope scope;
        GetContext(args)->Tilt(args);
        return scope.Close(Undefined());

    }

    void Context::Tilt(Arguments const &args)
    {
        if (args.Length() != 1 || !args[0]->IsInt32())
        {
            throw_message("Expected 1 integer");
            return;
        }

        if (freenect_set_tilt_degs(device_, args[0]->ToInt32()->Value()) != 0)
        {
            throw_message("Could not set tilt angle");
        }
    }


    // =====================================================================
    // = Helpers                                                           =
    // =====================================================================

    Context *Context::GetContext(Arguments const &args)
    {
        return ObjectWrap::Unwrap<Context>(args.This());
    }


    // =====================================================================
    // = Node initialization                                               =
    // =====================================================================

    void Context::Initialize(Handle<Object> target)
    {
        HandleScope scope;

        NODE_DEFINE_CONSTANT(target, LED_OFF);
        NODE_DEFINE_CONSTANT(target, LED_GREEN);
        NODE_DEFINE_CONSTANT(target, LED_RED);
        NODE_DEFINE_CONSTANT(target, LED_YELLOW);
        NODE_DEFINE_CONSTANT(target, LED_BLINK_GREEN);
        NODE_DEFINE_CONSTANT(target, LED_BLINK_RED_YELLOW);

        Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "enable", CallEnable);
        NODE_SET_PROTOTYPE_METHOD(tpl, "disable", CallDisable);

        NODE_SET_PROTOTYPE_METHOD(tpl, "pause", Pause);
        NODE_SET_PROTOTYPE_METHOD(tpl, "resume", Resume);

        NODE_SET_PROTOTYPE_METHOD(tpl, "startDepth", StartDepth);
        NODE_SET_PROTOTYPE_METHOD(tpl, "stopDepth", StopDepth);
        NODE_SET_PROTOTYPE_METHOD(tpl, "setDepthCallback",
                CallSetDepthCallback);
        NODE_SET_PROTOTYPE_METHOD(tpl, "unsetDepthCallback",
                CallUnsetDepthCallback);

        NODE_SET_PROTOTYPE_METHOD(tpl, "startVideo", StartVideo);
        NODE_SET_PROTOTYPE_METHOD(tpl, "stopVideo", StopVideo);
        NODE_SET_PROTOTYPE_METHOD(tpl, "setVideoCallback",
                CallSetVideoCallback);
        NODE_SET_PROTOTYPE_METHOD(tpl, "unsetVideoCallback",
                CallUnsetVideoCallback);

        NODE_SET_PROTOTYPE_METHOD(tpl, "setLedOption", CallSetLEDOption);
        NODE_SET_PROTOTYPE_METHOD(tpl, "setTilt", CallTilt);

        target->Set(String::NewSymbol("Context"), tpl->GetFunction());
    }
}


namespace
{
    void process_events_forever(void *const arg)
    {
        auto const context = static_cast<kinect::Context *>(arg);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        while (context->running_)
        {
            freenect_process_events_timeout(context->context_, &timeout);
        }
    }

    // = Depth =============================================================

    void depth_callback(freenect_device *dev, void *depth, uint32_t timestamp)
    {
        auto const context = get_kinect_context(dev);
        context->uv_async_depth_callback_.data = (void *) context;
        uv_async_send(&context->uv_async_depth_callback_);
    }

    void async_depth_callback(uv_async_t *handle, int notUsed)
    {
        auto const context = get_kinect_context(handle);
        context->DepthCallback();
    }


    // = Video =============================================================

    void video_callback(freenect_device *dev, void *video, uint32_t timestamp)
    {
        auto const context = get_kinect_context(dev);
        context->uv_async_video_callback_.data = (void *) context;
        uv_async_send(&context->uv_async_video_callback_);
    }

    void async_video_callback(uv_async_t *handle, int notUsed)
    {
        auto const context = get_kinect_context(handle);
        context->VideoCallback();
    }


    // = Helpers ===========================================================

    kinect::Context *get_kinect_context(uv_async_t *const handle)
    {
        auto const context = static_cast<kinect::Context *>(handle->data);
        assert(context != nullptr);
        return context;
    }

    kinect::Context *get_kinect_context(freenect_device *const device)
    {
        auto const context = static_cast<kinect::Context *>(
                freenect_get_user(device));
        assert(context != nullptr);
        return context;
    }

    void throw_message(char const *const message)
    {
        ThrowException(Exception::Error(String::New(message)));
    }
}


void init(Handle<Object> target)
{
    kinect::Context::Initialize(target);
}


NODE_MODULE(kinect, init)

