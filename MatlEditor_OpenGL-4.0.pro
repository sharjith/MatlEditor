######################################################################
# Automatically generated by qmake (3.1) Sat Dec 22 16:39:50 2018
######################################################################

TEMPLATE = app
TARGET = MatlEditor_OpenGL-4.0
INCLUDEPATH += .
INCLUDEPATH += /usr/include/freetype2/

LIBS += -lfreetype

QT += core gui widgets opengl

win32:RC_ICONS += res\MatlEditor.ico

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += AABB.h \
           AppleSurface.h \
           BentHorns.h \
		   BoundingSphere.h \
           BowTie.h \
           BoySurface.h \
		   BreatherSurface.h \
           Cone.h \
           ConeShell.h \
           Crescent.h \
           Cube.h \
           Cylinder.h \
           DoubleCone.h \
           Drawable.h \
           Figure8KleinBottle.h \
           Folium.h \
           GLView.h \
		   GLCamera.h \
           GraysKlein.h \
           Horn.h \
           IDrawable.h \
           IParametricSurface.h \
           KleinBottle.h \
           LimpetTorus.h \
           MatlEditor.h \
		   MainWindow.h \
           ObjMesh.h \
           ParametricSurface.h \
           Periwinkle.h \
           Plane.h \
           Point.h \
		   QuadMesh.h \
           Resource.h \
           SaddleTorus.h \
           Sphere.h \
           SphericalHarmonic.h \
           SpindleShell.h \
           SteinerSurface.h \
		   SuperToroid.h \
		   SuperEllipsoid.h \
		   Spring.h \
           Teapot.h \
           TeapotData.h \
           TextRenderer.h \
           TopShell.h \
           Torus.h \
           TriangleMesh.h \
           TriaxialHexatorus.h \
           TriaxialTritorus.h \
           TwistedPseudoSphere.h \
           TwistedTriaxial.h \
		   TurretShell.h \
           ui_MatlEditor.h \
           VerrillMinimal.h \
           WrinkledPeriwinkle.h \
           ParametricSurface.h \
    SphericalHarmonicsEditor.h \
    ClippingPlanesEditor.h \
	GraysKleinEditor.h \
	SuperToroidEditor.h \
	SuperEllipsoidEditor.h \
	SpringEditor.h
FORMS += MatlEditor.ui \
	MainWindow.ui \
    SphericalHarmonicsEditor.ui \
    ClippingPlanesEditor.ui \
	GraysKleinEditor.ui \
	SuperToroidEditor.ui \
	SuperEllipsoidEditor.ui \
	SpringEditor.ui
SOURCES += AppleSurface.cpp \
           BentHorns.cpp \
		   BoundingSphere.cpp \
           BowTie.cpp \
           BoySurface.cpp \
		   BreatherSurface.cpp \
           Cone.cpp \
           ConeShell.cpp \
           Crescent.cpp \
           Cube.cpp \
           Cylinder.cpp \
           DoubleCone.cpp \
           Figure8KleinBottle.cpp \
           Folium.cpp \
           GLView.cpp \
		   GLCamera.cpp \
           GraysKlein.cpp \
           Horn.cpp \
           KleinBottle.cpp \
           LimpetTorus.cpp \
           main.cpp \
           MatlEditor.cpp \
		   MainWindow.cpp \
           ObjMesh.cpp \
           ParametricSurface.cpp \
           Periwinkle.cpp \
           Plane.cpp \
           Point.cpp \
		   QuadMesh.cpp \
           SaddleTorus.cpp \
           Sphere.cpp \
           SphericalHarmonic.cpp \
           SpindleShell.cpp \
           SteinerSurface.cpp \
		   SuperToroid.cpp \
		   SuperEllipsoid.cpp \
		   Spring.cpp \
           Teapot.cpp \
           TextRenderer.cpp \
           TopShell.cpp \
           Torus.cpp \
           TriangleMesh.cpp \
           TriaxialHexatorus.cpp \
           TriaxialTritorus.cpp \
           TwistedPseudoSphere.cpp \
           TwistedTriaxial.cpp \
		   TurretShell.cpp \
           VerrillMinimal.cpp \
           WrinkledPeriwinkle.cpp \        
    SphericalHarmonicsEditor.cpp \
    ClippingPlanesEditor.cpp \
	GraysKleinEditor.cpp \
	SuperToroidEditor.cpp \
	SuperEllipsoidEditor.cpp \
	SpringEditor.cpp	
RESOURCES += MatlEditor.qrc

OTHER_FILES += shaders/background.vert \
shaders/background.frag \
shaders/twoside.vert \
shaders/twoside.frag \
shaders/text.vert \
shaders/text.frag
