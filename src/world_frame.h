#ifndef WORLD_FRAME_H
#define WORLD_FRAME_H


#include <cstdint>

#include <Eigen/Dense>

#include <node.h>
#include <node_buffer.h>


namespace kinect
{
    class WorldFrame
    {
        public:
            WorldFrame();
            ~WorldFrame();
            void update(uint8_t *depth, uint8_t *video);
            void set_callback(v8::Arguments const &args);
            void unset_callback();
            void call_callback();

        private:
            node::Buffer *buffer_;
            v8::Persistent<v8::Value> buffer_handle_;
            v8::Persistent<v8::Function> callback_;

            Eigen::Matrix3d R_;  // Rotation
            Eigen::Vector3d t_;  // Translation

            void world_to_video(Eigen::Vector3d const &world,
                    Eigen::Vector2i &video) const;
    };
}


#endif  // WORLD_FRAME_H

