// shaders.h
// IMPLEMENTACIA MANAGMENTU SHADEROV

#include <stdio.h>

// GLEW kniznica. pre spravu novsich rozsireni
#include <GL/glew.h>

// nacitanie zdrojoveho suboru shadera a ulozenie do OpenGL shadera
// navratova hodnota je OpenGL id shadera
GLuint loadShader(GLenum shaderType, const char *fileName);

// nacitanie sahderov a ich ulozenie do OpenGL
// navratova hodnota je OpenGL id shader programu
GLuint loadProgram(const char *fileVertexShader, const char *fileFragmentShader);

// setting uniform variable inside OpenGL shader program
bool SetShaderUniform1i(GLuint program, const char* name, GLint value);

// setting uniform variable inside OpenGL shader program
bool SetShaderUniform4f(GLuint program, const char* name, GLfloat* values);

// setting uniform variable inside OpenGL shader program
bool SetShaderUniformMatrix4f(GLuint program, const char* name, GLfloat* values);