//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

const char* loadTextFile(const char* fname)
{
	FILE *f=fopen(fname,"rb");
	if (!f)
	{
		printf("File %s not opened!\n",fname);
		return 0;
	}
	fseek(f,0,SEEK_END);
	unsigned int nSiz=ftell(f);
	fseek(f,0,SEEK_SET);
	char* s=new char[nSiz+1];
	fread(s,nSiz,1,f);
	s[nSiz]=0;
	fclose(f);

	return s;
}


//---------------------------------------------------------------------------

void printInfoLog(const GLhandleARB obj)
	{
	    int infoLogLength = 0;
	    int charsWritten  = 0;
	    char *infoLog;

	    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
						 &infoLogLength);

	    if (infoLogLength > 1)
	    {
		infoLog = (char *)malloc(infoLogLength);
		if(!infoLog)
		{
			fprintf(stderr,"Allocation memory failed.\n");
			return;
		}

		glGetInfoLogARB(obj, infoLogLength, &charsWritten, infoLog);
		fprintf(stderr, "shaderlog (%d): \"%s\"\n",infoLogLength,infoLog);
		free(infoLog);
	    }
	}

// shaderType
//GL_VERTEX_SHADER_ARB
//GL_FRAGMENT_SHADER_ARB
//GL_GEOMETRY_SHADER_EXT
GLhandleARB getShader(const char* fname,const GLenum shaderType)
{
	const char* shaderSrc=loadTextFile(fname);
	if (!shaderSrc) return 0;

	GLhandleARB shaderObj = glCreateShaderObjectARB(shaderType);
	glShaderSourceARB(shaderObj, 1, &shaderSrc, NULL);
	glCompileShaderARB(shaderObj);
	printInfoLog(shaderObj);
//	delete [] shaderSrc;
	return shaderObj;
}


GLhandleARB createProgram()
{
	//create and cmpile shader OBJECT
	GLhandleARB shaderObj = getShader("shaders/minimal.frag",GL_FRAGMENT_SHADER_ARB);

	//create shader program
	GLhandleARB GLSLprogram = glCreateProgramObjectARB();
	//attach shader object to shader program
	glAttachObjectARB(GLSLprogram, shaderObj);

	//link shader program
	glLinkProgramARB(GLSLprogram);
	//print shader log (errors)
	printInfoLog(GLSLprogram);

	GLuint shNormalMapID=glGetUniformLocation(GLSLprogram,"normalMap");
    fprintf(stderr,"%d\n",shNormalMapID);

	GLuint shColorMapID=glGetUniformLocation(GLSLprogram,"colorMap");
    fprintf(stderr,"%d\n",shColorMapID);

    glUseProgramObjectARB(GLSLprogram);

    glUniform1i(shColorMapID,0);
    glUniform1i(shNormalMapID,1); 

    glUseProgramObjectARB(0);

	return GLSLprogram;
}

