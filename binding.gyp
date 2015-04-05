{
  'targets': [{
    'target_name': 'kinect',
    'sources': [
      'src/async_handle.cc',
      'src/async_handles.cc',
      'src/context.cc',
      'src/util.cc',
      'src/world_frame.cc'
    ],
    'include_dirs': [
      '/usr/include/eigen3',
      '/usr/include/libfreenect',
      '/usr/include/libusb-1.0'
    ],
    'libraries': ['-lfreenect'],
    'cflags_cc': [
      '-O3',
      '-march=native',
      '-fdiagnostics-color=always',
      '-std=c++11'
    ],
    'cflags_cc!': ['-fno-exceptions'],
  }]
}

