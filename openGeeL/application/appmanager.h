#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <list>
#include "appglobals.h"

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
		template<typename ...Args>
		static Application& createApplication(Args&& ...args);

		static Application& getFirst();
		static std::list<Application*>::iterator applicationsBegin();
		static std::list<Application*>::iterator applicationsEnd();

		static void clear();

	private:
		static std::list<Application*> apps;
		
	};


	//RAII wrapper for memory management of application manager. 
	class ApplicationManagerInstance {

	public:
		ApplicationManagerInstance();
		~ApplicationManagerInstance();

	};


	template<typename ...Args>
	inline Application& ApplicationManager::createApplication(Args&& ...args) {
#if MULTI_APPLICATION_SUPPORT == 0
		if (apps.size() > 0) throw "Only one application allowed in single application mode\n";
#endif
		Application* app = new Application(std::forward<Args>(args)...);
		apps.push_back(app);

		return *app;
	}

}

#endif
