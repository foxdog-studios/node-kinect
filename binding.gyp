{
  'targets': [{
    'target_name': 'kinect',
    'sources': [
      'src/async_handles.cc',
      'src/kinect.cc'
    ],
    'include_dirs': [
      '/usr/include/libfreenect',
      '/usr/include/libusb-1.0'
    ],
    'libraries': ['-lfreenect'],
    'cflags_cc': ['-std=c++11'],
    'cflags_cc!': [ '-fno-exceptions' ],
  }]
}

