#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <list>

namespace geeL {

	class Application;
	class ContinuousThread;
	class InputManager;
	class Renderer;
	class RenderWindow;

	//Static control class that can manage multiple applications in the same program.
	//All applications should be instanced by this class to ensure correct render time data
	class ApplicationManager {
		friend class ApplicationLock;

	public:
		~ApplicationManager();
		
		template<typename ...Args>
		static Application& createApplication(Args& ...args);

		static std::list<Application*>::iterator applicationsBegin();
		static std::list<Application*>::iterator applicationsEnd();

	private:
		std::list<Application*> apps;

		ApplicationManager() {}
		ApplicationManager(const ApplicationManager& other) = delete;
		ApplicationManager& operator= (const ApplicationManager& other) = delete;

		template<typename ...Args>
		Application& createApplicationLocal(Args& ...args);

		static ApplicationManager& getInstance();
		static void removeInstance();

	};


	class ApplicationLock {

	public:
		ApplicationLock();
		~ApplicationLock();

	};



	template<typename ...Args>
	inline Application& ApplicationManager::createApplicationLocal(Args& ...args) {
		Application* app = new Application(args...);
		apps.push_back(app);

		return *app;
	}

	template<typename ...Args>
	inline Application& ApplicationManager::createApplication(Args& ...args) {
		return getInstance().createApplicationLocal(args...);
	}



	inline ApplicationLock::ApplicationLock() {
		ApplicationManager::getInstance();
	}

	inline ApplicationLock::~ApplicationLock() {
		ApplicationManager::removeInstance();
	}

}

#endif
