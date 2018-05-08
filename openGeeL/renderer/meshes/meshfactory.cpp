#include <iostream>
#include <fstream>
#include <limits>
#include <mat4x4.hpp>
#include "Importer.hpp"
#include "scene.h"
#include "postprocess.h"
#include "transformation/transform.h"
#include "materials/materialfactory.h"
#include "materials/defaultmaterial.h"
#include "animation/skeleton.h"
#include "animation/bone.h"
#include "animation/animation.h"
#include "utility/vectorextension.h"
#include "utility/matrixextension.h"
#include "texturing/maptype.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"
#include "meshfactory.h"

#include "memory/defaultmemory.h"
#include "memory/simplepool.h"

using namespace std;

namespace geeL {

	MeshFactory::MeshFactory(MaterialFactory& factory)
		: factory(factory)
		//, memory(new SimplePool(500000000)) {}
		, memory(new DefaultMemory()) {}



	StaticModelRenderer MeshFactory::createSingleMeshRenderers(MemoryObject<StaticModel> model,
		SceneShader& shader, Transform& transform, bool splitTransform) {

		list<unique_ptr<StaticMeshRenderer>> renderers;
		model->iterateMeshesGeneric([&](const StaticMesh& mesh) {
			string newName = mesh.getMaterialContainer().name;
			Transform& newTransform = splitTransform ? transform.getParent()->addChild(transform) : transform;

			StaticMeshRenderer* renderer = new StaticMeshRenderer(newTransform, mesh, shader, 
				model, CullingMode::cullFront, newName);

			renderers.push_back(unique_ptr<StaticMeshRenderer>(renderer));
		});

		return renderers;
	}

	SkinnedModelRenderer MeshFactory::createSingleMeshRenderers(MemoryObject<SkinnedModel> model,
		SceneShader& shader, Transform& transform, bool splitTransform) {

		std::shared_ptr<Skeleton> skeleton = nullptr;
		list<unique_ptr<SkinnedMeshRenderer>> renderers;
		model->iterateMeshesGeneric([&](const SkinnedMesh& mesh) {
			string newName = mesh.getMaterialContainer().name;
			Transform& newTransform = splitTransform ? transform.getParent()->addChild(transform) : transform;

			//Read skeleton from first mesh
			if (skeleton == nullptr) {
				SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer(newTransform, mesh, shader,
					model, CullingMode::cullFront, newName);

				skeleton = renderer->shareSkeleton();
				renderers.push_back(unique_ptr<SkinnedMeshRenderer>(renderer));
			}
			//Reuse skeleton for other meshes
			else {
				SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer(newTransform, mesh, shader,
					model, skeleton, CullingMode::cullFront, newName);

				renderers.push_back(unique_ptr<SkinnedMeshRenderer>(renderer));
			}
		});

		return renderers;
	}


	MemoryObject<StaticModel> MeshFactory::createStaticModel(const string& filePath) {

		auto it(staticModels.find(filePath));
		//Create new model if none exist yet
		if (it == staticModels.end() 
			//or memory of existing one expired
			|| (it != staticModels.end() && it->second.expired())) {

			MemoryObject<StaticModel> model(new StaticModel(filePath), [this](StaticModel* m) { 
				//Add a custom deleter to provide a callback when resource gets destroyed
				this->onRemove(shared_ptr<StaticModel>(m));
			});

			weak_ptr<StaticModel> wModel(model);

			staticModels[filePath] = model;
			fillStaticModel(*model, filePath);
			onAdd(*model);

			return model;
		}

		weak_ptr<StaticModel> wModel = it->second;
		return MemoryObject<StaticModel>(wModel.lock());
	}

	MemoryObject<SkinnedModel> MeshFactory::createSkinnedModel(const string& filePath) {

		auto it(skinnedModels.find(filePath));
		//Create new model if none exist yet
		if (skinnedModels.find(filePath) == skinnedModels.end()
			//or memory of existing one expired
			|| (it != skinnedModels.end() && it->second.expired())) {

			MemoryObject<SkinnedModel> model(new SkinnedModel(filePath), [this](SkinnedModel* m) {
				//Add a custom deleter to provide a callback when resource gets destroyed
				this->onRemove(std::shared_ptr<SkinnedModel>(m));
			});

			weak_ptr<SkinnedModel> wModel(model);

			skinnedModels[filePath] = model;
			fillSkinnedModel(*model, filePath);
			onAdd(*model);

			return model;
		}

		weak_ptr<SkinnedModel> wModel = it->second;
		return MemoryObject<SkinnedModel>(wModel.lock());
	}

#if MULTI_THREADING_SUPPORT
	void MeshFactory::createModelParallel(const string& filePath, function<void(MemoryObject<StaticModel>)> callback) {
		workerThread.addJob([this, filePath, callback]() {
			MemoryObject<StaticModel> model = createStaticModel(filePath);
			callback(model);
		});
	}

	void MeshFactory::createModelParallel(const string& filePath, function<void(MemoryObject<SkinnedModel>)> callback) {
		workerThread.addJob([this, filePath, callback]() {
			MemoryObject<SkinnedModel> model = createSkinnedModel(filePath);
			callback(model);
		});
	}
#endif



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
		string name = node->mName.C_Str();

		//Traverse node tree
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processStaticNode(model, directory, node->mChildren[i], scene);


		//Add mesh if current node contains one
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			MemoryAllocator<Vertex> allocator(*memory);
			vector<Vertex, MemoryAllocator<Vertex>> vertices(allocator);
			MemoryAllocator<unsigned int> allocator2(*memory);
			vector<unsigned int, MemoryAllocator<unsigned int>> indices(allocator2);
			vector<MemoryObject<TextureMap>> textures;

			vertices.reserve(mesh->mNumVertices);

			processVertices(vertices, mesh);
			processIndices(indices, mesh);
			processTextures(textures, directory, mesh, scene);

			MemoryObject<DefaultMaterialContainer> mat = std::move(factory.createDefaultMaterial());
			processMaterial(*mat, mesh, scene);
			mat->addTextures(textures);

			model.addMesh(StaticMesh(name, vertices, indices, mat));
		}
	}

	void MeshFactory::fillSkinnedModel(SkinnedModel& model, std::string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | 
			aiProcess_LimitBoneWeights | aiProcess_SplitByBoneCount);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		aiNode* node = scene->mRootNode;
		Bone* rootBone = new Bone(MatrixExtension::convertMatrix(node->mTransformation));
		rootBone->setName(string(node->mName.C_Str()));
		processSkeleton(*rootBone, node);

		string directory = path.substr(0, path.find_last_of('/'));
		processSkinnedNode(model, *rootBone, directory, node, scene);

		std::unique_ptr<Skeleton> skeleton(new Skeleton(*rootBone));
		model.setSkeleton(std::move(skeleton));

		std::list<AnimationMemory*> animations;
		processAnimations(model, scene);
	}

	void MeshFactory::processSkinnedNode(SkinnedModel& model, Bone& bone,
		string directory, aiNode* node, const aiScene* scene) {

		string name = node->mName.C_Str();

		//Traverse node tree
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processSkinnedNode(model, bone, directory, node->mChildren[i], scene);

		//Add mesh if current node contains one
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];


			MemoryAllocator<SkinnedVertex> allocator(*memory);
			vector<SkinnedVertex, MemoryAllocator<SkinnedVertex>> vertices(allocator);
			MemoryAllocator<unsigned int> allocator2(*memory);
			vector<unsigned int, MemoryAllocator<unsigned int>> indices(allocator2);
			map<string, MeshBone> bones;
			vector<MemoryObject<TextureMap>> textures;

			vertices.reserve(mesh->mNumVertices);

			processVertices(vertices, mesh);
			processIndices(indices, mesh);
			processBones(vertices, bones, mesh);
			processTextures(textures, directory, mesh, scene);

			MemoryObject<DefaultMaterialContainer> mat = std::move(factory.createDefaultMaterial());
			processMaterial(*mat, mesh, scene);
			mat->addTextures(textures);

			model.addMesh(SkinnedMesh(name, vertices, indices, bones, mat));
		}
	}


	template <class V>
	void MeshFactory::processVertices(std::vector<V, MemoryAllocator<V>>& vertices, aiMesh* mesh) {

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

	void MeshFactory::processIndices(vector<unsigned int, MemoryAllocator<unsigned int>>& indices, aiMesh* mesh) {

		//Walk through each of the meshes faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];

			//Retrieve indices of the face and store them in indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}

	void MeshFactory::processBones(vector<SkinnedVertex, MemoryAllocator<SkinnedVertex>>& vertices,
		std::map<std::string, MeshBone>& bones, aiMesh* mesh) {

		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			aiBone* bone = mesh->mBones[i];
			const string& name = bone->mName.data;
			unsigned int id = i + 1;

			aiMatrix4x4& mat = bone->mOffsetMatrix;
			bones[name].offsetMatrix = MatrixExtension::convertMatrix(mat);
			bones[name].id = id;

			//Iterate over all vertices that are affected by this bone
			for (unsigned int j = 0; j < bone->mNumWeights; j++) {
				aiVertexWeight& weight = bone->mWeights[j];

				//Populate vertices with bone weights
				vertices[weight.mVertexId].addBone(id, weight.mWeight);
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


	void MeshFactory::processTextures(vector<MemoryObject<TextureMap>>& textures, std::string directory, aiMesh* mesh, const aiScene* scene) {
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, MapType::Diffuse, directory, ColorType::GammaSpace);
			loadMaterialTextures(textures, material, aiTextureType_SPECULAR, MapType::Gloss, directory, ColorType::RGBA);
			loadMaterialTextures(textures, material, aiTextureType_HEIGHT, MapType::Normal, directory, ColorType::RGBA);
			loadMaterialTextures(textures, material, aiTextureType_AMBIENT, MapType::Metallic, directory, ColorType::RGBA);
		}
	}

	bool endsWith(const std::string& str, const std::string& suffix) {
		return str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}


	void MeshFactory::loadMaterialTextures(vector<MemoryObject<TextureMap>>& textures, aiMaterial* mat,
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
				MemoryObject<TextureMap> texture = factory.createTextureMap(fileName, type,
					ct, FilterMode::Trilinear, WrapMode::Repeat, AnisotropicFilter::Medium);
				textures.push_back(texture);
			}
		}
	}

	void MeshFactory::processAnimations(SkinnedModel& model, const aiScene* scene) {

		//Add a default animation that contains original transformation data
		const Skeleton& skeleton = model.getSkeleton();
		AnimationMemory* defAnimation = new AnimationMemory("Default", 0., 0.);

		skeleton.iterateBones([&defAnimation](const Bone& bone) {
			AnimationBoneData* data = new AnimationBoneData();

			data->positions.push_back(KeyFrame(bone.getPosition(), 0.));
			data->rotations.push_back(KeyFrame(bone.getEulerAngles(), 0.));
			data->scalings.push_back(KeyFrame(bone.getScaling(), 0.));

			defAnimation->addBoneData(bone.getName(), std::unique_ptr<AnimationBoneData>(data));
		});
		
		model.addAnimation(std::unique_ptr<AnimationMemory>(defAnimation));


		//Read animations from scene
		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* anim = scene->mAnimations[i];

			std::string animationName(anim->mName.C_Str());
			AnimationMemory* animation = new AnimationMemory(
				(animationName.size() > 0) ? animationName : "Animation " + std::to_string(i + 1),
				anim->mDuration, 
				anim->mTicksPerSecond);

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

					vec3 rotation = VectorExtension::quatToEuler(r.x, r.y, r.z, r.w);
					if (isnan(rotation.x)) rotation.x = 0.f;
					if (isnan(rotation.y)) rotation.y = 0.f;
					if (isnan(rotation.z)) rotation.z = 0.f;

					data->rotations.push_back(KeyFrame(rotation, key.mTime));
				}

				//Add scaling key frames to data
				for (unsigned int k = 0; k < node->mNumScalingKeys; k++) {
					aiVectorKey& key = node->mScalingKeys[k];
					aiVector3D& s = key.mValue;

					data->scalings.push_back(KeyFrame(glm::vec3(s.x, s.y, s.z), key.mTime));
				}

				animation->addBoneData(name, std::unique_ptr<AnimationBoneData>(data));
			}

			model.addAnimation(std::unique_ptr<AnimationMemory>(animation));
		}
	}

	void MeshFactory::processSkeleton(Bone& bone, aiNode* node) {

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			aiNode* child = node->mChildren[i];
			aiMatrix4x4& mat = child->mTransformation;

			Bone* childBone = new Bone(MatrixExtension::convertMatrix(mat));
			childBone->setName(string(child->mName.C_Str()));
			bone.addChild(std::unique_ptr<Transform>(childBone));

			processSkeleton(*childBone, child);
		}
	}


}