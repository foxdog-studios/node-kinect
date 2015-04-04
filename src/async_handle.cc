#include <cassert>
#include <uv.h>

#include "async_handle.h"


namespace kinect
{
    AsyncHandle::AsyncHandle(uv_async_cb cb) : is_enabled_(false), cb_(cb)
    {
       // Empty
    }

    AsyncHandle::~AsyncHandle()
    {
        disable();
    }

    bool AsyncHandle::enable()
    {
        if (!is_enabled_)
        {
            is_enabled_ = uv_async_init(uv_default_loop(), &handle_, cb_) == 0;
        }

        return is_enabled_;
    }

    void AsyncHandle::set_data(void *const data)
    {
        assert(is_enabled_);
        handle_.data = data;
    }

    void AsyncHandle::send()
    {
        uv_async_send(&handle_);
    }

    void AsyncHandle::disable()
    {
        if (is_enabled_)
        {
            uv_close((uv_handle_t *) &handle_, nullptr);
            is_enabled_ = false;
        }
    }
}

