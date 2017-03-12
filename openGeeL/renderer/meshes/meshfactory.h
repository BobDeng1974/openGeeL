#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>
#include <list>
#include <vector>

enum   aiTextureType;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

namespace geeL {

	struct SkinnedVertex;
	struct MeshBoneData;

	class Animation;
	class MaterialFactory;
	class Model;
	class StaticModel;
	class SkinnedModel;
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class StaticMesh;
	class SkinnedMesh;
	class SimpleTexture;
	class Transform;
	enum CullingMode;

	class MeshFactory {

	public:
		MeshFactory(MaterialFactory& factory);

		MeshRenderer& CreateMeshRenderer(StaticModel& model, Transform& transform,
			CullingMode faceCulling, std::string name = "MeshRenderer");

		//Create mesh renderer on heap. Caller has to manage its memory manually
		MeshRenderer* CreateMeshRendererManual(StaticModel& model, Transform& transform,
			CullingMode faceCulling, bool deferred = true, std::string name = "MeshRenderer");

		MeshRenderer& CreateSkinnedMeshRenderer(SkinnedModel& model, Transform& transform,
			CullingMode faceCulling, std::string name = "MeshRenderer");

		//Create skinned mesh renderer on heap. Caller has to manage its memory manually
		SkinnedMeshRenderer* CreateSkinnedMeshRendererManual(SkinnedModel& model, Transform& transform,
			CullingMode faceCulling, bool deferred = true, std::string name = "MeshRenderer");

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		StaticModel& CreateStaticModel(std::string filePath);

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		SkinnedModel& CreateSkinnedModel(std::string filePath);

		std::map<std::string, StaticModel>::iterator modelsBegin();
		std::map<std::string, StaticModel>::iterator modelsEnd();

		std::list<MeshRenderer>::iterator rendererBegin();
		std::list<MeshRenderer>::iterator rendererEnd();

	private:
		MaterialFactory& factory;
		std::map<std::string, StaticModel> staticModels;
		std::map<std::string, SkinnedModel> skinnedModels;
		std::list<MeshRenderer> meshRenderer;

		void fillStaticModel(StaticModel& model, std::string path);
		void processStaticNode(StaticModel& model, std::string directory, aiNode* node, const aiScene* scene);

		void fillSkinnedModel(SkinnedModel& model, std::string path);
		void processSkinnedNode(SkinnedModel& model, std::string directory, aiNode* node, const aiScene* scene);

		template<class V>
		void processVertices(std::vector<V>& vertices, aiMesh* mesh);
		void processIndices(std::vector<unsigned int>& indices, aiMesh* mesh);
		void processBones(std::vector<SkinnedVertex>& vertices, std::map<std::string, MeshBoneData>& bones, aiMesh* mesh);
		void processTextures(std::vector<SimpleTexture*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene);
		void processAnimations(std::list<Animation*>& animations, aiMesh* mesh, const aiScene* scene);


		void loadMaterialTextures(std::vector<SimpleTexture*>& textures, aiMaterial* mat,
			aiTextureType aiType, TextureType type, std::string directory, bool linear);

	};
}

#endif 
