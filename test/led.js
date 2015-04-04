var Kinect = require('..');
var assert = require('assert');

describe("LED", function() {
  var context;

  beforeEach(function() {
    context = new Kinect.Context;
    context.enable(0);
  });

  afterEach(function() {
    context.disable();
  });

  var option_tests = [
    {name: 'green'                , option: Kinect.LED_GREEN           },
    {name: 'red'                  , option: Kinect.LED_RED             },
    {name: 'yellow'               , option: Kinect.LED_YELLOW          },
    {name: 'blink green'          , option: Kinect.LED_BLINK_GREEN     },
    {name: 'blink red then yellow', option: Kinect.LED_BLINK_RED_YELLOW},
    {name: 'off'                  , option: Kinect.LED_OFF             }
  ]

  option_tests.forEach(function (test) {
    it("sets the state of Kinect's LED to " + test.name, function (done) {
      this.timeout(1500);
      context.setLedOption(test.option);;
      setTimeout(done, 1000);
    });
  });

  it("throws an error when no arguments as passed", function () {
    assert.throws(function() {
      context.led();
    });
  });

  it("throws an error when the LED option name is invalid", function () {
    assert.throws(function() {
      context.led('pink');
    });
  });
});

