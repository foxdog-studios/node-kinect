#include <v8.h>

#include "util.h"


using v8::Exception;
using v8::String;
using v8::ThrowException;


namespace kinect
{
    void throw_error(char const *const message)
    {
        ThrowException(Exception::Error(String::New(message)));
    }
}

