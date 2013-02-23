
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h> // http://glew.sourceforge.net/basic.html

#include <GL/glfw.h> // http://www.glfw.org/GLFWReference27.pdf

// http://www.khronos.org/files/opengl-quick-reference-card.pdf
#include "shaders.h"
#include "geometry.h"
#include "textures.h"

#ifndef M_PI
#define M_PI 3.141592654
#endif

float angleX=0.0;
float angleY=0.0;

unsigned int* gIndices=0;
float* gVertArr=0;
unsigned int gTriCnt=0;
unsigned int gVertCnt=0;

GLuint gPositionsVB=0;
GLuint gTrianglesIB=0;

int running=1;

//-------------- textures --------------


// Texture IDs
GLuint gColorMapID=0;
GLuint gNormalMapID=0;

GLuint shAngleID=0;
float gAngleVal=0.0;

GLhandleARB gGLSLprog=0;

void onInit()
{
	gColorMapID=loadTGA("./textures/rockwall_color.tga");
	gNormalMapID=loadTGA("./textures/rockwall_normal.tga");

        gGLSLprog=createProgram();
}

/* handle rendering */
void onRender()
{
	glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	glClearColor(0.4,0.1,0.1,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gColorMapID);
    glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormalMapID);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -2.0 );

    glRotatef(angleY,0,1,0);
	glRotatef(angleX,1,0,0);

        glUseProgramObjectARB(gGLSLprog);

    glColor4f(1.0,1.0,1.0,1.0);
	glBegin(GL_QUADS);

		glMultiTexCoord2f(GL_TEXTURE0,0.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1,0.0, 0.0);
		glVertex2f(-1.0, -1.0);

		glMultiTexCoord2f(GL_TEXTURE0,1.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1,1.0, 0.0);
		glVertex2f(1.0, -1.0);

		glMultiTexCoord2f(GL_TEXTURE0,1.0, 1.0);
		glMultiTexCoord2f(GL_TEXTURE1,1.0, 1.0);
		glVertex2f(1.0, 1.0);

		glMultiTexCoord2f(GL_TEXTURE0,0.0, 1.0);
		glMultiTexCoord2f(GL_TEXTURE1,0.0, 1.0);        
		glVertex2f(-1.0, 1.0);

	glEnd();

        glUseProgramObjectARB(0);

}


/* new window size callback */
void GLFWCALL onReshape( int width, int height )
{
  GLfloat h = (GLfloat) height / (GLfloat) width;
  GLfloat xmax, znear, zfar;

  znear = 0.1f;
  zfar  = 30.0f;
  xmax  = znear*1.0f;

  glViewport( 0, 0, (GLint) width, (GLint) height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
}

/* handle keyboard input */
void GLFWCALL onKey( int k, int action )
{
  if( action != GLFW_PRESS ) return;

  switch (k) {
    case GLFW_KEY_ESC:
        running = 0;
    break;
    default:
        return;
  }
}

/* handle mouse move event */
void GLFWCALL onMouseMove( int x, int y )
{
    int button = GLFW_MOUSE_BUTTON_LEFT;
 /*
 GLFW_MOUSE_BUTTON_LEFT
 GLFW_MOUSE_BUTTON_RIGHT
 GLFW_MOUSE_BUTTON_MIDDLE
 */
    int state = glfwGetMouseButton( button );
    if (state == GLFW_PRESS)
    {
        fprintf(stderr,"%d %d\r",x,y);
    // process
    	angleY=x;
	    angleX=y;
    } 
}

/* program entry */
int main(int argc, char *argv[])
{
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
/*
glfwOpenWindow(int width, int height,
    int redbits, int greenbits, int bluebits, int alphabits,
    int depthbits, int stencilbits,
    int mode)
*/
    if( !glfwOpenWindow( 512,512, 0,0,0,0, 16,0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    GLenum err = glewInit();
	if (err != GLEW_OK)
    {
		fprintf(stderr,"Error glew initialization failed (%s).\n",glewGetErrorString(err));
    }
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    glfwSetWindowTitle( "05_shaders" );
    glfwEnable( GLFW_KEY_REPEAT );
    glfwSwapInterval( 1 ); // VSync 1x

    // Set callback functions
    glfwSetWindowSizeCallback( onReshape );

    glfwSetKeyCallback( onKey );
    glfwSetMousePosCallback( onMouseMove );

    onInit();

    // Main loop
    while( running )
    {
         onRender();
        // Swap buffers
        glfwSwapBuffers();
        glfwSleep( 0.01 );    // don't eat whole CPU time
        // Was the window closed?
        if( !glfwGetWindowParam( GLFW_OPENED ) )
        {
            running = 0;
        }
    }

    // Terminate GLFW
    glfwTerminate();

    // Exit program
    exit( EXIT_SUCCESS );
}

