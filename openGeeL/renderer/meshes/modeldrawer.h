#ifndef MESHDRAWER_H
#define MESHDRAWER_H

namespace geeL {

class MeshFactory;


class MeshDrawer {

public:
	MeshDrawer(MeshFactory& factory);

	void draw() const;

private:
	MeshFactory& factory;


};

}

#endif
