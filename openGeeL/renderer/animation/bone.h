#ifndef ANIMATIONBONE_H
#define ANIMATIONBONE_H

#include "transformation/transform.h"

namespace geeL {

	using Bone = Transform;

	/*
	class Bone : public Transform {

	public:
		template<typename ...TransformArgs>
		Bone(TransformArgs&& ...args);
		Bone(const Bone& other); //Note: Gets ignored because templated construcor gets preferred
		

		Transform& addBone(const Bone& child);
		void iterateBones(std::function<void(Bone&)> function);

	};


	template<typename ...TransformArgs>
	inline Bone::Bone(TransformArgs&& ...args) 
		: Transform(std::forward<TransformArgs>(args)...) {}
	*/

}

#endif
