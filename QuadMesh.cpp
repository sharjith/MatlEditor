#include "QuadMesh.h"



void QuadMesh::render() 
{
	if (!_vertexArrayObject.isCreated())
		return;

	_vertexArrayObject.bind();
	glDrawElements(GL_QUADS, nVerts, GL_UNSIGNED_INT, 0);
	_vertexArrayObject.release();
}

QuadMesh::~QuadMesh()
{
}

