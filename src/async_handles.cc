#include <cassert>
#include <uv.h>

#include "async_handles.h"


namespace kinect
{
    AsyncHandles::AsyncHandles(uv_async_cb depth_cb, uv_async_cb video_cb) :
            is_depth_enabled_(false),
            is_video_enabled_(false),
            depth_cb_(depth_cb),
            video_cb_(video_cb)
    {
       // Empty
    }

    bool AsyncHandles::enable()
    {
        if (enable_depth() && enable_video())
        {
            return true;
        }
        disable();
        return false;
    }

    bool AsyncHandles::enable_depth()
    {
        return enable_handle(is_depth_enabled_, depth_handle_, depth_cb_);
    }

    bool AsyncHandles::enable_video()
    {
        return enable_handle(is_video_enabled_, video_handle_, video_cb_);
    }

    bool AsyncHandles::enable_handle(bool &is_enabled, uv_async_t &handle,
            uv_async_cb const &cb)
    {
        if (!is_enabled)
        {
            is_enabled = uv_async_init(uv_default_loop(), &handle, cb) == 0;
        }

        return is_enabled;
    }

    void AsyncHandles::set_depth_data(void *const data)
    {
        set_data(is_depth_enabled_, depth_handle_, data);
    }

    void AsyncHandles::set_video_data(void *const data)
    {
        set_data(is_video_enabled_, video_handle_, data);
    }

    void AsyncHandles::set_data(bool &is_enabled, uv_async_t &handle,
            void *const data)
    {
        assert(is_enabled);
        handle.data = data;
    }

    void AsyncHandles::send_video()
    {
        uv_async_send(&video_handle_);
    }

    void AsyncHandles::send_depth()
    {
        uv_async_send(&depth_handle_);
    }

    void AsyncHandles::disable()
    {
        disable_depth();
        disable_video();
    }

    void AsyncHandles::disable_depth()
    {
        disable_handle(is_depth_enabled_, depth_handle_);
    }

    void AsyncHandles::disable_video()
    {
        disable_handle(is_video_enabled_, video_handle_);
    }

    void AsyncHandles::disable_handle(bool &is_enabled, uv_async_t &handle)
    {
        if (is_enabled)
        {
            uv_close((uv_handle_t *) &handle, nullptr);
            is_enabled = false;
        }
    }
}

