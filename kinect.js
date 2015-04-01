var kinect       = require('./build/Release/kinect.node');
var EventEmitter = require('events').EventEmitter;
var inherits     = require('util').inherits;
var assert       = require('assert');

function Context (context) {
  this._kContext = context;
  EventEmitter.apply(this, arguments);
}

inherits(Context, EventEmitter);

kinect.Context.prototype.depthCallback = function depthCallback(depthBuffer) {
  this._context.emit('depth', depthBuffer);
};

kinect.Context.prototype.videoCallback = function videoCallback(videoBuffer) {
  this._context.emit('video', videoBuffer);
};

Context.prototype.startDepth = function startDepth () {
  this._kContext.startDepth();
};

Context.prototype.stopDepth = function stopDepth () {
  this._kContext.stopDepth();
};

Context.prototype.setDepthCallback = function setDepthCallback (callback) {
  this._kContext.setDepthCallback(callback);
};

Context.prototype.startVideo = function startVideo () {
  this._kContext.startVideo();
};

Context.prototype.stopVideo = function stopVideo () {
  this._kContext.stopVideo();
};

Context.prototype.setVideoCallback = function setVideoCallback (callback) {
  this._kContext.setVideoCallback(callback);
}

Context.prototype.led = function lef(color) {
  this._kContext.led(color);
};

Context.prototype.tilt = function tilt(angle) {
  this._kContext.tilt(angle);
};

Context.prototype.close = function close() {
  this._kContext.close();
};

Context.prototype.resume = function() {
  this._kContext.resume();
};

Context.prototype.pause = function() {
  this._kContext.pause();
};

module.exports = function(options) {
  if (! options) options = {};
  if (! options.device) options.device = 0;
  var kContext = new kinect.Context(options.device);
  var context = new Context(kContext);
  kContext._context = context;

  return context;
};
