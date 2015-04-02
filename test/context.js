var Kinect = require('..');
var assert = require('assert');

describe("Context", function () {
  var context;

  afterEach(function() {
    if (context) {
      context.disable();
      context = null;
    }
  });

  it('initializes and shuts down', function () {
    context = new Kinect.Context;
    context.enable(0);
    context.disable();
    context = null;
  });

  it('throws an error if the passed device index does not exists', function() {
    assert.throws(function() {
      context = new Kinect.Context;
      context.enable(100);;
    });
  });
});

