var Kinect = require('..');
var assert = require('assert');

describe("LED", function() {
  var context;

  beforeEach(function() {
    context = new Kinect.Context(0);
  });

  afterEach(function() {
    context.close();
  });

  var names = [
    'green',
    'red',
    'yellow',
    'blink green',
    'blink red yellow',
    'off'
  ]

  names.forEach(function (name) {
    it("sets the state of Kinect's LED to " + name, function(done) {
      this.timeout(2500);
      context.setLedOption(name);
      setTimeout(done, 2000);
    });
  });

  it("throws an error when no arguments as passed", function() {
    assert.throws(function() {
      context.led();
    });
  });

  it("throws an error when the LED option name is invalid", function() {
    assert.throws(function() {
      context.led('pink');
    });
  });
});

