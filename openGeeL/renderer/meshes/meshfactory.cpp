#include <iostream>
#include <fstream>
#include <limits>
#include <mat4x4.hpp>
#include "../../dependencies/assimp/Importer.hpp"
#include "../../dependencies/assimp/scene.h"
#include "../../dependencies/assimp/postprocess.h"
#include "../transformation/transform.h"
#include "../materials/materialfactory.h"
#include "../materials/defaultmaterial.h"
#include "../animation/skeleton.h"
#include "../animation/animation.h"
#include "../utility/vectorextension.h"
#include "../utility/matrixextension.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"
#include "skinnedrenderer.h"
#include "meshfactory.h"

using namespace std;

namespace geeL {

	MeshFactory::MeshFactory(MaterialFactory& factory) : factory(factory) {}

	MeshFactory::~MeshFactory() {
		for (auto it = meshRenderer.begin(); it != meshRenderer.end(); it++)
			delete *it;
	}


	MeshRenderer& MeshFactory::CreateMeshRenderer(StaticModel& model, 
		Transform& transform, CullingMode faceCulling, const string& name) {
		
		return CreateMeshRenderer(model, factory.getDefaultShader(DefaultShading::DeferredStatic), 
			transform, faceCulling, name);
	}

	MeshRenderer& MeshFactory::CreateMeshRenderer(StaticModel& model, SceneShader& shader, Transform& transform,
		CullingMode faceCulling, const string& name) {

		MeshRenderer* renderer = new MeshRenderer(transform, shader, model, faceCulling, name);

		meshRenderer.push_back(renderer);
		return *renderer;
	}


	SkinnedMeshRenderer& MeshFactory::CreateSkinnedMeshRenderer(SkinnedModel& model, Transform& transform,
		CullingMode faceCulling, const string& name) {

		return CreateSkinnedMeshRenderer(model, factory.getDefaultShader(DefaultShading::DeferredSkinned), 
			transform, faceCulling, name);
	}

	SkinnedMeshRenderer& MeshFactory::CreateSkinnedMeshRenderer(SkinnedModel& model, SceneShader& shader, 
		Transform& transform, CullingMode faceCulling, const string& name) {

		SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer(transform, shader, 
			model, faceCulling, name);

		meshRenderer.push_back(renderer);
		return *renderer;
	}

	StaticModel& MeshFactory::CreateStaticModel(string filePath) {
		if (staticModels.find(filePath) == staticModels.end()) {
			staticModels[filePath] = StaticModel(filePath);
			fillStaticModel(staticModels[filePath], filePath);
		}

		return staticModels[filePath];
	}

	SkinnedModel& MeshFactory::CreateSkinnedModel(string filePath) {
		if (skinnedModels.find(filePath) == skinnedModels.end()) {
			skinnedModels[filePath] = SkinnedModel(filePath);
			fillSkinnedModel(skinnedModels[filePath], filePath);
		}

		return skinnedModels[filePath];
	}


	void MeshFactory::fillStaticModel(StaticModel& model, string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		string directory = path.substr(0, path.find_last_of('/'));
		processStaticNode(model, directory, scene->mRootNode, scene);
	}

	void MeshFactory::processStaticNode(StaticModel& model, string directory, aiNode* node, const aiScene* scene) {

		//Add mesh if current node contains one
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<TextureMap*> textures;

			vertices.reserve(mesh->mNumVertices);

			processVertices(vertices, mesh);
			processIndices(indices, mesh);
			processTextures(textures, directory, mesh, scene);

			DefaultMaterialContainer& mat = factory.CreateMaterial();
			mat.addTextures(textures);
			processMaterial(mat, mesh, scene);

			model.addMesh(StaticMesh(vertices, indices, mat));
		}

		//Traverse node tree
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processStaticNode(model, directory, node->mChildren[i], scene);

	}

	void MeshFactory::fillSkinnedModel(SkinnedModel& model, std::string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		std::list<Animation*> animations;
		processAnimations(model, scene);

		aiNode* node = scene->mRootNode;
		Transform* rootBone = new Transform(MatrixExtension::convertMatrix(node->mTransformation));
		rootBone->setName(string(node->mName.C_Str()));

		string directory = path.substr(0, path.find_last_of('/'));
		processSkinnedNode(model, *rootBone, directory, node, scene);

		Skeleton* skeleton = new Skeleton(rootBone);
		model.setSkeleton(skeleton);
	}

	void MeshFactory::processSkinnedNode(SkinnedModel& model, Transform& bone, 
		string directory, aiNode* node, const aiScene* scene) {

		//Add mesh if current node contains one
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			vector<SkinnedVertex> vertices;
			vector<unsigned int> indices;
			map<string, MeshBoneData> bones;
			vector<TextureMap*> textures;

			vertices.reserve(mesh->mNumVertices);

			processVertices(vertices, mesh);
			processIndices(indices, mesh);
			processBones(vertices, bones, mesh);
			processTextures(textures, directory, mesh, scene);

			DefaultMaterialContainer& mat = factory.CreateMaterial();
			mat.addTextures(textures);
			processMaterial(mat, mesh, scene);

			model.addMesh(SkinnedMesh(vertices, indices, bones, mat));
		}

		//Traverse node tree
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			aiNode* child = node->mChildren[i];
			aiMatrix4x4& mat = child->mTransformation;
			Transform* trans = new Transform(MatrixExtension::convertMatrix(mat));
			trans->setName(string(child->mName.C_Str()));
			bone.AddChild(trans);

			processSkinnedNode(model, *trans, directory, child, scene);
		}
	}


	template <class V>
	void MeshFactory::processVertices(std::vector<V>& vertices, aiMesh* mesh) {

		//Walk through meshes vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			V vertex;
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
				vertex.normal = glm::vec3(0.f);

			if (mesh->mTangents != nullptr) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;

				vertex.tangent = vector;
			}
			else
				vertex.tangent = glm::vec3(0.f);

			if (mesh->mBitangents != nullptr) {
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;

				vertex.bitangent = vector;
			}
			else
				vertex.bitangent = glm::vec3(0.f);

			//Check for texture coordinates
			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;

				//We make the assumption that vertices don't have more than 
				//one texture coordinate and therefore use first (0)
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else
				vertex.texCoords = glm::vec2(0.f, 0.f);

			vertices.push_back(vertex);
		}
	}


	void MeshFactory::processIndices(vector<unsigned int>& indices, aiMesh* mesh) {

		//Walk through each of the meshes faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];

			//Retrieve indices of the face and store them in indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}


	void MeshFactory::processBones(vector<SkinnedVertex>& vertices, std::map<std::string, MeshBoneData>& bones, aiMesh* mesh) {

		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			aiBone* bone = mesh->mBones[i];
			string name = bone->mName.data;

			aiMatrix4x4& mat = bone->mOffsetMatrix;
			bones[name].offsetMatrix = MatrixExtension::convertMatrix(mat);

			//Iterate over all vertices that are affected by this bone
			for (unsigned int j = 0; j < bone->mNumWeights; j++) {
				aiVertexWeight weight = bone->mWeights[j];

				//Populate vertices with bone weights
				vertices[j].addBone(j, weight.mWeight);
			}
		}
	}

	void MeshFactory::processMaterial(DefaultMaterialContainer& mat, aiMesh* mesh, const aiScene* scene) {
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiString name;
			if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, name))
				mat.name = name.data;

			glm::vec3 color = glm::vec3();
			aiColor3D buffer;

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, buffer)) {
				color = glm::vec3(buffer.r, buffer.g, buffer.b);
				mat.setColor(color);
			}
			else
				mat.setColor(glm::vec3(0.5f));

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, buffer)) {
				float specular = std::min((buffer.r + buffer.g + buffer.b) / 3.f, 1.f);
				mat.setRoughness(1.f - specular);
			}
			else
				mat.setRoughness(0.4f);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, buffer)) {
				float metallic = std::min((buffer.r + buffer.g + buffer.b) / 3.f, 1.f);
				mat.setMetallic(metallic);
			}
			else
				mat.setMetallic(0.2f);
		}
	}


	void MeshFactory::processTextures(vector<TextureMap*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene) {
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, MapType::Diffuse, directory, ColorType::GammaSpace);
			loadMaterialTextures(textures, material, aiTextureType_SPECULAR, MapType::Specular, directory, ColorType::RGBA);
			loadMaterialTextures(textures, material, aiTextureType_HEIGHT, MapType::Normal, directory, ColorType::RGBA);
			//loadMaterialTextures(textures, material, aiTextureType_AMBIENT, MapType::Reflection, directory, ColorType::RGBA);
			loadMaterialTextures(textures, material, aiTextureType_EMISSIVE, MapType::Metallic, directory, ColorType::RGBA);
		}
	}

	bool endsWith(const std::string& str, const std::string& suffix) {
		return str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}


	void MeshFactory::loadMaterialTextures(vector<TextureMap*>& textures, aiMaterial* mat,
		aiTextureType aiType, MapType type, string directory, ColorType colorType) {

		for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++) {
			aiString str;
			mat->GetTexture(aiType, i, &str);

			std::string name = string(str.C_Str());
			//Dirty quick fix to detect linear spaced TGA images
			ColorType ct = (type == MapType::Diffuse && endsWith(name, ".tga")) ? ColorType::RGBA : colorType;

			string fileName = directory + "/" + name;
			std::ifstream ifile(fileName);

			if ((bool)ifile) {
				TextureMap& texture = factory.CreateTextureMap(fileName, type,
					ct, WrapMode::Repeat, FilterMode::Bilinear, AnisotropicFilter::Medium);
				textures.push_back(&texture);
			}
		}
	}

	void MeshFactory::processAnimations(SkinnedModel& model, const aiScene* scene) {

		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* anim = scene->mAnimations[i];
			Animation* animation = new Animation(string(anim->mName.C_Str()),
				anim->mDuration, anim->mTicksPerSecond);

			//Add key frame data to animation
			for (unsigned int j = 0; j < anim->mNumChannels; j++) {
				aiNodeAnim* node = anim->mChannels[j];
				string name = string(node->mNodeName.C_Str());
				AnimationBoneData* data = new AnimationBoneData();

				//Add position key frames to data
				for (unsigned int k = 0; k < node->mNumPositionKeys; k++) {
					aiVectorKey& key = node->mPositionKeys[k];
					aiVector3D& p = key.mValue;

					data->positions.push_back(KeyFrame(glm::vec3(p.x, p.y, p.z), key.mTime));
				}

				//Add rotation key frames to data
				for (unsigned int k = 0; k < node->mNumRotationKeys; k++) {
					aiQuatKey& key = node->mRotationKeys[k];
					aiQuaternion& r = key.mValue;
					
					data->rotations.push_back(KeyFrame(VectorExtension::quatToEuler(r.x, r.y, r.z, r.w), key.mTime));
				}

				//Add scaling key frames to data
				for (unsigned int k = 0; k < node->mNumScalingKeys; k++) {
					aiVectorKey& key = node->mScalingKeys[k];
					aiVector3D& s = key.mValue;

					data->scalings.push_back(KeyFrame(glm::vec3(s.x, s.y, s.z), key.mTime));
				}

				animation->addBoneData(name, data);
			}

			model.addAnimation(animation);
		}
	}


	map<string, StaticModel>::iterator MeshFactory::modelsBegin() {
		return staticModels.begin();
	}

	map<string, StaticModel>::iterator MeshFactory::modelsEnd() {
		return staticModels.end();
	}

	list<MeshRenderer*>::iterator MeshFactory::rendererBegin() {
		return meshRenderer.begin();
	}

	list<MeshRenderer*>::iterator MeshFactory::rendererEnd() {
		return meshRenderer.end();
	}

}