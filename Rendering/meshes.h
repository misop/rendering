// loader.h
// IMPLEMENTACIA NACITANIA MODELOV Z EXTERNEHO SUBORU

// GLEW kniznica. pre spravu novsich rozsireni
#include "glew.h"
#include <vector>

class Mesh
{   
public:
    Mesh()
    {
        id_vertices_VBO = 0;
        id_tex_coords_VBO = 0;
        id_normals_VBO = 0;
        id_triangle_indices_VBO = 0;
        num_face_indices = 3;
        id_material = -1;
    };

    // geometria ulozena vo VBO OpenGL
    GLuint id_vertices_VBO;
    GLuint id_tex_coords_VBO;
    GLuint id_normals_VBO;
    GLuint id_triangle_indices_VBO;
    unsigned int num_faces;

    // pocet vrcholov kazdeho primitivu v meshi
    unsigned int num_face_indices;

    // odkaz na material
    int id_material;
};

class Material
{
public:
    Material()
    {
        id_diffuse_texture = 0;
        color_ambient[0] = color_ambient[1] = color_ambient[2] = 0.2f;
        color_ambient[3] = 1.0f;
        color_diffuse[0] = color_diffuse[1] = color_diffuse[2] = 0.8f;
        color_diffuse[3] = 1.0f;
        color_specular[0] = color_specular[1] = color_specular[2] = 0.0f;
        color_specular[3] = 1.0f;
        color_emission[0] = color_emission[1] = color_emission[2] = 0.0f;
        color_emission[3] = 1.0f;
        shininess = 16;
    };

    // materialove vlastnosti
    GLuint id_diffuse_texture;
    float color_ambient[4];
    float color_diffuse[4];
    float color_specular[4];
    float color_emission[4];
    float shininess;
};

class Node
{
public:
    Node()
    {
        for (int i = 0; i < 16; i++)
            model_matrix[i] = 0.0f;
        model_matrix[0] = model_matrix[5] = model_matrix[10] = model_matrix[15] = 1.0f;
    }

    // model transformacia pre node
    float model_matrix[16];
    std::vector<int> id_meshes;
};

class Scene
{
public:
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::vector<Node> nodes;
};

// nacitanie modelu do vnutornych OpenGL struktur z externeho suboru
// podporovane formaty: COLLADA, 3DS, OBJ, ...
// navratova hodnota je zoznam meshov
Scene* LoadScene(const char* model_file);

// vykresli mesh aj s materialom
void RenderScene(Scene* scene, GLuint id_shader);