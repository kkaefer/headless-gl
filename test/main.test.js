'use strict';

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

    it('should not call a function with a wrong context', function() {
        assert.throws(function() { gl.destroy.call({}); }, /Illegal invocation/);
    });
});

describe('create contexts manually', function() {
    it('should throw on missing arguments', function() {
        assert.throws(function() { new GL.WebGLRenderingContext(); }, /Width and height arguments required/);
        assert.throws(function() { new GL.WebGLRenderingContext(32); }, /Width and height arguments required/);
    });

    it('should throw on invalid dimensions', function() {
        assert.throws(function() { new GL.WebGLRenderingContext(NaN, NaN); }, /Width is not a positive integer/);
        assert.throws(function() { new GL.WebGLRenderingContext(32, 0.9); }, /Height is not a positive integer/);
        assert.throws(function() { new GL.WebGLRenderingContext(32, -10); }, /Height is not a positive integer/);
    });
});
