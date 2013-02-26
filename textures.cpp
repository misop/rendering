
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h> // http://glew.sourceforge.net/basic.html

#include <GL/glfw.h> // http://www.glfw.org/GLFWReference27.pdf

//-------------- textures --------------
typedef unsigned char byte;

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    byte  identsize;          // size of ID field that follows 18 byte header (0 usually)
    byte  colourmaptype;      // type of colour map 0=none, 1=has palette
    byte  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    short colourmapstart;     // first colour map entry in palette
    short colourmaplength;    // number of colours in palette
    byte  colourmapbits;      // number of bits per palette entry 15,16,24,32

    short xstart;             // image x origin
    short ystart;             // image y origin
    short width;              // image width in pixels
    short height;             // image height in pixels
    byte  bpp;               // image bits per pixel 8,16,24,32
    byte  descriptor;         // image descriptor bits (vh flip bits)

    // pixel data follows header

} TGA_HEADER;

#pragma pack(pop)

GLuint loadTGA(const char* fname)
{
	FILE* f=fopen(fname,"rb");
	if (!f)
    {
        fprintf(stderr,"Error opening TGAfile %s\n",fname);
        return 0;
    }
	TGA_HEADER head;
	fread(&head,1,sizeof(TGA_HEADER),f);
	GLvoid* pixels=0;
	GLuint texID=0;
	if (head.imagetype==2) // RGB not packed
	{
	    pixels=(unsigned char*)malloc(head.width*head.height*head.bpp >> 3);
	    fread(pixels,head.height,head.width*(head.bpp>>3),f);
	    glGenTextures(1,&texID);
	    glBindTexture(GL_TEXTURE_2D,texID);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

        switch (head.bpp)
        {
		    case 24 :
			    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,head.width,head.height,0,GL_BGR,GL_UNSIGNED_BYTE,pixels);
		    break;
            case 32 :
			    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,head.width,head.height,0,GL_BGRA,GL_UNSIGNED_BYTE,pixels);
		    break;
        }
	}
	free(pixels);
	fclose(f);
	return texID;
}
