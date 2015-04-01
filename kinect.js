var kinect = require('./build/Release/kinect.node');

module.exports = function (options) {
  if (!options) {
    options = {};
  }

  if (!options.device) {
    options.device = 0;
  }

  return new kinect.Context(options.device);
};

