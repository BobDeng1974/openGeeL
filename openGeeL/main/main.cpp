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

#include "lua.hpp"
#include "sol.hpp"

class vars {
public:

	int boop = 0;
};

int main() {
	//BedroomScene::draw();
	//DeerScene::draw();
	//ArthouseScene::draw();
	//SponzaScene::draw();
	//SponzaGIScene::draw();
	//ScienceScene::draw();
	//CharacterViewer::draw();
	//CapsuleScene::draw();

	//lua_State* state = luaL_newstate();
	//lua_close(state);

	sol::state lua;
	lua.new_usertype<vars>("vars", "boop", &vars::boop);
	lua.script("beep = vars.new()\n"
		"beep.boop = 1");

	getchar();

	//TestScene::draw();
	//RaymarchTest::draw();

	return 0;
}
