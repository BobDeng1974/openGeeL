#ifndef DATAEVENTLISTENER_H
#define DATAEVENTLISTENER_H

#include <functional>
#include <list>

namespace geeL {

	template<typename Data>
	class DataEventListener {

	public:
		virtual void onAdd(Data& data) = 0;
		virtual void onRemove(Data& data) = 0;

	};


	template<typename Data>
	class DataEventActuator {

	public:
		void addListener(DataEventListener<Data>& listener);
		void removeListener(DataEventListener<Data>& listener);

	protected:
		void onAdd(Data& data);
		void onRemove(Data& data);

	private:
		std::list<DataEventListener<Data>*> listeners;

	};



	template<typename Data>
	inline void DataEventActuator<Data>::addListener(DataEventListener<Data>& listener) {
		listeners.push_back(&listener);
	}

	template<typename Data>
	inline void DataEventActuator<Data>::removeListener(DataEventListener<Data>& listener) {
		listeners.remove(listener);
	}

	template<typename Data>
	inline void DataEventActuator<Data>::onAdd(Data& data) {
		for (auto it(listeners.begin()); it != listeners.end(); it++) {
			auto& listener = **it;
			listener.onAdd(data);
		}
	}

	template<typename Data>
	inline void DataEventActuator<Data>::onRemove(Data& data) {
		for (auto it(listeners.begin()); it != listeners.end(); it++) {
			auto& listener = **it;
			listener.onRemove(data);
		}
	}

}

#endif
