#ifndef DATAEVENTLISTENER_H
#define DATAEVENTLISTENER_H

#include <list>
#include <memory>

namespace geeL {

	template<typename Data>
	class DataEventListener {

	public:
		virtual void onAdd(Data& data) = 0;
		virtual void onRemove(std::shared_ptr<Data> data) = 0;

	};


	template<typename Data>
	class DataEventActuator {

	public:
		void addListener(DataEventListener<Data>& listener);
		void removeListener(DataEventListener<Data>& listener);

	protected:
		//Called after data is created/added
		void onAdd(Data& data);

		//Called when data gets removed. The data will get
		//destroyed as soon as given shared pointer expires
		void onRemove(std::shared_ptr<Data> data);

	private:
		std::list<DataEventListener<Data>*> listeners;

	};



	template<typename T>
	class ChangeListener {

	public:
		virtual void onChange(const T& t) = 0;

	};


	template<typename T>
	class ChangeActuator {

	public:
		ChangeActuator();

		void addListener(ChangeListener<T>& listener) const;
		void removeListener(ChangeListener<T>& listener) const;

	protected:
		void onChange();

	private:
		mutable std::list<ChangeListener<T>*> listeners;

	};



	template<typename Data>
	inline void DataEventActuator<Data>::addListener(DataEventListener<Data>& listener) {
		listeners.push_back(&listener);
	}

	template<typename Data>
	inline void DataEventActuator<Data>::removeListener(DataEventListener<Data>& listener) {
		listeners.remove(&listener);
	}

	template<typename Data>
	inline void DataEventActuator<Data>::onAdd(Data& data) {
		for (auto it(listeners.begin()); it != listeners.end(); it++) {
			auto& listener = **it;
			listener.onAdd(data);
		}
	}

	template<typename Data>
	inline void DataEventActuator<Data>::onRemove(std::shared_ptr<Data> data) {
		for (auto it(listeners.begin()); it != listeners.end(); it++) {
			auto* listener = *it;
			listener->onRemove(data);
		}
	}

	template<typename T>
	inline ChangeActuator<T>::ChangeActuator() {
		static_assert(std::is_base_of<ChangeActuator<T>, T>::value, "T has to have type of inheriting child class");
	}

	template<typename T>
	inline void ChangeActuator<T>::addListener(ChangeListener<T>& listener) const {
		listeners.push_back(&listener);
	}

	template<typename T>
	inline void ChangeActuator<T>::removeListener(ChangeListener<T>& listener) const {
		listeners.remove(&listener);
	}

	template<typename T>
	inline void ChangeActuator<T>::onChange() {
		T* t = static_cast<T*>(this);

		for (auto it(listeners.begin()); it != listeners.end(); it++)
			(*it)->onChange(*t);
	}

}

#endif
