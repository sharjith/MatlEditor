#include "TriangleMesh.h"
#include <algorithm>

void TriangleMesh::initBuffers(
	std::vector<GLuint> * indices,
	std::vector<GLfloat> * points,
	std::vector<GLfloat> * normals,
	std::vector<GLfloat> * texCoords,
	std::vector<GLfloat> * tangents
)
{

	// Must have data for indices, points, and normals
	if (indices == nullptr || points == nullptr || normals == nullptr)
		return;

	nVerts = (GLuint)indices->size();

	_buffers.push_back(_indexBuffer);
	_indexBuffer.bind();
	_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	_indexBuffer.allocate(indices->data(), static_cast<int>(indices->size() * sizeof(GLuint)));

	_buffers.push_back(_positionBuffer);
	_positionBuffer.bind();
	_positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	_positionBuffer.allocate(points->data(), static_cast<int>(points->size() * sizeof(GLfloat)));

	_buffers.push_back(_normalBuffer);
	_normalBuffer.bind();
	_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	_normalBuffer.allocate(normals->data(), static_cast<int>(normals->size() * sizeof(GLfloat)));

	if (texCoords != nullptr) 
	{
		_buffers.push_back(_texCoordBuffer);
		_texCoordBuffer.bind();
		_texCoordBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		_texCoordBuffer.allocate(texCoords->data(), static_cast<int>(texCoords->size() * sizeof(GLfloat)));
	}

	if (tangents != nullptr) 
	{		
		_buffers.push_back(_tangentBuf);
		_tangentBuf.bind();
		_tangentBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
		_tangentBuf.allocate(tangents->data(), static_cast<int>(tangents->size() * sizeof(GLfloat)));
	}

	_vertexArrayObject.bind();
		
	_indexBuffer.bind();

	// _position
	_positionBuffer.bind();
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);  // Vertex position
	_prog->enableAttributeArray("vertexPosition");
	_prog->setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3);

	// Normal
	_normalBuffer.bind();
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);  // Normal
	_prog->enableAttributeArray("vertexNormal");
	_prog->setAttributeBuffer("vertexNormal", GL_FLOAT, 0, 3);

	// Tex coords
	if (texCoords != nullptr) 
	{
		_texCoordBuffer.bind();
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(2);  // Tex coord
		_prog->enableAttributeArray("texCoord2d");
		_prog->setAttributeBuffer("texCoord2d", GL_FLOAT, 0, 2);
	}

	if (tangents != nullptr)
	{
		_tangentBuf.bind();
		//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(3);  // Tangents
		_prog->enableAttributeArray("tangentCoord");
		_prog->setAttributeBuffer("tangentCoord", GL_FLOAT, 0, 4);
	}
	

	_vertexArrayObject.release();
}

void TriangleMesh::render() 
{
	if (!_vertexArrayObject.isCreated())
		return;
	_prog->bind();
	_vertexArrayObject.bind();
	glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
	_vertexArrayObject.release();
	_prog->release();
}


TriangleMesh::~TriangleMesh()
{
	deleteBuffers();
}

void TriangleMesh::deleteBuffers()
{
	if (_buffers.size() > 0)
	{
		for (QOpenGLBuffer& buff : _buffers)
		{
			buff.destroy();
		}
			
		_buffers.clear();
	}

	if (_vertexArrayObject.isCreated()) 
	{
		_vertexArrayObject.destroy();		
	}
}

void TriangleMesh::computeBoundingSphere(std::vector<GLfloat> points)
{
	/*
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	for (int i = 0; i < points.size(); i += 3)
	{
		// X
		if (points[i] > maxX)
			maxX = points[i];
		if (points[i] < minX)
			minX = points[i];
		// Y
		if (points[i+1] > maxY)
			maxY = points[i+1];
		if (points[i+1] < minY)
			minY = points[i+1];
		// Z
		if (points[i+2] > maxZ)
			maxZ = points[i+2];
		if (points[i+2] < minZ)
			minZ = points[i+2];
	}

	QVector3D sphereCenter = QVector3D(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2, minZ + (maxZ - minZ) / 2);
	float sphereRadius = std::max((maxX - minX) / 2, std::max((maxY - minY) / 2, (maxZ - minZ) / 2));

	_boundingSphere.setCenter(sphereCenter);
	_boundingSphere.setRadius(sphereRadius);
	*/
	
	// Ritter's algorithm
	std::vector<QVector3D> aPoints;
	for (int i = 0; i < points.size(); i += 3)
	{
		aPoints.push_back(QVector3D(points[i], points[i + 1], points[i + 2]));
	}
	QVector3D xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = ymin = zmin = QVector3D(1,1,1) * INFINITY;
	xmax = ymax = zmax = QVector3D(1,1,1) * -INFINITY;
	for(auto p : aPoints)
	{
		if (p.x() < xmin.x()) xmin = p;
		if (p.x() > xmax.x()) xmax = p;
		if (p.y() < ymin.y()) ymin = p;
		if (p.y() > ymax.y()) ymax = p;
		if (p.z() < zmin.z()) zmin = p;
		if (p.z() > zmax.z()) zmax = p;
	}
	auto xSpan = (xmax - xmin).lengthSquared();
	auto ySpan = (ymax - ymin).lengthSquared();
	auto zSpan = (zmax - zmin).lengthSquared();
	auto dia1 = xmin;
	auto dia2 = xmax;
	auto maxSpan = xSpan;
	if (ySpan > maxSpan)
	{
		maxSpan = ySpan;
		dia1 = ymin; dia2 = ymax;
	}
	if (zSpan > maxSpan)
	{
		dia1 = zmin; dia2 = zmax;
	}
	auto center = (dia1 + dia2) * 0.5f;
	auto sqRad = (dia2 - center).lengthSquared();
	auto radius = sqrt(sqRad);
	for(auto p : aPoints)
	{
		float d = (p - center).lengthSquared();
		if (d > sqRad)
		{
			auto r = sqrt(d);
			radius = (radius + r) * 0.5f;
			sqRad = radius * radius;
			auto offset = r - radius;
			center = (radius * center + offset * p) / r;
		}
	}
	
	_boundingSphere.setCenter(center);
	_boundingSphere.setRadius(radius);
}

QOpenGLVertexArrayObject& TriangleMesh::getVAO()
{
	return _vertexArrayObject;
}
