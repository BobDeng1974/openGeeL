#ifndef MESHDRAWER_H
#define MESHDRAWER_H

namespace geeL {

class MeshFactory;
class Shader;


class MeshDrawer {

public:
	MeshDrawer(MeshFactory& factory);

	void draw() const;

	//Draw all objects with given shader
	void draw(const Shader& shader) const;

private:
	MeshFactory& factory;


};

}

#endif
