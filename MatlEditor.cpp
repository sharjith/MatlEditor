#include <QApplication>
#include "MatlEditor.h"
#include "GLView.h"
#include "SphericalHarmonicsEditor.h"
#include "TriangleMesh.h"

MatlEditor::MatlEditor(QWidget* parent) : QWidget(parent)
{
	isometricView = new QAction(QIcon(":/new/prefix1/res/isometric.png"), "Isometric", this);
	isometricView->setObjectName(QString::fromUtf8("isometricView"));
	isometricView->setShortcut(QKeySequence(Qt::Key_Home));
	
	dimetricView = new QAction(QIcon(":/new/prefix1/res/dimetric.png"), "Dimetric", this);
	dimetricView->setObjectName(QString::fromUtf8("dimetricView"));
	dimetricView->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_End));
	
	trimetricView = new QAction(QIcon(":/new/prefix1/res/trimetric.png"), "Trimetric", this);
	trimetricView->setObjectName(QString::fromUtf8("trimetricView"));
	trimetricView->setShortcut(QKeySequence(Qt::Key_End));
	
	setupUi(this);

	QMenu* axoMenu = new QMenu;
	axoMenu->addAction(isometricView);
	axoMenu->addAction(dimetricView);
	axoMenu->addAction(trimetricView);

	toolButtonIsometricView->setMenu(axoMenu);
	toolButtonIsometricView->setDefaultAction(isometricView);
	QObject::connect(toolButtonIsometricView, SIGNAL(triggered(QAction*)),
		toolButtonIsometricView, SLOT(setDefaultAction(QAction*)));

	setAttribute(Qt::WA_DeleteOnClose);

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSamples(4);
	format.setStereo(true);
	_glView = new GLView(glframe, "glview");
	_glView->setFormat(format);
	// Put the GL widget inside the frame
	QHBoxLayout* flayout = new QHBoxLayout(glframe);
	flayout->addWidget(_glView, 1);

	QObject::connect(_glView, SIGNAL(modelChanged(int)), comboBoxModel, SLOT(setCurrentIndex(int)));
	QObject::connect(_glView, SIGNAL(windowZoomEnded()), toolButtonWindowZoom, SLOT(toggle()));

	updateControls();
}

MatlEditor::~MatlEditor()
{
	if (_glView)
	{
		delete _glView;
	}
}

void MatlEditor::on_modelButtonNext_clicked()
{
	_glView->changeModel(true);
	comboBoxModel->setCurrentIndex(_glView->getModelNum() - 1);
}

void MatlEditor::on_modelButtonPrev_clicked()
{
	_glView->changeModel(false);
	comboBoxModel->setCurrentIndex(_glView->getModelNum() - 1);
}

void MatlEditor::on_checkTexture_toggled(bool checked)
{
	_glView->_bHasTexture = checked;
	_glView->updateView();
}

void MatlEditor::on_checkShaded_toggled(bool checked)
{
	_glView->_bShaded = checked;
	_glView->updateView();
	checkShaded->setToolTip(checked ? "Wireframe" : "Shaded");
}

void MatlEditor::on_textureButton_clicked()
{
	QImage buf;
	QString str = QFileDialog::getOpenFileName(
		this,
		"Choose an image for texture",
		qApp->applicationDirPath(),
		"Images (*.bmp *.png *.xpm *.jpg)");
	if (str != "")
	{
		if (!buf.load(str))
		{	// Load first image from file
			qWarning("Could not read image file, using single-color instead.");
			QImage dummy(128, 128, (QImage::Format)5);
			dummy.fill(1);
			buf = dummy;
		}
		_glView->setTexture(buf);
		_glView->updateView();
	}
}

void MatlEditor::on_defaultButton_clicked()
{
	_glView->_opacity = 1.0;
	_glView->_ambiLight = { 0.623529434f, 0.396078438f, 0.490196079f, 1.0f };
	_glView->_diffLight = { 0.698039234f, 0.698039234f, 0.698039234f, 1.0f };
	_glView->_specLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	_glView->_ambiMat = { 0.819607854f, 0.756862760f, 0.482352942f, _glView->_opacity };
	_glView->_diffMat = {0.698039234f, 0.698039234f, 0.698039234f,  _glView->_opacity};
	_glView->_specMat = { 1.0f, 1.0f, 1.0f,	_glView->_opacity };
	_glView->_emmiMat = { 0, 0, 0, 1 };
	_glView->_shine = 128;
	_glView->_lightPosition = { 0.0, 0.0, 50.0f };
	updateControls(); 
	_glView->updateView();
}


void MatlEditor::on_isometricView_triggered(bool /*checked*/)
{
	buttonGroup->setExclusive(false);
	for (auto b : buttonGroup->buttons())
	{
		b->setChecked(false);
	}
	buttonGroup->setExclusive(true);
	
	toolButtonTopView->setChecked(false);
	_glView->setViewMode(ViewMode::ISOMETRIC);
	_glView->updateView();

	toolButtonIsometricView->setDefaultAction(dynamic_cast<QAction*>(sender()));
}

void MatlEditor::on_dimetricView_triggered(bool /*checked*/)
{
	buttonGroup->setExclusive(false);
	for (auto b : buttonGroup->buttons())
	{
		b->setChecked(false);
	}
	buttonGroup->setExclusive(true);

	_glView->setViewMode(ViewMode::DIMETRIC);
	_glView->updateView();

	toolButtonIsometricView->setDefaultAction(dynamic_cast<QAction*>(sender()));
}

void MatlEditor::on_trimetricView_triggered(bool /*checked*/)
{
	buttonGroup->setExclusive(false);
	for (auto b : buttonGroup->buttons())
	{
		b->setChecked(false);
	}
	buttonGroup->setExclusive(true);

	_glView->setViewMode(ViewMode::TRIMETRIC);
	_glView->updateView();

	toolButtonIsometricView->setDefaultAction(dynamic_cast<QAction*>(sender()));
}

void MatlEditor::on_toolButtonFitAll_clicked(bool /*checked*/)
{
	_glView->fitAll();
	_glView->updateView();
}

void MatlEditor::on_toolButtonWindowZoom_clicked(bool checked)
{
	if (checked)
	{
		_glView->beginWindowZoom();
	}
}

void MatlEditor::on_toolButtonTopView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::TOP);
	_glView->updateView();
}

void MatlEditor::on_toolButtonBottomView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::BOTTOM);
	_glView->updateView();
}

void MatlEditor::on_toolButtonLeftView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::LEFT);
	_glView->updateView();
}

void MatlEditor::on_toolButtonRightView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::RIGHT);
	_glView->updateView();
}

void MatlEditor::on_toolButtonFrontView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::FRONT);
	_glView->updateView();
}

void MatlEditor::on_toolButtonBackView_clicked(bool /*checked*/)
{
	_glView->setMultiView(false);
	toolButtonMultiView->setChecked(false);
	_glView->setViewMode(ViewMode::BACK);
	_glView->updateView();
}

void MatlEditor::on_toolButtonProjection_toggled(bool checked)
{
	_glView->setProjection(checked ? ViewProjection::PERSPECTIVE : ViewProjection::ORTHOGRAPHIC);
	toolButtonProjection->setToolTip(checked ? "Orthographic" : "Perspective");
}

void MatlEditor::on_toolButtonSectionView_toggled(bool checked)
{
	_glView->showClippingPlaneEditor(checked);
}

void MatlEditor::on_toolButtonMultiView_toggled(bool checked)
{
	_glView->setMultiView(checked);
	toolButtonIsometricView->animateClick(0);
	_glView->resizeView(glframe->width(), glframe->height());
	_glView->updateView();
}

void MatlEditor::on_pushButtonLightAmbient_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_ambiLight.x(), _glView->_ambiLight.y(), _glView->_ambiLight.z()), this, "Ambient Light Color");
	if (c.isValid())
	{
		_glView->_ambiLight = { 
			static_cast<float>(c.redF()), 
			static_cast<float>(c.greenF()), 
			static_cast<float>(c.blueF()), 
			static_cast<float>(c.alphaF()) 
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonLightDiffuse_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_diffLight.x(), _glView->_diffLight.y(), _glView->_diffLight.z()), this, "Diffuse Light Color");
	if (c.isValid())
	{
		_glView->_diffLight = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonLightSpecular_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_specLight.x(), _glView->_specLight.y(), _glView->_specLight.z()), this, "Specular Light Color");
	if (c.isValid())
	{
		_glView->_specLight = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonMaterialAmbient_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_ambiMat.x(), _glView->_ambiMat.y(), _glView->_ambiMat.z()), this, "Ambient Material Color");
	if (c.isValid())
	{
		_glView->_ambiMat = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonMaterialDiffuse_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_diffMat.x(), _glView->_diffMat.y(), _glView->_diffMat.z()), this, "Diffuse Material Color");
	if (c.isValid())
	{
		_glView->_diffMat = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonMaterialSpecular_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_specMat.x(), _glView->_specMat.y(), _glView->_specMat.z()), this, "Specular Material Color");
	if (c.isValid())
	{
		_glView->_specMat = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_pushButtonMaterialEmissive_clicked()
{
	QColor c = QColorDialog::getColor(QColor::fromRgbF(_glView->_emmiMat.x(), _glView->_emmiMat.y(), _glView->_emmiMat.z()), this, "Emissive Material Color");
	if (c.isValid())
	{
		_glView->_emmiMat = {
			static_cast<float>(c.redF()),
			static_cast<float>(c.greenF()),
			static_cast<float>(c.blueF()),
			static_cast<float>(c.alphaF())
		};
		updateControls();
		_glView->updateView();
	}
}

void MatlEditor::on_sliderLightPosX_valueChanged(int value)
{
	_glView->_lightPosition.setX((float)value);
	_glView->updateView();
}

void MatlEditor::on_sliderLightPosY_valueChanged(int value)
{
	_glView->_lightPosition.setY((float)value);
	_glView->updateView();
}

void MatlEditor::on_sliderLightPosZ_valueChanged(int value)
{
	_glView->_lightPosition.setZ((float)value);
	_glView->updateView();
}

void MatlEditor::on_sliderTransparency_valueChanged(int value)
{
	_glView->_opacity = (GLfloat)value / 100.0;
	_glView->_ambiMat[3] = _glView->_opacity;
	_glView->_diffMat[3] = _glView->_opacity;
	_glView->_specMat[3] = _glView->_opacity;
	_glView->updateView();
}

void MatlEditor::on_sliderShine_valueChanged(int value)
{
	_glView->_shine = value;
	_glView->updateView();
}

void MatlEditor::on_pushButtonBrass_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat = { 0.329412f, 0.223529f,	0.027451f, 1 };
	_glView->_diffMat = { 0.780392f, 0.568627f,	0.113725f, 1 };
	_glView->_specMat = { 0.992157f, 0.941176f,	0.807843f, 1 };
	_glView->_shine = fabs(128.0*0.21794872);

	_glView->updateView();
	updateControls();

}


void MatlEditor::on_pushButtonBronze_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat = { 0.2125f, 0.1275f, 0.054f, 1 };
	_glView->_diffMat = { 0.714f, 0.4284f, 0.18144f, 1 };
	_glView->_specMat = { 0.393548f, 0.271906f, 0.166721f, 1 };
	_glView->_shine = fabs(128.0*0.2);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonCopper_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.19125f;
	_glView->_ambiMat[1] = 0.0735f;
	_glView->_ambiMat[2] = 0.0225f;
	_glView->_diffMat[0] = 0.7038f;
	_glView->_diffMat[1] = 0.27048f;
	_glView->_diffMat[2] = 0.0828f;
	_glView->_specMat[0] = 0.256777f;
	_glView->_specMat[1] = 0.137622f;
	_glView->_specMat[2] = 0.086014f;
	_glView->_shine = fabs(128.0*0.1);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonGold_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.24725f;
	_glView->_ambiMat[1] = 0.1995f;
	_glView->_ambiMat[2] = 0.0745f;
	_glView->_diffMat[0] = 0.75164f;
	_glView->_diffMat[1] = 0.60648f;
	_glView->_diffMat[2] = 0.22648f;
	_glView->_specMat[0] = 0.628281f;
	_glView->_specMat[1] = 0.555802f;
	_glView->_specMat[2] = 0.366065f;
	_glView->_shine = fabs(128.0*0.4);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonSilver_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.19225f;
	_glView->_ambiMat[1] = 0.19225f;
	_glView->_ambiMat[2] = 0.19225f;
	_glView->_diffMat[0] = 0.50754f;
	_glView->_diffMat[1] = 0.50654f;
	_glView->_diffMat[2] = 0.50754f;
	_glView->_specMat[0] = 0.508273f;
	_glView->_specMat[1] = 0.508273f;
	_glView->_specMat[2] = 0.508273f;
	_glView->_shine = fabs(128.0*0.4);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonRuby_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.1745f;
	_glView->_ambiMat[1] = 0.01175f;
	_glView->_ambiMat[2] = 0.01175f;
	_glView->_diffMat[0] = 0.61424f;
	_glView->_diffMat[1] = 0.04136f;
	_glView->_diffMat[2] = 0.04136f;
	_glView->_specMat[0] = 0.727811f;
	_glView->_specMat[1] = 0.626959f;
	_glView->_specMat[2] = 0.626959f;
	_glView->_shine = fabs(128.0*0.6);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonEmerald_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0215f;
	_glView->_ambiMat[1] = 0.1745f;
	_glView->_ambiMat[2] = 0.0215f;
	_glView->_diffMat[0] = 0.07568f;
	_glView->_diffMat[1] = 0.61424f;
	_glView->_diffMat[2] = 0.07568f;
	_glView->_specMat[0] = 0.633f;
	_glView->_specMat[1] = 0.727811f;
	_glView->_specMat[2] = 0.633f;
	_glView->_shine = fabs(128.0*0.6);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonTurquoise_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.1f;
	_glView->_ambiMat[1] = 0.18725f;
	_glView->_ambiMat[2] = 0.1745f;
	_glView->_diffMat[0] = 0.396f;
	_glView->_diffMat[1] = 0.74151f;
	_glView->_diffMat[2] = 0.69102f;
	_glView->_specMat[0] = 0.297254f;
	_glView->_specMat[1] = 0.30829f;
	_glView->_specMat[2] = 0.306678f;
	_glView->_shine = fabs(128.0*0.1);

	_glView->updateView();
	updateControls();
}

void MatlEditor::on_pushButtonJade_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.135f;
	_glView->_ambiMat[1] = 0.2225f;
	_glView->_ambiMat[2] = 0.1575f;
	_glView->_diffMat[0] = 0.54f;
	_glView->_diffMat[1] = 0.89f;
	_glView->_diffMat[2] = 0.63f;
	_glView->_specMat[0] = 0.316228f;
	_glView->_specMat[1] = 0.316228f;
	_glView->_specMat[2] = 0.316228f;
	_glView->_shine = fabs(128.0*0.1);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonObsidian_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.05375f;
	_glView->_ambiMat[1] = 0.05f;
	_glView->_ambiMat[2] = 0.06625f;
	_glView->_diffMat[0] = 0.18275f;
	_glView->_diffMat[1] = 0.17f;
	_glView->_diffMat[2] = 0.22525f;
	_glView->_specMat[0] = 0.332741f;
	_glView->_specMat[1] = 0.328634f;
	_glView->_specMat[2] = 0.346435f;
	_glView->_shine = fabs(128.0*0.3);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonPearl_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.25f;
	_glView->_ambiMat[1] = 0.20725f;
	_glView->_ambiMat[2] = 0.20725f;
	_glView->_diffMat[0] = 1.0f;
	_glView->_diffMat[1] = 0.829f;
	_glView->_diffMat[2] = 0.829f;
	_glView->_specMat[1] = 0.296648f;
	_glView->_specMat[2] = 0.296648f;
	_glView->_specMat[0] = 0.299948f;
	_glView->_shine = fabs(128.0*0.088);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonChrome_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.25f;
	_glView->_ambiMat[1] = 0.25f;
	_glView->_ambiMat[2] = 0.25f;
	_glView->_diffMat[0] = 0.4f;
	_glView->_diffMat[1] = 0.4f;
	_glView->_diffMat[2] = 0.4f;
	_glView->_specMat[0] = 0.774597f;
	_glView->_specMat[1] = 0.774597f;
	_glView->_specMat[2] = 0.774597f;
	_glView->_shine = fabs(128.0*0.6);

	_glView->updateView();
	updateControls();
}



void MatlEditor::on_pushButtonBlackPlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.01f;
	_glView->_diffMat[1] = 0.01f;
	_glView->_diffMat[2] = 0.01f;
	_glView->_specMat[0] = 0.5f;
	_glView->_specMat[1] = 0.5f;
	_glView->_specMat[2] = 0.5f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonCyanPlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.1f;
	_glView->_ambiMat[2] = 0.06f;
	_glView->_diffMat[0] = 0.0f;
	_glView->_diffMat[1] = 0.50980392f;
	_glView->_diffMat[2] = 0.50980392f;
	_glView->_specMat[0] = 0.50196078f;
	_glView->_specMat[1] = 0.50196078f;
	_glView->_specMat[2] = 0.50196078f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonGreenPlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.1f;
	_glView->_diffMat[1] = 0.35f;
	_glView->_diffMat[2] = 0.1f;
	_glView->_specMat[0] = 0.45f;
	_glView->_specMat[1] = 0.55f;
	_glView->_specMat[2] = 0.45f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonRedPlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.5f;
	_glView->_diffMat[1] = 0.0f;
	_glView->_diffMat[2] = 0.0f;
	_glView->_specMat[0] = 0.7f;
	_glView->_specMat[1] = 0.6f;
	_glView->_specMat[2] = 0.6f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonWhitePlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.55f;
	_glView->_diffMat[1] = 0.55f;
	_glView->_diffMat[2] = 0.55f;
	_glView->_specMat[0] = 0.70f;
	_glView->_specMat[1] = 0.70f;
	_glView->_specMat[2] = 0.70f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonYellowPlastic_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.5f;
	_glView->_diffMat[1] = 0.5f;
	_glView->_diffMat[2] = 0.0f;
	_glView->_specMat[0] = 0.6f;
	_glView->_specMat[1] = 0.6f;
	_glView->_specMat[2] = 0.5f;
	_glView->_shine = fabs(128.0*0.25);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonBlackRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.02f;
	_glView->_ambiMat[1] = 0.02f;
	_glView->_ambiMat[2] = 0.02f;
	_glView->_diffMat[0] = 0.01f;
	_glView->_diffMat[1] = 0.01f;
	_glView->_diffMat[2] = 0.01f;
	_glView->_specMat[0] = 0.4f;
	_glView->_specMat[1] = 0.4f;
	_glView->_specMat[2] = 0.4f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonCyanRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.05f;
	_glView->_ambiMat[2] = 0.05f;
	_glView->_diffMat[0] = 0.4f;
	_glView->_diffMat[1] = 0.5f;
	_glView->_diffMat[2] = 0.5f;
	_glView->_specMat[0] = 0.04f;
	_glView->_specMat[1] = 0.7f;
	_glView->_specMat[2] = 0.7f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonGreenRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.0f;
	_glView->_ambiMat[1] = 0.05f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.4f;
	_glView->_diffMat[1] = 0.5f;
	_glView->_diffMat[2] = 0.4f;
	_glView->_specMat[0] = 0.04f;
	_glView->_specMat[1] = 0.7f;
	_glView->_specMat[2] = 0.04f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonRedRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.05f;
	_glView->_ambiMat[1] = 0.0f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.7f;
	_glView->_diffMat[1] = 0.4f;
	_glView->_diffMat[2] = 0.4f;
	_glView->_specMat[0] = 0.7f;
	_glView->_specMat[1] = 0.04f;
	_glView->_specMat[2] = 0.04f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonWhiteRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.05f;
	_glView->_ambiMat[1] = 0.05f;
	_glView->_ambiMat[2] = 0.05f;
	_glView->_diffMat[0] = 0.5f;
	_glView->_diffMat[1] = 0.5f;
	_glView->_diffMat[2] = 0.5f;
	_glView->_specMat[0] = 0.7f;
	_glView->_specMat[1] = 0.7f;
	_glView->_specMat[2] = 0.7f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}


void MatlEditor::on_pushButtonYellowRubber_clicked()
{
	//Light Values 
	_glView->_ambiLight = { 0, 0, 0, 1 };
	_glView->_diffLight = { 1, 1, 1, 1 };
	_glView->_specLight = { 0.5, 0.5, 0.5, 1 };

	//Material Values
	_glView->_ambiMat[0] = 0.05f;
	_glView->_ambiMat[1] = 0.05f;
	_glView->_ambiMat[2] = 0.0f;
	_glView->_diffMat[0] = 0.5f;
	_glView->_diffMat[1] = 0.5f;
	_glView->_diffMat[2] = 0.4f;
	_glView->_specMat[0] = 0.7f;
	_glView->_specMat[1] = 0.7f;
	_glView->_specMat[2] = 0.04f;
	_glView->_shine = fabs(128.0*0.078125);

	_glView->updateView();
	updateControls();
}

void MatlEditor::on_comboBoxModel_currentIndexChanged(int index)
{
	_glView->setModelNum(index+1);
}

void MatlEditor::updateControls()
{
	sliderShine->setValue((int)_glView->_shine);
	sliderTransparency->setValue((int)(255 * _glView->_opacity));
	sliderLightPosX->setValue((int)_glView->_lightPosition.x());
	sliderLightPosY->setValue((int)_glView->_lightPosition.y());
	sliderLightPosZ->setValue((int)_glView->_lightPosition.z());

	QColor col;
	col.setRgbF(_glView->_ambiLight.x(), _glView->_ambiLight.y(), _glView->_ambiLight.z());
	QString qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonLightAmbient->setStyleSheet(qss);

	col.setRgbF(_glView->_diffLight.x(), _glView->_diffLight.y(), _glView->_diffLight.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonLightDiffuse->setStyleSheet(qss);

	col.setRgbF(_glView->_specLight.x(), _glView->_specLight.y(), _glView->_specLight.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonLightSpecular->setStyleSheet(qss);

	col.setRgbF(_glView->_ambiMat.x(), _glView->_ambiMat.y(), _glView->_ambiMat.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonMaterialAmbient->setStyleSheet(qss);

	col.setRgbF(_glView->_diffMat.x(), _glView->_diffMat.y(), _glView->_diffMat.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonMaterialDiffuse->setStyleSheet(qss);

	col.setRgbF(_glView->_specMat.x(), _glView->_specMat.y(), _glView->_specMat.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonMaterialSpecular->setStyleSheet(qss);

	col.setRgbF(_glView->_emmiMat.x(), _glView->_emmiMat.y(), _glView->_emmiMat.z());
	qss = QString("background-color: %1;color: %2").arg(col.name()).arg(col.lightness() < 75 ? QColor(Qt::white).name() : QColor(Qt::black).name());
	pushButtonMaterialEmissive->setStyleSheet(qss);
}


void MatlEditor::updateComboBox()
{
	std::vector<TriangleMesh*> store = _glView->getMeshStore();
	for (TriangleMesh* mesh : store)
	{
		comboBoxModel->addItem(mesh->getName());
	}
}


void MatlEditor::showEvent(QShowEvent*)
{
	updateComboBox();
}

void MatlEditor::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	QApplication::sendEvent(_glView, event);
}
