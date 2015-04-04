#include <cassert>
#include <uv.h>

#include "async_handles.h"


namespace kinect
{
    AsyncHandles::AsyncHandles(uv_async_cb depth_cb, uv_async_cb video_cb) :
            depth_handle_(depth_cb),
            video_handle_(video_cb)
    {
       // Empty
    }

    bool AsyncHandles::enable()
    {
        return depth_handle_.enable() && video_handle_.enable();
    }

    void AsyncHandles::set_depth_data(void *const data)
    {
        depth_handle_.set_data(data);
    }

    void AsyncHandles::set_video_data(void *const data)
    {
        video_handle_.set_data(data);
    }

    void AsyncHandles::send_depth()
    {
        depth_handle_.send();
    }

    void AsyncHandles::send_video()
    {
        video_handle_.send();
    }

    void AsyncHandles::disable()
    {
        depth_handle_.disable();
        video_handle_.disable();
    }
}

