#pragma once
#include <IL\il.h>

#include <fstream>

//to map image filenames to textureIds
#include <string.h>
#include <map>
// assimp include files. These three are usually needed.
#include <assimp\Importer.hpp>	//OO version Header!
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <assimp\DefaultLogger.hpp>
#include <assimp\LogStream.hpp>
#include <GL/glew.h> // http://glew.sourceforge.net/basic.html

#include <GL/glfw.h> // http://www.glfw.org/GLFWReference27.pdf

class AssimpLoader
{
public:
	const aiScene *scene;
	Assimp::Importer importer;
	std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
	GLuint*	textureIds;							    // pointer to texture Array

#pragma region Init
	AssimpLoader(void);
	~AssimpLoader(void);
#pragma endregion

#pragma region Assimp Logger
	void createAILogger();
	void destroyAILogger();
	void logInfo(std::string logString);
	void logDebug(const char* logString);
#pragma endregion

#pragma region Import
	bool Import3DFromFile( const std::string& pFile);
#pragma endregion

#pragma region Load Textures
	int LoadGLTextures();
	void apply_material(const aiMaterial *mtl);
#pragma endregion

#pragma region Drawing
	void recursive_render(const struct aiNode* nd, float scale);
	void drawAiScene();
	int DrawGLScene();
#pragma endregion

#pragma region Utility
	void Color4f(const aiColor4D *color);
	void set_float4(float f[4], float a, float b, float c, float d);
	void color4_to_float4(const aiColor4D *c, float f[4]);
#pragma endregion
};

