#ifndef ASYNC_HANDLE_H
#define ASYNC_HANDLE_H


namespace kinect
{
    class AsyncHandle
    {
        public:
            AsyncHandle(uv_async_cb cb);
            ~AsyncHandle();
            bool enable();
            void set_data(void *data);
            void send();
            void disable();

        private:
            AsyncHandle(AsyncHandle const &that) = delete;
            bool is_enabled_;
            uv_async_t handle_;
            uv_async_cb const cb_;
    };
}


#endif  // ASYNC_HANDLE_H

