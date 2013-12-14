var assert = require('assert');

var GL = require('..');

describe('context properties', function() {
    var gl;

    beforeEach(function() {
        gl = GL.createContext(30, 40);
    });

    afterEach(function() {
        gl.destroy();
        gl = null;
    });

    it('should have a createContext function', function() {
        assert.equal(typeof GL.createContext, 'function');
    });

    it('should be named WebGLRenderingContext', function() {
        assert.equal(gl.constructor.name, 'WebGLRenderingContext');
    });

    it('should have the correct drawingBufferWidth and Height', function() {
        assert.equal(gl.drawingBufferWidth, 30);
        assert.equal(gl.drawingBufferHeight, 40);
    });
});
