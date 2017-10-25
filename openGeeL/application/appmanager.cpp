#include "utility/rendertime.h"
#include "threading.h"
#include "application.h"
#include "appmanager.h"

namespace geeL {
	
	ApplicationManager::~ApplicationManager() {
		for (auto it(apps.begin()); it != apps.end(); it++)
			delete *it;
	}


	ApplicationManager* singleton = nullptr;

	ApplicationManager& ApplicationManager::getInstance() {
		if (singleton == nullptr)
			singleton = new ApplicationManager();

		return *singleton;
	}

	void ApplicationManager::removeInstance() {
		if (singleton != nullptr) {
			delete singleton;
			singleton = nullptr;
		}
	}

	std::list<Application*>::iterator ApplicationManager::applicationsBegin() {
		return getInstance().apps.begin();
	}

	std::list<Application*>::iterator ApplicationManager::applicationsEnd() {
		return getInstance().apps.end();
	}



	float RenderTime::deltaTime() {
		for (auto it(ApplicationManager::applicationsBegin()); it != ApplicationManager::applicationsEnd(); it++) {
			Application& app = **it;
			const ContinuousThread* thread = app.getCurrentThread();

			if (thread != nullptr) return thread->getTime().deltaTime();
		}

		return 0.f;
	}

}