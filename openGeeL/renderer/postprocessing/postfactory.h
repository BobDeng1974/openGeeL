#ifndef POSTFACTORY_H
#define POSTFACTORY_H

#include "postprocessing.h"

class PostProcessingFactory {

public:
	template<typename T, typename... Args>
	T createEffect(Args&... args);

	template<typename T, typename V, typename... Args>
	T createEffect(V& v, const Args&... args);

};


template<typename T, typename... Args>
inline T PostProcessingFactory::createEffect(Args& ...args) {
	return T(args...);
}

template<typename T, typename V, typename ...Args>
inline T PostProcessingFactory::createEffect(V& v, const Args & ...args) {
	return T(v, args...);
}

#endif