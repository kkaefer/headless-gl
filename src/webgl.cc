#include <cstring>
#include <vector>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "webgl.h"
#include "macros.h"

#ifdef _WIN32
  #define  strcasestr(s, t) strstr(strupr(s), t)
#endif

#define JS_METHOD(name) v8::Handle<v8::Value> WebGL::name(const v8::Arguments& args)

#define JS_BOILERPLATE   \
  HandleScope scope; \
  if(args.This()->InternalFieldCount() <= 0) { \
    return ThrowError("Invalid WebGL Object"); \
  } \
  WebGL* inst = node::ObjectWrap::Unwrap<WebGL>(args.This()); \
  if(!(inst && inst->checkContext())) { \
    return ThrowError("Invalid GL context"); \
  }

v8::Handle<v8::Value> ThrowError(const char* msg) {
  return v8::ThrowException(v8::Exception::Error(v8::String::New(msg)));
}

using namespace node;
using namespace v8;
using namespace std;


vector<WebGL*> contexts;
WebGL*  active_context = NULL;


////////////////////////////////////////////////////////////////////////
// Context creation and object management
////////////////////////////////////////////////////////////////////////

WebGL::WebGL(int width, int height) :
  initialized(false),
  atExit(false) {
  
  #ifdef USE_AGL
  
    //Create AGL context
    GLint pixelAttr[] = {
      AGL_RGBA,
      AGL_DOUBLEBUFFER,
      AGL_PIXEL_SIZE, 32,
      AGL_ACCELERATED,
      AGL_NONE
    };
  
    AGLPixelFormat aglPixelFormat = aglChoosePixelFormat(NULL, 0, pixelAttr);
    if (aglPixelFormat == NULL) {
      fprintf(stderr, "Error pixel format\n");
      return;
    }

    gl_context = aglCreateContext(aglPixelFormat, NULL);
    aglDestroyPixelFormat(aglPixelFormat);
    if (gl_context == NULL) {
      fprintf(stderr, "Error creating GL context!\n");
      return;
    }
  
    if (!aglSetCurrentContext(gl_context)) {
      fprintf(stderr, "aglSetCurrentContext failed\n");
      return;
    }
    initialized = true;
  #endif

  #ifdef USE_GLX
    
    display = XOpenDisplay(0);

    static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, None };
    XVisualInfo *vi = glXChooseVisual(display, DefaultScreen(display),attributeList);
   
    //oldstyle context:
    gl_context = glXCreateContext(display, vi, 0, GL_TRUE);

    pixmap = XCreatePixmap (display, DefaultRootWindow(display), width, height, 24);
    glXPixmap = glXCreateGLXPixmap(display, vi, pixmap);

    if (!glXMakeCurrent(display, glXPixmap, gl_context)) {
      fprintf(stderr, "Failed to initialize GLX\n");
      return;
    }

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return;
    }

    initialized = true;
  #endif
  
  if (!initialized) {
    initialized = false;
    fprintf(stderr, "Unsupported system\n");
    return;
  }

  contexts.push_back(this);
}

WebGL::~WebGL() {
  dispose();
}

void WebGL::dispose() {
  if(!checkContext()) {
    return;
  }
  
  //Remove context from list
  for(vector<WebGL*>::iterator it=contexts.begin(); it!=contexts.end(); ++it) {
    if(*it == this) {
      contexts.erase(it);
      break;
    }
  }
  
  atExit=true;
  for(vector<GLObj*>::iterator it = globjs.begin(); it != globjs.end(); ++it) {
    GLObj *globj=*it;
    GLuint obj=globj->obj;

    switch(globj->type) {
    case GLOBJECT_TYPE_PROGRAM:
      glDeleteProgram(obj);
      break;
    case GLOBJECT_TYPE_BUFFER:
      glDeleteBuffers(1,&obj);
      break;
    case GLOBJECT_TYPE_FRAMEBUFFER:
      glDeleteFramebuffers(1,&obj);
      break;
    case GLOBJECT_TYPE_RENDERBUFFER:
      glDeleteRenderbuffers(1,&obj);
      break;
    case GLOBJECT_TYPE_SHADER:
      glDeleteShader(obj);
      break;
    case GLOBJECT_TYPE_TEXTURE:
      glDeleteTextures(1,&obj);
      break;
    default:
      break;
    }
    delete globj;
  }

  globjs.clear();
  
  
  //Destroy context
  #ifdef USE_AGL
    aglDestroyContext(gl_context);
  #endif
  
  #ifdef USE_GLX
    glXDestroyContext(display, gl_context);
    glXDestroyPixmap (display, glXPixmap);
    XFreePixmap (display, pixmap);
  #endif
}

bool WebGL::checkContext() {

  if(!initialized || atExit) {
    return false;
  }
  if(this == active_context) {
    return true;
  }
  active_context = this;

  #ifdef USE_AGL
    return aglSetCurrentContext(gl_context);
  #endif
  
  #ifdef USE_GLX
    return true;
  #endif
  return false;
}

void WebGL::registerGLObj(GLObjectType type, GLuint obj) {
  globjs.push_back(new GLObj(type,obj));
}

void WebGL::unregisterGLObj(GLuint obj) {
  if(atExit) return;

  vector<GLObj*>::iterator it = globjs.begin();
  while(globjs.size() && it != globjs.end()) {
    GLObj *globj=*it;
    if(globj->obj==obj) {
      delete globj;
      globjs.erase(it);
      break;
    }
    ++it;
  }
}

void WebGL::disposeAll() {
  for(int i=contexts.size()-1; i>=0; --i) {
    contexts[i]->dispose();
  }
}

Handle<Value> WebGL::New(const Arguments& args) {
  HandleScope scope;
  
  WebGL* instance = new WebGL(args[0]->Int32Value(), args[1]->Int32Value());
  if(!instance->initialized) {
    return ThrowError("Error creating WebGLRenderingContext");
  }
  
  instance->Wrap(args.This());
  return args.This();
}


////////////////////////////////////////////////////////////////////////
// Helper methods
////////////////////////////////////////////////////////////////////////


// A 32-bit and 64-bit compatible way of converting a pointer to a GLuint.
static GLuint ToGLuint(const void* ptr) {
  return static_cast<GLuint>(reinterpret_cast<size_t>(ptr));
}

static int SizeOfArrayElementForType(v8::ExternalArrayType type) {
  switch (type) {
  case v8::kExternalByteArray:
  case v8::kExternalUnsignedByteArray:
    return 1;
  case v8::kExternalShortArray:
  case v8::kExternalUnsignedShortArray:
    return 2;
  case v8::kExternalIntArray:
  case v8::kExternalUnsignedIntArray:
  case v8::kExternalFloatArray:
    return 4;
  default:
    return 0;
  }
}

inline void *getImageData(Local<Value> arg) {
  void *pixels = NULL;
  if (!arg->IsNull()) {
    Local<Object> obj = Local<Object>::Cast(arg);
    if (!obj->IsObject())
      return NULL;

    pixels = obj->GetIndexedPropertiesExternalArrayData();
  }
  return pixels;
}

template<typename Type>
inline Type* getArrayData(Local<Value> arg, int* num = NULL) {
  Type *data=NULL;
  if(num) *num=0;

  if(!arg->IsNull()) {
    if(arg->IsArray()) {
      Local<Array> arr = Array::Cast(*arg);
      if(num) *num=arr->Length();
      data = reinterpret_cast<Type*>(arr->GetIndexedPropertiesExternalArrayData());
    } else if(arg->IsObject()) {
      if(num) *num = arg->ToObject()->GetIndexedPropertiesExternalArrayDataLength();
      data = reinterpret_cast<Type*>(arg->ToObject()->GetIndexedPropertiesExternalArrayData());
    } else {
      return NULL;
    }
  }

  return data;
}



////////////////////////////////////////////////////////////////////////
// WebGL Wrappers
////////////////////////////////////////////////////////////////////////


JS_METHOD(Destroy) {
  JS_BOILERPLATE
  
  inst->dispose();
  return scope.Close(Undefined());
}

JS_METHOD(Uniform1f) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();

  glUniform1f(location, x);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform2f) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();

  glUniform2f(location, x, y);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform3f) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();
  float z = (float) args[3]->NumberValue();

  glUniform3f(location, x, y, z);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform4f) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();
  float z = (float) args[3]->NumberValue();
  float w = (float) args[4]->NumberValue();

  glUniform4f(location, x, y, z, w);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform1i) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int x = args[1]->Int32Value();

  glUniform1i(location, x);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform2i) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int x = args[1]->Int32Value();
  int y = args[2]->Int32Value();

  glUniform2i(location, x, y);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform3i) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int x = args[1]->Int32Value();
  int y = args[2]->Int32Value();
  int z = args[3]->Int32Value();

  glUniform3i(location, x, y, z);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform4i) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int x = args[1]->Int32Value();
  int y = args[2]->Int32Value();
  int z = args[3]->Int32Value();
  int w = args[4]->Int32Value();

  glUniform4i(location, x, y, z, w);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform1fv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int num=0;
  GLfloat *ptr=getArrayData<GLfloat>(args[1],&num);
  glUniform1fv(location, num, ptr);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform2fv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();

  if (args[1]->IsArray()) {
    Local<Array> arr =  Local<Array>::Cast(args[1]);
    if (arr->Length() >= 2) {
      double a = arr->Get(0)->NumberValue();
      double b = arr->Get(1)->NumberValue();
      glUniform2f(location, a, b);
    } else {
      // TODO: handle
    }
  } else {
    // TODO: handle
  }

  return scope.Close(Undefined());
}

JS_METHOD(Uniform3fv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  if (args[1]->IsArray()) {
    Local<Array> arr =  Local<Array>::Cast(args[1]);
    if (arr->Length() >= 3) {
      double a = arr->Get(0)->NumberValue();
      double b = arr->Get(1)->NumberValue();
      double c = arr->Get(2)->NumberValue();
      glUniform3f(location, a, b, c);
    } else {
      // TODO: handle
    }
  } else {
    // TODO: handle
  }

  return scope.Close(Undefined());
}

JS_METHOD(Uniform4fv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();

  if (args[1]->IsArray()) {
    Local<Array> arr =  Local<Array>::Cast(args[1]);
    if (arr->Length() >= 4) {
      double a = arr->Get(0)->NumberValue();
      double b = arr->Get(1)->NumberValue();
      double c = arr->Get(2)->NumberValue();
      double d = arr->Get(3)->NumberValue();
      glUniform4f(location, a, b, c, d);
    } else {
      // TODO: handle
    }
  } else {
    // TODO: handle
  }

  return scope.Close(Undefined());
}

JS_METHOD(Uniform1iv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int num=0;
  GLint *ptr=getArrayData<GLint>(args[1],&num);

  glUniform1iv(location, num, ptr);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform2iv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int num=0;
  GLint *ptr=getArrayData<GLint>(args[1],&num);
  num /= 2;

  glUniform2iv(location, num, ptr);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform3iv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int num=0;
  GLint *ptr=getArrayData<GLint>(args[1],&num);
  num /= 3;
  glUniform3iv(location, num, ptr);
  return scope.Close(Undefined());
}

JS_METHOD(Uniform4iv) {
  JS_BOILERPLATE

  int location = args[0]->Int32Value();
  int num=0;
  GLint *ptr=getArrayData<GLint>(args[1],&num);
  num /= 4;
  glUniform4iv(location, num, ptr);
  return scope.Close(Undefined());
}

JS_METHOD(PixelStorei) {
  JS_BOILERPLATE

  int pname = args[0]->Int32Value();
  int param = args[1]->Int32Value();

  glPixelStorei(pname,param);

  return scope.Close(Undefined());
}

JS_METHOD(BindAttribLocation) {
  JS_BOILERPLATE

  int program = args[0]->Int32Value();
  int index = args[1]->Int32Value();
  String::Utf8Value name(args[2]);

  glBindAttribLocation(program, index, *name);

  return scope.Close(Undefined());
}


JS_METHOD(GetError) {
  JS_BOILERPLATE

  return scope.Close(Number::New(glGetError()));
}


JS_METHOD(DrawArrays) {
  JS_BOILERPLATE

  int mode = args[0]->Int32Value();
  int first = args[1]->Int32Value();
  int count = args[2]->Int32Value();

  glDrawArrays(mode, first, count);

  return scope.Close(Undefined());
}

JS_METHOD(UniformMatrix2fv) {
  JS_BOILERPLATE

  GLint location = args[0]->Int32Value();
  GLboolean transpose = args[1]->BooleanValue();

  GLsizei count=0;
  GLfloat* data=getArrayData<GLfloat>(args[2],&count);

  if (count < 4) {
    return ThrowError("Not enough data for UniformMatrix2fv");
  }

  glUniformMatrix2fv(location, count / 4, transpose, data);

  return scope.Close(Undefined());
}

JS_METHOD(UniformMatrix3fv) {
  JS_BOILERPLATE

  GLint location = args[0]->Int32Value();
  GLboolean transpose = args[1]->BooleanValue();
  GLsizei count=0;
  GLfloat* data=getArrayData<GLfloat>(args[2],&count);

  if (count < 9) {
    return ThrowError("Not enough data for UniformMatrix3fv");
  }

  glUniformMatrix3fv(location, count / 9, transpose, data);

  return scope.Close(Undefined());
}

JS_METHOD(UniformMatrix4fv) {
  JS_BOILERPLATE

  GLint location = args[0]->Int32Value();
  GLboolean transpose = args[1]->BooleanValue();
  GLsizei count=0;
  GLfloat* data=getArrayData<GLfloat>(args[2],&count);

  if (count < 16) {
    return ThrowError("Not enough data for UniformMatrix4fv");
  }

  glUniformMatrix4fv(location, count / 16, transpose, data);

  return scope.Close(Undefined());
}

JS_METHOD(GenerateMipmap) {
  JS_BOILERPLATE

  GLint target = args[0]->Int32Value();
  glGenerateMipmap(target);

  return scope.Close(Undefined());
}

JS_METHOD(GetAttribLocation) {
  JS_BOILERPLATE

  int program = args[0]->Int32Value();
  String::Utf8Value name(args[1]);

  return scope.Close(Number::New(glGetAttribLocation(program, *name)));
}


JS_METHOD(DepthFunc) {
  JS_BOILERPLATE

  glDepthFunc(args[0]->Int32Value());

  return scope.Close(Undefined());
}


JS_METHOD(Viewport) {
  JS_BOILERPLATE

  int x = args[0]->Int32Value();
  int y = args[1]->Int32Value();
  int width = args[2]->Int32Value();
  int height = args[3]->Int32Value();

  glViewport(x, y, width, height);

  return scope.Close(Undefined());
}

JS_METHOD(CreateShader) {
  JS_BOILERPLATE

  GLuint shader=glCreateShader(args[0]->Int32Value());
  inst->registerGLObj(GLOBJECT_TYPE_SHADER, shader);
  return scope.Close(Number::New(shader));
}


JS_METHOD(ShaderSource) {
  JS_BOILERPLATE

  int id = args[0]->Int32Value();
  String::Utf8Value code(args[1]);

  const char* codes[1];
  codes[0] = *code;
  GLint length=code.length();

  glShaderSource  (id, 1, codes, &length);

  return scope.Close(Undefined());
}


JS_METHOD(CompileShader) {
  JS_BOILERPLATE

  glCompileShader(args[0]->Int32Value());

  return scope.Close(Undefined());
}

JS_METHOD(FrontFace) {
  JS_BOILERPLATE

  glFrontFace(args[0]->Int32Value());

  return scope.Close(Undefined());
}


JS_METHOD(GetShaderParameter) {
  JS_BOILERPLATE

  int shader = args[0]->Int32Value();
  int pname = args[1]->Int32Value();
  int value = 0;
  switch (pname) {
  case GL_DELETE_STATUS:
  case GL_COMPILE_STATUS:
    glGetShaderiv(shader, pname, &value);
    return scope.Close(JS_BOOL(static_cast<bool>(value!=0)));
  case GL_SHADER_TYPE:
    glGetShaderiv(shader, pname, &value);
    return scope.Close(JS_INT(static_cast<unsigned long>(value)));
  case GL_INFO_LOG_LENGTH:
  case GL_SHADER_SOURCE_LENGTH:
    glGetShaderiv(shader, pname, &value);
    return scope.Close(JS_INT(static_cast<long>(value)));
  default:
    return ThrowException(Exception::TypeError(String::New("GetShaderParameter: Invalid Enum")));
  }
}

JS_METHOD(GetShaderInfoLog) {
  JS_BOILERPLATE

  int id = args[0]->Int32Value();
  int Len = 1024;
  char Error[1024];
  glGetShaderInfoLog(id, 1024, &Len, Error);

  return scope.Close(String::New(Error));
}


JS_METHOD(CreateProgram) {
  JS_BOILERPLATE

  GLuint program=glCreateProgram();
  inst->registerGLObj(GLOBJECT_TYPE_PROGRAM, program);
  return scope.Close(Number::New(program));
}


JS_METHOD(AttachShader) {
  JS_BOILERPLATE

  int program = args[0]->Int32Value();
  int shader = args[1]->Int32Value();

  glAttachShader(program, shader);

  return scope.Close(Undefined());
}


JS_METHOD(LinkProgram) {
  JS_BOILERPLATE

  glLinkProgram(args[0]->Int32Value());

  return scope.Close(Undefined());
}


JS_METHOD(GetProgramParameter) {
  JS_BOILERPLATE

  int program = args[0]->Int32Value();
  int pname = args[1]->Int32Value();

  int value = 0;
  switch (pname) {
  case GL_DELETE_STATUS:
  case GL_LINK_STATUS:
  case GL_VALIDATE_STATUS:
    glGetProgramiv(program, pname, &value);
    return scope.Close(JS_BOOL(static_cast<bool>(value!=0)));
  case GL_ATTACHED_SHADERS:
  case GL_ACTIVE_ATTRIBUTES:
  case GL_ACTIVE_UNIFORMS:
    glGetProgramiv(program, pname, &value);
    return scope.Close(JS_INT(static_cast<long>(value)));
  default:
    return ThrowException(Exception::TypeError(String::New("GetProgramParameter: Invalid Enum")));
  }
}


JS_METHOD(GetUniformLocation) {
  JS_BOILERPLATE

  int program = args[0]->Int32Value();
  String::AsciiValue name(args[1]);

  return scope.Close(JS_INT(glGetUniformLocation(program, *name)));
}


JS_METHOD(ClearColor) {
  JS_BOILERPLATE

  float red = (float) args[0]->NumberValue();
  float green = (float) args[1]->NumberValue();
  float blue = (float) args[2]->NumberValue();
  float alpha = (float) args[3]->NumberValue();

  glClearColor(red, green, blue, alpha);

  return scope.Close(Undefined());
}


JS_METHOD(ClearDepth) {
  JS_BOILERPLATE

  float depth = (float) args[0]->NumberValue();

  glClearDepthf(depth);

  return scope.Close(Undefined());
}

JS_METHOD(Disable) {
  JS_BOILERPLATE

  glDisable(args[0]->Int32Value());
  return scope.Close(Undefined());
}

JS_METHOD(Enable) {
  JS_BOILERPLATE

  glEnable(args[0]->Int32Value());
  return scope.Close(Undefined());
}


JS_METHOD(CreateTexture) {
  JS_BOILERPLATE

  GLuint texture;
  glGenTextures(1, &texture);
  inst->registerGLObj(GLOBJECT_TYPE_TEXTURE, texture);
  return scope.Close(Number::New(texture));
}


JS_METHOD(BindTexture) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int texture = args[1]->IsNull() ? 0 : args[1]->Int32Value();

  glBindTexture(target, texture);
  return scope.Close(Undefined());
}


JS_METHOD(TexImage2D) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int level = args[1]->Int32Value();
  int internalformat = args[2]->Int32Value();
  int width = args[3]->Int32Value();
  int height = args[4]->Int32Value();
  int border = args[5]->Int32Value();
  int format = args[6]->Int32Value();
  int type = args[7]->Int32Value();
  void *pixels=getImageData(args[8]);

  glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

  return scope.Close(Undefined());
}


JS_METHOD(TexParameteri) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int pname = args[1]->Int32Value();
  int param = args[2]->Int32Value();

  glTexParameteri(target, pname, param);

  return scope.Close(Undefined());
}

JS_METHOD(TexParameterf) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int pname = args[1]->Int32Value();
  float param = (float) args[2]->NumberValue();

  glTexParameterf(target, pname, param);

  return scope.Close(Undefined());
}


JS_METHOD(Clear) {
  JS_BOILERPLATE

  glClear(args[0]->Int32Value());

  return scope.Close(Undefined());
}


JS_METHOD(UseProgram) {
  JS_BOILERPLATE

  glUseProgram(args[0]->Int32Value());

  return scope.Close(Undefined());
}

JS_METHOD(CreateBuffer) {
  JS_BOILERPLATE

  GLuint buffer;
  glGenBuffers(1, &buffer);
  inst->registerGLObj(GLOBJECT_TYPE_BUFFER, buffer);
  return scope.Close(Number::New(buffer));
}

JS_METHOD(BindBuffer) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int buffer = args[1]->Uint32Value();
  glBindBuffer(target,buffer);

  return scope.Close(Undefined());
}


JS_METHOD(CreateFramebuffer) {
  JS_BOILERPLATE

  GLuint buffer;
  glGenFramebuffers(1, &buffer);
  inst->registerGLObj(GLOBJECT_TYPE_FRAMEBUFFER, buffer);
  return scope.Close(Number::New(buffer));
}


JS_METHOD(BindFramebuffer) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int buffer = args[1]->IsNull() ? 0 : args[1]->Int32Value();

  glBindFramebuffer(target, buffer);

  return scope.Close(Undefined());
}


JS_METHOD(FramebufferTexture2D) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int attachment = args[1]->Int32Value();
  int textarget = args[2]->Int32Value();
  int texture = args[3]->Int32Value();
  int level = args[4]->Int32Value();

  glFramebufferTexture2D(target, attachment, textarget, texture, level);

  return scope.Close(Undefined());
}


JS_METHOD(BufferData) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  if(args[1]->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(args[1]);
    GLenum usage = args[2]->Int32Value();

    int element_size = SizeOfArrayElementForType(obj->GetIndexedPropertiesExternalArrayDataType());
    GLsizeiptr size = obj->GetIndexedPropertiesExternalArrayDataLength() * element_size;
    void* data = obj->GetIndexedPropertiesExternalArrayData();
    glBufferData(target, size, data, usage);
  }
  else if(args[1]->IsNumber()) {
    GLsizeiptr size = args[1]->Uint32Value();
    GLenum usage = args[2]->Int32Value();
    glBufferData(target, size, NULL, usage);
  }
  return scope.Close(Undefined());
}


JS_METHOD(BufferSubData) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int offset = args[1]->Int32Value();
  Local<Object> obj = Local<Object>::Cast(args[2]);

  int element_size = SizeOfArrayElementForType( obj->GetIndexedPropertiesExternalArrayDataType());
  int size = obj->GetIndexedPropertiesExternalArrayDataLength() * element_size;
  void* data = obj->GetIndexedPropertiesExternalArrayData();

  glBufferSubData(target, offset, size, data);

  return scope.Close(Undefined());
}


JS_METHOD(BlendEquation) {
  JS_BOILERPLATE

  int mode=args[0]->Int32Value();;

  glBlendEquation(mode);

  return scope.Close(Undefined());
}


JS_METHOD(BlendFunc) {
  JS_BOILERPLATE

  int sfactor=args[0]->Int32Value();;
  int dfactor=args[1]->Int32Value();;

  glBlendFunc(sfactor,dfactor);

  return scope.Close(Undefined());
}


JS_METHOD(EnableVertexAttribArray) {
  JS_BOILERPLATE

  glEnableVertexAttribArray(args[0]->Int32Value());

  return scope.Close(Undefined());
}


JS_METHOD(VertexAttribPointer) {
  JS_BOILERPLATE

  int indx = args[0]->Int32Value();
  int size = args[1]->Int32Value();
  int type = args[2]->Int32Value();
  int normalized = args[3]->BooleanValue();
  int stride = args[4]->Int32Value();
  int offset = args[5]->Int32Value();

  //    printf("VertexAttribPointer %d %d %d %d %d %d\n", indx, size, type, normalized, stride, offset);
  glVertexAttribPointer(indx, size, type, normalized, stride, (const GLvoid *)offset);

  return scope.Close(Undefined());
}


JS_METHOD(ActiveTexture) {
  JS_BOILERPLATE

  glActiveTexture(args[0]->Int32Value());
  return scope.Close(Undefined());
}


JS_METHOD(DrawElements) {
  JS_BOILERPLATE

  int mode = args[0]->Int32Value();
  int count = args[1]->Int32Value();
  int type = args[2]->Int32Value();
  GLvoid *offset = reinterpret_cast<GLvoid*>(args[3]->Uint32Value());
  glDrawElements(mode, count, type, offset);
  return scope.Close(Undefined());
}


JS_METHOD(Flush) {
  JS_BOILERPLATE
  glFlush();
  return scope.Close(Undefined());
}

JS_METHOD(Finish) {
  JS_BOILERPLATE
  glFinish();
  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib1f) {
  JS_BOILERPLATE

  GLuint indx = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();

  glVertexAttrib1f(indx, x);
  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib2f) {
  JS_BOILERPLATE

  GLuint indx = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();

  glVertexAttrib2f(indx, x, y);
  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib3f) {
  JS_BOILERPLATE

  GLuint indx = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();
  float z = (float) args[3]->NumberValue();

  glVertexAttrib3f(indx, x, y, z);
  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib4f) {
  JS_BOILERPLATE

  GLuint indx = args[0]->Int32Value();
  float x = (float) args[1]->NumberValue();
  float y = (float) args[2]->NumberValue();
  float z = (float) args[3]->NumberValue();
  float w = (float) args[4]->NumberValue();

  glVertexAttrib4f(indx, x, y, z, w);
  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib1fv) {
  JS_BOILERPLATE

  int indx = args[0]->Int32Value();
  GLfloat *data = getArrayData<GLfloat>(args[1]);
  glVertexAttrib1fv(indx, data);

  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib2fv) {
  JS_BOILERPLATE

  int indx = args[0]->Int32Value();
  GLfloat *data = getArrayData<GLfloat>(args[1]);
  glVertexAttrib2fv(indx, data);

  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib3fv) {
  JS_BOILERPLATE

  int indx = args[0]->Int32Value();
  GLfloat *data = getArrayData<GLfloat>(args[1]);
  glVertexAttrib3fv(indx, data);

  return scope.Close(Undefined());
}

JS_METHOD(VertexAttrib4fv) {
  JS_BOILERPLATE

  int indx = args[0]->Int32Value();
  GLfloat *data = getArrayData<GLfloat>(args[1]);
  glVertexAttrib4fv(indx, data);

  return scope.Close(Undefined());
}

JS_METHOD(BlendColor) {
  JS_BOILERPLATE

  GLclampf r= (float) args[0]->NumberValue();
  GLclampf g= (float) args[1]->NumberValue();
  GLclampf b= (float) args[2]->NumberValue();
  GLclampf a= (float) args[3]->NumberValue();

  glBlendColor(r,g,b,a);
  return scope.Close(Undefined());
}

JS_METHOD(BlendEquationSeparate) {
  JS_BOILERPLATE

  GLenum modeRGB= args[0]->Int32Value();
  GLenum modeAlpha= args[1]->Int32Value();

  glBlendEquationSeparate(modeRGB,modeAlpha);
  return scope.Close(Undefined());
}

JS_METHOD(BlendFuncSeparate) {
  JS_BOILERPLATE

  GLenum srcRGB= args[0]->Int32Value();
  GLenum dstRGB= args[1]->Int32Value();
  GLenum srcAlpha= args[2]->Int32Value();
  GLenum dstAlpha= args[3]->Int32Value();

  glBlendFuncSeparate(srcRGB,dstRGB,srcAlpha,dstAlpha);
  return scope.Close(Undefined());
}

JS_METHOD(ClearStencil) {
  JS_BOILERPLATE

  GLint s = args[0]->Int32Value();

  glClearStencil(s);
  return scope.Close(Undefined());
}

JS_METHOD(ColorMask) {
  JS_BOILERPLATE

  GLboolean r = args[0]->BooleanValue();
  GLboolean g = args[1]->BooleanValue();
  GLboolean b = args[2]->BooleanValue();
  GLboolean a = args[3]->BooleanValue();

  glColorMask(r,g,b,a);
  return scope.Close(Undefined());
}

JS_METHOD(CopyTexImage2D) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLint level = args[1]->Int32Value();
  GLenum internalformat = args[2]->Int32Value();
  GLint x = args[3]->Int32Value();
  GLint y = args[4]->Int32Value();
  GLsizei width = args[5]->Int32Value();
  GLsizei height = args[6]->Int32Value();
  GLint border = args[7]->Int32Value();

  glCopyTexImage2D( target, level, internalformat, x, y, width, height, border);
  return scope.Close(Undefined());
}

JS_METHOD(CopyTexSubImage2D) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLint level = args[1]->Int32Value();
  GLint xoffset = args[2]->Int32Value();
  GLint yoffset = args[3]->Int32Value();
  GLint x = args[4]->Int32Value();
  GLint y = args[5]->Int32Value();
  GLsizei width = args[6]->Int32Value();
  GLsizei height = args[7]->Int32Value();

  glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height);
  return scope.Close(Undefined());
}

JS_METHOD(CullFace) {
  JS_BOILERPLATE

  GLenum mode = args[0]->Int32Value();

  glCullFace(mode);
  return scope.Close(Undefined());
}

JS_METHOD(DepthMask) {
  JS_BOILERPLATE

  GLboolean flag = args[0]->BooleanValue();

  glDepthMask(flag);
  return scope.Close(Undefined());
}

JS_METHOD(DepthRange) {
  JS_BOILERPLATE

  GLclampf zNear = (float) args[0]->NumberValue();
  GLclampf zFar = (float) args[1]->NumberValue();

  glDepthRangef(zNear, zFar);
  return scope.Close(Undefined());
}

JS_METHOD(DisableVertexAttribArray) {
  JS_BOILERPLATE

  GLuint index = args[0]->Int32Value();

  glDisableVertexAttribArray(index);
  return scope.Close(Undefined());
}

JS_METHOD(Hint) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum mode = args[1]->Int32Value();

  glHint(target, mode);
  return scope.Close(Undefined());
}

JS_METHOD(IsEnabled) {
  JS_BOILERPLATE

  GLenum cap = args[0]->Int32Value();

  bool ret=glIsEnabled(cap)!=0;
  return scope.Close(v8::Boolean::New(ret));
}

JS_METHOD(LineWidth) {
  JS_BOILERPLATE

  GLfloat width = (float) args[0]->NumberValue();

  glLineWidth(width);
  return scope.Close(Undefined());
}

JS_METHOD(PolygonOffset) {
  JS_BOILERPLATE

  GLfloat factor = (float) args[0]->NumberValue();
  GLfloat units = (float) args[1]->NumberValue();

  glPolygonOffset(factor, units);
  return scope.Close(Undefined());
}

JS_METHOD(SampleCoverage) {
  JS_BOILERPLATE

  GLclampf value = (float) args[0]->NumberValue();
  GLboolean invert = args[1]->BooleanValue();

  glSampleCoverage(value, invert);
  return scope.Close(Undefined());
}

JS_METHOD(Scissor) {
  JS_BOILERPLATE

  GLint x = args[0]->Int32Value();
  GLint y = args[1]->Int32Value();
  GLsizei width = args[2]->Int32Value();
  GLsizei height = args[3]->Int32Value();

  glScissor(x, y, width, height);
  return scope.Close(Undefined());
}

JS_METHOD(StencilFunc) {
  JS_BOILERPLATE

  GLenum func = args[0]->Int32Value();
  GLint ref = args[1]->Int32Value();
  GLuint mask = args[2]->Int32Value();

  glStencilFunc(func, ref, mask);
  return scope.Close(Undefined());
}

JS_METHOD(StencilFuncSeparate) {
  JS_BOILERPLATE

  GLenum face = args[0]->Int32Value();
  GLenum func = args[1]->Int32Value();
  GLint ref = args[2]->Int32Value();
  GLuint mask = args[3]->Int32Value();

  glStencilFuncSeparate(face, func, ref, mask);
  return scope.Close(Undefined());
}

JS_METHOD(StencilMask) {
  JS_BOILERPLATE

  GLuint mask = args[0]->Uint32Value();

  glStencilMask(mask);
  return scope.Close(Undefined());
}

JS_METHOD(StencilMaskSeparate) {
  JS_BOILERPLATE

  GLenum face = args[0]->Int32Value();
  GLuint mask = args[1]->Uint32Value();

  glStencilMaskSeparate(face, mask);
  return scope.Close(Undefined());
}

JS_METHOD(StencilOp) {
  JS_BOILERPLATE

  GLenum fail = args[0]->Int32Value();
  GLenum zfail = args[1]->Int32Value();
  GLenum zpass = args[2]->Int32Value();

  glStencilOp(fail, zfail, zpass);
  return scope.Close(Undefined());
}

JS_METHOD(StencilOpSeparate) {
  JS_BOILERPLATE

  GLenum face = args[0]->Int32Value();
  GLenum fail = args[1]->Int32Value();
  GLenum zfail = args[2]->Int32Value();
  GLenum zpass = args[3]->Int32Value();

  glStencilOpSeparate(face, fail, zfail, zpass);
  return scope.Close(Undefined());
}

JS_METHOD(BindRenderbuffer) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLuint buffer = args[1]->IsNull() ? 0 : args[1]->Int32Value();

  glBindRenderbuffer(target, buffer);

  return scope.Close(Undefined());
}

JS_METHOD(CreateRenderbuffer) {
  JS_BOILERPLATE

  GLuint renderbuffers;
  glGenRenderbuffers(1,&renderbuffers);
  inst->registerGLObj(GLOBJECT_TYPE_RENDERBUFFER, renderbuffers);
  return scope.Close(Number::New(renderbuffers));
}

JS_METHOD(DeleteBuffer) {
  JS_BOILERPLATE

  GLuint buffer = args[0]->Uint32Value();

  glDeleteBuffers(1,&buffer);
  return scope.Close(Undefined());
}

JS_METHOD(DeleteFramebuffer) {
  JS_BOILERPLATE

  GLuint buffer = args[0]->Uint32Value();

  glDeleteFramebuffers(1,&buffer);
  return scope.Close(Undefined());
}

JS_METHOD(DeleteProgram) {
  JS_BOILERPLATE

  GLuint program = args[0]->Uint32Value();

  glDeleteProgram(program);
  return scope.Close(Undefined());
}

JS_METHOD(DeleteRenderbuffer) {
  JS_BOILERPLATE

  GLuint renderbuffer = args[0]->Uint32Value();

  glDeleteRenderbuffers(1, &renderbuffer);
  return scope.Close(Undefined());
}

JS_METHOD(DeleteShader) {
  JS_BOILERPLATE

  GLuint shader = args[0]->Uint32Value();

  glDeleteShader(shader);
  return scope.Close(Undefined());
}

JS_METHOD(DeleteTexture) {
  JS_BOILERPLATE

  GLuint texture = args[0]->Uint32Value();

  glDeleteTextures(1,&texture);
  return scope.Close(Undefined());
}

JS_METHOD(DetachShader) {
  JS_BOILERPLATE

  GLuint program = args[0]->Uint32Value();
  GLuint shader = args[1]->Uint32Value();

  glDetachShader(program, shader);
  return scope.Close(Undefined());
}

JS_METHOD(FramebufferRenderbuffer) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum attachment = args[1]->Int32Value();
  GLenum renderbuffertarget = args[2]->Int32Value();
  GLuint renderbuffer = args[3]->Uint32Value();

  glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  return scope.Close(Undefined());
}

JS_METHOD(GetVertexAttribOffset) {
  JS_BOILERPLATE

  GLuint index = args[0]->Uint32Value();
  GLenum pname = args[1]->Int32Value();
  void *ret=NULL;

  glGetVertexAttribPointerv(index, pname, &ret);
  return scope.Close(JS_INT(ToGLuint(ret)));
}

JS_METHOD(IsBuffer) {
  JS_BOILERPLATE

  return scope.Close(v8::Boolean::New(glIsBuffer(args[0]->Uint32Value())!=0));
}

JS_METHOD(IsFramebuffer) {
  JS_BOILERPLATE

  return scope.Close(JS_BOOL(glIsFramebuffer(args[0]->Uint32Value())!=0));
}

JS_METHOD(IsProgram) {
  JS_BOILERPLATE

  return scope.Close(JS_BOOL(glIsProgram(args[0]->Uint32Value())!=0));
}

JS_METHOD(IsRenderbuffer) {
  JS_BOILERPLATE

  return scope.Close(JS_BOOL(glIsRenderbuffer( args[0]->Uint32Value())!=0));
}

JS_METHOD(IsShader) {
  JS_BOILERPLATE

  return scope.Close(JS_BOOL(glIsShader(args[0]->Uint32Value())!=0));
}

JS_METHOD(IsTexture) {
  JS_BOILERPLATE

  return scope.Close(JS_BOOL(glIsTexture(args[0]->Uint32Value())!=0));
}

JS_METHOD(RenderbufferStorage) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum internalformat = args[1]->Int32Value();
  GLsizei width = args[2]->Uint32Value();
  GLsizei height = args[3]->Uint32Value();

  glRenderbufferStorage(target, internalformat, width, height);
  return scope.Close(Undefined());
}

JS_METHOD(GetShaderSource) {
  JS_BOILERPLATE

  int shader = args[0]->Int32Value();

  GLint len;
  glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
  GLchar *source=new GLchar[len];
  glGetShaderSource(shader, len, NULL, source);

  Local<String> str=String::New(source);
  delete source;

  return str;
}

JS_METHOD(ValidateProgram) {
  JS_BOILERPLATE

  glValidateProgram(args[0]->Int32Value());

  return scope.Close(Undefined());
}

JS_METHOD(TexSubImage2D) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLint level = args[1]->Int32Value();
  GLint xoffset = args[2]->Int32Value();
  GLint yoffset = args[3]->Int32Value();
  GLsizei width = args[4]->Int32Value();
  GLsizei height = args[5]->Int32Value();
  GLenum format = args[6]->Int32Value();
  GLenum type = args[7]->Int32Value();
  void *pixels=getImageData(args[8]);

  glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);

  return scope.Close(Undefined());
}

JS_METHOD(ReadPixels) {
  JS_BOILERPLATE

  GLint x = args[0]->Int32Value();
  GLint y = args[1]->Int32Value();
  GLsizei width = args[2]->Int32Value();
  GLsizei height = args[3]->Int32Value();
  GLenum format = args[4]->Int32Value();
  GLenum type = args[5]->Int32Value();
  void *pixels=getImageData(args[6]);

  glReadPixels(x, y, width, height, format, type, pixels);

  return scope.Close(Undefined());
}

JS_METHOD(GetTexParameter) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum pname = args[1]->Int32Value();

  GLint param_value=0;
  glGetTexParameteriv(target, pname, &param_value);

  return scope.Close(Number::New(param_value));
}

JS_METHOD(GetActiveAttrib) {
  JS_BOILERPLATE

  GLuint program = args[0]->Int32Value();
  GLuint index = args[1]->Int32Value();

  char name[1024];
  GLsizei length=0;
  GLenum type;
  GLsizei size;
  glGetActiveAttrib(program, index, 1024, &length, &size, &type, name);

  Local<Array> activeInfo = Array::New(3);
  activeInfo->Set(JS_STR("size"), JS_INT(size));
  activeInfo->Set(JS_STR("type"), JS_INT((int)type));
  activeInfo->Set(JS_STR("name"), JS_STR(name));

  return scope.Close(activeInfo);
}

JS_METHOD(GetActiveUniform) {
  JS_BOILERPLATE

  GLuint program = args[0]->Int32Value();
  GLuint index = args[1]->Int32Value();

  char name[1024];
  GLsizei length=0;
  GLenum type;
  GLsizei size;
  glGetActiveUniform(program, index, 1024, &length, &size, &type, name);

  Local<Array> activeInfo = Array::New(3);
  activeInfo->Set(JS_STR("size"), JS_INT(size));
  activeInfo->Set(JS_STR("type"), JS_INT((int)type));
  activeInfo->Set(JS_STR("name"), JS_STR(name));

  return scope.Close(activeInfo);
}

JS_METHOD(GetAttachedShaders) {
  JS_BOILERPLATE

  GLuint program = args[0]->Int32Value();

  GLuint shaders[1024];
  GLsizei count;
  glGetAttachedShaders(program, 1024, &count, shaders);

  Local<Array> shadersArr = Array::New(count);
  for(int i=0;i<count;i++)
    shadersArr->Set(i, JS_INT((int)shaders[i]));

  return scope.Close(shadersArr);
}

JS_METHOD(GetParameter) {
  JS_BOILERPLATE

  GLenum name = args[0]->Int32Value();

  switch(name) {
  case GL_BLEND:
  case GL_CULL_FACE:
  case GL_DEPTH_TEST:
  case GL_DEPTH_WRITEMASK:
  case GL_DITHER:
  case GL_POLYGON_OFFSET_FILL:
  case GL_SAMPLE_COVERAGE_INVERT:
  case GL_SCISSOR_TEST:
  case GL_STENCIL_TEST:
  case 0x9240 /* UNPACK_FLIP_Y_WEBGL */:
  case 0x9241 /* UNPACK_PREMULTIPLY_ALPHA_WEBGL*/:
  {
    // return a boolean
    GLboolean params;
    ::glGetBooleanv(name, &params);
    return scope.Close(JS_BOOL(params!=0));
  }
  case GL_DEPTH_CLEAR_VALUE:
  case GL_LINE_WIDTH:
  case GL_POLYGON_OFFSET_FACTOR:
  case GL_POLYGON_OFFSET_UNITS:
  case GL_SAMPLE_COVERAGE_VALUE:
  {
    // return a float
    GLfloat params;
    ::glGetFloatv(name, &params);
    return scope.Close(JS_FLOAT(params));
  }
  case GL_RENDERER:
  case GL_SHADING_LANGUAGE_VERSION:
  case GL_VENDOR:
  case GL_VERSION:
  case GL_EXTENSIONS:
  {
    // return a string
    char *params=(char*) ::glGetString(name);
    return scope.Close(params ? JS_STR(params) : Undefined());
  }
  case GL_MAX_VIEWPORT_DIMS:
  {
    // return a int32[2]
    GLint params[2];
    ::glGetIntegerv(name, params);

    Local<Array> arr=Array::New(2);
    arr->Set(0,JS_INT(params[0]));
    arr->Set(1,JS_INT(params[1]));
    return scope.Close(arr);
  }
  case GL_SCISSOR_BOX:
  case GL_VIEWPORT:
  {
    // return a int32[4]
    GLint params[4];
    ::glGetIntegerv(name, params);

    Local<Array> arr=Array::New(4);
    arr->Set(0,JS_INT(params[0]));
    arr->Set(1,JS_INT(params[1]));
    arr->Set(2,JS_INT(params[2]));
    arr->Set(3,JS_INT(params[3]));
    return scope.Close(arr);
  }
  case GL_ALIASED_LINE_WIDTH_RANGE:
  case GL_ALIASED_POINT_SIZE_RANGE:
  case GL_DEPTH_RANGE:
  {
    // return a float[2]
    GLfloat params[2];
    ::glGetFloatv(name, params);
    Local<Array> arr=Array::New(2);
    arr->Set(0,JS_FLOAT(params[0]));
    arr->Set(1,JS_FLOAT(params[1]));
    return scope.Close(arr);
  }
  case GL_BLEND_COLOR:
  case GL_COLOR_CLEAR_VALUE:
  {
    // return a float[4]
    GLfloat params[4];
    ::glGetFloatv(name, params);
    Local<Array> arr=Array::New(4);
    arr->Set(0,JS_FLOAT(params[0]));
    arr->Set(1,JS_FLOAT(params[1]));
    arr->Set(2,JS_FLOAT(params[2]));
    arr->Set(3,JS_FLOAT(params[3]));
    return scope.Close(arr);
  }
  case GL_COLOR_WRITEMASK:
  {
    // return a boolean[4]
    GLboolean params[4];
    ::glGetBooleanv(name, params);
    Local<Array> arr=Array::New(4);
    arr->Set(0,JS_BOOL(params[0]==1));
    arr->Set(1,JS_BOOL(params[1]==1));
    arr->Set(2,JS_BOOL(params[2]==1));
    arr->Set(3,JS_BOOL(params[3]==1));
    return scope.Close(arr);
  }
  case GL_ARRAY_BUFFER_BINDING:
  case GL_CURRENT_PROGRAM:
  case GL_ELEMENT_ARRAY_BUFFER_BINDING:
  case GL_FRAMEBUFFER_BINDING:
  case GL_RENDERBUFFER_BINDING:
  case GL_TEXTURE_BINDING_2D:
  case GL_TEXTURE_BINDING_CUBE_MAP:
  {
    GLint params;
    ::glGetIntegerv(name, &params);
    return scope.Close(JS_INT(params));
  }
  default: {
    // return a long
    GLint params;
    ::glGetIntegerv(name, &params);
    return scope.Close(JS_INT(params));
  }
  }

  return scope.Close(Undefined());
}

JS_METHOD(GetBufferParameter) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum pname = args[1]->Int32Value();

  GLint params;
  glGetBufferParameteriv(target,pname,&params);
  return scope.Close(JS_INT(params));
}

JS_METHOD(GetFramebufferAttachmentParameter) {
  JS_BOILERPLATE

  GLenum target = args[0]->Int32Value();
  GLenum attachment = args[1]->Int32Value();
  GLenum pname = args[2]->Int32Value();

  GLint params;
  glGetFramebufferAttachmentParameteriv(target,attachment, pname,&params);
  return scope.Close(JS_INT(params));
}

JS_METHOD(GetProgramInfoLog) {
  JS_BOILERPLATE

  GLuint program = args[0]->Int32Value();
  int Len = 1024;
  char Error[1024];
  glGetProgramInfoLog(program, 1024, &Len, Error);

  return scope.Close(String::New(Error));
}

JS_METHOD(GetRenderbufferParameter) {
  JS_BOILERPLATE

  int target = args[0]->Int32Value();
  int pname = args[1]->Int32Value();
  int value = 0;
  glGetRenderbufferParameteriv(target,pname,&value);

  return scope.Close(JS_INT(value));
}

JS_METHOD(GetVertexAttrib) {
  JS_BOILERPLATE

  GLuint index = args[0]->Int32Value();
  GLuint pname = args[1]->Int32Value();

  GLint value=0;

  switch (pname) {
  case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
  case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
    glGetVertexAttribiv(index,pname,&value);
    return scope.Close(JS_BOOL(value!=0));
  case GL_VERTEX_ATTRIB_ARRAY_SIZE:
  case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
  case GL_VERTEX_ATTRIB_ARRAY_TYPE:
    glGetVertexAttribiv(index,pname,&value);
    return scope.Close(JS_INT(value));
  case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
    glGetVertexAttribiv(index,pname,&value);
    return scope.Close(JS_INT(value));
  case GL_CURRENT_VERTEX_ATTRIB: {
    float vextex_attribs[4];
    glGetVertexAttribfv(index,pname,vextex_attribs);
    Local<Array> arr=Array::New(4);
    arr->Set(0,JS_FLOAT(vextex_attribs[0]));
    arr->Set(1,JS_FLOAT(vextex_attribs[1]));
    arr->Set(2,JS_FLOAT(vextex_attribs[2]));
    arr->Set(3,JS_FLOAT(vextex_attribs[3]));
    return scope.Close(arr);
  }
  default:
    return ThrowError("GetVertexAttrib: Invalid Enum");
  }

  return scope.Close(Undefined());
}

JS_METHOD(GetSupportedExtensions) {
  JS_BOILERPLATE

  char *extensions=(char*) glGetString(GL_EXTENSIONS);

  return scope.Close(JS_STR(extensions));
}

// TODO GetExtension(name) return the extension name if found, should be an object...
JS_METHOD(GetExtension) {
  JS_BOILERPLATE

  String::AsciiValue name(args[0]);
  char *sname=*name;
  char *extensions=(char*) glGetString(GL_EXTENSIONS);

  char *ext=strcasestr(extensions, sname);

  if(!ext) return scope.Close(Undefined());
  return scope.Close(JS_STR(ext, (int)::strlen(sname)));
}

JS_METHOD(CheckFramebufferStatus) {
  JS_BOILERPLATE

  GLenum target=args[0]->Int32Value();

  return scope.Close(JS_INT((int)glCheckFramebufferStatus(target)));
}

