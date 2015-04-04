#ifndef KINECT_H
#define KINECT_H

#include <string>
#include <node.h>

#include "async_handles.h"


namespace kinect {

  class Context : node::ObjectWrap {
    public:
      static void            Initialize (v8::Handle<v8::Object> target);
      virtual                ~Context   ();
      void                   DepthCallback    ();
      void                   VideoCallback    ();
      bool                   running_;
      freenect_context*      context_;
      AsyncHandles async_handles;

      void process_events_forever();

    private:
      Context();
      void Enable(v8::Arguments const &args);
      void Disable();

      static Context *GetContext(v8::Arguments const &args);
      static v8::Handle<v8::Value> New(v8::Arguments const &args);
      static v8::Handle<v8::Value> CallEnable(v8::Arguments const &args);
      static v8::Handle<v8::Value> CallDisable(v8::Arguments const &args);
      static v8::Handle<v8::Value> Close(v8::Arguments const &args);

      // = Events ==============================================================

      void StartProcessingEvents();
      void StopProcessingEvents();

      static v8::Handle<v8::Value> CallStartProcessingEvents(
              v8::Arguments const &args);

      static v8::Handle<v8::Value> CallStopProcessingEvents(
              v8::Arguments const &args);


      // = Depth ===============================================================

      static v8::Handle<v8::Value> StartDepth(v8::Arguments const &args);

      void StartDepth();

      static v8::Handle<v8::Value> StopDepth(v8::Arguments const &args);

      void StopDepth();

      static v8::Handle<v8::Value> CallSetDepthCallback(
              v8::Arguments const &args);

      void SetDepthCallback(v8::Arguments const&args);

      static v8::Handle<v8::Value> CallUnsetDepthCallback(
              v8::Arguments const &args);

      void UnsetDepthCallback();


      // = Video ===============================================================

      static v8::Handle<v8::Value> StartVideo(v8::Arguments const &args);

      void StartVideo();

      static v8::Handle<v8::Value> StopVideo(v8::Arguments const &args);

      void StopVideo();

      static v8::Handle<v8::Value> CallSetVideoCallback(
              v8::Arguments const &args);

      void SetVideoCallback(v8::Arguments const &args);

      static v8::Handle<v8::Value> CallUnsetVideoCallback(
              v8::Arguments const &args);

      void UnsetVideoCallback();


      // = LED =================================================================

      static v8::Handle<v8::Value> CallSetLEDOption(v8::Arguments const &args);

      void SetLEDOption(v8::Arguments const &args);


      // = Tilt ================================================================

      static v8::Handle<v8::Value> CallTilt(v8::Arguments const &args);
      void Tilt(v8::Arguments const &args);

      v8::Persistent<v8::Function> depth_callback_;
      v8::Persistent<v8::Function> video_callback_;

      node::Buffer *video_buffer_;
      v8::Persistent<v8::Value> video_buffer_handle_;
      node::Buffer *depthBuffer_;
      v8::Persistent<v8::Value> depth_buffer_handle_;

      freenect_device*      device_;
      freenect_frame_mode   video_mode_;
      freenect_frame_mode   depth_mode_;

      uv_thread_t event_thread_;
  };

}

#endif  // KINECT_H

