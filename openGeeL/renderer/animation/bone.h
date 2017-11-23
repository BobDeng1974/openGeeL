#ifndef ANIMATIONBONE_H
#define ANIMATIONBONE_H

#include "transformation/transform.h"

namespace geeL {

	class Bone : public Transform {

	public:
		template<typename ...TransformArgs>
		Bone(TransformArgs&& ...args);

		unsigned int getBoneID() const;
		void setBoneID(unsigned int id);

	private:
		unsigned int boneID;

	};


	template<typename ...TransformArgs>
	inline Bone::Bone(TransformArgs&& ...args) 
		: Transform(std::forward<TransformArgs>(args)...) {}


}

#endif
