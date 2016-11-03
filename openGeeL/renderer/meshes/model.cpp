#include <iostream>
#include "../shader/shader.h"
#include "../materials/defaultmaterial.h"
#include "../materials/materialfactory.h"
#include "../texturing/simpletexture.h"
#include "../../dependencies/assimp/Importer.hpp"
#include "../../dependencies/assimp/scene.h"
#include "../../dependencies/assimp/postprocess.h"
#include "mesh.h"
#include "model.h"

using namespace std;

namespace geeL {

	void Model::loadModel(MaterialFactory& factory) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(factory, scene->mRootNode, scene);
	}

	void Model::draw(bool shade) const {
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].draw(shade);
	}

	void Model::drawInstanced(bool shade) const {
		//TODO: implement this
	}

	void Model::draw(vector<Material*> customMaterials) const {
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

	void Model::draw(map<unsigned int, Material*> customMaterials) const {

		for (map<unsigned int, Material*>::iterator it = customMaterials.begin(); 
			it != customMaterials.end(); it++) {
			
			unsigned int i = it->first;
			Material& mat = *it->second;
			meshes[i].draw(mat);
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

	vector<SimpleTexture*> loadMaterialTextures(MaterialFactory& factory, aiMaterial* mat, 
		aiTextureType aiType, TextureType type, string directory, bool linear) {
		
		vector<SimpleTexture*> textures;

		for (GLuint i = 0; i < mat->GetTextureCount(aiType); i++) {
			aiString str;
			mat->GetTexture(aiType, i, &str);

			string fileName = directory + "/" + string(str.C_Str());
			SimpleTexture& texture = factory.CreateTexture(fileName, linear, type, ColorRGBA, GL_REPEAT, Bilinear);
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
			
			if (mesh->mNormals != nullptr) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;

				vertex.normal = vector;
			}
			else
				vertex.normal = glm::vec3(0, 0, 0);

			if (mesh->mTangents != nullptr) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;

				vertex.tangent = vector;
			}
			else
				vertex.tangent = glm::vec3(0, 0, 0);
			
			if (mesh->mBitangents != nullptr) {
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;

				vertex.bitangent = vector;
			}
			else
				vertex.bitangent = glm::vec3(0, 0, 0);

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

		// Now walk through each of the mesh's faces and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Process materials
		if (mesh->mMaterialIndex >= 0) {

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<SimpleTexture*> diffuseMaps = loadMaterialTextures(factory, material, 
				aiTextureType_DIFFUSE, Diffuse, directory, false);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<SimpleTexture*> specularMaps = loadMaterialTextures(factory, material, 
				aiTextureType_SPECULAR, Specular, directory, true);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<SimpleTexture*> normalMaps = loadMaterialTextures(factory, material,
				aiTextureType_HEIGHT, Normal, directory, true);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			vector<SimpleTexture*> reflectionMaps = loadMaterialTextures(factory, material, 
				aiTextureType_AMBIENT, Reflection, directory, true);
			textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
		}

		DefaultMaterial& mat = factory.CreateMaterial();
		mat.addTextures(textures);
		mat.setShininess(64.f);

		return Mesh(vertices, indices, mat);
	}

	vector<Mesh>::iterator Model::meshesBegin() {
		return meshes.begin();
	}

	vector<Mesh>::iterator Model::meshesEnd() {
		return meshes.end();
	}

	const Mesh& Model::getMesh(unsigned int index) {
		if (index < meshes.size()) {
			return meshes[index];
		}

		throw std::out_of_range("Index out of range");
	}

	int Model::meshCount() const {
		return meshes.size();
	}

}