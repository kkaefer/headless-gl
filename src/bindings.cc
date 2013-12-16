/*
 * bindings.cc
 *
 *  Created on: Dec 13, 2011
 *      Author: ngk437
 *
 * Modified by Mikola Lysenko, Feb. 2013
 */

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <cstdlib>
#include <v8.h>
#include <node.h>
#include "arch_wrapper.h"
#include "webgl.h"
#include "macros.h"

using namespace v8;

Persistent<FunctionTemplate> webgl_template;

#define JS_SET_GL_CONSTANT(tmpl, name) tmpl->Set(String::New(#name), Integer::New(GL_ ## name));
#define JS_SET_NUM_CONSTANT(tmpl, name, val) tmpl->Set(String::New("UNPACK_FLIP_Y_WEBGL"), Integer::New(val));
#define JS_SET_METHOD(tmpl, name, fn) tmpl->Set(String::New(name), FunctionTemplate::New(fn, Handle<Value>(), sig));

namespace HeadlessGL {

void Init(Handle<Object> target) {
  // When node exits kill any stray gl contexts
  atexit(WebGL::disposeAll);

  // Create the WebGL template
  webgl_template = Persistent<FunctionTemplate>::New(FunctionTemplate::New(WebGL::New));
  Handle<Signature> sig = Signature::New(webgl_template);
  webgl_template->InstanceTemplate()->SetInternalFieldCount(1);
  webgl_template->SetClassName(String::New("WebGLRenderingContext"));

  //Add methods
  Handle<ObjectTemplate> proto = webgl_template->PrototypeTemplate();
  JS_SET_METHOD(proto, "uniform1f", WebGL::Uniform1f);
  JS_SET_METHOD(proto, "uniform2f", WebGL::Uniform2f);
  JS_SET_METHOD(proto, "uniform3f", WebGL::Uniform3f);
  JS_SET_METHOD(proto, "uniform4f", WebGL::Uniform4f);
  JS_SET_METHOD(proto, "uniform1i", WebGL::Uniform1i);
  JS_SET_METHOD(proto, "uniform2i", WebGL::Uniform2i);
  JS_SET_METHOD(proto, "uniform3i", WebGL::Uniform3i);
  JS_SET_METHOD(proto, "uniform4i", WebGL::Uniform4i);
  JS_SET_METHOD(proto, "uniform1fv", WebGL::Uniform1fv);
  JS_SET_METHOD(proto, "uniform2fv", WebGL::Uniform2fv);
  JS_SET_METHOD(proto, "uniform3fv", WebGL::Uniform3fv);
  JS_SET_METHOD(proto, "uniform4fv", WebGL::Uniform4fv);
  JS_SET_METHOD(proto, "uniform1iv", WebGL::Uniform1iv);
  JS_SET_METHOD(proto, "uniform2iv", WebGL::Uniform2iv);
  JS_SET_METHOD(proto, "uniform3iv", WebGL::Uniform3iv);
  JS_SET_METHOD(proto, "uniform4iv", WebGL::Uniform4iv);
  JS_SET_METHOD(proto, "pixelStorei", WebGL::PixelStorei);
  JS_SET_METHOD(proto, "bindAttribLocation", WebGL::BindAttribLocation);
  JS_SET_METHOD(proto, "getError", WebGL::GetError);
  JS_SET_METHOD(proto, "drawArrays", WebGL::DrawArrays);
  JS_SET_METHOD(proto, "uniformMatrix2fv", WebGL::UniformMatrix2fv);
  JS_SET_METHOD(proto, "uniformMatrix3fv", WebGL::UniformMatrix3fv);
  JS_SET_METHOD(proto, "uniformMatrix4fv", WebGL::UniformMatrix4fv);

  JS_SET_METHOD(proto, "generateMipmap", WebGL::GenerateMipmap);

  JS_SET_METHOD(proto, "getAttribLocation", WebGL::GetAttribLocation);
  JS_SET_METHOD(proto, "depthFunc", WebGL::DepthFunc);
  JS_SET_METHOD(proto, "viewport", WebGL::Viewport);
  JS_SET_METHOD(proto, "createShader", WebGL::CreateShader);
  JS_SET_METHOD(proto, "shaderSource", WebGL::ShaderSource);
  JS_SET_METHOD(proto, "compileShader", WebGL::CompileShader);
  JS_SET_METHOD(proto, "getShaderParameter", WebGL::GetShaderParameter);
  JS_SET_METHOD(proto, "getShaderInfoLog", WebGL::GetShaderInfoLog);
  JS_SET_METHOD(proto, "createProgram", WebGL::CreateProgram);
  JS_SET_METHOD(proto, "attachShader", WebGL::AttachShader);
  JS_SET_METHOD(proto, "linkProgram", WebGL::LinkProgram);
  JS_SET_METHOD(proto, "getProgramParameter", WebGL::GetProgramParameter);
  JS_SET_METHOD(proto, "getUniformLocation", WebGL::GetUniformLocation);
  JS_SET_METHOD(proto, "clearColor", WebGL::ClearColor);
  JS_SET_METHOD(proto, "clearDepth", WebGL::ClearDepth);

  JS_SET_METHOD(proto, "disable", WebGL::Disable);
  JS_SET_METHOD(proto, "createTexture", WebGL::CreateTexture);
  JS_SET_METHOD(proto, "bindTexture", WebGL::BindTexture);
  JS_SET_METHOD(proto, "texImage2D", WebGL::TexImage2D);
  JS_SET_METHOD(proto, "texParameteri", WebGL::TexParameteri);
  JS_SET_METHOD(proto, "texParameterf", WebGL::TexParameterf);
  JS_SET_METHOD(proto, "clear", WebGL::Clear);
  JS_SET_METHOD(proto, "useProgram", WebGL::UseProgram);
  JS_SET_METHOD(proto, "createFramebuffer", WebGL::CreateFramebuffer);
  JS_SET_METHOD(proto, "bindFramebuffer", WebGL::BindFramebuffer);
  JS_SET_METHOD(proto, "framebufferTexture2D", WebGL::FramebufferTexture2D);
  JS_SET_METHOD(proto, "createBuffer", WebGL::CreateBuffer);
  JS_SET_METHOD(proto, "bindBuffer", WebGL::BindBuffer);
  JS_SET_METHOD(proto, "bufferData", WebGL::BufferData);
  JS_SET_METHOD(proto, "bufferSubData", WebGL::BufferSubData);
  JS_SET_METHOD(proto, "enable", WebGL::Enable);
  JS_SET_METHOD(proto, "blendEquation", WebGL::BlendEquation);
  JS_SET_METHOD(proto, "blendFunc", WebGL::BlendFunc);
  JS_SET_METHOD(proto, "enableVertexAttribArray", WebGL::EnableVertexAttribArray);
  JS_SET_METHOD(proto, "vertexAttribPointer", WebGL::VertexAttribPointer);
  JS_SET_METHOD(proto, "activeTexture", WebGL::ActiveTexture);
  JS_SET_METHOD(proto, "drawElements", WebGL::DrawElements);
  JS_SET_METHOD(proto, "flush", WebGL::Flush);
  JS_SET_METHOD(proto, "finish", WebGL::Finish);

  JS_SET_METHOD(proto, "vertexAttrib1f", WebGL::VertexAttrib1f);
  JS_SET_METHOD(proto, "vertexAttrib2f", WebGL::VertexAttrib2f);
  JS_SET_METHOD(proto, "vertexAttrib3f", WebGL::VertexAttrib3f);
  JS_SET_METHOD(proto, "vertexAttrib4f", WebGL::VertexAttrib4f);
  JS_SET_METHOD(proto, "vertexAttrib1fv", WebGL::VertexAttrib1fv);
  JS_SET_METHOD(proto, "vertexAttrib2fv", WebGL::VertexAttrib2fv);
  JS_SET_METHOD(proto, "vertexAttrib3fv", WebGL::VertexAttrib3fv);
  JS_SET_METHOD(proto, "vertexAttrib4fv", WebGL::VertexAttrib4fv);

  JS_SET_METHOD(proto, "blendColor", WebGL::BlendColor);
  JS_SET_METHOD(proto, "blendEquationSeparate", WebGL::BlendEquationSeparate);
  JS_SET_METHOD(proto, "blendFuncSeparate", WebGL::BlendFuncSeparate);
  JS_SET_METHOD(proto, "clearStencil", WebGL::ClearStencil);
  JS_SET_METHOD(proto, "colorMask", WebGL::ColorMask);
  JS_SET_METHOD(proto, "copyTexImage2D", WebGL::CopyTexImage2D);
  JS_SET_METHOD(proto, "copyTexSubImage2D", WebGL::CopyTexSubImage2D);
  JS_SET_METHOD(proto, "cullFace", WebGL::CullFace);
  JS_SET_METHOD(proto, "depthMask", WebGL::DepthMask);
  JS_SET_METHOD(proto, "depthRange", WebGL::DepthRange);
  JS_SET_METHOD(proto, "disableVertexAttribArray", WebGL::DisableVertexAttribArray);
  JS_SET_METHOD(proto, "hint", WebGL::Hint);
  JS_SET_METHOD(proto, "isEnabled", WebGL::IsEnabled);
  JS_SET_METHOD(proto, "lineWidth", WebGL::LineWidth);
  JS_SET_METHOD(proto, "polygonOffset", WebGL::PolygonOffset);

  JS_SET_METHOD(proto, "scissor", WebGL::Scissor);
  JS_SET_METHOD(proto, "stencilFunc", WebGL::StencilFunc);
  JS_SET_METHOD(proto, "stencilFuncSeparate", WebGL::StencilFuncSeparate);
  JS_SET_METHOD(proto, "stencilMask", WebGL::StencilMask);
  JS_SET_METHOD(proto, "stencilMaskSeparate", WebGL::StencilMaskSeparate);
  JS_SET_METHOD(proto, "stencilOp", WebGL::StencilOp);
  JS_SET_METHOD(proto, "stencilOpSeparate", WebGL::StencilOpSeparate);
  JS_SET_METHOD(proto, "bindRenderbuffer", WebGL::BindRenderbuffer);
  JS_SET_METHOD(proto, "createRenderbuffer", WebGL::CreateRenderbuffer);

  JS_SET_METHOD(proto, "deleteBuffer", WebGL::DeleteBuffer);
  JS_SET_METHOD(proto, "deleteFramebuffer", WebGL::DeleteFramebuffer);
  JS_SET_METHOD(proto, "deleteProgram", WebGL::DeleteProgram);
  JS_SET_METHOD(proto, "deleteRenderbuffer", WebGL::DeleteRenderbuffer);
  JS_SET_METHOD(proto, "deleteShader", WebGL::DeleteShader);
  JS_SET_METHOD(proto, "deleteTexture", WebGL::DeleteTexture);
  JS_SET_METHOD(proto, "detachShader", WebGL::DetachShader);
  JS_SET_METHOD(proto, "framebufferRenderbuffer", WebGL::FramebufferRenderbuffer);
  JS_SET_METHOD(proto, "getVertexAttribOffset", WebGL::GetVertexAttribOffset);

  JS_SET_METHOD(proto, "isBuffer", WebGL::IsBuffer);
  JS_SET_METHOD(proto, "isFramebuffer", WebGL::IsFramebuffer);
  JS_SET_METHOD(proto, "isProgram", WebGL::IsProgram);
  JS_SET_METHOD(proto, "isRenderbuffer", WebGL::IsRenderbuffer);
  JS_SET_METHOD(proto, "isShader", WebGL::IsShader);
  JS_SET_METHOD(proto, "isTexture", WebGL::IsTexture);

  JS_SET_METHOD(proto, "renderbufferStorage", WebGL::RenderbufferStorage);
  JS_SET_METHOD(proto, "getShaderSource", WebGL::GetShaderSource);
  JS_SET_METHOD(proto, "validateProgram", WebGL::ValidateProgram);

  JS_SET_METHOD(proto, "texSubImage2D", WebGL::TexSubImage2D);
  JS_SET_METHOD(proto, "readPixels", WebGL::ReadPixels);
  JS_SET_METHOD(proto, "getTexParameter", WebGL::GetTexParameter);
  JS_SET_METHOD(proto, "getActiveAttrib", WebGL::GetActiveAttrib);
  JS_SET_METHOD(proto, "getActiveUniform", WebGL::GetActiveUniform);
  JS_SET_METHOD(proto, "getAttachedShaders", WebGL::GetAttachedShaders);
  JS_SET_METHOD(proto, "getParameter", WebGL::GetParameter);
  JS_SET_METHOD(proto, "getBufferParameter", WebGL::GetBufferParameter);
  JS_SET_METHOD(proto, "getFramebufferAttachmentParameter", WebGL::GetFramebufferAttachmentParameter);
  JS_SET_METHOD(proto, "getProgramInfoLog", WebGL::GetProgramInfoLog);
  JS_SET_METHOD(proto, "getRenderbufferParameter", WebGL::GetRenderbufferParameter);
  JS_SET_METHOD(proto, "getVertexAttrib", WebGL::GetVertexAttrib);
  JS_SET_METHOD(proto, "getSupportedExtensions", WebGL::GetSupportedExtensions);
  JS_SET_METHOD(proto, "getExtension", WebGL::GetExtension);
  JS_SET_METHOD(proto, "checkFramebufferStatus", WebGL::CheckFramebufferStatus);

  JS_SET_METHOD(proto, "frontFace", WebGL::FrontFace);
  JS_SET_METHOD(proto, "sampleCoverage", WebGL::SampleCoverage);
  JS_SET_METHOD(proto, "destroy", WebGL::Destroy);


  // OpenGL ES 2.1 constants

  /* ClearBufferMask */
  JS_SET_GL_CONSTANT(proto, DEPTH_BUFFER_BIT);
  JS_SET_GL_CONSTANT(proto, STENCIL_BUFFER_BIT);
  JS_SET_GL_CONSTANT(proto, COLOR_BUFFER_BIT);

  /* Boolean */
  JS_SET_GL_CONSTANT(proto, FALSE);
  JS_SET_GL_CONSTANT(proto, TRUE);

  /* BeginMode */
  JS_SET_GL_CONSTANT(proto, POINTS);
  JS_SET_GL_CONSTANT(proto, LINES);
  JS_SET_GL_CONSTANT(proto, LINE_LOOP);
  JS_SET_GL_CONSTANT(proto, LINE_STRIP);
  JS_SET_GL_CONSTANT(proto, TRIANGLES);
  JS_SET_GL_CONSTANT(proto, TRIANGLE_STRIP);
  JS_SET_GL_CONSTANT(proto, TRIANGLE_FAN);

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
  JS_SET_GL_CONSTANT(proto, ZERO);
  JS_SET_GL_CONSTANT(proto, ONE);
  JS_SET_GL_CONSTANT(proto, SRC_COLOR);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_SRC_COLOR);
  JS_SET_GL_CONSTANT(proto, SRC_ALPHA);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_SRC_ALPHA);
  JS_SET_GL_CONSTANT(proto, DST_ALPHA);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_DST_ALPHA);

  /* BlendingFactorSrc */
  /*      GL_ZERO */
  /*      GL_ONE */
  JS_SET_GL_CONSTANT(proto, DST_COLOR);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_DST_COLOR);
  JS_SET_GL_CONSTANT(proto, SRC_ALPHA_SATURATE);
  /*      GL_SRC_ALPHA */
  /*      GL_ONE_MINUS_SRC_ALPHA */
  /*      GL_DST_ALPHA */
  /*      GL_ONE_MINUS_DST_ALPHA */

  /* BlendEquationSeparate */
  JS_SET_GL_CONSTANT(proto, FUNC_ADD);
  JS_SET_GL_CONSTANT(proto, BLEND_EQUATION);
  JS_SET_GL_CONSTANT(proto, BLEND_EQUATION_RGB);    /* same as BLEND_EQUATION */
  JS_SET_GL_CONSTANT(proto, BLEND_EQUATION_ALPHA);

  /* BlendSubtract */
  JS_SET_GL_CONSTANT(proto, FUNC_SUBTRACT);
  JS_SET_GL_CONSTANT(proto, FUNC_REVERSE_SUBTRACT);

  /* Separate Blend Functions */
  JS_SET_GL_CONSTANT(proto, BLEND_DST_RGB);
  JS_SET_GL_CONSTANT(proto, BLEND_SRC_RGB);
  JS_SET_GL_CONSTANT(proto, BLEND_DST_ALPHA);
  JS_SET_GL_CONSTANT(proto, BLEND_SRC_ALPHA);
  JS_SET_GL_CONSTANT(proto, CONSTANT_COLOR);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_CONSTANT_COLOR);
  JS_SET_GL_CONSTANT(proto, CONSTANT_ALPHA);
  JS_SET_GL_CONSTANT(proto, ONE_MINUS_CONSTANT_ALPHA);
  JS_SET_GL_CONSTANT(proto, BLEND_COLOR);

  /* Buffer Objects */
  JS_SET_GL_CONSTANT(proto, ARRAY_BUFFER);
  JS_SET_GL_CONSTANT(proto, ELEMENT_ARRAY_BUFFER);
  JS_SET_GL_CONSTANT(proto, ARRAY_BUFFER_BINDING);
  JS_SET_GL_CONSTANT(proto, ELEMENT_ARRAY_BUFFER_BINDING);

  JS_SET_GL_CONSTANT(proto, STREAM_DRAW);
  JS_SET_GL_CONSTANT(proto, STATIC_DRAW);
  JS_SET_GL_CONSTANT(proto, DYNAMIC_DRAW);

  JS_SET_GL_CONSTANT(proto, BUFFER_SIZE);
  JS_SET_GL_CONSTANT(proto, BUFFER_USAGE);

  JS_SET_GL_CONSTANT(proto, CURRENT_VERTEX_ATTRIB);

  /* CullFaceMode */
  JS_SET_GL_CONSTANT(proto, FRONT);
  JS_SET_GL_CONSTANT(proto, BACK);
  JS_SET_GL_CONSTANT(proto, FRONT_AND_BACK);

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
  JS_SET_GL_CONSTANT(proto, TEXTURE_2D);
  JS_SET_GL_CONSTANT(proto, CULL_FACE);
  JS_SET_GL_CONSTANT(proto, BLEND);
  JS_SET_GL_CONSTANT(proto, DITHER);
  JS_SET_GL_CONSTANT(proto, STENCIL_TEST);
  JS_SET_GL_CONSTANT(proto, DEPTH_TEST);
  JS_SET_GL_CONSTANT(proto, SCISSOR_TEST);
  JS_SET_GL_CONSTANT(proto, POLYGON_OFFSET_FILL);
  JS_SET_GL_CONSTANT(proto, SAMPLE_ALPHA_TO_COVERAGE);
  JS_SET_GL_CONSTANT(proto, SAMPLE_COVERAGE);

  /* ErrorCode */
  JS_SET_GL_CONSTANT(proto, NO_ERROR);
  JS_SET_GL_CONSTANT(proto, INVALID_ENUM);
  JS_SET_GL_CONSTANT(proto, INVALID_VALUE);
  JS_SET_GL_CONSTANT(proto, INVALID_OPERATION);
  JS_SET_GL_CONSTANT(proto, OUT_OF_MEMORY);

  /* FrontFaceDirection */
  JS_SET_GL_CONSTANT(proto, CW);
  JS_SET_GL_CONSTANT(proto, CCW);

  /* GetPName */
  JS_SET_GL_CONSTANT(proto, LINE_WIDTH);
  JS_SET_GL_CONSTANT(proto, ALIASED_POINT_SIZE_RANGE);
  JS_SET_GL_CONSTANT(proto, ALIASED_LINE_WIDTH_RANGE);
  JS_SET_GL_CONSTANT(proto, CULL_FACE_MODE);
  JS_SET_GL_CONSTANT(proto, FRONT_FACE);
  JS_SET_GL_CONSTANT(proto, DEPTH_RANGE);
  JS_SET_GL_CONSTANT(proto, DEPTH_WRITEMASK);
  JS_SET_GL_CONSTANT(proto, DEPTH_CLEAR_VALUE);
  JS_SET_GL_CONSTANT(proto, DEPTH_FUNC);
  JS_SET_GL_CONSTANT(proto, STENCIL_CLEAR_VALUE);
  JS_SET_GL_CONSTANT(proto, STENCIL_FUNC);
  JS_SET_GL_CONSTANT(proto, STENCIL_FAIL);
  JS_SET_GL_CONSTANT(proto, STENCIL_PASS_DEPTH_FAIL);
  JS_SET_GL_CONSTANT(proto, STENCIL_PASS_DEPTH_PASS);
  JS_SET_GL_CONSTANT(proto, STENCIL_REF);
  JS_SET_GL_CONSTANT(proto, STENCIL_VALUE_MASK);
  JS_SET_GL_CONSTANT(proto, STENCIL_WRITEMASK);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_FUNC);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_FAIL);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_PASS_DEPTH_FAIL);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_PASS_DEPTH_PASS);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_REF);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_VALUE_MASK);
  JS_SET_GL_CONSTANT(proto, STENCIL_BACK_WRITEMASK);
  JS_SET_GL_CONSTANT(proto, VIEWPORT);
  JS_SET_GL_CONSTANT(proto, SCISSOR_BOX);
  /*      GL_SCISSOR_TEST */
  JS_SET_GL_CONSTANT(proto, COLOR_CLEAR_VALUE);
  JS_SET_GL_CONSTANT(proto, COLOR_WRITEMASK);
  JS_SET_GL_CONSTANT(proto, UNPACK_ALIGNMENT);
  JS_SET_GL_CONSTANT(proto, PACK_ALIGNMENT);
  JS_SET_GL_CONSTANT(proto, MAX_TEXTURE_SIZE);
  JS_SET_GL_CONSTANT(proto, MAX_VIEWPORT_DIMS);
  JS_SET_GL_CONSTANT(proto, SUBPIXEL_BITS);
  JS_SET_GL_CONSTANT(proto, RED_BITS);
  JS_SET_GL_CONSTANT(proto, GREEN_BITS);
  JS_SET_GL_CONSTANT(proto, BLUE_BITS);
  JS_SET_GL_CONSTANT(proto, ALPHA_BITS);
  JS_SET_GL_CONSTANT(proto, DEPTH_BITS);
  JS_SET_GL_CONSTANT(proto, STENCIL_BITS);
  JS_SET_GL_CONSTANT(proto, POLYGON_OFFSET_UNITS);
  /*      GL_POLYGON_OFFSET_FILL */
  JS_SET_GL_CONSTANT(proto, POLYGON_OFFSET_FACTOR);
  JS_SET_GL_CONSTANT(proto, TEXTURE_BINDING_2D);
  JS_SET_GL_CONSTANT(proto, SAMPLE_BUFFERS);
  JS_SET_GL_CONSTANT(proto, SAMPLES);
  JS_SET_GL_CONSTANT(proto, SAMPLE_COVERAGE_VALUE);
  JS_SET_GL_CONSTANT(proto, SAMPLE_COVERAGE_INVERT);

  /* GetTextureParameter */
  /*      GL_TEXTURE_MAG_FILTER */
  /*      GL_TEXTURE_MIN_FILTER */
  /*      GL_TEXTURE_WRAP_S */
  /*      GL_TEXTURE_WRAP_T */

  JS_SET_GL_CONSTANT(proto, NUM_COMPRESSED_TEXTURE_FORMATS);
  JS_SET_GL_CONSTANT(proto, COMPRESSED_TEXTURE_FORMATS);

  /* HintMode */
  JS_SET_GL_CONSTANT(proto, DONT_CARE);
  JS_SET_GL_CONSTANT(proto, FASTEST);
  JS_SET_GL_CONSTANT(proto, NICEST);

  /* HintTarget */
  JS_SET_GL_CONSTANT(proto, GENERATE_MIPMAP_HINT);

  /* DataType */
  JS_SET_GL_CONSTANT(proto, BYTE);
  JS_SET_GL_CONSTANT(proto, UNSIGNED_BYTE);
  JS_SET_GL_CONSTANT(proto, SHORT);
  JS_SET_GL_CONSTANT(proto, UNSIGNED_SHORT);
  JS_SET_GL_CONSTANT(proto, INT);
  JS_SET_GL_CONSTANT(proto, UNSIGNED_INT);
  JS_SET_GL_CONSTANT(proto, FLOAT);
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, FIXED);
#endif

  /* PixelFormat */
  JS_SET_GL_CONSTANT(proto, DEPTH_COMPONENT);
  JS_SET_GL_CONSTANT(proto, ALPHA);
  JS_SET_GL_CONSTANT(proto, RGB);
  JS_SET_GL_CONSTANT(proto, RGBA);
  JS_SET_GL_CONSTANT(proto, LUMINANCE);
  JS_SET_GL_CONSTANT(proto, LUMINANCE_ALPHA);

  /* PixelType */
  /*      GL_UNSIGNED_BYTE */
  JS_SET_GL_CONSTANT(proto, UNSIGNED_SHORT_4_4_4_4);
  JS_SET_GL_CONSTANT(proto, UNSIGNED_SHORT_5_5_5_1);
  JS_SET_GL_CONSTANT(proto, UNSIGNED_SHORT_5_6_5);

  /* Shaders */
  JS_SET_GL_CONSTANT(proto, FRAGMENT_SHADER);
  JS_SET_GL_CONSTANT(proto, VERTEX_SHADER);
  JS_SET_GL_CONSTANT(proto, MAX_VERTEX_ATTRIBS);
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, MAX_VERTEX_UNIFORM_VECTORS);
  JS_SET_GL_CONSTANT(proto, MAX_VARYING_VECTORS);
#endif
  JS_SET_GL_CONSTANT(proto, MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  JS_SET_GL_CONSTANT(proto, MAX_VERTEX_TEXTURE_IMAGE_UNITS);
  JS_SET_GL_CONSTANT(proto, MAX_TEXTURE_IMAGE_UNITS);
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, MAX_FRAGMENT_UNIFORM_VECTORS);
#endif
  JS_SET_GL_CONSTANT(proto, SHADER_TYPE);
  JS_SET_GL_CONSTANT(proto, DELETE_STATUS);
  JS_SET_GL_CONSTANT(proto, LINK_STATUS);
  JS_SET_GL_CONSTANT(proto, VALIDATE_STATUS);
  JS_SET_GL_CONSTANT(proto, ATTACHED_SHADERS);
  JS_SET_GL_CONSTANT(proto, ACTIVE_UNIFORMS);
  JS_SET_GL_CONSTANT(proto, ACTIVE_UNIFORM_MAX_LENGTH);
  JS_SET_GL_CONSTANT(proto, ACTIVE_ATTRIBUTES);
  JS_SET_GL_CONSTANT(proto, ACTIVE_ATTRIBUTE_MAX_LENGTH);
  JS_SET_GL_CONSTANT(proto, SHADING_LANGUAGE_VERSION);
  JS_SET_GL_CONSTANT(proto, CURRENT_PROGRAM);

  /* StencilFunction */
  JS_SET_GL_CONSTANT(proto, NEVER);
  JS_SET_GL_CONSTANT(proto, LESS);
  JS_SET_GL_CONSTANT(proto, EQUAL);
  JS_SET_GL_CONSTANT(proto, LEQUAL);
  JS_SET_GL_CONSTANT(proto, GREATER);
  JS_SET_GL_CONSTANT(proto, NOTEQUAL);
  JS_SET_GL_CONSTANT(proto, GEQUAL);
  JS_SET_GL_CONSTANT(proto, ALWAYS);

  /* StencilOp */
  /*      GL_ZERO */
  JS_SET_GL_CONSTANT(proto, KEEP);
  JS_SET_GL_CONSTANT(proto, REPLACE);
  JS_SET_GL_CONSTANT(proto, INCR);
  JS_SET_GL_CONSTANT(proto, DECR);
  JS_SET_GL_CONSTANT(proto, INVERT);
  JS_SET_GL_CONSTANT(proto, INCR_WRAP);
  JS_SET_GL_CONSTANT(proto, DECR_WRAP);

  /* StringName */
  JS_SET_GL_CONSTANT(proto, VENDOR);
  JS_SET_GL_CONSTANT(proto, RENDERER);
  JS_SET_GL_CONSTANT(proto, VERSION);
  JS_SET_GL_CONSTANT(proto, EXTENSIONS);

  /* TextureMagFilter */
  JS_SET_GL_CONSTANT(proto, NEAREST);
  JS_SET_GL_CONSTANT(proto, LINEAR);

  /* TextureMinFilter */
  /*      GL_NEAREST */
  /*      GL_LINEAR */
  JS_SET_GL_CONSTANT(proto, NEAREST_MIPMAP_NEAREST);
  JS_SET_GL_CONSTANT(proto, LINEAR_MIPMAP_NEAREST);
  JS_SET_GL_CONSTANT(proto, NEAREST_MIPMAP_LINEAR);
  JS_SET_GL_CONSTANT(proto, LINEAR_MIPMAP_LINEAR);

  /* TextureParameterName */
  JS_SET_GL_CONSTANT(proto, TEXTURE_MAG_FILTER);
  JS_SET_GL_CONSTANT(proto, TEXTURE_MIN_FILTER);
  JS_SET_GL_CONSTANT(proto, TEXTURE_WRAP_S);
  JS_SET_GL_CONSTANT(proto, TEXTURE_WRAP_T);

  /* TextureTarget */
  /*      GL_TEXTURE_2D */
  JS_SET_GL_CONSTANT(proto, TEXTURE);

  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP);
  JS_SET_GL_CONSTANT(proto, TEXTURE_BINDING_CUBE_MAP);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_POSITIVE_X);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_NEGATIVE_X);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_POSITIVE_Y);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_NEGATIVE_Y);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_POSITIVE_Z);
  JS_SET_GL_CONSTANT(proto, TEXTURE_CUBE_MAP_NEGATIVE_Z);
  JS_SET_GL_CONSTANT(proto, MAX_CUBE_MAP_TEXTURE_SIZE);

  /* TextureUnit */
  JS_SET_GL_CONSTANT(proto, TEXTURE0);
  JS_SET_GL_CONSTANT(proto, TEXTURE1);
  JS_SET_GL_CONSTANT(proto, TEXTURE2);
  JS_SET_GL_CONSTANT(proto, TEXTURE3);
  JS_SET_GL_CONSTANT(proto, TEXTURE4);
  JS_SET_GL_CONSTANT(proto, TEXTURE5);
  JS_SET_GL_CONSTANT(proto, TEXTURE6);
  JS_SET_GL_CONSTANT(proto, TEXTURE7);
  JS_SET_GL_CONSTANT(proto, TEXTURE8);
  JS_SET_GL_CONSTANT(proto, TEXTURE9);
  JS_SET_GL_CONSTANT(proto, TEXTURE10);
  JS_SET_GL_CONSTANT(proto, TEXTURE11);
  JS_SET_GL_CONSTANT(proto, TEXTURE12);
  JS_SET_GL_CONSTANT(proto, TEXTURE13);
  JS_SET_GL_CONSTANT(proto, TEXTURE14);
  JS_SET_GL_CONSTANT(proto, TEXTURE15);
  JS_SET_GL_CONSTANT(proto, TEXTURE16);
  JS_SET_GL_CONSTANT(proto, TEXTURE17);
  JS_SET_GL_CONSTANT(proto, TEXTURE18);
  JS_SET_GL_CONSTANT(proto, TEXTURE19);
  JS_SET_GL_CONSTANT(proto, TEXTURE20);
  JS_SET_GL_CONSTANT(proto, TEXTURE21);
  JS_SET_GL_CONSTANT(proto, TEXTURE22);
  JS_SET_GL_CONSTANT(proto, TEXTURE23);
  JS_SET_GL_CONSTANT(proto, TEXTURE24);
  JS_SET_GL_CONSTANT(proto, TEXTURE25);
  JS_SET_GL_CONSTANT(proto, TEXTURE26);
  JS_SET_GL_CONSTANT(proto, TEXTURE27);
  JS_SET_GL_CONSTANT(proto, TEXTURE28);
  JS_SET_GL_CONSTANT(proto, TEXTURE29);
  JS_SET_GL_CONSTANT(proto, TEXTURE30);
  JS_SET_GL_CONSTANT(proto, TEXTURE31);
  JS_SET_GL_CONSTANT(proto, ACTIVE_TEXTURE);

  /* TextureWrapMode */
  JS_SET_GL_CONSTANT(proto, REPEAT);
  JS_SET_GL_CONSTANT(proto, CLAMP_TO_EDGE);
  JS_SET_GL_CONSTANT(proto, MIRRORED_REPEAT);

  /* Uniform Types */
  JS_SET_GL_CONSTANT(proto, FLOAT_VEC2);
  JS_SET_GL_CONSTANT(proto, FLOAT_VEC3);
  JS_SET_GL_CONSTANT(proto, FLOAT_VEC4);
  JS_SET_GL_CONSTANT(proto, INT_VEC2);
  JS_SET_GL_CONSTANT(proto, INT_VEC3);
  JS_SET_GL_CONSTANT(proto, INT_VEC4);
  JS_SET_GL_CONSTANT(proto, BOOL);
  JS_SET_GL_CONSTANT(proto, BOOL_VEC2);
  JS_SET_GL_CONSTANT(proto, BOOL_VEC3);
  JS_SET_GL_CONSTANT(proto, BOOL_VEC4);
  JS_SET_GL_CONSTANT(proto, FLOAT_MAT2);
  JS_SET_GL_CONSTANT(proto, FLOAT_MAT3);
  JS_SET_GL_CONSTANT(proto, FLOAT_MAT4);
  JS_SET_GL_CONSTANT(proto, SAMPLER_2D);
  JS_SET_GL_CONSTANT(proto, SAMPLER_CUBE);

  /* Vertex Arrays */
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_ENABLED);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_SIZE);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_STRIDE);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_TYPE);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_NORMALIZED);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_POINTER);
  JS_SET_GL_CONSTANT(proto, VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);

  /* Read Format */
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, IMPLEMENTATION_COLOR_READ_TYPE);
  JS_SET_GL_CONSTANT(proto, IMPLEMENTATION_COLOR_READ_FORMAT);
#endif

  /* Shader Source */
  JS_SET_GL_CONSTANT(proto, COMPILE_STATUS);
  JS_SET_GL_CONSTANT(proto, INFO_LOG_LENGTH);
  JS_SET_GL_CONSTANT(proto, SHADER_SOURCE_LENGTH);
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, SHADER_COMPILER);
#endif

  /* Shader Binary */
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, SHADER_BINARY_FORMATS);
  JS_SET_GL_CONSTANT(proto, NUM_SHADER_BINARY_FORMATS);
#endif

  /* Shader Precision-Specified Types */
#ifndef __APPLE__
  JS_SET_GL_CONSTANT(proto, LOW_FLOAT);
  JS_SET_GL_CONSTANT(proto, MEDIUM_FLOAT);
  JS_SET_GL_CONSTANT(proto, HIGH_FLOAT);
  JS_SET_GL_CONSTANT(proto, LOW_INT);
  JS_SET_GL_CONSTANT(proto, MEDIUM_INT);
  JS_SET_GL_CONSTANT(proto, HIGH_INT);
#endif

  /* Framebuffer Object. */
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER);

  JS_SET_GL_CONSTANT(proto, RGBA4);
  JS_SET_GL_CONSTANT(proto, RGB5_A1);
#ifndef __APPLE__
  //JS_SET_GL_CONSTANT(proto, RGB565);
#endif
  JS_SET_GL_CONSTANT(proto, DEPTH_COMPONENT16);
  JS_SET_GL_CONSTANT(proto, STENCIL_INDEX);
  JS_SET_GL_CONSTANT(proto, STENCIL_INDEX8);

  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_WIDTH);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_HEIGHT);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_INTERNAL_FORMAT);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_RED_SIZE);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_GREEN_SIZE);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_BLUE_SIZE);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_ALPHA_SIZE);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_DEPTH_SIZE);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_STENCIL_SIZE);

  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);

  JS_SET_GL_CONSTANT(proto, COLOR_ATTACHMENT0);
  JS_SET_GL_CONSTANT(proto, DEPTH_ATTACHMENT);
  JS_SET_GL_CONSTANT(proto, STENCIL_ATTACHMENT);

  JS_SET_GL_CONSTANT(proto, NONE);

  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_COMPLETE);
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
#ifndef __APPLE__
  //JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
#endif
  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_UNSUPPORTED);

  JS_SET_GL_CONSTANT(proto, FRAMEBUFFER_BINDING);
  JS_SET_GL_CONSTANT(proto, RENDERBUFFER_BINDING);
  JS_SET_GL_CONSTANT(proto, MAX_RENDERBUFFER_SIZE);

  JS_SET_GL_CONSTANT(proto, INVALID_FRAMEBUFFER_OPERATION);

  // WebGL-specific enums
  JS_SET_NUM_CONSTANT(proto, "UNPACK_FLIP_Y_WEBGL", 0x9240);
  JS_SET_NUM_CONSTANT(proto, "UNPACK_PREMULTIPLY_ALPHA_WEBGL", 0x9241);
  JS_SET_NUM_CONSTANT(proto, "CONTEXT_LOST_WEBGL", 0x9242);
  JS_SET_NUM_CONSTANT(proto, "UNPACK_COLORSPACE_CONVERSION_WEBGL", 0x9243);
  JS_SET_NUM_CONSTANT(proto, "BROWSER_DEFAULT_WEBGL", 0x9244);

  // Export function
  target->Set(String::New("WebGLRenderingContext"), webgl_template->GetFunction());
}

} // end namespace HeadlessGL

NODE_MODULE(webgl, HeadlessGL::Init)
