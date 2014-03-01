// loader.cpp
// IMPLEMENTACIA NACITANIA MODELOV Z EXTERNEHO SUBORU

#include "meshes.h"

// pouzivameAssimp kniznicu, http://assimp.sourceforge.net/
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "textures.h"
#include "shaders.h"
#include <map>
#include <windows.h>

Scene* LoadScene(const char* model_file)
{
    printf("Importing model from file %s ...\n", model_file);

    // nacitaj scenu a zisti, ci sa nacitanie podarilo
    // pri nacitani sa aj optimalizuju a trianguluju vsetky meshe
    const aiScene* ai_scene = aiImportFile(model_file, aiProcessPreset_TargetRealtime_Quality);
    if (ai_scene == NULL)
    {
        printf("Import failed!\n");
        return NULL;
    }

    // pouzijem win32 api na ziskanie plnej cesty k suboru
    #define BUFSIZE 4096
    char  buffer[BUFSIZE]; 
    char** lppPart = {NULL};
    GetFullPathName(model_file, BUFSIZE, buffer, lppPart);
    std::string directory_path = buffer;
    for (unsigned int i = 0; i < directory_path.length(); i++)
        if (directory_path[i] == '\\')
            directory_path[i] = '/';
    directory_path = directory_path.substr(0, directory_path.find_last_of('/') + 1);

    Scene* result = new Scene();

    // vyrob vsetky materialy
    std::map<std::string, GLuint> already_loaded_textures;
    for (unsigned int i = 0; i < ai_scene->mNumMaterials; i++)
    {
        aiMaterial* ai_material = ai_scene->mMaterials[i];
        Material material;
        
        aiColor4D ambient;
        if(AI_SUCCESS == aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_AMBIENT, &ambient))
        {
            material.color_ambient[0] = ambient.r;
            material.color_ambient[1] = ambient.g;
            material.color_ambient[2] = ambient.b;
            material.color_ambient[3] = ambient.a;
        }

        aiColor4D diffuse;
        if(AI_SUCCESS == aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        {
            material.color_diffuse[0] = diffuse.r;
            material.color_diffuse[1] = diffuse.g;
            material.color_diffuse[2] = diffuse.b;
            material.color_diffuse[3] = diffuse.a;
        }

        aiColor4D specular;
        if(AI_SUCCESS == aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_SPECULAR, &specular))
        {
            material.color_specular[0] = specular.r;
            material.color_specular[1] = specular.g;
            material.color_specular[2] = specular.b;
            material.color_specular[3] = specular.a;
        }

        aiColor4D emission;
        if(AI_SUCCESS == aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_EMISSIVE, &emission))
        {
            material.color_emission[0] = emission.r;
            material.color_emission[1] = emission.g;
            material.color_emission[2] = emission.b;
            material.color_emission[3] = emission.a;
        }

        unsigned int max = 1;
        float shininess, strength;
	    if (aiGetMaterialFloatArray(ai_material, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS) 
        {
    	    max = 1;
		    if(aiGetMaterialFloatArray(ai_material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) == AI_SUCCESS)
			    material.shininess =  shininess * strength;
            else
        	    material.shininess =  shininess;
        }
	    else 
        {
            material.shininess = 0.0f;
		    material.color_specular[0] = 0.0f;
            material.color_specular[1] = 0.0f;
            material.color_specular[2] = 0.0f;
            material.color_specular[3] = 1.0f;
	    }

        // nacitaj difuznu texturu
        aiString path;
        if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            if (already_loaded_textures.find(path.data) != already_loaded_textures.end())
            {
                // texturu sme uz nacitali, iba pouzijeme uz existujuce id
                material.id_diffuse_texture = already_loaded_textures.find(path.data)->second;
            }
            else
            {
                // nacitaj texturu
                material.id_diffuse_texture = LoadTexture(path.data);
                // skus absolutnu cestu
                if (material.id_diffuse_texture == 0)
                    material.id_diffuse_texture = LoadTexture((directory_path + path.data).c_str());
                if (material.id_diffuse_texture > 0)
                    already_loaded_textures.insert(std::pair<std::string, GLuint>(path.data, material.id_diffuse_texture));
            }
        }

        result->materials.push_back(material);
    }

    // nacitaj vsetky meshe do OpenGL ako VBO
    for (unsigned int i = 0; i < ai_scene->mNumMeshes; i++)
    {
        aiMesh* ai_mesh = ai_scene->mMeshes[i];
        Mesh mesh;

        // nastav material
        mesh.id_material = ai_mesh->mMaterialIndex;

        // nacitaj vrcholy do OpenGL
        glGenBuffers(1, &mesh.id_vertices_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices_VBO);
        glBufferData(GL_ARRAY_BUFFER, ai_mesh->mNumVertices*3*sizeof(float), &ai_mesh->mVertices[0].x, GL_STATIC_DRAW);

        // nacitaj normaly
        if(ai_mesh->mNormals != NULL)
        {
            glGenBuffers(1, &mesh.id_normals_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normals_VBO);
            glBufferData(GL_ARRAY_BUFFER, ai_mesh->mNumVertices*3*sizeof(float), &ai_mesh->mNormals[0].x, GL_STATIC_DRAW);
        }

        // nacitaj texturne suradnice
        if(ai_mesh->HasTextureCoords(0))
        {
            glGenBuffers(1, &mesh.id_tex_coords_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.id_tex_coords_VBO);
            float* tex_coords = new float[2 * ai_mesh->mNumVertices];
            for (unsigned int j = 0; j < ai_mesh->mNumVertices; j++)
            {
                tex_coords[2*j] = ai_mesh->mTextureCoords[0][j].x;
                tex_coords[2*j+1] = 1-ai_mesh->mTextureCoords[0][j].y;
            }
            glBufferData(GL_ARRAY_BUFFER, ai_mesh->mNumVertices*2*sizeof(float), tex_coords, GL_STATIC_DRAW);
            delete [] tex_coords;
        }

        // nacitaj indexy pre trojuholniky
        // najprv zisti pocet indexov pre jeden face
        // toto definuje ci sa jedna o samostatne body, usecky alebo trojuholniky
        mesh.num_face_indices = 0;
        for (unsigned int t = 0; t < ai_mesh->mNumFaces; t++)
        {
            if (mesh.num_face_indices < ai_mesh->mFaces[t].mNumIndices)
                mesh.num_face_indices = ai_mesh->mFaces[t].mNumIndices;
        }
        mesh.num_faces = ai_mesh->mNumFaces;

        // vypln pole indexov
        GLuint* indices = new GLuint[ai_mesh->mNumFaces * mesh.num_face_indices];
        memset(indices, 0, ai_mesh->mNumFaces*mesh.num_face_indices*sizeof(GLuint));
        for (unsigned int t = 0; t < ai_mesh->mNumFaces; t++)
        {
            for(unsigned int j = 0; j < ai_mesh->mFaces[t].mNumIndices; j++)
                indices[t * mesh.num_face_indices + j] = ai_mesh->mFaces[t].mIndices[j];
        }

        glGenBuffers(1, &mesh.id_triangle_indices_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_triangle_indices_VBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ai_mesh->mNumFaces*mesh.num_face_indices*sizeof(GLuint), indices, GL_STATIC_DRAW);
        delete [] indices;

        result->meshes.push_back(mesh);
    }

    // rekurzivne prejdi vsetky nody v scene
    std::vector<aiNode*> nodes;
    std::vector<aiMatrix4x4> nodes_model_matrix;
    nodes.push_back(ai_scene->mRootNode);
    nodes_model_matrix.push_back(ai_scene->mRootNode->mTransformation);
    while (nodes.size() > 0)
    {
        // ziskaj prvy node v zozname
        aiNode* current_node = nodes[0];
        nodes.erase(nodes.begin());
        aiMatrix4x4 current_node_matrix = nodes_model_matrix[0];
        nodes_model_matrix.erase(nodes_model_matrix.begin());
        int i = nodes_model_matrix.size();

        float det = current_node_matrix.Determinant();
        if (det == 0)
            int iii = 0;

        // zisti ci aktualny node ma mesh a nastav maticu
        if (current_node->mNumMeshes > 0)
        {
            Node node;
            aiMatrix4x4 transposed = current_node_matrix.Transpose();
            node.model_matrix[0] = transposed.a1;
            node.model_matrix[1] = transposed.a2;
            node.model_matrix[2] = transposed.a3;
            node.model_matrix[3] = transposed.a4;
            node.model_matrix[4] = transposed.b1;
            node.model_matrix[5] = transposed.b2;
            node.model_matrix[6] = transposed.b3;
            node.model_matrix[7] = transposed.b4;
            node.model_matrix[8] = transposed.c1;
            node.model_matrix[9] = transposed.c2;
            node.model_matrix[10] = transposed.c3;
            node.model_matrix[11] = transposed.c4;
            node.model_matrix[12] = transposed.d1;
            node.model_matrix[13] = transposed.d2;
            node.model_matrix[14] = transposed.d3;
            node.model_matrix[15] = transposed.d4;
            current_node_matrix.Transpose();

            for (unsigned int n = 0; n < current_node->mNumMeshes; ++n)
            {
                node.id_meshes.push_back(current_node->mMeshes[n]);
            }

            result->nodes.push_back(node);
        }

        // pridaj potomkov nodu
        for (unsigned int n = 0; n < current_node->mNumChildren; ++n)
        {
            nodes.push_back(current_node->mChildren[n]);
            aiMatrix4x4 aux = current_node->mChildren[n]->mTransformation * current_node_matrix;
            nodes_model_matrix.push_back(current_node_matrix * current_node->mChildren[n]->mTransformation);
        }
    }

    printf("Import successfull.\n");
    return result;
}

void SetActualMaterial(Material material, GLuint id_shader)
{
    // zakladne vlastnosti materialu
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.color_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.color_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.color_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material.color_emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);

    // nastav aktualnu texturu
    if (material.id_diffuse_texture > 0)
    {
        glEnable(GL_TEXTURE_2D);
        SetShaderUniform1i(id_shader, "texturing_enabled", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.id_diffuse_texture);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        SetShaderUniform1i(id_shader, "texturing_enabled", 0);
    }
}

void RenderMesh(Mesh mesh)
{
    // VBO so suradnicami
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices_VBO);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // VBO s texturnymi suradnicami
    if (mesh.id_tex_coords_VBO > 0)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.id_tex_coords_VBO);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    }

    // VBO s normalami
    if (mesh.id_normals_VBO > 0)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normals_VBO);
        glNormalPointer(GL_FLOAT, 0, NULL);
    }

    // vykreslenie geometricky primitiv s pomocou VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_triangle_indices_VBO);
    GLenum mode = GL_POINTS;
    if (mesh.num_face_indices == 2)
        mode = GL_LINES;
    else if (mesh.num_face_indices == 3)
        mode = GL_TRIANGLES;
    glDrawElements(mode, mesh.num_face_indices * mesh.num_faces, GL_UNSIGNED_INT, NULL);

    // deaktivacia bufferov
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderScene(Scene* scene, GLuint id_shader)
{
    for (unsigned int n = 0; n < scene->nodes.size(); ++n)
    {
        // nastav aktualnu model transformaciu pre node
        Node node = scene->nodes[n];
        glPushMatrix();
        glMultMatrixf(node.model_matrix);

        // vyrenderuj vsetky meshe
        for (unsigned int m = 0; m < node.id_meshes.size(); m++)
        {
            // ziskaj aktualny mesh
            Mesh mesh;
            if (node.id_meshes[m] > -1 && node.id_meshes[m] < (int)scene->meshes.size())
                mesh = scene->meshes[node.id_meshes[m]];

            // ziskaj aktualny material
            Material material;
            if (mesh.id_material > -1 && mesh.id_material < (int)scene->materials.size())
                material = scene->materials[mesh.id_material];

            // nastav material pre mesh v OpenGL
            SetActualMaterial(material, id_shader);

            // vykresli mesh cez OpenGL
            RenderMesh(mesh);
        }

        glPopMatrix();
    }
}