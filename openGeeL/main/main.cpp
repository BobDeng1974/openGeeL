#include <string>
#include "appmanager.h"
#include "luaconfig.h"
#include "arthousescene.h"
#include "bedroomscene.h"
#include "characterviewer.h"
#include "deerscene.h"
#include "modelviewer.h"
#include "raymarchscene.h"
#include "sciencescene.h"
#include "sponzascene.h"
#include "testscene.h"
#include "capsulescene.h"

#include "simplepool.h"
#include "defaultmemory.h"

using namespace geeL::memory;


struct Foo {

	int i;
	float f;

	Foo(int i, float f) : i(i), f(f) {}

};

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
			catch (const std::exception& e) {
				std::cout << e.what() << "\n";
			}
		}

		return 0;
	}

	//BedroomScene::draw();
	//DeerScene::draw();
	//ArthouseScene::draw();
	//SponzaGIScene::draw();
	//ScienceScene::draw();
	//CharacterViewer::draw();
	//ModelViewer::draw();
	//CapsuleScene::draw();

	//TestScene::draw();
	//RaymarchTest::draw();


	//SimplePool pool(1000);
	DefaultMemory pool;

	//Foo* ayy = new(pool.allocate(sizeof(Foo))) Foo(5, 13.4f);

	Foo* ayy = reinterpret_cast<Foo*>(pool.allocate(sizeof(Foo)));
	ayy->Foo::Foo(5, 13.5f);


	std::cout << ayy->f << ": "  << ": " << sizeof(Foo) << "\n";

	pool.deallocate(ayy);

	getchar();

	return 0;
}
