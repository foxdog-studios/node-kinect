var Kinect = require('..');
var assert = require('assert');

describe("Depth", function() {

  var context;

  beforeEach(function() {
    context = Kinect();
  });

  afterEach(function() {
    context.unsetDepthCallback();
    context.close();
  });

  it("should pass depth frames to the callback", function(done) {
    this.timeout(60000);
    context.setDepthCallback(handleDepth);
    context.startDepth();
    context.resume();

    var remaining = 100;

    function handleDepth(buf) {
      remaining--;

      if (! (remaining % 10)) {
        process.stdout.write('.');
      }

      assert(buf instanceof Buffer, 'buf is not an instance of Buffer');
      assert(buf.length > 0, 'Buffer length is zero');
      assert.equal(buf.length, 640 * 480 * 2, 'Buffer length is ' + buf.length);

      if (remaining == 0) {
        done();
      }
    }
  });
});

