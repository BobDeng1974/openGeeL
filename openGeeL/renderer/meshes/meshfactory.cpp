#include <iostream>
#include "../../dependencies/assimp/Importer.hpp"
#include "../../dependencies/assimp/scene.h"
#include "../../dependencies/assimp/postprocess.h"
#include "../materials/materialfactory.h"
#include "../materials/defaultmaterial.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"
#include "meshfactory.h"

using namespace glm;
using namespace std;

namespace geeL {

	MeshFactory::MeshFactory(MaterialFactory& factory) : factory(factory) {}


	Model& MeshFactory::CreateModel(string filePath) {
		if (models.find(filePath) == models.end()) {
			models[filePath] = Model(filePath);
			fillModel(models[filePath], filePath);
		}

		return models[filePath];
	}

	MeshRenderer& MeshFactory::CreateMeshRenderer(Model& model, Transform& transform, CullingMode faceCulling, string name) {
		meshRenderer.push_back(MeshRenderer(transform, *factory.defaultShader, model, faceCulling, true, name));

		return meshRenderer.back();
	}

	MeshRenderer* MeshFactory::CreateMeshRendererManual(Model& model, Transform& transform, 
		CullingMode faceCulling, bool deferred, string name) {
		
		return new MeshRenderer(transform, deferred ? factory.getDeferredShader() 
			: factory.getForwardShader(), model, faceCulling, deferred, name);
	}

	map<string, Model>::iterator MeshFactory::modelsBegin() {
		return models.begin();
	}

	map<string, Model>::iterator MeshFactory::modelsEnd() {
		return models.end();
	}

	list<MeshRenderer>::iterator MeshFactory::rendererBegin() {
		return meshRenderer.begin();
	}

	list<MeshRenderer>::iterator MeshFactory::rendererEnd() {
		return meshRenderer.end();
	}


	void MeshFactory::fillModel(Model& model, string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		string directory = path.substr(0, path.find_last_of('/'));
		processNode(model, directory, scene->mRootNode, scene);
	}

	void MeshFactory::processNode(Model& model, string directory, aiNode* node, const aiScene* scene) {

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model.addMesh(processMesh(directory, mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(model, directory, node->mChildren[i], scene);
	}

	Mesh MeshFactory::processMesh(string directory, aiMesh* mesh, const aiScene* scene) {

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<SimpleTexture*> textures;

		//Walk through meshes vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vec3 vector;

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
				vertex.normal = vec3(0, 0, 0);

			if (mesh->mTangents != nullptr) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;

				vertex.tangent = vector;
			}
			else
				vertex.tangent = vec3(0, 0, 0);

			if (mesh->mBitangents != nullptr) {
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;

				vertex.bitangent = vector;
			}
			else
				vertex.bitangent = vec3(0, 0, 0);

			//Check for texture coordinates
			if (mesh->mTextureCoords[0]) {
				vec2 vec;

				//We make the assumption that vertices don't have more than 
				//one texture coordinate and therefore use first (0)
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else
				vertex.texCoords = vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		//Walk through each of the meshes faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			
			//Retrieve indices of the face and store them in indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		//Process materials
		if (mesh->mMaterialIndex >= 0) {

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<SimpleTexture*> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, Diffuse, directory, false);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<SimpleTexture*> specularMaps = loadMaterialTextures(material,
				aiTextureType_SPECULAR, Specular, directory, true);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<SimpleTexture*> normalMaps = loadMaterialTextures(material,
				aiTextureType_HEIGHT, Normal, directory, true);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			vector<SimpleTexture*> reflectionMaps = loadMaterialTextures(material,
				aiTextureType_AMBIENT, Reflection, directory, true);
			textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());

			//TODO: choose a better texture type than Emmissive
			vector<SimpleTexture*> metallicnMaps = loadMaterialTextures(material,
				aiTextureType_EMISSIVE, Metallic, directory, true);
			textures.insert(textures.end(), metallicnMaps.begin(), metallicnMaps.end());
		}

		DefaultMaterialContainer& mat = factory.CreateMaterial();

		mat.addTextures(textures);
		mat.setRoughness(0.4f);
		mat.setMetallic(0.2f);

		return Mesh(vertices, indices, mat);
	}

	vector<SimpleTexture*> MeshFactory::loadMaterialTextures(aiMaterial* mat,
		aiTextureType aiType, TextureType type, string directory, bool linear) {

		vector<SimpleTexture*> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++) {
			aiString str;
			mat->GetTexture(aiType, i, &str);

			string fileName = directory + "/" + string(str.C_Str());
			SimpleTexture& texture = factory.CreateTexture(fileName, linear, type, ColorRGBA, GL_REPEAT, Bilinear);
			textures.push_back(&texture);
		}

		return textures;
	}

}