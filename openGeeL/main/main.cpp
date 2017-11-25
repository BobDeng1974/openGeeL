#include <string>
#include "appmanager.h"
#include "luaconfig.h"
#include "arthousescene.h"
#include "bedroomscene.h"
#include "characterviewer.h"
#include "deerscene.h"
#include "giscene.h"
#include "raymarchscene.h"
#include "sciencescene.h"
#include "sponzascene.h"
#include "testscene.h"
#include "capsulescene.h"

int main(int argc, char* argv[]) {
	ApplicationManagerInstance instance;

	if (argc >= 2) {
		 std::string argument = argv[1];

		if (argument == "-h" || argument == "-help")
			std::cout << "Pass path of lua config file as first argument to run given scene\n";
		else {
			try {
				LUAConfigurator config(argument);
				config.run();
			}
			catch (const exception& e) {
				std::cout << e.what() << "\n";
			}
		}

		return 0;
	}

	//BedroomScene::draw();
	//DeerScene::draw();
	//ArthouseScene::draw();
	//SponzaScene::draw();
	//SponzaGIScene::draw();
	//ScienceScene::draw();
	CharacterViewer::draw();
	//CapsuleScene::draw();

	//TestScene::draw();
	//RaymarchTest::draw();

	return 0;
}
