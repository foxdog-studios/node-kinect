var Kinect = require('..');
var assert = require('assert');

describe("Video", function() {
  var context;

  beforeEach(function() {
    context = new Kinect.Context;
    context.enable(0);
  });

  afterEach(function() {
    context.stopProcessingEvents();
    context.unsetDepthCallback();
    context.stopVideo();
    context.disable();
  });

  it("should pass video frames to the callback", function(done) {
    this.timeout(60000);
    context.setVideoCallback(handleVideo);
    context.startVideo();
    context.startProcessingEvents();

    var remaining = 100;

    function handleVideo(buf) {
      remaining--;

      if (!(remaining % 10)) {
        process.stdout.write('.');
      }

      assert(buf instanceof Buffer, 'buf is not an instance of Buffer');
      assert(buf.length > 0, 'Buffer length is zero');
      assert.equal(buf.length, 640 * 480 * 3, 'Buffer length is ' + buf.length);

      if (remaining == 0) {
        done();
      }
    }
  });
});
