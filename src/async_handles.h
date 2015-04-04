#ifndef ASYNC_HANDLES_H
#define ASYNC_HANDLES_H

#include <uv.h>


namespace kinect
{
    class AsyncHandles
    {
        public:
            AsyncHandles(uv_async_cb depth_cb, uv_async_cb video_cb);
            bool enable();
            void set_depth_data(void *data);
            void set_video_data(void *data);
            void send_depth();
            void send_video();
            void disable();

        private:
            AsyncHandles(AsyncHandles const &that) = delete;

            bool enable_depth();
            bool enable_video();
            bool enable_handle(bool &is_enabled, uv_async_t &handle,
                    uv_async_cb const &cb);

            void set_data(bool &is_enabled, uv_async_t &handle, void *data);

            void disable_depth();
            void disable_video();
            void disable_handle(bool &is_enabled, uv_async_t &handle);

            bool is_depth_enabled_;
            bool is_video_enabled_;
            uv_async_t depth_handle_;
            uv_async_t video_handle_;
            uv_async_cb const depth_cb_;
            uv_async_cb const video_cb_;
    };
}


#endif  // ASYNC_HANDLES_H

