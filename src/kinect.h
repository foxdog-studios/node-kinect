#ifndef KINECT_H
#define KINECT_H

#include <string>
#include <node.h>

namespace kinect {

  class Context : node::ObjectWrap {
    public:
      static void            Initialize (v8::Handle<v8::Object> target);
      virtual                ~Context   ();
      void                   DepthCallback    ();
      void                   VideoCallback    ();
      bool                   running_;
      bool                   sending_;
      freenect_context*      context_;
      uv_async_t             uv_async_video_callback_;
      uv_async_t             uv_async_depth_callback_;

    private:
      Context(int user_device_number);

      static v8::Handle<v8::Value>  New              (const v8::Arguments& args);
      static Context*               GetContext       (const v8::Arguments &args);

      void                          Close            ();
      static v8::Handle<v8::Value>  Close            (const v8::Arguments &args);

      void                          Led              (const std::string option);
      static v8::Handle<v8::Value>  Led              (const v8::Arguments &args);

      void                          Tilt             (const double angle);
      static v8::Handle<v8::Value>  Tilt             (const v8::Arguments &args);

      // = Depth ===============================================================

      static v8::Handle<v8::Value> StartDepth(v8::Arguments const &args);

      void StartDepth();

      static v8::Handle<v8::Value> StopDepth(v8::Arguments const &args);

      void StopDepth();

      static v8::Handle<v8::Value> CallSetDepthCallback(
              v8::Arguments const&args);

      void SetDepthCallback(v8::Arguments const&args);


      // = Video ===============================================================

      static v8::Handle<v8::Value> StartVideo(v8::Arguments const &args);
      void StartVideo();

      static v8::Handle<v8::Value> StopVideo(v8::Arguments const &args);
      void StopVideo();

      static v8::Handle<v8::Value> SetVideoCallback(v8::Arguments const &args);
      v8::Handle<v8::Value> ConfigureVideo(v8::Arguments const &args);

      void                          Pause            ();
      static v8::Handle<v8::Value>  Pause            (const v8::Arguments &args);

      void                          Resume           ();
      static v8::Handle<v8::Value>  Resume           (const v8::Arguments &args);

      void                          InitProcessEventThread();

      bool                  depthCallback_;
      bool                  videoCallback_;

      v8::Persistent<v8::Function> depth_callback_;
      v8::Persistent<v8::Function> video_callback_;

      node::Buffer*         videoBuffer_;
      v8::Persistent<v8::Value> videoBufferPersistentHandle_;
      node::Buffer*         depthBuffer_;
      v8::Persistent<v8::Value> depthBufferPersistentHandle_;

      freenect_device*      device_;
      freenect_frame_mode   videoMode_;
      freenect_frame_mode   depthMode_;

      uv_thread_t           event_thread_;

  };

}

#endif  // KINECT_H

