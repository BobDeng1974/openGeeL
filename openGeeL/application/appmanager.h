#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#define MULTI_APPLICATION_SUPPORT 0

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

	public:
		ApplicationManager() {}
		~ApplicationManager();
		
		template<typename ...Args>
		static Application& createApplication(Args& ...args);

		static Application& getFirst();
		static std::list<Application*>::iterator applicationsBegin();
		static std::list<Application*>::iterator applicationsEnd();

	private:
		std::list<Application*> apps;
		
		ApplicationManager(const ApplicationManager& other) = delete;
		ApplicationManager& operator= (const ApplicationManager& other) = delete;

		template<typename ...Args>
		Application& createApplicationLocal(Args& ...args);

		static ApplicationManager& getInstance();

	};


	template<typename ...Args>
	inline Application& ApplicationManager::createApplicationLocal(Args& ...args) {
#if MULTI_APPLICATION_SUPPORT == 0
		if (apps.size() > 0) throw "Only one application allowed in single application mode\n";
#endif
		Application* app = new Application(args...);
		apps.push_back(app);

		return *app;
	}

	template<typename ...Args>
	inline Application& ApplicationManager::createApplication(Args& ...args) {
		return getInstance().createApplicationLocal(args...);
	}

}

#endif
