#ifndef DATAEVENTLISTENER_H
#define DATAEVENTLISTENER_H

#include <functional>
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
	inline void DataEventActuator<Data>::onRemove(std::shared_ptr<Data> data) {
		for (auto it(listeners.begin()); it != listeners.end(); it++) {
			auto& listener = **it;
			listener.onRemove(data);
		}
	}

}

#endif
