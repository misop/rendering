// geometry.h
// DEFINOVANIE ZAKLADNYCH GEOMETRICKYCH OBJEKTOV

// GLEW kniznica. pre spravu novsich rozsireni
#include "glew.h"

// priprava VBO pre renderovanie kocky
void initCubeVBO();

// priprava VBO pre renderovanie sfery
void initSphereVBO(int xSlices, int ySlices);

// priprava VBO pre renderovanie torusu
void initTorusVBO(float innerRadius, float outerRadius, int xSlices, int ySlices);

// vykresli jednotkovu kocku s pomocou pripravenych VBO
void renderCubeVBO();

// vykresli jednotkovu sferu s pomocou pripravenych VBO
void renderSphereVBO();

// vykresli torus s pomocou pripravenych VBO
void renderTorusVBO();

// vycitime alokovane objekty
void DeleteAllVBO();