#ifndef GLVIEW_H
#define GLVIEW_H

#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_4_5_Core>
#include <QImage>
#include <QColor>

#include <math.h>
#include "GLCamera.h"
#include "BoundingSphere.h"

/* Custom OpenGL Viewer Widget */

class TextRenderer;
class TriangleMesh;
class SphericalHarmonicsEditor;
class SuperToroidEditor;
class SuperEllipsoidEditor;
class SpringEditor;
class ClippingPlanesEditor;
class GraysKleinEditor;

enum class ViewMode { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK, ISOMETRIC, DIMETRIC, TRIMETRIC, NONE };
enum class ViewProjection { ORTHOGRAPHIC, PERSPECTIVE };

class GLView : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
	friend class ClippingPlanesEditor;
	Q_OBJECT
public:
	GLView(QWidget *parent = 0, const char *name = 0);
	~GLView();
	void changeModel(bool forward);
	void updateView();
	void setTexture(QImage img);

	void resizeView(int w, int h) { resizeGL(w, h); }
	void setViewMode(ViewMode mode);
	void setProjection(ViewProjection proj);

	void setMultiView(bool active) { _bMultiView = active; }

	void fitAll();

	void beginWindowZoom();
	void endWindowZoom();

	void setModelNum(const int& num);
	int getModelNum() const
	{
		return _modelNum;
	}

	void showClippingPlaneEditor(bool show);

	std::vector<TriangleMesh*> getMeshStore() const { return _meshStore; }

public:
	QVector4D _ambiLight;
	QVector4D _diffLight;
	QVector4D _specLight;

	QVector4D _ambiMat;
	QVector4D _diffMat;
	QVector4D _specMat;
	QVector4D _emmiMat;

	QVector4D _specRef;
    GLfloat _opacity;
    GLfloat _shine;

	QVector3D _lightPosition;

	bool _bHasTexture;
	bool _bShaded;

	bool _bWindowZoomActive;

    void updateViewBoundingSphere();

signals:
	void modelChanged(int num);
	void windowZoomEnded();

public slots:
	void animateViewChange();
	void animateFitAll();
	void animateWindowZoom();

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void render();

	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);

	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
	void moveEvent(QMoveEvent *event);

    QRect getViewportFromPoint(const QPoint& pixel);
    QRect getClientRectFromPoint(const QPoint& pixel);
    QVector3D get3dTranslationVectorFromMousePoints(const QPoint& start, const QPoint& end);


private:
    int _modelNum;
	QImage _texImage, _texBuffer;
	TextRenderer* _textRenderer;
	QString _modelName;

	QVector3D _currentTranslation;
	QQuaternion _currentRotation;
    GLfloat _slerpStep;
    GLfloat _slerpFrac;

    GLfloat _currentViewRange;
    GLfloat _scaleFrac;

    GLfloat _viewRange;
	float _viewBoundingSphereDia;
    GLfloat _FOV;

	bool _bLeftButtonDown;
	QPoint _leftButtonPoint;

	bool _bRightButtonDown;
	QPoint _rightButtonPoint;

	bool _bMiddleButtonDown;
	QPoint _middleButtonPoint;

	QRubberBand* _rubberBand;
	QVector3D _rubberBandPan;
    GLfloat _rubberBandZoomRatio;

	bool _bMultiView;

    GLfloat _clipXCoeff;
    GLfloat _clipYCoeff;
    GLfloat _clipZCoeff;

	bool _clipXEnabled;
	bool _clipYEnabled;
	bool _clipZEnabled;

	bool _clipXFlipped;
	bool _clipYFlipped;
	bool _clipZFlipped;

	QMatrix4x4 _projectionMatrix, _viewMatrix, _modelMatrix;
	QMatrix4x4 _modelViewMatrix;
	QMatrix4x4 _viewportMatrix;

	QOpenGLShaderProgram*     _fgShader;

	QOpenGLShaderProgram     _textShader;
	GLuint                   _texture;

	QOpenGLShaderProgram     _bgShader;
	QOpenGLVertexArrayObject _bgVAO;

	QOpenGLShaderProgram     _bgSplitShader;
	QOpenGLVertexArrayObject _bgSplitVAO;
	QOpenGLBuffer _bgSplitVBO;

	std::vector<TriangleMesh*> _meshStore;

	SphericalHarmonicsEditor* _sphericalHarmonicsEditor;
	SuperToroidEditor* _superToroidEditor;
	SuperEllipsoidEditor* _superEllipsoidEditor;
	SpringEditor* _springEditor;
	GraysKleinEditor*		_graysKleinEditor;
	ClippingPlanesEditor*			_clippingPlanesEditor;

	ViewMode _viewMode;
	ViewProjection _projection;

	GLCamera* _camera;

	QTimer* _animateViewTimer;
	QTimer* _animateFitAllTimer;
	QTimer* _animateWindowZoomTimer;

	BoundingSphere _boundingSphere;

private:

	void createShaderPrograms();
	void createGeometry();
	void createTexture();

    void setRotations(GLfloat xRot, GLfloat yRot, GLfloat zRot);
    void setZoomAndPan(GLfloat zoom, QVector3D pan);
	void setView(QVector3D viewPos, QVector3D viewDir, QVector3D upDir, QVector3D rightDir);

	void gradientBackground(float top_r, float top_g, float top_b, float top_a,
		float bot_r, float bot_g, float bot_b, float bot_a);

	void splitScreen();
};

#endif
