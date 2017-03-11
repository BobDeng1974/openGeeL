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

#include <mat4x4.hpp>

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
			model.addMesh(processStaticMesh(directory, mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(model, directory, node->mChildren[i], scene);
	}


	StaticMesh* MeshFactory::processStaticMesh(string directory, aiMesh* mesh, const aiScene* scene) {

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<SimpleTexture*> textures;

		vertices.reserve(mesh->mNumVertices);
		
		processVertices(vertices, mesh);
		processIndices(indices, mesh);
		processTextures(textures, directory, mesh, scene);

		DefaultMaterialContainer& mat = factory.CreateMaterial();
		mat.addTextures(textures);
		mat.setRoughness(0.4f);
		mat.setMetallic(0.2f);

		return new StaticMesh(vertices, indices, mat);
	}

	SkinnedMesh* MeshFactory::processSkinnedMesh(std::string directory, aiMesh* mesh, const aiScene* scene) {

		vector<SkinnedVertex> vertices;
		vector<unsigned int> indices;
		map<string, Bone> bones;
		vector<SimpleTexture*> textures;

		vertices.reserve(mesh->mNumVertices);

		processVertices(vertices, mesh);
		processIndices(indices, mesh);
		processBones(vertices, bones, mesh);
		processTextures(textures, directory, mesh, scene);

		DefaultMaterialContainer& mat = factory.CreateMaterial();
		mat.addTextures(textures);
		mat.setRoughness(0.4f);
		mat.setMetallic(0.2f);

		return new SkinnedMesh(vertices, indices, bones, mat);
	}

	template <class V>
	void MeshFactory::processVertices(std::vector<V>& vertices, aiMesh* mesh) {

		//Walk through meshes vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			V vertex;
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
				vertex.normal = vec3(0.f, 0.f, 0.f);

			if (mesh->mTangents != nullptr) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;

				vertex.tangent = vector;
			}
			else
				vertex.tangent = vec3(0.f, 0.f, 0.f);

			if (mesh->mBitangents != nullptr) {
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;

				vertex.bitangent = vector;
			}
			else
				vertex.bitangent = vec3(0.f, 0.f, 0.f);

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
	}


	void MeshFactory::processIndices(vector<unsigned int>& indices, aiMesh* mesh) {

		//Walk through each of the meshes faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			//Retrieve indices of the face and store them in indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}


	void MeshFactory::processBones(vector<SkinnedVertex>& vertices, std::map<std::string, Bone>& bones, aiMesh* mesh) {

		unsigned int boneCount = 0;
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			aiBone* bone = mesh->mBones[i];
			string name = bone->mName.data;
			unsigned int index = 0;

			//Search for bone in bone map and add if not present
			if (bones.find(name) == bones.end()) {
				index = boneCount;

				auto mat = bone->mOffsetMatrix;
				//mat.Transpose();

				bones[name].offsetMatrix = *reinterpret_cast<mat4*>(&mat);
				bones[name].id = index;
				boneCount++;
			}

			//Iterate over all vertices that are affected by this bone
			for (unsigned int j = 0; j < bone->mNumWeights; j++) {
				aiVertexWeight weight = bone->mWeights[j];

				//Populate vertices with bone weights
				VertexBoneData& boneData = vertices[weight.mVertexId].bones;
				boneData.addBone(weight.mVertexId, weight.mWeight);
			}
		}
	}


	void MeshFactory::processTextures(vector<SimpleTexture*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene) {
		if (mesh->mMaterialIndex >= 0) {

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, Diffuse, directory, false);
			loadMaterialTextures(textures, material, aiTextureType_SPECULAR, Specular, directory, true);
			loadMaterialTextures(textures, material, aiTextureType_HEIGHT, Normal, directory, true);
			loadMaterialTextures(textures, material, aiTextureType_AMBIENT, Reflection, directory, true);

			//TODO: choose a better texture type than Emmissive
			loadMaterialTextures(textures, material, aiTextureType_EMISSIVE, Metallic, directory, true);
		}
	}


	void MeshFactory::loadMaterialTextures(vector<SimpleTexture*>& textures, aiMaterial* mat,
		aiTextureType aiType, TextureType type, string directory, bool linear) {

		for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++) {
			aiString str;
			mat->GetTexture(aiType, i, &str);

			string fileName = directory + "/" + string(str.C_Str());
			SimpleTexture& texture = factory.CreateTexture(fileName, linear, type, ColorRGBA, GL_REPEAT, Bilinear);
			textures.push_back(&texture);
		}
	}

}