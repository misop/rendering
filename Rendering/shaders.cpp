// shaders.cpp
// IMPLEMENTACIA MANAGMENTU SHADEROV

#include "shaders.h"

// funkcia pre nacitanie textoveho suboru do pola znakov
GLchar* readShaderFile(const char *fileName)
{
    FILE* file = fopen(fileName, "r" );
    if( file == NULL )
    {
        printf("Cannot open file %s. \n", fileName);
		return NULL;
    }

    // ziskaj velkost suboru v bajtoch
    fseek(file, 0L, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // nacitaj data zo suboru
    GLchar* buffer = new GLchar[fileSize + 1];
	int bytes = fread(buffer, 1, fileSize, file);
    buffer[bytes] = 0;
	fclose(file);

	return buffer;
}

// nacitanie zdrojoveho suboru shadera a ulozenie do OpenGL shadera
// navratova hodnota je OpenGL id shadera
GLuint loadShader(GLenum shaderType, const char *fileName)
{
    GLchar *shaderSource = readShaderFile(fileName);
    if (shaderSource == NULL)
        return 0;

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar**)&shaderSource, NULL);
    glCompileShader(shader);
    delete shaderSource;

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)  
    {
        GLchar log[1024];
        GLsizei length;
        glGetShaderInfoLog(shader, 1024, &length, log);
        printf("Failed compiling shader %s \n", fileName);
        printf("   Compilation error: %s \n", log);
        glDeleteShader(shader);

        return 0;
    }

    printf("Shader %s compiled successfully.\n", fileName);
    return shader;
}

// nacitanie sahderov a ich ulozenie do OpenGL
// navratova hodnota je OpenGL id shader programu
GLuint loadProgram(const char *fileVertexShader, const char *fileFragmentShader)
{
    // nacitanie a kompilacia shaderov
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, fileVertexShader);
    if (vertexShader == 0)
        return 0;
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fileFragmentShader);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    // vytvorenie program objektu a pripojenie shaderov
    GLuint programObj = glCreateProgram();
    glAttachShader(programObj, vertexShader);
    glAttachShader(programObj, fragmentShader);
    glLinkProgram(programObj);

    // zisti uspesnost zlinkovania shaderov
    GLint status;
    glGetProgramiv(programObj, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)  
    {
        GLchar log[1024];
        GLsizei length;
        glGetProgramInfoLog(programObj, 1024, &length, log);
        printf("Failed linking shaders with error %s\n", log);

        glDeleteProgram(programObj);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        return 0;
    }

    printf("Shaders linked successfully.\n");
    return programObj;
}

// setting uniform variable inside OpenGL shader program
bool SetShaderUniform1i(GLuint program, const char* name, GLint value)
{
    int location = glGetUniformLocation(program, name);
    if (location == -1)
    {
        printf("Erro getting location for uniform %s.\n", name);
        return false;
    }

    glUniform1i(location, value);
    return true;
}

bool SetShaderUniform4f(GLuint program, const char* name, GLfloat* values)
{
    int location = glGetUniformLocation(program, name);
    if (location == -1)
    {
        printf("Erro getting location for uniform %s.\n", name);
        return false;
    }

    glUniform4fv(location, 1, values);
    return true;
}

bool SetShaderUniformMatrix4f(GLuint program, const char* name, GLfloat* values)
{
    int location = glGetUniformLocation(program, name);
    if (location == -1)
    {
        printf("Erro getting location for uniform %s.\n", name);
        return false;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, values);
    return true;
}