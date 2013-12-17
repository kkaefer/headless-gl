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


    it('should require a string when getting extension info', function() {
        assert.throws(function() { gl.getExtension(null); }, /First parameter must be a string/);
        assert.throws(function() { gl.getExtension(); }, /First parameter must be a string/);
        assert.throws(function() { gl.getExtension(32); }, /First parameter must be a string/);
    });


    it('should return an extension description', function() {
        var ext = gl.getExtension('extension that definitely does not exist');
        assert.equal(ext, null);
    });

});
