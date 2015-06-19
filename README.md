# DEPRECATED. DO NOT USE.

Use https://github.com/mapbox/headless-gl instead.


headless-gl
===========
This is a gutted version of [node-webgl](https://github.com/mikeseven/node-webgl) with no external dependencies, DOM emulation or window handling.  You can use it to create WebGL contexts for GPGPU programming and server-side rendering in node.  It should also work in browsers that support WebGL.

Dependencies
============

* Opengl
* GLEW (if running on Linux)

Installation
============
Just do:

    npm install gl
    
Currently only works on OS X and Linux. If you have access to working Windows system and want to contribute, please let me know.

Usage
=====
To get a handle on the headless OpenGL context, just do:

    var gl = require("gl");

Then you can use all the ordinary [WebGL methods](https://www.khronos.org/registry/webgl/specs/1.0/).

Example
=======
Here is an example that creates an offscreen framebuffer, clears it to red, reads the result and writes the image to stdout as a [PPM](http://netpbm.sourceforge.net/doc/ppm.html) :

    //Create context
    var width   = 64;
    var height  = 64;
    var gl = require("gl").createContext(width, height);
    
    //Create texture
    var tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex)
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);

    //Create frame buffer
    var fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);

    //Clear screen to red
    gl.clearColor(1.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    //Write output
    var pixels = new Uint8Array(width * height * 3);
    gl.readPixels(0, 0, width, height, gl.RGB, gl.UNSIGNED_BYTE, pixels);
    process.stdout.write(["P3\n# gl.ppm\n", width, " ", height, "\n255\n"].join(""));
    for(var i=0; i<pixels.length; ++i) {
        process.stdout.write(pixels[i] + " ");
    }


Multiple Contexts
=================
You can create multiple WebGL contexts if you like.  To do this, just call:

    var my_context = gl.createContext();
    
Which will work just like any other WebGL context.  To get rid of a context when you are done with it, jst call destroy() on it:

    my_context.destroy();


Why use this thing instead of node-webgl?
-----------------------------------------
It depends on what you are trying to do.  [node-webgl](https://github.com/mikeseven/node-webgl) is good if you are making a graphical application like a game.  On the other hand, because headless-gl does not create any windows, it is suitable for running in a server environment.  This means that you can use it to generate figures using OpenGL or perform GPGPU computations using shaders.

Credits
=======
See LICENSES
