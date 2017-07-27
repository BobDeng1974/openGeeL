#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>
#include <list>
#include <string>

namespace geeL {

	//Generic wrapper base class for properties that can be 
	//monitored by user-specified observer functions
	template<class T>
	class PropertyBase {

	public:
		PropertyBase(const T& value);
		PropertyBase(const PropertyBase<T>& other);

		void remove();

		//Add listener function to this property. It can be specified if
		//function should immediately be invoked with current state or not
		void addListener(std::function<void(const T&)> listener, bool invoke = false);

		operator T() const;
		operator T*() const;

		PropertyBase<T>& operator= (const PropertyBase<T>& other);
		PropertyBase<T>& operator= (const T& other);

	protected:
		T value;

		virtual void onChange();

	private:
		std::list<std::function<void(const T&)>>* listeners;

	};


	//Property class for singular datatypes like floats or integers
	template<class T>
	class Property : public PropertyBase<T> {

	public:
		Property(const T& value);
		Property(const PropertyBase<T>& other);
		~Property();

	};

	
	//Property class for one-dimensional collections (of properties)
	//where the manipulation of single elements will trigger observer 
	//calls as well
	//Note: T specifies collection class and V underlying class of 
	//listed property elements
	template<class T, class V>
	class NestedProperty1x : public PropertyBase<T> {

	public:
		NestedProperty1x(const T& value);
		NestedProperty1x(const PropertyBase<T>& other);
		~NestedProperty1x();

		T& getValue();
		PropertyBase<V>& operator[] (int index);

	private:
		void init();

	};


	//Property class for two-dimensional collections (of properties)
	//where the manipulation of single elements will trigger observer 
	//calls as well
	//Note: T specifies collection class, U specifies the inner 'row'
	//class and V underlying class of listed property elements
	template<class T, class U, class V>
	class NestedProperty2x : public PropertyBase<T> {

	public:
		NestedProperty2x(const T& value);
		NestedProperty2x(const PropertyBase<T>& other);
		~NestedProperty2x();

		T& getValue();
		U& operator[] (int index);

	private:
		void init();

	};



	template<class T>
	inline Property<T>::Property(const T& value) : PropertyBase<T>(value) {}

	template<class T>
	inline Property<T>::Property(const PropertyBase<T>& other) : PropertyBase<T>(other) {}

	template<class T>
	inline Property<T>::~Property() {
		remove();
	}
	


	template<class T>
	inline PropertyBase<T>::PropertyBase(const T& value) : value(value) {
		listeners = new std::list<std::function<void(const T&)>>();
	}

	template<class T>
	inline PropertyBase<T>::PropertyBase(const PropertyBase<T>& other) : value(other.value) {
		listeners = new std::list<std::function<void(const T&)>>();
	}

	template<class T>
	inline void PropertyBase<T>::remove() {
		delete listeners;
	}

	template<class T>
	inline void PropertyBase<T>::addListener(std::function<void(const T&)> listener, bool invoke) {
		listeners->push_back(listener);
		if(invoke) listener(value);
	}

	template<class T>
	inline PropertyBase<T>::operator T() const {
		return value;
	}

	template<class T>
	inline PropertyBase<T>::operator T*() const {
		return &value;
	}

	template<class T>
	inline PropertyBase<T>& PropertyBase<T>::operator=(const PropertyBase<T>& other) {
		if (&other != this) {
			value = other.value;
			onChange();
		}

		return *this;
	}

	template<class T>
	inline PropertyBase<T>& PropertyBase<T>::operator=(const T& other) {
		value = other;
		onChange();

		return *this;
	}


	template<class T>
	inline void PropertyBase<T>::onChange() {
		for (auto it(listeners->begin()); it != listeners->end(); it++)
			(*it)(value);
	}



	template<class T, class V>
	inline NestedProperty1x<T, V>::NestedProperty1x(const T& value) : PropertyBase<T>(value) {
		init();
	}

	template<class T, class V>
	inline NestedProperty1x<T, V>::NestedProperty1x(const PropertyBase<T>& other) : PropertyBase<T>(other) {
		init();
	}

	template<class T, class V>
	inline NestedProperty1x<T, V>::~NestedProperty1x() {
		for (int i = 0; i < value.length(); i++) {
			PropertyBase<V>& v = value[i];
			v.remove();
		}
	}

	template<class T, class V>
	inline T& NestedProperty1x<T, V>::getValue() {
		return value;
	}

	template<class T, class V>
	inline PropertyBase<V>& NestedProperty1x<T, V>::operator[](int index) {
		return value[index];
	}

	template<class T, class V>
	inline void NestedProperty1x<T, V>::init() {
		//Add listeners to all child properties that call onChange method of this parent property
		auto callback = [this](const V& val) { this->onChange(); };

		for (int i = 0; i < value.length(); i++) {
			PropertyBase<V>& v = value[i];
			v.addListener(callback);
		}
	}


	
	template<class T, class U, class V>
	inline NestedProperty2x<T, U, V>::NestedProperty2x(const T & value) : PropertyBase<T>(value) {
		init();
	}

	template<class T, class U, class V>
	inline NestedProperty2x<T, U, V>::NestedProperty2x(const PropertyBase<T>& other) : PropertyBase<T>(other) {
		init();
	}

	template<class T, class U, class V>
	inline NestedProperty2x<T, U, V>::~NestedProperty2x() {
		for (int i = 0; i < value.length(); i++) {
			U& row = value[i];

			for (int j = 0; j < row.length(); j++) {
				PropertyBase<V>& v = row[j];
				v.remove();
			}
		}
	}

	template<class T, class U, class V>
	inline T& NestedProperty2x<T, U, V>::getValue() {
		return value;
	}

	template<class T, class U, class V>
	inline U& NestedProperty2x<T, U, V>::operator[](int index) {
		return value[index];
	}

	template<class T, class U, class V>
	inline void NestedProperty2x<T, U, V>::init() {
		//Add listeners to all child properties that call onChange method of this parent property
		auto callback = [this](const V& val) { this->onChange(); };

		for (int i = 0; i < value.length(); i++) {
			U& row = value[i];

			for (int j = 0; j < row.length(); j++) {
				PropertyBase<V>& v = row[j];
				v.addListener(callback);
			}
		}
	}

}

#endif
