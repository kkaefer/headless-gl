"use strict"

var GL = module.exports = require("./webgl.js");
var WebGLRenderingContext = GL.WebGLRenderingContext;

//Interoperate with node-canvas when available
(function() {
    var pGetContext;

    function getContextGLShim(contextid) {
        if (contextid === "webgl" ||
            contextid === "experimental-webgl") {
            if (this._gl_context) {
                return this._gl_context;
            }
            this._gl_context = new WebGLRenderingContext();
            return this._gl_context;
        }
        return pgetContext.call(this, contextid);
    }
    try {
        var Canvas = require("canvas");
        if (Canvas) {
            pGetContext = Canvas.prototype.getContext;
            Canvas.prototype.getContext = getContextGLShim;
        }
    } catch (e) {}
})();


module.exports.createContext = function(width, height) {
    return new WebGLRenderingContext(width, height);
};
