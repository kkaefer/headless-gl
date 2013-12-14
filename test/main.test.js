var assert = require('assert');

var GL = require('..');

describe('context properties', function() {
    it('should have a createContext function', function() {
        assert.equal(typeof GL.createContext, 'function');
    });

    it('should be named WebGLRenderingContext', function() {
        var gl = GL.createContext(30, 40);
        assert.equal(gl.constructor.name, 'WebGLRenderingContext');
    });
});
