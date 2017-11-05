#ifndef GLCLEARING_H
#define GLCLEARING_H

namespace geeL {


	enum class ClearingMethod {
		Depth = 1,
		Color = 2,
		Stencil = 4
	};


	class Clearer {


	public:
		Clearer();
		Clearer(ClearingMethod method);

		template<typename... ClearingMethods>
		Clearer(ClearingMethod method, ClearingMethods ...methods);
		
		void clear() const;
		static void setClearColor(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f);


	private:
		unsigned int i;


		void add(ClearingMethod method);

		template<typename... ClearingMethods>
		void add(ClearingMethod method, ClearingMethods ...methods);

	};


	const Clearer clearAll(ClearingMethod::Color, ClearingMethod::Depth, ClearingMethod::Stencil);
	const Clearer clearNothing;



	inline Clearer::Clearer() : i(0) {}

	inline Clearer::Clearer(ClearingMethod method) {
		add(method);
	}

	template<typename ...ClearingMethods>
	inline Clearer::Clearer(ClearingMethod method, ClearingMethods ...methods) {
		add(method, methods...);
	}


	inline void Clearer::add(ClearingMethod method) {
		i = i | (unsigned int) method;
	}

	template<typename ...ClearingMethods>
	inline void Clearer::add(ClearingMethod method, ClearingMethods ...methods) {
		add(method);
		add(methods...);
	}

}

#endif
