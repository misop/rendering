// textures.cpp
// IMPLEMENTACIA NACITANIA TEXTUR

#include "textures.h"

// OpenIL kniznica pre nacitanie obrazkov
#include "il.h"

// nacitanie textury z externeho suboru a jej ulozenie do OpenGL
// navratova hodnota je OpenGL id textury 
GLuint LoadTexture(const char* tex_file)
{
    // iniciailizacia OpenIL
    ilInit();

    ILuint texIL;
    ilGenImages(1, &texIL);
    ilBindImage(texIL);

    // nacitanie textury zo suboru
    ILboolean result = ilLoadImage(tex_file);
    if (!result)
    {
        printf("Can not open texture file %s. No texture loaded.\n", tex_file);
        ilDeleteImages(1, &texIL);
        return 0;
    }

    // generovanie OpenGL textur
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // nastavenie parametrov textury
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);

    // definovanie texlov
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    // zamzanie OpenIL textury
    ilDeleteImages(1, &texIL);

    printf("Texture %s loaded successfully.\n", tex_file);

    return tex;
}

// nacitanie cube mapy z 6 textur a jej ulozenie do OpenGL
// navratova hodnota je OpenGL id textury 
GLuint LoadCubeTexture(const char* tex_pos_x_file, const char* tex_neg_x_file,
                       const char* tex_pos_y_file, const char* tex_neg_y_file,
                       const char* tex_pos_z_file, const char* tex_neg_z_file)
{
    ilInit();
    ILuint cubeTexIL;
    ilGenImages(1, &cubeTexIL);
    ilBindImage(cubeTexIL);

    // vytvorenie OpenGL cube map textury
    GLuint cube_tex;
    glGenTextures(1, &cube_tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_tex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // nacitanie textury pre posx stenu zo suboru a do OpenGL
    ILboolean result = ilLoadImage(tex_pos_x_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_pos_x_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negx stenu zo suboru a do OpenGL
    result = ilLoadImage(tex_neg_x_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_neg_x_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre posy stenu zo suboru a do OpenGL
    result = ilLoadImage(tex_pos_y_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_pos_y_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negy stenu zo suboru a do OpenGL
    result = ilLoadImage(tex_neg_y_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_neg_y_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre posz stenu zo suboru a do OpenGL
    result = ilLoadImage(tex_pos_z_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_pos_z_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negz stenu zo suboru a do OpenGL
    result = ilLoadImage(tex_neg_z_file);
    if (!result)
    {
        printf("Can not open cube map texture file %s. No texture loaded.\n", tex_neg_z_file);
        ilDeleteImages(1, &cubeTexIL);
        glDeleteTextures(1, &cube_tex);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    printf("Cube map loaded successfully.\n");

    return cube_tex;
}