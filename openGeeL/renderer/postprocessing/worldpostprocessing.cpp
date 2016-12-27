#include "worldpostprocessing.h"

using namespace std;

namespace geeL {

	WorldPostProcessingEffect::WorldPostProcessingEffect(string fragmentPath) 
		: PostProcessingEffect(fragmentPath) {}

	WorldPostProcessingEffect::WorldPostProcessingEffect(string vertexPath, string fragmentPath)
		: PostProcessingEffect(vertexPath, fragmentPath) {}



	list<WorldMaps> WorldPostProcessingEffect::requiredWorldMapsList() const {
		WorldMaps maps = requiredWorldMaps();

		list<WorldMaps> mapList = list<WorldMaps>();

		if ((maps & WorldMaps::RenderedImage) == WorldMaps::RenderedImage) mapList.push_back(WorldMaps::RenderedImage);
		if ((maps & WorldMaps::DiffuseRoughness) == WorldMaps::DiffuseRoughness) mapList.push_back(WorldMaps::DiffuseRoughness);
		if ((maps & WorldMaps::PositionDepth) == WorldMaps::PositionDepth) mapList.push_back(WorldMaps::PositionDepth);
		if ((maps & WorldMaps::NormalMetallic) == WorldMaps::NormalMetallic) mapList.push_back(WorldMaps::NormalMetallic);
		if (mapList.size() == 0) mapList.push_back(WorldMaps::None);

		return mapList;
	}

	list<WorldMatrices> WorldPostProcessingEffect::requiredWorldMatricesList() const {
		WorldMatrices matrices = requiredWorldMatrices();

		list<WorldMatrices> matrixList = list<WorldMatrices>();

		if ((matrices & WorldMatrices::View) == WorldMatrices::View) matrixList.push_back(WorldMatrices::View);
		if ((matrices & WorldMatrices::Projection) == WorldMatrices::Projection) matrixList.push_back(WorldMatrices::Projection);
		if (matrixList.size() == 0) matrixList.push_back(WorldMatrices::None);

		return matrixList;
	}

	list<WorldVectors> WorldPostProcessingEffect::requiredWorldVectorsList() const {
		WorldVectors vectors = requiredWorldVectors();

		list<WorldVectors> vectorList = list<WorldVectors>();

		if ((vectors & WorldVectors::CameraPosition) == WorldVectors::CameraPosition) vectorList.push_back(WorldVectors::CameraPosition);
		if ((vectors & WorldVectors::CameraDirection) == WorldVectors::CameraDirection) vectorList.push_back(WorldVectors::CameraDirection);
		if ((vectors & WorldVectors::OriginView) == WorldVectors::OriginView) vectorList.push_back(WorldVectors::OriginView);
		if (vectorList.size() == 0) vectorList.push_back(WorldVectors::None);

		return vectorList;
	}
}