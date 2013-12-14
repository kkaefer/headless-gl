/*
 * bindings.cc
 *
 *  Created on: Dec 13, 2011
 *      Author: ngk437
 *
 * Modified by Mikola Lysenko, Feb. 2013
 */


#include <cstdlib>
#include <v8.h>
#include <node.h>
#include "arch_wrapper.h"
#include "webgl.h"
#include "macros.h"

using namespace std;
using namespace v8;
using namespace node;


v8::Persistent<FunctionTemplate> webgl_template;
#define JS_GL_CONSTANT(name) webgl_template->PrototypeTemplate()->Set(JS_STR( #name ), JS_INT(GL_ ## name))

extern "C" {
static void init(Handle<Object> target)
{
  //When node exits kill any stray gl contexts
  atexit(WebGL::disposeAll);

  //Create the WebGL template
  v8::Local<FunctionTemplate> t = v8::FunctionTemplate::New(WebGL::New);
  webgl_template = v8::Persistent<v8::FunctionTemplate>::New(t);

  webgl_template->InstanceTemplate()->SetInternalFieldCount(1);
  webgl_template->SetClassName(JS_STR("WebGLRenderingContext"));

  //Add methods
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform1f", WebGL::Uniform1f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform2f", WebGL::Uniform2f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform3f", WebGL::Uniform3f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform4f", WebGL::Uniform4f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform1i", WebGL::Uniform1i);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform2i", WebGL::Uniform2i);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform3i", WebGL::Uniform3i);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform4i", WebGL::Uniform4i);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform1fv", WebGL::Uniform1fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform2fv", WebGL::Uniform2fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform3fv", WebGL::Uniform3fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform4fv", WebGL::Uniform4fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform1iv", WebGL::Uniform1iv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform2iv", WebGL::Uniform2iv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform3iv", WebGL::Uniform3iv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniform4iv", WebGL::Uniform4iv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "pixelStorei", WebGL::PixelStorei);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bindAttribLocation", WebGL::BindAttribLocation);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getError", WebGL::GetError);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "drawArrays", WebGL::DrawArrays);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniformMatrix2fv", WebGL::UniformMatrix2fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniformMatrix3fv", WebGL::UniformMatrix3fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "uniformMatrix4fv", WebGL::UniformMatrix4fv);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "generateMipmap", WebGL::GenerateMipmap);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getAttribLocation", WebGL::GetAttribLocation);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "depthFunc", WebGL::DepthFunc);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "viewport", WebGL::Viewport);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createShader", WebGL::CreateShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "shaderSource", WebGL::ShaderSource);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "compileShader", WebGL::CompileShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getShaderParameter", WebGL::GetShaderParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getShaderInfoLog", WebGL::GetShaderInfoLog);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createProgram", WebGL::CreateProgram);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "attachShader", WebGL::AttachShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "linkProgram", WebGL::LinkProgram);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getProgramParameter", WebGL::GetProgramParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getUniformLocation", WebGL::GetUniformLocation);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "clearColor", WebGL::ClearColor);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "clearDepth", WebGL::ClearDepth);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "disable", WebGL::Disable);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createTexture", WebGL::CreateTexture);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bindTexture", WebGL::BindTexture);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "texImage2D", WebGL::TexImage2D);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "texParameteri", WebGL::TexParameteri);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "texParameterf", WebGL::TexParameterf);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "clear", WebGL::Clear);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "useProgram", WebGL::UseProgram);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createFramebuffer", WebGL::CreateFramebuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bindFramebuffer", WebGL::BindFramebuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "framebufferTexture2D", WebGL::FramebufferTexture2D);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createBuffer", WebGL::CreateBuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bindBuffer", WebGL::BindBuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bufferData", WebGL::BufferData);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bufferSubData", WebGL::BufferSubData);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "enable", WebGL::Enable);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "blendEquation", WebGL::BlendEquation);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "blendFunc", WebGL::BlendFunc);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "enableVertexAttribArray", WebGL::EnableVertexAttribArray);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttribPointer", WebGL::VertexAttribPointer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "activeTexture", WebGL::ActiveTexture);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "drawElements", WebGL::DrawElements);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "flush", WebGL::Flush);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "finish", WebGL::Finish);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib1f", WebGL::VertexAttrib1f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib2f", WebGL::VertexAttrib2f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib3f", WebGL::VertexAttrib3f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib4f", WebGL::VertexAttrib4f);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib1fv", WebGL::VertexAttrib1fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib2fv", WebGL::VertexAttrib2fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib3fv", WebGL::VertexAttrib3fv);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "vertexAttrib4fv", WebGL::VertexAttrib4fv);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "blendColor", WebGL::BlendColor);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "blendEquationSeparate", WebGL::BlendEquationSeparate);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "blendFuncSeparate", WebGL::BlendFuncSeparate);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "clearStencil", WebGL::ClearStencil);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "colorMask", WebGL::ColorMask);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "copyTexImage2D", WebGL::CopyTexImage2D);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "copyTexSubImage2D", WebGL::CopyTexSubImage2D);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "cullFace", WebGL::CullFace);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "depthMask", WebGL::DepthMask);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "depthRange", WebGL::DepthRange);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "disableVertexAttribArray", WebGL::DisableVertexAttribArray);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "hint", WebGL::Hint);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isEnabled", WebGL::IsEnabled);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "lineWidth", WebGL::LineWidth);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "polygonOffset", WebGL::PolygonOffset);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "scissor", WebGL::Scissor);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilFunc", WebGL::StencilFunc);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilFuncSeparate", WebGL::StencilFuncSeparate);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilMask", WebGL::StencilMask);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilMaskSeparate", WebGL::StencilMaskSeparate);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilOp", WebGL::StencilOp);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "stencilOpSeparate", WebGL::StencilOpSeparate);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "bindRenderbuffer", WebGL::BindRenderbuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "createRenderbuffer", WebGL::CreateRenderbuffer);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteBuffer", WebGL::DeleteBuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteFramebuffer", WebGL::DeleteFramebuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteProgram", WebGL::DeleteProgram);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteRenderbuffer", WebGL::DeleteRenderbuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteShader", WebGL::DeleteShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "deleteTexture", WebGL::DeleteTexture);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "detachShader", WebGL::DetachShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "framebufferRenderbuffer", WebGL::FramebufferRenderbuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getVertexAttribOffset", WebGL::GetVertexAttribOffset);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isBuffer", WebGL::IsBuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isFramebuffer", WebGL::IsFramebuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isProgram", WebGL::IsProgram);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isRenderbuffer", WebGL::IsRenderbuffer);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isShader", WebGL::IsShader);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "isTexture", WebGL::IsTexture);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "renderbufferStorage", WebGL::RenderbufferStorage);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getShaderSource", WebGL::GetShaderSource);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "validateProgram", WebGL::ValidateProgram);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "texSubImage2D", WebGL::TexSubImage2D);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "readPixels", WebGL::ReadPixels);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getTexParameter", WebGL::GetTexParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getActiveAttrib", WebGL::GetActiveAttrib);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getActiveUniform", WebGL::GetActiveUniform);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getAttachedShaders", WebGL::GetAttachedShaders);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getParameter", WebGL::GetParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getBufferParameter", WebGL::GetBufferParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getFramebufferAttachmentParameter", WebGL::GetFramebufferAttachmentParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getProgramInfoLog", WebGL::GetProgramInfoLog);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getRenderbufferParameter", WebGL::GetRenderbufferParameter);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getVertexAttrib", WebGL::GetVertexAttrib);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getSupportedExtensions", WebGL::GetSupportedExtensions);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "getExtension", WebGL::GetExtension);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "checkFramebufferStatus", WebGL::CheckFramebufferStatus);

  NODE_SET_PROTOTYPE_METHOD(webgl_template, "frontFace", WebGL::FrontFace);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "sampleCoverage", WebGL::SampleCoverage);
  NODE_SET_PROTOTYPE_METHOD(webgl_template, "destroy", WebGL::Destroy);


  // OpenGL ES 2.1 constants

  /* ClearBufferMask */
  JS_GL_CONSTANT(DEPTH_BUFFER_BIT);
  JS_GL_CONSTANT(STENCIL_BUFFER_BIT);
  JS_GL_CONSTANT(COLOR_BUFFER_BIT);

  /* Boolean */
  JS_GL_CONSTANT(FALSE);
  JS_GL_CONSTANT(TRUE);

  /* BeginMode */
  JS_GL_CONSTANT(POINTS);
  JS_GL_CONSTANT(LINES);
  JS_GL_CONSTANT(LINE_LOOP);
  JS_GL_CONSTANT(LINE_STRIP);
  JS_GL_CONSTANT(TRIANGLES);
  JS_GL_CONSTANT(TRIANGLE_STRIP);
  JS_GL_CONSTANT(TRIANGLE_FAN);

  /* AlphaFunction (not supported in ES20) */
  /*      GL_NEVER */
  /*      GL_LESS */
  /*      GL_EQUAL */
  /*      GL_LEQUAL */
  /*      GL_GREATER */
  /*      GL_NOTEQUAL */
  /*      GL_GEQUAL */
  /*      GL_ALWAYS */

  /* BlendingFactorDest */
  JS_GL_CONSTANT(ZERO);
  JS_GL_CONSTANT(ONE);
  JS_GL_CONSTANT(SRC_COLOR);
  JS_GL_CONSTANT(ONE_MINUS_SRC_COLOR);
  JS_GL_CONSTANT(SRC_ALPHA);
  JS_GL_CONSTANT(ONE_MINUS_SRC_ALPHA);
  JS_GL_CONSTANT(DST_ALPHA);
  JS_GL_CONSTANT(ONE_MINUS_DST_ALPHA);

  /* BlendingFactorSrc */
  /*      GL_ZERO */
  /*      GL_ONE */
  JS_GL_CONSTANT(DST_COLOR);
  JS_GL_CONSTANT(ONE_MINUS_DST_COLOR);
  JS_GL_CONSTANT(SRC_ALPHA_SATURATE);
  /*      GL_SRC_ALPHA */
  /*      GL_ONE_MINUS_SRC_ALPHA */
  /*      GL_DST_ALPHA */
  /*      GL_ONE_MINUS_DST_ALPHA */

  /* BlendEquationSeparate */
  JS_GL_CONSTANT(FUNC_ADD);
  JS_GL_CONSTANT(BLEND_EQUATION);
  JS_GL_CONSTANT(BLEND_EQUATION_RGB);    /* same as BLEND_EQUATION */
  JS_GL_CONSTANT(BLEND_EQUATION_ALPHA);

  /* BlendSubtract */
  JS_GL_CONSTANT(FUNC_SUBTRACT);
  JS_GL_CONSTANT(FUNC_REVERSE_SUBTRACT);

  /* Separate Blend Functions */
  JS_GL_CONSTANT(BLEND_DST_RGB);
  JS_GL_CONSTANT(BLEND_SRC_RGB);
  JS_GL_CONSTANT(BLEND_DST_ALPHA);
  JS_GL_CONSTANT(BLEND_SRC_ALPHA);
  JS_GL_CONSTANT(CONSTANT_COLOR);
  JS_GL_CONSTANT(ONE_MINUS_CONSTANT_COLOR);
  JS_GL_CONSTANT(CONSTANT_ALPHA);
  JS_GL_CONSTANT(ONE_MINUS_CONSTANT_ALPHA);
  JS_GL_CONSTANT(BLEND_COLOR);

  /* Buffer Objects */
  JS_GL_CONSTANT(ARRAY_BUFFER);
  JS_GL_CONSTANT(ELEMENT_ARRAY_BUFFER);
  JS_GL_CONSTANT(ARRAY_BUFFER_BINDING);
  JS_GL_CONSTANT(ELEMENT_ARRAY_BUFFER_BINDING);

  JS_GL_CONSTANT(STREAM_DRAW);
  JS_GL_CONSTANT(STATIC_DRAW);
  JS_GL_CONSTANT(DYNAMIC_DRAW);

  JS_GL_CONSTANT(BUFFER_SIZE);
  JS_GL_CONSTANT(BUFFER_USAGE);

  JS_GL_CONSTANT(CURRENT_VERTEX_ATTRIB);

  /* CullFaceMode */
  JS_GL_CONSTANT(FRONT);
  JS_GL_CONSTANT(BACK);
  JS_GL_CONSTANT(FRONT_AND_BACK);

  /* DepthFunction */
  /*      GL_NEVER */
  /*      GL_LESS */
  /*      GL_EQUAL */
  /*      GL_LEQUAL */
  /*      GL_GREATER */
  /*      GL_NOTEQUAL */
  /*      GL_GEQUAL */
  /*      GL_ALWAYS */

  /* EnableCap */
  JS_GL_CONSTANT(TEXTURE_2D);
  JS_GL_CONSTANT(CULL_FACE);
  JS_GL_CONSTANT(BLEND);
  JS_GL_CONSTANT(DITHER);
  JS_GL_CONSTANT(STENCIL_TEST);
  JS_GL_CONSTANT(DEPTH_TEST);
  JS_GL_CONSTANT(SCISSOR_TEST);
  JS_GL_CONSTANT(POLYGON_OFFSET_FILL);
  JS_GL_CONSTANT(SAMPLE_ALPHA_TO_COVERAGE);
  JS_GL_CONSTANT(SAMPLE_COVERAGE);

  /* ErrorCode */
  JS_GL_CONSTANT(NO_ERROR);
  JS_GL_CONSTANT(INVALID_ENUM);
  JS_GL_CONSTANT(INVALID_VALUE);
  JS_GL_CONSTANT(INVALID_OPERATION);
  JS_GL_CONSTANT(OUT_OF_MEMORY);

  /* FrontFaceDirection */
  JS_GL_CONSTANT(CW);
  JS_GL_CONSTANT(CCW);

  /* GetPName */
  JS_GL_CONSTANT(LINE_WIDTH);
  JS_GL_CONSTANT(ALIASED_POINT_SIZE_RANGE);
  JS_GL_CONSTANT(ALIASED_LINE_WIDTH_RANGE);
  JS_GL_CONSTANT(CULL_FACE_MODE);
  JS_GL_CONSTANT(FRONT_FACE);
  JS_GL_CONSTANT(DEPTH_RANGE);
  JS_GL_CONSTANT(DEPTH_WRITEMASK);
  JS_GL_CONSTANT(DEPTH_CLEAR_VALUE);
  JS_GL_CONSTANT(DEPTH_FUNC);
  JS_GL_CONSTANT(STENCIL_CLEAR_VALUE);
  JS_GL_CONSTANT(STENCIL_FUNC);
  JS_GL_CONSTANT(STENCIL_FAIL);
  JS_GL_CONSTANT(STENCIL_PASS_DEPTH_FAIL);
  JS_GL_CONSTANT(STENCIL_PASS_DEPTH_PASS);
  JS_GL_CONSTANT(STENCIL_REF);
  JS_GL_CONSTANT(STENCIL_VALUE_MASK);
  JS_GL_CONSTANT(STENCIL_WRITEMASK);
  JS_GL_CONSTANT(STENCIL_BACK_FUNC);
  JS_GL_CONSTANT(STENCIL_BACK_FAIL);
  JS_GL_CONSTANT(STENCIL_BACK_PASS_DEPTH_FAIL);
  JS_GL_CONSTANT(STENCIL_BACK_PASS_DEPTH_PASS);
  JS_GL_CONSTANT(STENCIL_BACK_REF);
  JS_GL_CONSTANT(STENCIL_BACK_VALUE_MASK);
  JS_GL_CONSTANT(STENCIL_BACK_WRITEMASK);
  JS_GL_CONSTANT(VIEWPORT);
  JS_GL_CONSTANT(SCISSOR_BOX);
  /*      GL_SCISSOR_TEST */
  JS_GL_CONSTANT(COLOR_CLEAR_VALUE);
  JS_GL_CONSTANT(COLOR_WRITEMASK);
  JS_GL_CONSTANT(UNPACK_ALIGNMENT);
  JS_GL_CONSTANT(PACK_ALIGNMENT);
  JS_GL_CONSTANT(MAX_TEXTURE_SIZE);
  JS_GL_CONSTANT(MAX_VIEWPORT_DIMS);
  JS_GL_CONSTANT(SUBPIXEL_BITS);
  JS_GL_CONSTANT(RED_BITS);
  JS_GL_CONSTANT(GREEN_BITS);
  JS_GL_CONSTANT(BLUE_BITS);
  JS_GL_CONSTANT(ALPHA_BITS);
  JS_GL_CONSTANT(DEPTH_BITS);
  JS_GL_CONSTANT(STENCIL_BITS);
  JS_GL_CONSTANT(POLYGON_OFFSET_UNITS);
  /*      GL_POLYGON_OFFSET_FILL */
  JS_GL_CONSTANT(POLYGON_OFFSET_FACTOR);
  JS_GL_CONSTANT(TEXTURE_BINDING_2D);
  JS_GL_CONSTANT(SAMPLE_BUFFERS);
  JS_GL_CONSTANT(SAMPLES);
  JS_GL_CONSTANT(SAMPLE_COVERAGE_VALUE);
  JS_GL_CONSTANT(SAMPLE_COVERAGE_INVERT);

  /* GetTextureParameter */
  /*      GL_TEXTURE_MAG_FILTER */
  /*      GL_TEXTURE_MIN_FILTER */
  /*      GL_TEXTURE_WRAP_S */
  /*      GL_TEXTURE_WRAP_T */

  JS_GL_CONSTANT(NUM_COMPRESSED_TEXTURE_FORMATS);
  JS_GL_CONSTANT(COMPRESSED_TEXTURE_FORMATS);

  /* HintMode */
  JS_GL_CONSTANT(DONT_CARE);
  JS_GL_CONSTANT(FASTEST);
  JS_GL_CONSTANT(NICEST);

  /* HintTarget */
  JS_GL_CONSTANT(GENERATE_MIPMAP_HINT);

  /* DataType */
  JS_GL_CONSTANT(BYTE);
  JS_GL_CONSTANT(UNSIGNED_BYTE);
  JS_GL_CONSTANT(SHORT);
  JS_GL_CONSTANT(UNSIGNED_SHORT);
  JS_GL_CONSTANT(INT);
  JS_GL_CONSTANT(UNSIGNED_INT);
  JS_GL_CONSTANT(FLOAT);
#ifndef __APPLE__
  JS_GL_CONSTANT(FIXED);
#endif

  /* PixelFormat */
  JS_GL_CONSTANT(DEPTH_COMPONENT);
  JS_GL_CONSTANT(ALPHA);
  JS_GL_CONSTANT(RGB);
  JS_GL_CONSTANT(RGBA);
  JS_GL_CONSTANT(LUMINANCE);
  JS_GL_CONSTANT(LUMINANCE_ALPHA);

  /* PixelType */
  /*      GL_UNSIGNED_BYTE */
  JS_GL_CONSTANT(UNSIGNED_SHORT_4_4_4_4);
  JS_GL_CONSTANT(UNSIGNED_SHORT_5_5_5_1);
  JS_GL_CONSTANT(UNSIGNED_SHORT_5_6_5);

  /* Shaders */
  JS_GL_CONSTANT(FRAGMENT_SHADER);
  JS_GL_CONSTANT(VERTEX_SHADER);
  JS_GL_CONSTANT(MAX_VERTEX_ATTRIBS);
#ifndef __APPLE__
  JS_GL_CONSTANT(MAX_VERTEX_UNIFORM_VECTORS);
  JS_GL_CONSTANT(MAX_VARYING_VECTORS);
#endif
  JS_GL_CONSTANT(MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  JS_GL_CONSTANT(MAX_VERTEX_TEXTURE_IMAGE_UNITS);
  JS_GL_CONSTANT(MAX_TEXTURE_IMAGE_UNITS);
#ifndef __APPLE__
  JS_GL_CONSTANT(MAX_FRAGMENT_UNIFORM_VECTORS);
#endif
  JS_GL_CONSTANT(SHADER_TYPE);
  JS_GL_CONSTANT(DELETE_STATUS);
  JS_GL_CONSTANT(LINK_STATUS);
  JS_GL_CONSTANT(VALIDATE_STATUS);
  JS_GL_CONSTANT(ATTACHED_SHADERS);
  JS_GL_CONSTANT(ACTIVE_UNIFORMS);
  JS_GL_CONSTANT(ACTIVE_UNIFORM_MAX_LENGTH);
  JS_GL_CONSTANT(ACTIVE_ATTRIBUTES);
  JS_GL_CONSTANT(ACTIVE_ATTRIBUTE_MAX_LENGTH);
  JS_GL_CONSTANT(SHADING_LANGUAGE_VERSION);
  JS_GL_CONSTANT(CURRENT_PROGRAM);

  /* StencilFunction */
  JS_GL_CONSTANT(NEVER);
  JS_GL_CONSTANT(LESS);
  JS_GL_CONSTANT(EQUAL);
  JS_GL_CONSTANT(LEQUAL);
  JS_GL_CONSTANT(GREATER);
  JS_GL_CONSTANT(NOTEQUAL);
  JS_GL_CONSTANT(GEQUAL);
  JS_GL_CONSTANT(ALWAYS);

  /* StencilOp */
  /*      GL_ZERO */
  JS_GL_CONSTANT(KEEP);
  JS_GL_CONSTANT(REPLACE);
  JS_GL_CONSTANT(INCR);
  JS_GL_CONSTANT(DECR);
  JS_GL_CONSTANT(INVERT);
  JS_GL_CONSTANT(INCR_WRAP);
  JS_GL_CONSTANT(DECR_WRAP);

  /* StringName */
  JS_GL_CONSTANT(VENDOR);
  JS_GL_CONSTANT(RENDERER);
  JS_GL_CONSTANT(VERSION);
  JS_GL_CONSTANT(EXTENSIONS);

  /* TextureMagFilter */
  JS_GL_CONSTANT(NEAREST);
  JS_GL_CONSTANT(LINEAR);

  /* TextureMinFilter */
  /*      GL_NEAREST */
  /*      GL_LINEAR */
  JS_GL_CONSTANT(NEAREST_MIPMAP_NEAREST);
  JS_GL_CONSTANT(LINEAR_MIPMAP_NEAREST);
  JS_GL_CONSTANT(NEAREST_MIPMAP_LINEAR);
  JS_GL_CONSTANT(LINEAR_MIPMAP_LINEAR);

  /* TextureParameterName */
  JS_GL_CONSTANT(TEXTURE_MAG_FILTER);
  JS_GL_CONSTANT(TEXTURE_MIN_FILTER);
  JS_GL_CONSTANT(TEXTURE_WRAP_S);
  JS_GL_CONSTANT(TEXTURE_WRAP_T);

  /* TextureTarget */
  /*      GL_TEXTURE_2D */
  JS_GL_CONSTANT(TEXTURE);

  JS_GL_CONSTANT(TEXTURE_CUBE_MAP);
  JS_GL_CONSTANT(TEXTURE_BINDING_CUBE_MAP);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_X);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_X);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Y);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Y);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Z);
  JS_GL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Z);
  JS_GL_CONSTANT(MAX_CUBE_MAP_TEXTURE_SIZE);

  /* TextureUnit */
  JS_GL_CONSTANT(TEXTURE0);
  JS_GL_CONSTANT(TEXTURE1);
  JS_GL_CONSTANT(TEXTURE2);
  JS_GL_CONSTANT(TEXTURE3);
  JS_GL_CONSTANT(TEXTURE4);
  JS_GL_CONSTANT(TEXTURE5);
  JS_GL_CONSTANT(TEXTURE6);
  JS_GL_CONSTANT(TEXTURE7);
  JS_GL_CONSTANT(TEXTURE8);
  JS_GL_CONSTANT(TEXTURE9);
  JS_GL_CONSTANT(TEXTURE10);
  JS_GL_CONSTANT(TEXTURE11);
  JS_GL_CONSTANT(TEXTURE12);
  JS_GL_CONSTANT(TEXTURE13);
  JS_GL_CONSTANT(TEXTURE14);
  JS_GL_CONSTANT(TEXTURE15);
  JS_GL_CONSTANT(TEXTURE16);
  JS_GL_CONSTANT(TEXTURE17);
  JS_GL_CONSTANT(TEXTURE18);
  JS_GL_CONSTANT(TEXTURE19);
  JS_GL_CONSTANT(TEXTURE20);
  JS_GL_CONSTANT(TEXTURE21);
  JS_GL_CONSTANT(TEXTURE22);
  JS_GL_CONSTANT(TEXTURE23);
  JS_GL_CONSTANT(TEXTURE24);
  JS_GL_CONSTANT(TEXTURE25);
  JS_GL_CONSTANT(TEXTURE26);
  JS_GL_CONSTANT(TEXTURE27);
  JS_GL_CONSTANT(TEXTURE28);
  JS_GL_CONSTANT(TEXTURE29);
  JS_GL_CONSTANT(TEXTURE30);
  JS_GL_CONSTANT(TEXTURE31);
  JS_GL_CONSTANT(ACTIVE_TEXTURE);

  /* TextureWrapMode */
  JS_GL_CONSTANT(REPEAT);
  JS_GL_CONSTANT(CLAMP_TO_EDGE);
  JS_GL_CONSTANT(MIRRORED_REPEAT);

  /* Uniform Types */
  JS_GL_CONSTANT(FLOAT_VEC2);
  JS_GL_CONSTANT(FLOAT_VEC3);
  JS_GL_CONSTANT(FLOAT_VEC4);
  JS_GL_CONSTANT(INT_VEC2);
  JS_GL_CONSTANT(INT_VEC3);
  JS_GL_CONSTANT(INT_VEC4);
  JS_GL_CONSTANT(BOOL);
  JS_GL_CONSTANT(BOOL_VEC2);
  JS_GL_CONSTANT(BOOL_VEC3);
  JS_GL_CONSTANT(BOOL_VEC4);
  JS_GL_CONSTANT(FLOAT_MAT2);
  JS_GL_CONSTANT(FLOAT_MAT3);
  JS_GL_CONSTANT(FLOAT_MAT4);
  JS_GL_CONSTANT(SAMPLER_2D);
  JS_GL_CONSTANT(SAMPLER_CUBE);

  /* Vertex Arrays */
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_ENABLED);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_SIZE);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_STRIDE);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_TYPE);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_NORMALIZED);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_POINTER);
  JS_GL_CONSTANT(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);

  /* Read Format */
#ifndef __APPLE__
  JS_GL_CONSTANT(IMPLEMENTATION_COLOR_READ_TYPE);
  JS_GL_CONSTANT(IMPLEMENTATION_COLOR_READ_FORMAT);
#endif

  /* Shader Source */
  JS_GL_CONSTANT(COMPILE_STATUS);
  JS_GL_CONSTANT(INFO_LOG_LENGTH);
  JS_GL_CONSTANT(SHADER_SOURCE_LENGTH);
#ifndef __APPLE__
  JS_GL_CONSTANT(SHADER_COMPILER);
#endif

  /* Shader Binary */
#ifndef __APPLE__
  JS_GL_CONSTANT(SHADER_BINARY_FORMATS);
  JS_GL_CONSTANT(NUM_SHADER_BINARY_FORMATS);
#endif

  /* Shader Precision-Specified Types */
#ifndef __APPLE__
  JS_GL_CONSTANT(LOW_FLOAT);
  JS_GL_CONSTANT(MEDIUM_FLOAT);
  JS_GL_CONSTANT(HIGH_FLOAT);
  JS_GL_CONSTANT(LOW_INT);
  JS_GL_CONSTANT(MEDIUM_INT);
  JS_GL_CONSTANT(HIGH_INT);
#endif

  /* Framebuffer Object. */
  JS_GL_CONSTANT(FRAMEBUFFER);
  JS_GL_CONSTANT(RENDERBUFFER);

  JS_GL_CONSTANT(RGBA4);
  JS_GL_CONSTANT(RGB5_A1);
#ifndef __APPLE__
  //JS_GL_CONSTANT(RGB565);
#endif
  JS_GL_CONSTANT(DEPTH_COMPONENT16);
  JS_GL_CONSTANT(STENCIL_INDEX);
  JS_GL_CONSTANT(STENCIL_INDEX8);

  JS_GL_CONSTANT(RENDERBUFFER_WIDTH);
  JS_GL_CONSTANT(RENDERBUFFER_HEIGHT);
  JS_GL_CONSTANT(RENDERBUFFER_INTERNAL_FORMAT);
  JS_GL_CONSTANT(RENDERBUFFER_RED_SIZE);
  JS_GL_CONSTANT(RENDERBUFFER_GREEN_SIZE);
  JS_GL_CONSTANT(RENDERBUFFER_BLUE_SIZE);
  JS_GL_CONSTANT(RENDERBUFFER_ALPHA_SIZE);
  JS_GL_CONSTANT(RENDERBUFFER_DEPTH_SIZE);
  JS_GL_CONSTANT(RENDERBUFFER_STENCIL_SIZE);

  JS_GL_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
  JS_GL_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
  JS_GL_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
  JS_GL_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);

  JS_GL_CONSTANT(COLOR_ATTACHMENT0);
  JS_GL_CONSTANT(DEPTH_ATTACHMENT);
  JS_GL_CONSTANT(STENCIL_ATTACHMENT);

  JS_GL_CONSTANT(NONE);

  JS_GL_CONSTANT(FRAMEBUFFER_COMPLETE);
  JS_GL_CONSTANT(FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
  JS_GL_CONSTANT(FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
#ifndef __APPLE__
  //JS_GL_CONSTANT(FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
#endif
  JS_GL_CONSTANT(FRAMEBUFFER_UNSUPPORTED);

  JS_GL_CONSTANT(FRAMEBUFFER_BINDING);
  JS_GL_CONSTANT(RENDERBUFFER_BINDING);
  JS_GL_CONSTANT(MAX_RENDERBUFFER_SIZE);

  JS_GL_CONSTANT(INVALID_FRAMEBUFFER_OPERATION);

  /* WebGL-specific enums */
  webgl_template->PrototypeTemplate()->Set(JS_STR( "UNPACK_FLIP_Y_WEBGL" ), JS_INT(0x9240));
  webgl_template->PrototypeTemplate()->Set(JS_STR( "UNPACK_PREMULTIPLY_ALPHA_WEBGL" ), JS_INT(0x9241));
  webgl_template->PrototypeTemplate()->Set(JS_STR( "CONTEXT_LOST_WEBGL" ), JS_INT(0x9242));
  webgl_template->PrototypeTemplate()->Set(JS_STR( "UNPACK_COLORSPACE_CONVERSION_WEBGL" ), JS_INT(0x9243));
  webgl_template->PrototypeTemplate()->Set(JS_STR( "BROWSER_DEFAULT_WEBGL" ), JS_INT(0x9244));

  //Export function
  target->Set(JS_STR("WebGLRenderingContext"), webgl_template->GetFunction());
}

NODE_MODULE(webgl, init)
} // extern "C"
