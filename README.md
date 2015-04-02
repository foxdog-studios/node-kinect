# node-kinect

Kinect in Node.js.

# Install

* Install libusb from http://www.libusb.org/
* Install libfreenect from https://github.com/OpenKinect/libfreenect

Then:

```sh
$ git clone git://github.com/pgte/node-kinect.git
```

# Test

Plug your kinect and do:

```sh
$ cd node-kinect
$ npm test
```

# Use

## Create a context

```js
var Kinect = require('kinect');
var context = new Kinect.Context;
```

Accepts options like this:

```js
context.enable(0 /* device number */);
```


Options:

* device: integer for device number. Default is 0

## Start/Stop Processing Events

To start processing events;

```js
var context = new Kinect.Context;
context.enable();
context.startProcessingEvents();
```

To stop processing events;

```js
context.stopProcessingEvents();
```

## Video

Enable video:

```js
var Kinect = require('kinect');
var context = new Kinect.Context
var context.enable(0);
context.setVideoCallback(function (buffer) {
  // buffer is a 1D-array encoding a 640 x 480 RGB image
  console.log(buffer.length);
});
context.startVideo();
context.startProcessingEvents();
```

## Depth

Enable depth:

```js
var Kinect = require('kinect');
var context = new Kinect.Context();
context.enable(0);
context.setDepthCallback(function (buffer) {
  // each depth pixel in buf has 2 bytes, 640 x 480, 11 bit resolution
  console.log(buffer.length);
});
context.startDepth();
context.startProcessingEvents();

```


## LED

```js
var Kinect = require('kinect');
var context = new Kinect.Context;
context.enable();
context.setLedOption(Kinect.LED_RED);
```

`setLedOption()` accepts the constants;

    * `Kinect.LED_OFF`
    * `Kinect.LED_GREEN`
    * `Kinect.LED_RED`
    * `Kinect.LED_YELLOW`
    * `Kinect.LED_BLINK_GREEN`
    * `Kinect.LED_BLINK_RED_YELLOW`


## Tilt

Set tilt angle:

```js
var Kinect = require('kinect');
var context = new Kinect.Context();
context.enable();
context.setTitle(8 /* angle */);
```

`angle` can be any number from -15 to 15. Number out of the range will be set to min/max.

# FAQ


# License

(The MIT License)

Copyright (c) 2011 Pedro Teixeira. http://about.me/pedroteixeira
Copyright (c) 2015 Foxdog Studios Ltd. https://foxdogstudios.com/

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the 'Software'), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

