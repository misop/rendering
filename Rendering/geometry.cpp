// geometry.cpp
// DEFINOVANIE ZAKLADNYCH GEOMETRICKYCH OBJEKTOV

#include "geometry.h"
#include <math.h>


// id cube VBO
GLuint g_uiVBOCubeCoords = 0, g_uiVBOCubeTexCoords = 0, g_uiVBOCubeNormals = 0, g_uiVBOCubeIndices = 0;

// id sphere VBO
GLuint g_uiVBOSphereCoords = 0, g_uiVBOSphereTexCoords = 0, g_uiVBOSphereNormals = 0, g_uiVBOSphereIndices = 0;
GLuint g_uiVBOSphereNumIndices;

// id torus VBO
GLuint g_uiVBOTorusCoords = 0, g_uiVBOTorusTexCoords = 0, g_uiVBOTorusNormals = 0, g_uiVBOTorusIndices = 0;
GLuint g_uiVBOTorusNumIndices;

#define PI 3.14159f
#define NULL 0

GLfloat cube_vertices[] = {1,1,1,    -1,1,1,   -1,-1,1,  1,-1,1,  
                          1,1,1,    1,-1,1,   1,-1,-1,  1,1,-1,
                          1,1,1,    1,1,-1,   -1,1,-1,  -1,1,1,
                          -1,1,1,   -1,1,-1,  -1,-1,-1, -1,-1,1,
                          -1,-1,-1, 1,-1,-1,  1,-1,1,   -1,-1,1,
                          1,-1,-1,  -1,-1,-1, -1,1,-1,  1,1,-1 };
GLfloat cube_normals[] = {0,0,1,    0,0,1,   0,0,1,  0,0,1,  
                          1,0,0,    1,0,0,   1,0,0,  1,0,0,
                          0,1,0,    0,1,0,   0,1,0,  0,1,0,
                          -1,0,0,   -1,0,0,  -1,0,0, -1,0,0,
                          0,-1,0, 0,-1,0,  0,-1,0,   0,-1,0,
                          0,0,-1,  0,0,-1, 0,0,-1,  0,0,-1};
GLfloat cube_texcoords[] = {0,0,   1,0,  1,1,   0,1,  
                           0,0,   1,0,  1,1,   0,1,
                           0,0,   1,0,  1,1,   0,1,
                           0,0,   1,0,  1,1,   0,1,
                           0,0,   1,0,  1,1,   0,1,
                           0,0,   1,0,  1,1,   0,1};
GLuint cube_indices[] = {0,1,2,3,  4,5,6,7,  8,9,10,11,   12,13,14,15,   16,17,18,19,  20,21,22,23};


// priprava VBO pre renderovanie kocky
void initCubeVBO()
{
    glGenBuffers(1, &g_uiVBOCubeCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeCoords);
    glBufferData(GL_ARRAY_BUFFER, 24*3*sizeof(float), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOCubeNormals);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeNormals);
    glBufferData(GL_ARRAY_BUFFER, 24*3*sizeof(float), cube_normals, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOCubeTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeTexCoords);
    glBufferData(GL_ARRAY_BUFFER, 24*2*sizeof(float), cube_texcoords, GL_STATIC_DRAW);
    
    glGenBuffers(1, &g_uiVBOCubeIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOCubeIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*4*sizeof(GLuint), cube_indices, GL_STATIC_DRAW);
}

// priprava VBO pre renderovanie sfery
void initSphereVBO(int xSlices, int ySlices)
{
    float* vertices = new float[3 * (xSlices + 1) * (ySlices + 1)];
    float* normals = new float[3 * (xSlices + 1) * (ySlices + 1)];
    float* texcoords = new float[2 * (xSlices + 1) * (ySlices + 1)];

    for (int i = 0; i <= xSlices; i++)
    {
        float u = i / float(xSlices);
        for (int j = 0; j <= ySlices; j++)
        {
            float v = j / float(ySlices);

            int index = i + j * (xSlices + 1);

            vertices[3 * index + 0] = cos(2 * u * PI) * cos((v - 0.5f) * PI);
            vertices[3 * index + 1] = sin(2 * u * PI) * cos((v - 0.5f) * PI);
            vertices[3 * index + 2] = sin((v - 0.5f) * PI);

            normals[3 * index + 0] = vertices[3 * index + 0];
            normals[3 * index + 1] = vertices[3 * index + 1];
            normals[3 * index + 2] = vertices[3 * index + 2];

            texcoords[2 * index + 0] = u;
            texcoords[2 * index + 1] = 1.0f - v;
        }
    }

    GLuint* indices = new GLuint[4 * (xSlices + 0) * (ySlices + 0)];
    int index = 0;
    for (int i = 0; i < xSlices; i++)
    {
        for (int j = 0; j < ySlices; j++)
        {
            indices[index] = i + j * (xSlices + 1);
            index++;
            indices[index] = (i + 1) + j * (xSlices + 1);
            index++;
            indices[index] = (i + 1) + (j + 1) * (xSlices + 1);
            index++;
            indices[index] = i + (j + 1) * (xSlices + 1);
            index++;
        }
    }

    glGenBuffers(1, &g_uiVBOSphereCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereCoords);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xSlices + 1) * (ySlices + 1) * sizeof(float), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOSphereNormals);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereNormals);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xSlices + 1) * (ySlices + 1) * sizeof(float), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOSphereTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereTexCoords);
    glBufferData(GL_ARRAY_BUFFER, 2 * (xSlices + 1) * (ySlices + 1) * sizeof(float), texcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOSphereIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOSphereIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * (xSlices + 0) * (ySlices + 0) * sizeof(GLuint), indices, GL_STATIC_DRAW);

    g_uiVBOSphereNumIndices = 4 * (xSlices + 0) * (ySlices + 0);

    delete [] vertices;
    delete [] normals;
    delete [] texcoords;
    delete [] indices;
}

// priprava VBO pre renderovanie torusu
void initTorusVBO(float innerRadius, float outerRadius, int xSlices, int ySlices)
{
    float* vertices = new float[3 * (xSlices + 1) * (ySlices + 1)];
    float* normals = new float[3 * (xSlices + 1) * (ySlices + 1)];
    float* texcoords = new float[2 * (xSlices + 1) * (ySlices + 1)];

    for (int i = 0; i <= xSlices; i++)
    {
        float u = i / float(xSlices);
        for (int j = 0; j <= ySlices; j++)
        {
            float v = j / float(ySlices);

            int index = i + j * (xSlices + 1);

            vertices[3 * index + 0] = cos(2 * u * PI) * (outerRadius + innerRadius * cos(2 * v * PI));
            vertices[3 * index + 1] = sin(2 * u * PI) * (outerRadius + innerRadius * cos(2 * v * PI));
            vertices[3 * index + 2] = innerRadius * sin(2 * v * PI);

            float tx = -sin(2 * u * PI);
            float ty = cos(2 * u * PI);
            float tz = 0;
            float sx = cos(2 * u * PI)*(-sin(2 * v * PI));
            float sy = sin(2 * u * PI)*(-sin(2 * v * PI));
            float sz = cos(2 * v * PI);
            normals[3 * index + 0] = ty*sz - tz*sy;
            normals[3 * index + 1] = tz*sx - tx*sz;
            normals[3 * index + 2] = tx*sy - ty*sx;

            texcoords[2 * index + 0] = u;
            texcoords[2 * index + 1] = 1.0f - v;
        }
    }

    GLuint* indices = new GLuint[4 * (xSlices + 0) * (ySlices + 0)];
    int index = 0;
    for (int i = 0; i < xSlices; i++)
    {
        for (int j = 0; j < ySlices; j++)
        {
            indices[index] = i + j * (xSlices + 1);
            index++;
            indices[index] = (i + 1) + j * (xSlices + 1);
            index++;
            indices[index] = (i + 1) + (j + 1) * (xSlices + 1);
            index++;
            indices[index] = i + (j + 1) * (xSlices + 1);
            index++;
        }
    }

    glGenBuffers(1, &g_uiVBOTorusCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusCoords);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xSlices + 1) * (ySlices + 1) * sizeof(float), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOTorusNormals);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusNormals);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xSlices + 1) * (ySlices + 1) * sizeof(float), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOTorusTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusTexCoords);
    glBufferData(GL_ARRAY_BUFFER, 2 * (xSlices + 1) * (ySlices + 1) * sizeof(float), texcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &g_uiVBOTorusIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOTorusIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * (xSlices + 0) * (ySlices + 0) * sizeof(GLuint), indices, GL_STATIC_DRAW);

    g_uiVBOTorusNumIndices = 4 * (xSlices + 0) * (ySlices + 0);

    delete [] vertices;
    delete [] normals;
    delete [] texcoords;
    delete [] indices;
}

// vykresli jednotkovu kocku s pomocou pripravenych VBO
void renderCubeVBO()
{
    if (g_uiVBOCubeCoords == 0)
        initCubeVBO();

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeCoords);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeTexCoords);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOCubeNormals);
    glNormalPointer(GL_FLOAT, 0, NULL);

    // draw a cube using buffer with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOCubeIndices);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, NULL);

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// vykresli jednotkovu sferu s pomocou pripravenych VBO
void renderSphereVBO()
{
    if (g_uiVBOSphereCoords == 0)
        initSphereVBO(16, 16);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereCoords);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereTexCoords);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOSphereNormals);
    glNormalPointer(GL_FLOAT, 0, NULL);

    // draw a cube using buffer with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOSphereIndices);
    glDrawElements(GL_QUADS, g_uiVBOSphereNumIndices, GL_UNSIGNED_INT, NULL);

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// vykresli torus s pomocou pripravenych VBO
void renderTorusVBO()
{
    if (g_uiVBOTorusCoords == 0)
        initTorusVBO(2, 0.2f, 16, 16);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusCoords);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusTexCoords);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, g_uiVBOTorusNormals);
    glNormalPointer(GL_FLOAT, 0, NULL);

    // draw a cube using buffer with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_uiVBOTorusIndices);
    glDrawElements(GL_QUADS, g_uiVBOTorusNumIndices, GL_UNSIGNED_INT, NULL);

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// vycitime alokovane objekty
void DeleteAllVBO()
{
    if (g_uiVBOSphereCoords > 0)
        glDeleteBuffers(1, &g_uiVBOSphereCoords);
    if (g_uiVBOSphereTexCoords > 0)
        glDeleteBuffers(1, &g_uiVBOSphereTexCoords);
    if (g_uiVBOSphereNormals > 0)
        glDeleteBuffers(1, &g_uiVBOSphereNormals);
    if (g_uiVBOSphereIndices > 0)
        glDeleteBuffers(1, &g_uiVBOSphereIndices);

    if (g_uiVBOTorusCoords > 0)
        glDeleteBuffers(1, &g_uiVBOTorusCoords);
    if (g_uiVBOTorusTexCoords > 0)
        glDeleteBuffers(1, &g_uiVBOTorusTexCoords);
    if (g_uiVBOTorusNormals > 0)
        glDeleteBuffers(1, &g_uiVBOTorusNormals);
    if (g_uiVBOTorusIndices > 0)
        glDeleteBuffers(1, &g_uiVBOTorusIndices);
}