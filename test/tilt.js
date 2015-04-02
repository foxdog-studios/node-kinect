var Kinect = require('..');
var assert = require('assert');

describe("Tilt", function () {
  var context;

  beforeEach(function () {
    context = new Kinect.Context;
    context.enable(0);
  });

  afterEach(function () {
    context.disable();
  });

  var angles = [-15, -12, -9, -3, 3, 8, 12, 0];

  angles.forEach(function (angle) {
    it("moves Kinect to " + angle + " degrees", function(done) {
      this.timeout(2500);
      context.setTilt(angle);
      setTimeout(done, 2000);
    });
  });

  it("throws an error when no arguments are passed", function() {
    assert.throws(function() {
      context.setTitle();
    });
  });

  it("throws an error when more than one argument is passed", function () {
    assert.throws(function() {
      context.setTilt(1, 2);
    });
  });

  it("throwa an erorr when the first argument is not a number", function () {
    assert.throws(function() {
      context.setTilt("NoANumber");
    });
  });
});

