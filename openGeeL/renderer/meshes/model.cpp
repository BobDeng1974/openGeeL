#include <iostream>

#include "../shader/shader.h"
#include "../materials/material.h"
#include "../materials/materialfactory.h"
#include "../texturing/simpletexture.h"
#include "../../dependencies/assimp/Importer.hpp"
#include "../../dependencies/assimp/scene.h"
#include "../../dependencies/assimp/postprocess.h"
#include "mesh.h"
#include "model.h"

namespace geeL {

	void Model::loadModel(MaterialFactory& factory) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(factory, scene->mRootNode, scene);
	}

	void Model::draw() {
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].draw();
	}

	void Model::draw(vector<Material*> customMaterials) {
		size_t size = customMaterials.size();

		for (unsigned int i = 0; i < meshes.size(); i++) {
			if (i < size) {
				Material& mat = *customMaterials[i];
				meshes[i].draw(mat);
			}
			else
				meshes[i].draw();
		}
	}

	void Model::processNode(MaterialFactory& factory, aiNode* node, const aiScene* scene) {

		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(factory, mesh, scene));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++)
			processNode(factory, node->mChildren[i], scene);
	}

	vector<SimpleTexture*> loadMaterialTextures(MaterialFactory& factory, aiMaterial* mat, aiTextureType aiType, TextureType type, string directory) {
		vector<SimpleTexture*> textures;

		for (GLuint i = 0; i < mat->GetTextureCount(aiType); i++) {
			aiString str;
			mat->GetTexture(aiType, i, &str);

			string fileName = directory + "/" + string(str.C_Str());
			SimpleTexture& texture = factory.CreateTexture(fileName, type);	//TODO: reference it up
			textures.push_back(&texture);
		}

		return textures;
	}

	Mesh Model::processMesh(MaterialFactory& factory, aiMesh* mesh, const aiScene* scene) {
		
		// Data to fill
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<SimpleTexture*> textures;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			glm::vec3 vector; 
							  
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.position = vector;
			
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;

			vertex.normal = vector;
			
			// Does the mesh contain texture coordinates?
			if (mesh->mTextureCoords[0]) {

				glm::vec2 vec;

				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else
				vertex.texCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		// Now wak through each of the mesh's faces and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Process materials
		if (mesh->mMaterialIndex >= 0) {

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<SimpleTexture*> diffuseMaps = loadMaterialTextures(factory, material, aiTextureType_DIFFUSE, Diffuse, directory);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<SimpleTexture*> specularMaps = loadMaterialTextures(factory, material, aiTextureType_SPECULAR, Specular, directory);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		Material& mat = factory.CreateMaterial();
		mat.addTextures(textures);
		mat.addParameter("shininess", 64.f);

		return Mesh(vertices, indices, mat);
	}

	vector<Mesh>::iterator Model::meshesBegin() {
		return meshes.begin();
	}

	vector<Mesh>::iterator Model::meshesEnd() {
		return meshes.end();
	}

}