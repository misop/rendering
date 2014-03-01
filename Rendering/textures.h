// textures.h
// IMPLEMENTACIA NACITANIA TEXTUR

// GLEW kniznica. pre spravu novsich rozsireni
#include <GL/glew.h>

// nacitanie textury z externeho suboru a jej ulozenie do OpenGL
// navratova hodnota je OpenGL id textury 
GLuint LoadTexture(const char* tex_file);

// nacitanie cube mapy z 6 textur a jej ulozenie do OpenGL
// navratova hodnota je OpenGL id cube textury 
GLuint LoadCubeTexture(const char* tex_pos_x_file, const char* tex_neg_x_file,
                       const char* tex_pos_y_file, const char* tex_neg_y_file,
                       const char* tex_pos_z_file, const char* tex_neg_z_file);