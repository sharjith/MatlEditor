#pragma once

#include <vector>
#include "Drawable.h"
#include "BoundingSphere.h"

class TriangleMesh : public Drawable 
{

public:
	TriangleMesh(QOpenGLShaderProgram* prog, const QString name) : Drawable(prog) 
	{
		_name = name; 

		_indexBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		_positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		_normalBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		_texCoordBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		_tangentBuf = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

		_indexBuffer.create();
		_positionBuffer.create();
		_normalBuffer.create(); 
		_texCoordBuffer.create();
		_tangentBuf.create();

		_vertexArrayObject.create();
	}

    virtual ~TriangleMesh();
    virtual void render();
	virtual BoundingSphere getBoundingSphere() const { return _boundingSphere; }

	virtual QOpenGLVertexArrayObject& getVAO();
	virtual QString getName() const 
	{ 
		return _name; 
	}

protected: // methods
	virtual void initBuffers(
		std::vector<GLuint> * indices,
		std::vector<GLfloat> * points,
		std::vector<GLfloat> * normals,
		std::vector<GLfloat> * texCoords = nullptr,
		std::vector<GLfloat> * tangents = nullptr
	);

	virtual void deleteBuffers();

	void computeBoundingSphere(std::vector<GLfloat> points);

protected:

	QOpenGLBuffer _indexBuffer;
	QOpenGLBuffer _positionBuffer;
	QOpenGLBuffer _normalBuffer;
	QOpenGLBuffer _texCoordBuffer;
	QOpenGLBuffer _tangentBuf;

	GLuint nVerts;     // Number of vertices
	QOpenGLVertexArrayObject _vertexArrayObject;        // The Vertex Array Object

	// Vertex buffers
	std::vector<QOpenGLBuffer> _buffers;

	BoundingSphere _boundingSphere;

	QString _name;

};
