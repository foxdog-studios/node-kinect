#ifndef ASYNC_HANDLES_H
#define ASYNC_HANDLES_H

#include <uv.h>

#include "async_handle.h"


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
            AsyncHandle depth_handle_;
            AsyncHandle video_handle_;
    };
}


#endif  // ASYNC_HANDLES_H

