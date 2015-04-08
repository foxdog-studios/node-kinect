#include <cstdint>
#include <cstring>

#include <Eigen/Dense>

#include "world_frame.h"
#include "util.h"


using Eigen::Matrix3d;
using Eigen::Vector2i;
using Eigen::Vector3d;
using node::Buffer;
using v8::Arguments;
using v8::Context;
using v8::Function;
using v8::Handle;
using v8::Local;
using v8::Persistent;
using v8::Value;


namespace
{
    constexpr size_t WIDTH = 640;
    constexpr size_t HEIGHT = 480;
    constexpr size_t CHANNELS = 4;
    constexpr size_t SIZE = WIDTH * HEIGHT * CHANNELS;

    constexpr double DEPTH_MIN = 0.5;
    constexpr double DEPTH_MAX = 0.8;

    constexpr double FX_DEPTH = 5.9421434211923247e+02;
    constexpr double FY_DEPTH = 5.9104053696870778e+02;
    constexpr double CX_DEPTH = 3.3930780975300314e+02;
    constexpr double CY_DEPTH = 2.4273913761751615e+02;

    constexpr double FX_VIDEO = 5.2921508098293293e+02;
    constexpr double FY_VIDEO = 5.2556393630057437e+02;
    constexpr double CX_VIDEO = 3.2894272028759258e+02;
    constexpr double CY_VIDEO = 2.6748068171871557e+02;

    void depth_to_world(double, double, double, Vector3d &);
    void world_to_video(Vector3d &, Vector2i &);
    int w2v(double, double, double, double, int);
    int bound(int, int, int);
    double raw_depth_to_meters(uint16_t);
}


namespace kinect
{
    WorldFrame::WorldFrame()
    {
        // Rotation Matrix

        // Row 0
        R_(0, 0) =  9.9984628826577793e-01;
        R_(0, 1) =  1.2635359098409581e-03;
        R_(0, 2) = -1.7487233004436643e-02;

        // Row 1
        R_(1, 0) = -1.4779096108364480e-03;
        R_(1, 1) =  9.9992385683542895e-01;
        R_(1, 2) = -1.2251380107679535e-02;

        // Row 2
        R_(2, 0) = 1.7470421412464927e-02;
        R_(2, 1) = 1.2275341476520762e-02;
        R_(2, 2) = 9.9977202419716948e-01;

        // Translation Vector
        t_(0) =  1.9985242312092553e-02;
        t_(1) = -7.4423738761617583e-04;
        t_(2) = -1.0916736334336222e-02;

        buffer_ = Buffer::New(SIZE);
        buffer_handle_ = Persistent<Value>::New(buffer_->handle_);
    }

    WorldFrame::~WorldFrame()
    {
        unset_callback();
        buffer_handle_.Dispose();
        buffer_handle_.Clear();
    }


    // == Frame ============================================================

    void WorldFrame::update(uint8_t *const depth, uint8_t *const video)
    {
        uint8_t *data = (uint8_t *) Buffer::Data(buffer_);

        memset(data, 255, SIZE);

        Vector3d world_point;
        Vector2i video_point;

        for (size_t y = 0; y < HEIGHT; ++y)
        {
            for (size_t x = 0; x < WIDTH; ++x)
            {
                // Pixel index
                size_t const pi = WIDTH * y + x;

                // Depth
                size_t const di = 2 * pi;
                uint16_t const l = static_cast<uint16_t>(depth[di]);
                uint16_t const u = static_cast<uint16_t>(depth[di + 1]) << 8;
                double const d = raw_depth_to_meters(u | l);
                size_t const i = 4 * pi;

                if (d < DEPTH_MIN || d > DEPTH_MAX)
                    data[i + 3] = 0;
                    continue;

                // World
                depth_to_world(x, y, d, world_point);
                world_to_video(world_point, video_point);

                // Video
                size_t const v = 3 * (WIDTH * video_point(1) + video_point(0));
                data[i] = video[v];
                data[i + 1] = video[v + 1];
                data[i + 2] = video[v + 2];
            }
        }

        call_callback();
    }

    void WorldFrame::world_to_video(Vector3d const &world, Vector2i &video)
            const
    {
        auto const tmp = R_ * world + t_;
        video(0) = w2v(tmp(0), tmp(2), FX_VIDEO, CX_VIDEO, WIDTH);
        video(1) = w2v(tmp(1), tmp(2), FY_VIDEO, CY_VIDEO, HEIGHT);
    }


    // == Callback =========================================================

    void WorldFrame::set_callback(Arguments const &args)
    {
        if (args.Length() != 1 || !args[0]->IsFunction())
        {
            throw_error("Expected 1 functions as arguments");
            return;
        }

        callback_ = Persistent<Function>::New(Local<Function>::Cast(args[0]));
    }

    void WorldFrame::unset_callback()
    {
        callback_.Dispose();
        callback_.Clear();
    }

    void WorldFrame::call_callback()
    {
        if (!callback_.IsEmpty())
        {
            unsigned const argc = 1;
            Handle<Value> argv[1] = { buffer_->handle_ };
            callback_->Call(Context::GetCurrent()->Global(), argc, argv);
        }
    }
}


namespace
{
    void depth_to_world(double const x, double const y, double const z,
            Vector3d &world)
    {
        world(0) = (x - CX_DEPTH) * z / FX_DEPTH;
        world(1) = (y - CY_DEPTH) * z / FY_DEPTH;
        world(2) = z;
    }



    int w2v(double const u, double const z, double const f, double const c,
            int const max)
    {
        return bound(round(u * f / z + c), 0, max - 1);
    }

    int bound(int const x, int const min, int const max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    double raw_depth_to_meters(uint16_t const raw_depth)
    {
        if (raw_depth == 2047)
        {
            return 0.0;
        }

        return 1.0 / (raw_depth * -0.0030711016 + 3.3309495161);
    }
}

