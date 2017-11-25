#include "utility/rendertime.h"
#include "threading.h"
#include "application.h"
#include "appmanager.h"

namespace geeL {
	
	std::list<Application*> ApplicationManager::apps;


	Application& ApplicationManager::getFirst() {
		return **ApplicationManager::apps.begin();
	}

	std::list<Application*>::iterator ApplicationManager::applicationsBegin() {
		return ApplicationManager::apps.begin();
	}

	std::list<Application*>::iterator ApplicationManager::applicationsEnd() {
		return ApplicationManager::apps.end();
	}

	void ApplicationManager::clear() {
		for (auto it(applicationsBegin()); it != applicationsEnd(); it++) {
			Application* app = *it;
			delete app;
		}
	}


	ApplicationManagerInstance::ApplicationManagerInstance() {}

	ApplicationManagerInstance::~ApplicationManagerInstance() {
		ApplicationManager::clear();
	}



	float RenderTime::deltaTime() {
#if MULTI_APPLICATION_SUPPORT
		for (auto it(ApplicationManager::applicationsBegin()); it != ApplicationManager::applicationsEnd(); it++) {
			Application& app = **it;
			const ContinuousThread* thread = app.getCurrentThread();

			if (thread != nullptr) return thread->getTime().deltaTime();
		}
		
		return 0.f;

#else
		Application& app = ApplicationManager::getFirst();
		return app.getCurrentTime().deltaTime();

#endif
	}

	
}