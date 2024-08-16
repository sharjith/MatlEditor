
#include "GLView.h"

#include "TextRenderer.h"

#include "Cylinder.h"
#include "Cone.h"
#include "Torus.h"
#include "Sphere.h"
#include "Cube.h"
#include "Teapot.h"
#include "KleinBottle.h"
#include "Figure8KleinBottle.h"
#include "BoySurface.h"
#include "TwistedTriaxial.h"
#include "SteinerSurface.h"
#include "SuperToroid.h"
#include "SuperToroidEditor.h"
#include "SuperEllipsoid.h"
#include "SuperEllipsoidEditor.h"
#include "Spring.h"
#include "SpringEditor.h"
#include "AppleSurface.h"
#include "DoubleCone.h"
#include "BentHorns.h"
#include "Folium.h"
#include "LimpetTorus.h"
#include "SaddleTorus.h"
#include "GraysKlein.h"
#include "GraysKleinEditor.h"
#include "BowTie.h"
#include "TriaxialTritorus.h"
#include "TriaxialHexatorus.h"
#include "VerrillMinimal.h"
#include "Horn.h"
#include "Crescent.h"
#include "ConeShell.h"
#include "Periwinkle.h"
#include "TopShell.h"
#include "WrinkledPeriwinkle.h"
#include "SpindleShell.h"
#include "TurretShell.h"
#include "TwistedPseudoSphere.h"
#include "BreatherSurface.h"
#include "SphericalHarmonic.h"
#include "SphericalHarmonicsEditor.h"
#include "ClippingPlanesEditor.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat4;

GLView::GLView(QWidget *parent, const char * /*name*/) : QOpenGLWidget(parent),
    _textRenderer(nullptr),
    _sphericalHarmonicsEditor(nullptr),
    _superToroidEditor(nullptr),
    _superEllipsoidEditor(nullptr),
    _springEditor(nullptr),
    _graysKleinEditor(nullptr),
    _clippingPlanesEditor(nullptr)
{
    _fgShader = new QOpenGLShaderProgram(this);
    _viewBoundingSphereDia = 200.0f;
    _viewRange = _viewBoundingSphereDia;
    _rubberBandZoomRatio = 1.0f;
    _FOV = 60.0f;
    _currentViewRange = 1.0f;
    _viewMode = ViewMode::ISOMETRIC;
    _projection = ViewProjection::ORTHOGRAPHIC;

    _camera = new GLCamera(width(), height(), _viewRange, _FOV);
    _camera->setView(GLCamera::ViewProjection::SE_ISOMETRIC_VIEW);

    _currentRotation = QQuaternion::fromRotationMatrix(_camera->getViewMatrix().toGenericMatrix<3, 3>());
    _currentTranslation = _camera->getPosition();
    _currentViewRange = _viewRange;

    _slerpStep = 0.0f;
    _slerpFrac = 0.02f;

    _modelNum = 6;
    _opacity = 1.0f;
    _ambiLight = { 0.623529434f, 0.396078438f, 0.490196079f, 1.0f };
    _diffLight = { 0.698039234f,	0.698039234f, 0.698039234f,	1.0f };
    _specLight = { 1.0f, 1.0f, 1.0f, 1.0f };

    _ambiMat = { 0.819607854f, 0.756862760f, 0.482352942f, _opacity };
    _diffMat = { 0.698039234f, 0.698039234f, 0.698039234f, _opacity };
    _specMat = { 1.0f, 1.0f, 1.0f, _opacity };
    _emmiMat = { 0.0f, 0.0f, 0.0f, _opacity };

    _specRef = {1.0f, 1.0f, 1.0f, 1.0f};

    _shine = 128;

    _lightPosition = {0.0f, 0.0f, 50.0f};

    _bHasTexture = false;
    _bShaded = true;

    _bMultiView = false;

    _bWindowZoomActive = false;
    _rubberBand = nullptr;

    _bLeftButtonDown = false;
    _bRightButtonDown = false;
    _bMiddleButtonDown = false;

    _modelName = "Model";

    _clipXEnabled = false;
    _clipYEnabled = false;
    _clipZEnabled = false;

    _clipXFlipped = false;
    _clipYFlipped = false;
    _clipZFlipped = false;

    _clipXCoeff = 0.0f;
    _clipYCoeff = 0.0f;
    _clipZCoeff = 0.0f;

    _animateViewTimer = new QTimer(this);
    _animateViewTimer->setTimerType(Qt::PreciseTimer);
    connect(_animateViewTimer, SIGNAL(timeout()), this, SLOT(animateViewChange()));

    _animateFitAllTimer = new QTimer(this);
    _animateFitAllTimer->setTimerType(Qt::PreciseTimer);
    connect(_animateFitAllTimer, SIGNAL(timeout()), this, SLOT(animateFitAll()));

    _animateWindowZoomTimer = new QTimer(this);
    _animateWindowZoomTimer->setTimerType(Qt::PreciseTimer);
    connect(_animateWindowZoomTimer, SIGNAL(timeout()), this, SLOT(animateWindowZoom()));
}

GLView::~GLView()
{
    if (_textRenderer)
        delete _textRenderer;
    for (auto a : _meshStore)
    {
        delete a;
    }
    if (_camera)
        delete _camera;

    if (_fgShader)
        delete _fgShader;

    _bgSplitVBO.destroy();
    _bgSplitVAO.destroy();
}

void GLView::changeModel(bool forward)
{
    forward ? _modelNum++ : _modelNum--;
    if (_modelNum > static_cast<int>(_meshStore.size()))
        _modelNum = 1;
    if (_modelNum < 1)
        _modelNum = static_cast<int>(_meshStore.size());
    update();
}

void GLView::updateView()
{
    if (_fgShader->isLinked())
    {
        makeCurrent();
        _fgShader->bind();
        _fgShader->setUniformValue("lightSource.ambient", _ambiLight.toVector3D());
        _fgShader->setUniformValue("lightSource.diffuse", _diffLight.toVector3D());
        _fgShader->setUniformValue("lightSource.specular", _specLight.toVector3D());
        _fgShader->setUniformValue("lightSource.position", _lightPosition);
        _fgShader->setUniformValue("lightModel.ambient", QVector3D(0.2f, 0.2f, 0.2f));
        _fgShader->setUniformValue("material.emission", _emmiMat.toVector3D());
        _fgShader->setUniformValue("material.ambient", _ambiMat.toVector3D());
        _fgShader->setUniformValue("material.diffuse", _diffMat.toVector3D());
        _fgShader->setUniformValue("material.specular", _specMat.toVector3D());
        _fgShader->setUniformValue("material.shininess", _shine);
        _fgShader->setUniformValue("b_texEnabled", _bHasTexture);
        _fgShader->setUniformValue("f_alpha", _opacity);
        _fgShader->release();
        update();
    }
}

void GLView::setTexture(QImage img)
{
    _texImage = QGLWidget::convertToGLFormat(img);  // flipped 32bit RGBA
}

void GLView::setViewMode(ViewMode mode)
{
    if (!_animateViewTimer->isActive())
    {
        _animateViewTimer->start(5);
        _viewMode = mode;
        _slerpStep = 0.0f;
    }
}

void GLView::fitAll()
{
    _viewBoundingSphereDia = _boundingSphere.getRadius() * 2;

    if (!_animateFitAllTimer->isActive())
    {
        _animateFitAllTimer->start(5);
        _slerpStep = 0.0f;
    }
}

void GLView::beginWindowZoom()
{
    _bWindowZoomActive = true;
    setCursor(QCursor(QPixmap(":/new/prefix1/res/window-zoom-cursor.png"), 12, 12));
}

void GLView::endWindowZoom()
{
    _bWindowZoomActive = false;
    if (_rubberBand)
    {
        QVector3D Z(0, 0, 0); // instead of 0 for x and y we need worldPosition.x() and worldPosition.y() ....
        Z = Z.project(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));

        QRect clientRect = geometry();
        QPoint clientWinCen = clientRect.center();
        QVector3D o(clientWinCen.x(), height() - clientWinCen.y(), Z.z());
        QVector3D O = o.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));

        QRect zoomRect = _rubberBand->geometry();
        QPoint zoomWinCen = zoomRect.center();
        QVector3D p(zoomWinCen.x(), height() - zoomWinCen.y(), Z.z());
        QVector3D P = p.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));

        double widthRatio = static_cast<double>(clientRect.width() / zoomRect.width());
        double heightRatio = static_cast<double>(clientRect.height() / zoomRect.height());
        _rubberBandZoomRatio = (heightRatio < widthRatio) ? heightRatio : widthRatio;
        _rubberBandPan = P - O;
    }
    if (!_animateWindowZoomTimer->isActive())
    {
        _animateWindowZoomTimer->start(5);
        _slerpStep = 0.0f;
    }
    emit windowZoomEnded();
}

void GLView::setProjection(ViewProjection proj)
{
    _projection = proj;
    resizeGL(width(), height());
}

void GLView::setModelNum(const int& num)
{
    _modelNum = num;
    _boundingSphere = _meshStore.at(_modelNum - 1)->getBoundingSphere();
    _viewBoundingSphereDia = _boundingSphere.getRadius() * 2;
    fitAll();
    //qDebug() << "Bounding Sphere Dia " << _viewBoundingSphereDia;
    update();
    emit modelChanged(_modelNum - 1);
}

void GLView::showClippingPlaneEditor(bool show)
{
    if (!_clippingPlanesEditor)
    {
        _clippingPlanesEditor = new ClippingPlanesEditor(this);
        _clippingPlanesEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        _clippingPlanesEditor->setAttribute(Qt::WA_NoSystemBackground);
        _clippingPlanesEditor->setAttribute(Qt::WA_TranslucentBackground);
        _clippingPlanesEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
        QPoint point = mapToGlobal(QPoint(frameGeometry().width() - _clippingPlanesEditor->width() - 20,
                                          frameGeometry().height() - _clippingPlanesEditor->height()));
        _clippingPlanesEditor->move(point.x(), point.y());
    }
    show ? _clippingPlanesEditor->show() : _clippingPlanesEditor->hide();
}


void GLView::createShaderPrograms()
{
    // foreground objects shader program
    // per fragment lighting
    if (!_fgShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/twoside_per_fragment.vert")) {
        qDebug() << "Error in vertex shader:" << _fgShader->log();
        //exit(1);
    }
    /*if (!_fgShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "shaders/twoside_per_fragment.geom")) {
        qDebug() << "Error in geometry shader:" << _fgShader->log();
        //exit(1);
    }*/
    if (!_fgShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/twoside_per_fragment.frag")) {
        qDebug() << "Error in fragment shader:" << _fgShader->log();
        //exit(1);
    }
    if (!_fgShader->link()) {
        qDebug() << "Error linking shader program:" << _fgShader->log();
        //exit(1);
    }


    // text shader program
    if (!_textShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/text.vert")) {
        qDebug() << "Error in vertex shader:" << _textShader.log();
        //exit(1);
    }
    if (!_textShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/text.frag")) {
        qDebug() << "Error in fragment shader:" << _textShader.log();
        //exit(1);
    }
    if (!_textShader.link()) {
        qDebug() << "Error linking shader program:" << _textShader.log();
        //exit(1);
    }

    // background gradient shader program
    if (!_bgShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/background.vert")) {
        qDebug() << "Error in vertex shader:" << _bgShader.log();
        //exit(1);
    }
    if (!_bgShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/background.frag")) {
        qDebug() << "Error in fragment shader:" << _bgShader.log();
        //exit(1);
    }
    if (!_bgShader.link()) {
        qDebug() << "Error linking shader program:" << _bgShader.log();
        //exit(1);
    }

    // background split shader program
    if (!_bgSplitShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/splitScreen.vert")) {
        qDebug() << "Error in vertex shader:" << _bgSplitShader.log();
        //exit(1);
    }
    if (!_bgSplitShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/splitScreen.frag")) {
        qDebug() << "Error in fragment shader:" << _bgSplitShader.log();
        //exit(1);
    }
    if (!_bgSplitShader.link()) {
        qDebug() << "Error linking shader program:" << _bgSplitShader.log();
        //exit(1);
    }
}


void GLView::createGeometry()
{
    _meshStore.push_back(new Cube(_fgShader, 100.0f));
    _meshStore.push_back(new Sphere(_fgShader, 75.0f, 50.0f, 50.0f));
    _meshStore.push_back(new Cylinder(_fgShader, 60.0f, 100.0f, 100.0f, 1.0f));
    _meshStore.push_back(new Cone(_fgShader, 60.0f, 100.0f, 100.0f, 1.0f));
    _meshStore.push_back(new Torus(_fgShader, 50.0f, 25.0f, 100.0f, 100.0f));
    _meshStore.push_back(new Teapot(_fgShader, 35.0f, 50, glm::translate(mat4(1.0f), vec3(0.0f, 15.0f, 25.0f))));
    _meshStore.push_back(new KleinBottle(_fgShader, 30.0f, 150.0f, 150.0f));
    _meshStore.push_back(new Figure8KleinBottle(_fgShader, 30.0f, 150.0f, 150.0f));
    _meshStore.push_back(new BoySurface(_fgShader, 60.0f, 150.0f, 150.0f));
    _meshStore.push_back(new TwistedTriaxial(_fgShader, 110.0f, 150.0f, 150.0f));
    _meshStore.push_back(new SteinerSurface(_fgShader, 150.0f, 150.0f, 150.0f));
    _meshStore.push_back(new AppleSurface(_fgShader, 7.5f, 150.0f, 150.0f));
    _meshStore.push_back(new DoubleCone(_fgShader, 35.0f, 150.0f, 150.0f));
    _meshStore.push_back(new BentHorns(_fgShader, 15.0f, 150.0f, 150.0f));
    _meshStore.push_back(new Folium(_fgShader, 75.0f, 150.0f, 150.0f));
    _meshStore.push_back(new LimpetTorus(_fgShader, 35.0f, 150.0f, 150.0f));
    _meshStore.push_back(new SaddleTorus(_fgShader, 30.0f, 150.0f, 150.0f));
    _meshStore.push_back(new BowTie(_fgShader, 40.0f, 150.0f, 150.0f));
    _meshStore.push_back(new TriaxialTritorus(_fgShader, 45.0f, 150.0f, 150.0f));
    _meshStore.push_back(new TriaxialHexatorus(_fgShader, 45.0f, 150.0f, 150.0f));
    _meshStore.push_back(new VerrillMinimal(_fgShader, 25.0f, 150.0f, 150.0f));
    _meshStore.push_back(new Horn(_fgShader, 30.0f, 150.0f, 150.0f));
    _meshStore.push_back(new Crescent(_fgShader, 30.0f, 150.0f, 150.0f));
    _meshStore.push_back(new ConeShell(_fgShader, 45.0f, 150.0f, 150.0f));
    _meshStore.push_back(new Periwinkle(_fgShader, 40.0f, 150.0f, 150.0f));
    _meshStore.push_back(new TopShell(_fgShader, Point(-50,0,0), 35.0f, 250.0f, 150.0f));
    _meshStore.push_back(new WrinkledPeriwinkle(_fgShader, 45.0f, 150.0f, 150.0f));
    _meshStore.push_back(new SpindleShell(_fgShader, 25.0f, 150.0f, 150.0f));
    _meshStore.push_back(new TurretShell(_fgShader, 20.0f, 250.0f, 150.0f));
    _meshStore.push_back(new TwistedPseudoSphere(_fgShader, 50.0f, 150.0f, 150.0f));
    _meshStore.push_back(new BreatherSurface(_fgShader, 15.0f, 150.0f, 150.0f));

    Spring* spring = new Spring(_fgShader, 10.0f, 30.0f, 10.0f, 2.0f, 50.0f, 150.0f);
    _meshStore.push_back(spring);
    _springEditor = new SpringEditor(spring, this);
    _springEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _springEditor->setAttribute(Qt::WA_NoSystemBackground);
    _springEditor->setAttribute(Qt::WA_TranslucentBackground);
    _springEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
    QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
    _springEditor->move(point.x(), point.y());

    SuperToroid* storoid = new SuperToroid(_fgShader, 50, 25, 1, 1, 150.0f, 150.0f);
    _meshStore.push_back(storoid);
    _superToroidEditor = new SuperToroidEditor(storoid, this);
    _superToroidEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _superToroidEditor->setAttribute(Qt::WA_NoSystemBackground);
    _superToroidEditor->setAttribute(Qt::WA_TranslucentBackground);
    _superToroidEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
    point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
    _superToroidEditor->move(point.x(), point.y());

    SuperEllipsoid* sellipsoid = new SuperEllipsoid(_fgShader, 50, 1.0, 1.0, 1.0, 1.0, 1.0, 150.0f, 150.0f);
    _meshStore.push_back(sellipsoid);
    _superEllipsoidEditor = new SuperEllipsoidEditor(sellipsoid, this);
    _superEllipsoidEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _superEllipsoidEditor->setAttribute(Qt::WA_NoSystemBackground);
    _superEllipsoidEditor->setAttribute(Qt::WA_TranslucentBackground);
    _superEllipsoidEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
    point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
    _superEllipsoidEditor->move(point.x(), point.y());

    GraysKlein* gklein = new GraysKlein(_fgShader, 30.0f, 150.0f, 150.0f);
    _meshStore.push_back(gklein);
    _graysKleinEditor = new GraysKleinEditor(gklein, this);
    _graysKleinEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _graysKleinEditor->setAttribute(Qt::WA_NoSystemBackground);
    _graysKleinEditor->setAttribute(Qt::WA_TranslucentBackground);
    _graysKleinEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
    point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
    _graysKleinEditor->move(point.x(), point.y());

    SphericalHarmonic* sph = new SphericalHarmonic(_fgShader, 30.0f, 150.0f, 150.0f);
    _meshStore.push_back(sph);
    _sphericalHarmonicsEditor = new SphericalHarmonicsEditor(sph, this);
    _sphericalHarmonicsEditor->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _sphericalHarmonicsEditor->setAttribute(Qt::WA_NoSystemBackground);
    _sphericalHarmonicsEditor->setAttribute(Qt::WA_TranslucentBackground);
    _sphericalHarmonicsEditor->setAttribute(Qt::WA_TransparentForMouseEvents);
    _sphericalHarmonicsEditor->move(point.x(), point.y());
}


void GLView::createTexture(void)
{
    if (!_texBuffer.load("textures/opengllogo.png"))
    {	// Load first image from file
        qWarning("Could not read image file, using single-color instead.");
        QImage dummy(128, 128, static_cast<QImage::Format>(5));
        dummy.fill(Qt::white);
        _texBuffer = dummy;
    }
    _texImage = QGLWidget::convertToGLFormat(_texBuffer);  // flipped 32bit RGBA

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, _texImage.width(), _texImage.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, _texImage.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GLView::initializeGL()
{
    initializeOpenGLFunctions();

    cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    cout << "Vendor:   " << glGetString(GL_VENDOR)   << '\n';
    cout << "Version:  " << glGetString(GL_VERSION)  << '\n';
    cout << "Shader:   " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << "\n\n";

    /*GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (GLint i = 0; i < n; i++)
    {
        const char* extension =
            (const char*)glGetStringi(GL_EXTENSIONS, i);
        printf("GL Extension %d: %s\n", i, extension);
    }*/

    makeCurrent();

    createShaderPrograms();
    createGeometry();
    createTexture();

    _textShader.bind();
    _textRenderer = new TextRenderer(&_textShader, width(), height());
    _textRenderer->Load("fonts/calibri.ttf", 24);
    _textShader.release();

    // Set lighting information
    _fgShader->bind();
    _fgShader->setUniformValue("lightSource.ambient", _ambiLight.toVector3D());
    _fgShader->setUniformValue("lightSource.diffuse", _diffLight.toVector3D());
    _fgShader->setUniformValue("lightSource.specular", _specLight.toVector3D());
    _fgShader->setUniformValue("lightSource.position", _lightPosition);
    _fgShader->setUniformValue("lightModel.ambient", QVector3D(0.2f, 0.2f, 0.2f));
    _fgShader->setUniformValue("material.emission", _emmiMat.toVector3D());
    _fgShader->setUniformValue("material.ambient", _ambiMat.toVector3D());
    _fgShader->setUniformValue("material.diffuse", _diffMat.toVector3D());
    _fgShader->setUniformValue("material.specular", _specMat.toVector3D());
    _fgShader->setUniformValue("material.shininess", _shine);
    _fgShader->setUniformValue("f_alpha", _opacity);
    _fgShader->release();

    _viewMatrix.setToIdentity();
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLView::resizeGL(int width, int height)
{
    GLfloat w = (GLfloat)width;
    GLfloat h = (GLfloat)height;

    glViewport(0, 0, w, h);
    _viewportMatrix = QMatrix4x4(w/2, 0.0f, 0.0f, 0.0f,
                                 0.0f, h/2, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 w/2 + 0, h/2 + 0, 0.0f, 1.0f);

    _projectionMatrix.setToIdentity();
    _camera->setScreenSize(w, h);
    _camera->setViewRange(_viewRange);
    if (_projection == ViewProjection::ORTHOGRAPHIC)
    {
        _camera->setProjection(GLCamera::ProjectionType::ORTHOGRAPHIC);
    }
    else
    {
        _camera->setProjection(GLCamera::ProjectionType::PERSPECTIVE);
    }
    _projectionMatrix = _camera->getProjectionMatrix();

    // Resize the text frame
    QMatrix4x4 projection;
    projection.ortho(QRect(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h)));
    _textShader.bind();
    _textShader.setUniformValue("projection", projection);
    _textShader.release();

    update();
}

void GLView::paintGL()
{	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gradientBackground(0.3f, 0.3f, 0.3f, 1.0f,
                       0.925f, 0.913f, 0.847f, 1.0f);

    glViewport(0, 0, width(), height());

    QMatrix4x4 projection;
    projection.ortho(QRect(0.0f, 0.0f, static_cast<float>(width()), static_cast<float>(height())));
    _textShader.bind();
    _textShader.setUniformValue("projection", projection);
    _textShader.release();
    // Text rendering
    _textRenderer->RenderText(_meshStore.at(_modelNum - 1)->getName().toStdString(), 4, 4, 1, glm::vec3(1.0f, 1.0f, 0.0f));

    _modelMatrix.setToIdentity();
    if (_bMultiView)
    {
        // Top View
        _projectionMatrix.setToIdentity();
        _viewMatrix.setToIdentity();
        _modelMatrix.setToIdentity();
        glViewport(0, height() / 2, width() / 2, height() / 2);
        _camera->setScreenSize(width() / 2, height() / 2);
        glViewport(0, 0, width() / 2, height() / 2);
        _camera->setView(GLCamera::ViewProjection::TOP_VIEW);
        _projectionMatrix = _camera->getProjectionMatrix();
        _viewMatrix = _camera->getViewMatrix();
        _textRenderer->RenderText("Top", width() - 150, 4, 1.5, glm::vec3(1.0f, 1.0f, 0.0f));
        render();

        // Front View
        _projectionMatrix.setToIdentity();
        _viewMatrix.setToIdentity();
        _modelMatrix.setToIdentity();
        glViewport(0, height() / 2, width() / 2, height() / 2);
        _camera->setView(GLCamera::ViewProjection::FRONT_VIEW);
        _projectionMatrix = _camera->getProjectionMatrix();
        _viewMatrix = _camera->getViewMatrix();
        _textRenderer->RenderText("Front", width()-150, 4, 1.5, glm::vec3(1.0f, 1.0f, 0.0f));
        render();

        // Left View
        _projectionMatrix.setToIdentity();
        _viewMatrix.setToIdentity();
        _modelMatrix.setToIdentity();
        glViewport(width() / 2, height() / 2, width() / 2, height() / 2);
        _camera->setView(GLCamera::ViewProjection::LEFT_VIEW);
        _projectionMatrix = _camera->getProjectionMatrix();
        _viewMatrix = _camera->getViewMatrix();
        _textRenderer->RenderText("Left", width() - 150, 4, 1.5, glm::vec3(1.0f, 1.0f, 0.0f));
        render();

        // Isometric View
        _projectionMatrix.setToIdentity();
        _viewMatrix.setToIdentity();
        _modelMatrix.setToIdentity();
        glViewport(width()/2, 0, width() / 2, height()/2);
        _camera->setView(GLCamera::ViewProjection::SE_ISOMETRIC_VIEW);
        _projectionMatrix = _camera->getProjectionMatrix();
        _viewMatrix = _camera->getViewMatrix();
        _textRenderer->RenderText("Isometric", width() - 150, 4, 1.5, glm::vec3(1.0f, 1.0f, 0.0f));
        render();

        // draw screen partitioning lines
        splitScreen();
    }
    else
    {
        QMatrix4x4 projection;
        projection.ortho(QRect(0.0f, 0.0f, static_cast<float>(width()), static_cast<float>(height())));
        _textShader.bind();
        _textShader.setUniformValue("projection", projection);
        _textShader.release();
        render();
    }


    // Display Harmonics Editor
    if (dynamic_cast<SphericalHarmonic*>(_meshStore.at(_modelNum - 1)))
        _sphericalHarmonicsEditor->show();
    else
        _sphericalHarmonicsEditor->hide();

    // Display Gray's Klein Editor
    if (dynamic_cast<GraysKlein*>(_meshStore.at(_modelNum - 1)))
        _graysKleinEditor->show();
    else
        _graysKleinEditor->hide();

    // Display Super Toroid Editor
    if (dynamic_cast<SuperToroid*>(_meshStore.at(_modelNum - 1)))
        _superToroidEditor->show();
    else
        _superToroidEditor->hide();

    // Display Super Ellipsoid Editor
    if (dynamic_cast<SuperEllipsoid*>(_meshStore.at(_modelNum - 1)))
        _superEllipsoidEditor->show();
    else
        _superEllipsoidEditor->hide();

    // Display Spring Editor
    if (dynamic_cast<Spring*>(_meshStore.at(_modelNum - 1)))
        _springEditor->show();
    else
        _springEditor->hide();
}

void GLView::render()
{
    glEnable(GL_DEPTH_TEST);    

    _viewMatrix.setToIdentity();
    _viewMatrix = _camera->getViewMatrix();

    /*_modelMatrix.translate(QVector3D(_xTran, _yTran, _zTran));
    _modelMatrix.rotate(_xRot, 1, 0, 0);
    _modelMatrix.rotate(_yRot, 0, 1, 0);
    _modelMatrix.rotate(_zRot, 0, 0, 1);
    _modelMatrix.scale(_scaleFactor);*/

    _modelViewMatrix = _viewMatrix * _modelMatrix;

    _fgShader->bind();
    _fgShader->setUniformValue("modelViewMatrix", _modelViewMatrix);
    _fgShader->setUniformValue("normalMatrix", _modelViewMatrix.normalMatrix());
    _fgShader->setUniformValue("projectionMatrix", _projectionMatrix);
    _fgShader->setUniformValue("viewportMatrix", _viewportMatrix);
    _fgShader->setUniformValue("Line.Width", 0.75f);
    _fgShader->setUniformValue("Line.Color", QVector4D(0.05f, 0.0f, 0.05f, 1.0f));
    _fgShader->setUniformValue("b_wireframe", !_bShaded);

    glPolygonMode(GL_FRONT_AND_BACK, _bShaded ? GL_FILL : GL_LINE);
    glLineWidth(_bShaded ? 1.0 : 1.5);

    // Clipping Planes
    if (_clipXEnabled)
        glEnable(GL_CLIP_DISTANCE0);
    if (_clipYEnabled)
        glEnable(GL_CLIP_DISTANCE1);
    if (_clipZEnabled)
        glEnable(GL_CLIP_DISTANCE2);

    if (_clipXEnabled || _clipYEnabled || _clipZEnabled)
    {
        _fgShader->setUniformValue("b_SectionActive", true);
    }
    else
    {
        _fgShader->setUniformValue("b_SectionActive", false);
    }

    QVector3D pos = _camera->getPosition();
    _fgShader->setUniformValue("clipPlaneX", QVector4D(_modelViewMatrix * (QVector3D(_clipXFlipped ? -1 : 1, 0, 0) + pos),
                                                       (_clipXFlipped ? -1 : 1)*pos.x() + _clipXCoeff));
    _fgShader->setUniformValue("clipPlaneY", QVector4D(_modelViewMatrix * (QVector3D(0, _clipYFlipped ? -1 : 1, 0) + pos),
                                                       (_clipYFlipped ? -1 : 1)*pos.y() + _clipYCoeff));
    _fgShader->setUniformValue("clipPlaneZ", QVector4D(_modelViewMatrix * (QVector3D(0, 0, _clipZFlipped ? -1 : 1) + pos),
                                                       (_clipZFlipped ? -1 : 1)*pos.z() + _clipZCoeff));


    // Render
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    _fgShader->setUniformValue("texUnit", 0);
    _meshStore.at(_modelNum - 1)->render();

    glDisable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);
    glDisable(GL_CLIP_DISTANCE2);

    _fgShader->release();
}


void GLView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() & Qt::LeftButton)
    {
        setCursor(QCursor(QPixmap(":/new/prefix1/res/rotatecursor.png")));
        _bLeftButtonDown = true;
        _leftButtonPoint.setX(e->x());
        _leftButtonPoint.setY(e->y());

        if (_bWindowZoomActive)
        {
            if (!_rubberBand)
            {
                _rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                _rubberBand->setStyle(QStyleFactory::create("Fusion"));
            }
            _rubberBand->setGeometry(QRect(_leftButtonPoint, QSize()));
            _rubberBand->show();
        }
    }

    if (e->button() & Qt::RightButton)
    {
        setCursor(QCursor(QPixmap(":/new/prefix1/res/pancursor.png")));
        _bRightButtonDown = true;
        _rightButtonPoint.setX(e->x());
        _rightButtonPoint.setY(e->y());
    }

    if (e->button() & Qt::MiddleButton)
    {
        setCursor(QCursor(QPixmap(":/new/prefix1/res/zoomcursor.png")));
        _bMiddleButtonDown = true;
        _middleButtonPoint.setX(e->x());
        _middleButtonPoint.setY(e->y());
    }
}

void GLView::mouseReleaseEvent(QMouseEvent* e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    if (e->button() & Qt::LeftButton)
    {
        _bLeftButtonDown = false;
        if (_bWindowZoomActive)
        {
            _rubberBand->hide();
            endWindowZoom();
        }
    }

    if (e->button() & Qt::RightButton)
    {
        _bRightButtonDown = false;
    }

    if (e->button() & Qt::MiddleButton)
    {
        _bMiddleButtonDown = false;
    }
}

void GLView::mouseMoveEvent(QMouseEvent* e)
{
    _animateViewTimer->stop();

    QPoint downPoint(e->x(), e->y());
    if (_bLeftButtonDown)
    {
        if (_bWindowZoomActive)
        {
            _rubberBand->setGeometry(QRect(_leftButtonPoint, e->pos()).normalized());
            setCursor(QCursor(QPixmap(":/new/prefix1/res/window-zoom-cursor.png"), 12, 12));
        }
        else
        {
            QPoint rotate = _leftButtonPoint - downPoint;

            _camera->rotateX(rotate.y() / 2.0);
            _camera->rotateY(rotate.x() / 2.0);
            _currentRotation = QQuaternion::fromRotationMatrix(_camera->getViewMatrix().toGenericMatrix<3, 3>());
            _leftButtonPoint = downPoint;
        }
    }

    if (_bRightButtonDown)
    {
        //QPoint translate = downPoint - _rightButtonPoint;

        QVector3D Z(0, 0, 0); // instead of 0 for x and y we need worldPosition.x() and worldPosition.y() ....
        Z = Z.project(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));
        QVector3D p1(downPoint.x(), height() - downPoint.y(), Z.z());
        QVector3D O = p1.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));
        QVector3D p2(_rightButtonPoint.x(), height() - _rightButtonPoint.y(), Z.z());
        QVector3D P = p2.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));
        QVector3D OP = P - O;
        _camera->move(OP.x(), OP.y(), OP.z());
        _currentTranslation = _camera->getPosition();

        _rightButtonPoint = downPoint;
    }

    if (_bMiddleButtonDown)
    {
        //QPoint zoom = downPoint - _middleButtonPoint;

        if (downPoint.x() > _middleButtonPoint.x() || downPoint.y() < _middleButtonPoint.y())
            _viewRange /= 1.05f;
        else
            _viewRange *= 1.05f;
        if (_viewRange < 0.05) _viewRange = 0.05f;
        if (_viewRange > 50000.0) _viewRange = 50000.0f;
        _currentViewRange = _viewRange;

        // Translate to focus on mouse center
        QPoint cen = getClientRectFromPoint(downPoint).center();
        float sign = (downPoint.x() > _middleButtonPoint.x() || downPoint.y() < _middleButtonPoint.y()) ? 1.0f : -1.0f;
        QVector3D OP = get3dTranslationVectorFromMousePoints(cen, _middleButtonPoint);
        OP *= sign * 0.05f;
        _camera->move(OP.x(), OP.y(), OP.z());
        _currentTranslation = _camera->getPosition();

        resizeGL(width(), height());

        _middleButtonPoint = downPoint;
    }

    update();
}

void GLView::wheelEvent(QWheelEvent* e)
{
    /*
    int delta = static_cast<GLfloat>(e->delta());

    if (delta < 0)
        _viewRange *= abs(delta) / 114.2857142857;
    else
        _viewRange /= abs(delta) / 114.2857142857;
     */

    // Zoom
    QPoint numDegrees = e->angleDelta() / 8;
    QPoint numSteps = numDegrees / 15;
    float zoomStep = numSteps.y();
    float zoomFactor = abs(zoomStep) + 0.05;

    if (zoomStep < 0)
        _viewRange *= zoomFactor;
    else
        _viewRange /= zoomFactor;

    if (_viewRange < 0.05) _viewRange = 0.05f;
    if (_viewRange > 50000.0) _viewRange = 50000.0f;
    _currentViewRange = _viewRange;

    // Translate to focus on mouse center
    QPoint cen = getClientRectFromPoint(e->position().toPoint()).center();
    float sign = (e->position().x() > cen.x() || e->position().y() < cen.y() ||
        (e->position().x() < cen.x() && e->position().y() > cen.y())) && (zoomStep > 0) ? 1.0f : -1.0f;
    QVector3D OP = get3dTranslationVectorFromMousePoints(cen, e->position().toPoint());
    OP *= sign * 0.05f;
    _camera->move(OP.x(), OP.y(), OP.z());
    _currentTranslation = _camera->getPosition();

    resizeGL(width(), height());

    update();
}

QRect GLView::getViewportFromPoint(const QPoint& pixel)
{
    QRect viewport;
    if (_bMultiView)
    {
        // top view
        if (pixel.x() < width() / 2 && pixel.y() > height() / 2)
            viewport = QRect(0, 0, width() / 2, height() / 2);
        // front view
        if (pixel.x() < width() / 2 && pixel.y() < height() / 2)
            viewport = QRect(0, height() / 2, width() / 2, height() / 2);
        // left view
        if (pixel.x() > width() / 2 && pixel.y() < height() / 2)
            viewport = QRect(width() / 2, height() / 2, width() / 2, height() / 2);
        // isometric
        if (pixel.x() > width() / 2 && pixel.y() > height() / 2)
            viewport = QRect(width() / 2, 0, width() / 2, height() / 2);
    }
    else
    {
        // single viewport
        viewport = QRect(0, 0, width(), height());
    }

    return viewport;
}

QRect GLView::getClientRectFromPoint(const QPoint& pixel)
{
    QRect clientRect;
    if (_bMultiView)
    {
        // top view
        if (pixel.x() < width() / 2 && pixel.y() > height() / 2)
            clientRect = QRect(0, height() / 2, width() / 2, height() / 2);
        // front view
        if (pixel.x() < width() / 2 && pixel.y() < height() / 2)
            clientRect = QRect(0, 0, width() / 2, height() / 2);
        // left view
        if (pixel.x() > width() / 2 && pixel.y() < height() / 2)
            clientRect = QRect(width() / 2, 0, width() / 2, height() / 2);
        // isometric
        if (pixel.x() > width() / 2 && pixel.y() > height() / 2)
            clientRect = QRect(width() / 2, height() / 2, width() / 2, height() / 2);
    }
    else
    {
        // single viewport
        clientRect = QRect(0, 0, width(), height());
    }

    return clientRect;
}

QVector3D GLView::get3dTranslationVectorFromMousePoints(const QPoint& start, const QPoint& end)
{
    QVector3D Z(0, 0, 0); // instead of 0 for x and y we need worldPosition.x() and worldPosition.y() ....
    Z = Z.project(_viewMatrix * _modelMatrix, _projectionMatrix, getViewportFromPoint(start));
    QVector3D p1(start.x(), height() - start.y(), Z.z());
    QVector3D O = p1.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, getViewportFromPoint(start));
    QVector3D p2(end.x(), height() - end.y(), Z.z());
    QVector3D P = p2.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, getViewportFromPoint(start));
    QVector3D OP = P - O;
    return OP;
}


void GLView::animateViewChange()
{
    if (_viewMode == ViewMode::TOP)
    {
        setRotations(0.0f, 0.0f, 0.0f);
    }
    if (_viewMode == ViewMode::BOTTOM)
    {
        setRotations(0.0f, -180.0f, 0.0f);
    }
    if (_viewMode == ViewMode::LEFT)
    {
        setRotations(0.0f, -90.0f, 90.0f);
    }
    if (_viewMode == ViewMode::RIGHT)
    {
        setRotations(0.0f, -90.0f, -90.0f);
    }
    if (_viewMode == ViewMode::FRONT)
    {
        setRotations(0.0f, -90.0f, 0.0f);
    }
    if (_viewMode == ViewMode::BACK)
    {
        setRotations(0.0f, -90.0f, 180.0f);
    }
    if (_viewMode == ViewMode::ISOMETRIC)
    {
        //setRotations(0.0f, -35.2640f, 45.0f);
        setRotations(-45.0f, -54.7356f, 0.0f);
    }
    if (_viewMode == ViewMode::DIMETRIC)
    {
        //setRotations(-19.4712, -20.7048f, 0.0f);
        //setRotations(-45.0f, -70.5288f, 0.0f);
        setRotations(-14.1883f, -73.9639f, -0.148236f);
    }
    if (_viewMode == ViewMode::TRIMETRIC)
    {
        //setRotations(-35.0f, -30.0f, -0.0f);
        setRotations(-32.5829f, -61.4997f, -0.877613f);
    }

    resizeGL(width(), height());
}

void GLView::animateFitAll()
{
    setZoomAndPan(_viewBoundingSphereDia, -_currentTranslation + _boundingSphere.getCenter());
    resizeGL(width(), height());
}

void GLView::animateWindowZoom()
{
    setZoomAndPan(_currentViewRange /_rubberBandZoomRatio, _rubberBandPan);
    resizeGL(width(), height());
}


void GLView::setView(QVector3D viewPos, QVector3D viewDir, QVector3D upDir, QVector3D rightDir)
{
    _camera->setView(viewPos, viewDir, upDir, rightDir);
}


void GLView::setRotations(GLfloat xRot, GLfloat yRot, GLfloat zRot)
{
    // Rotation
    QQuaternion targetRotation = QQuaternion::fromEulerAngles(yRot, zRot, xRot);//Pitch, Yaw, Roll
    QQuaternion curRot = QQuaternion::slerp(_currentRotation, targetRotation, _slerpStep += _slerpFrac);

    // Translation
    QVector3D curPos = _currentTranslation - (_slerpStep * _currentTranslation) + (_boundingSphere.getCenter() * _slerpStep);

    // Set camera vectors
    QMatrix4x4 rotMat = QMatrix4x4(curRot.toRotationMatrix());
    QVector3D viewDir = -rotMat.row(2).toVector3D();
    QVector3D upDir = rotMat.row(1).toVector3D();
    QVector3D rightDir = rotMat.row(0).toVector3D();
    _camera->setView(curPos, viewDir, upDir, rightDir);

    // Set zoom
    GLfloat scaleStep = (_currentViewRange - _viewBoundingSphereDia) * _slerpFrac;
    _viewRange -= scaleStep;

    if (qFuzzyCompare(_slerpStep, 1.0f))
    {
        // Set camera vectors
        QMatrix4x4 rotMat = QMatrix4x4(curRot.toRotationMatrix());
        QVector3D viewDir = -rotMat.row(2).toVector3D();
        QVector3D upDir = rotMat.row(1).toVector3D();
        QVector3D rightDir = rotMat.row(0).toVector3D();
        _camera->setView(curPos, viewDir, upDir, rightDir);

        // Set all defaults
        _currentRotation = QQuaternion::fromRotationMatrix(_camera->getViewMatrix().toGenericMatrix<3, 3>());
        _currentTranslation = _camera->getPosition();
        _currentViewRange = _viewRange;
        _viewMode = ViewMode::NONE;
        _slerpStep = 0.0f;

        _animateViewTimer->stop();
    }
}

void GLView::setZoomAndPan(GLfloat zoom, QVector3D pan)
{
    _slerpStep += _slerpFrac;

    // Translation
    QVector3D curPos = pan *_slerpFrac;
    _camera->move(curPos.x(), curPos.y(), curPos.z());

    // Set zoom
    GLfloat scaleStep = (_currentViewRange - zoom) * _slerpFrac;
    _viewRange -= scaleStep;

    if (qFuzzyCompare(_slerpStep, 1.0f))
    {
        // position the camera for rotation center
        //QVector3D Z(0, 0, 0); // instead of 0 for x and y we need worldPosition.x() and worldPosition.y() ....
        //Z = Z.project(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));
        //QPoint point = geometry().center();
        //QVector3D p(point.x(), height() - point.y(), Z.z());
        //QVector3D P = p.unproject(_viewMatrix * _modelMatrix, _projectionMatrix, QRect(0, 0, width(), height()));
        //_camera->setPosition(P.x(), P.y(), P.z());

        // Set all defaults
        _currentTranslation = _camera->getPosition();
        _currentViewRange = _viewRange;
        _viewMode = ViewMode::NONE;
        _slerpStep = 0.0f;

        _animateFitAllTimer->stop();
        _animateWindowZoomTimer->stop();
    }
}

void GLView::showEvent(QShowEvent* /*event*/)
{

}

void GLView::closeEvent(QCloseEvent *event)
{
    if (_sphericalHarmonicsEditor)
    {
        _sphericalHarmonicsEditor->hide();
        _sphericalHarmonicsEditor->close();
    }

    if (_graysKleinEditor)
    {
        _graysKleinEditor->hide();
        _graysKleinEditor->close();
    }

    if (_superToroidEditor)
    {
        _superToroidEditor->hide();
        _superToroidEditor->close();
    }

    if (_superEllipsoidEditor)
    {
        _superEllipsoidEditor->hide();
        _superEllipsoidEditor->close();
    }

    if (_springEditor)
    {
        _springEditor->hide();
        _springEditor->close();
    }
    event->accept();
}

void GLView::moveEvent(QMoveEvent *)
{
    if (_sphericalHarmonicsEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
        _sphericalHarmonicsEditor->move(point.x(), point.y());
    }

    if (_graysKleinEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
        _graysKleinEditor->move(point.x(), point.y());
    }

    if (_superToroidEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
        _superToroidEditor->move(point.x(), point.y());
    }

    if (_superEllipsoidEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
        _superEllipsoidEditor->move(point.x(), point.y());
    }

    if (_springEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().x(), frameGeometry().y() + 10));
        _springEditor->move(point.x(), point.y());
    }

    if (_clippingPlanesEditor)
    {
        QPoint point = mapToGlobal(QPoint(frameGeometry().width() - _clippingPlanesEditor->width() - 20,
                                          frameGeometry().height() - _clippingPlanesEditor->height()));
        _clippingPlanesEditor->move(point.x(), point.y());
    }
}

void GLView::gradientBackground(float top_r, float top_g, float top_b, float top_a,
                                float bot_r, float bot_g, float bot_b, float bot_a)
{
    if (!_bgVAO.isCreated())
    {
        _bgVAO.create();
    }

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    _bgShader.bind();

    _bgShader.setUniformValue("top_color", QVector4D(top_r, top_g, top_b, top_a));
    _bgShader.setUniformValue("bot_color", QVector4D(bot_r, bot_g, bot_b, bot_a));

    _bgVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glEnable(GL_DEPTH_TEST);

    _bgVAO.release();
    _bgShader.release();
}

void GLView::splitScreen()
{
    if (!_bgSplitVAO.isCreated())
    {
        _bgSplitVAO.create();
        _bgSplitVAO.bind();
    }

    if (!_bgSplitVBO.isCreated())
    {
        _bgSplitVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        _bgSplitVBO.create();
        _bgSplitVBO.bind();
        _bgSplitVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

        static const std::vector<GLfloat> vertices = {
            -static_cast<GLfloat>(width()) / 2, 0,0,
            static_cast<GLfloat>(width()) / 2, 0,0,
            0, -static_cast<GLfloat>(height()) / 2, 0,
            0, static_cast<GLfloat>(height()) / 2, 0,
        };

        _bgSplitVBO.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

        _bgSplitShader.bind();
        _bgSplitShader.enableAttributeArray("vertexPosition");
        _bgSplitShader.setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3);

        _bgSplitVBO.release();
    }

    glViewport(0, 0, width(), height());

    glDisable(GL_DEPTH_TEST);

    _bgSplitVAO.bind();
    glLineWidth(2.0);
    glDrawArrays(GL_LINES, 0, 4);
    glLineWidth(1);

    glEnable(GL_DEPTH_TEST);

    _bgSplitVAO.release();
    _bgSplitShader.release();
}
