#include "SuperEllipsoidEditor.h"
#include "ui_SuperEllipsoidEditor.h"
#include "SuperEllipsoid.h"
#include "GLView.h"

SuperEllipsoidEditor::SuperEllipsoidEditor(SuperEllipsoid* sellipsoid, QWidget *parent)
	:_ellipsoid(sellipsoid),
	QWidget(parent)
{
	ui = new Ui::SuperEllipsoidEditor();
	ui->setupUi(this);

	ui->doubleSpinBoxRad->setValue(_ellipsoid->_radius);
	ui->doubleSpinBoxN1->setValue(_ellipsoid->_n1);
	ui->doubleSpinBoxN2->setValue(_ellipsoid->_n2);
}

SuperEllipsoidEditor::~SuperEllipsoidEditor()
{
	delete ui;
}

void SuperEllipsoidEditor::on_doubleSpinBoxScaleX_valueChanged(double val)
{
	_ellipsoid->_scaleX = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SuperEllipsoidEditor::on_doubleSpinBoxScaleY_valueChanged(double val)
{
	_ellipsoid->_scaleY = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SuperEllipsoidEditor::on_doubleSpinBoxScaleZ_valueChanged(double val)
{
	_ellipsoid->_scaleZ = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SuperEllipsoidEditor::on_doubleSpinBoxN1_valueChanged(double val)
{
	_ellipsoid->_n1 = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SuperEllipsoidEditor::on_doubleSpinBoxN2_valueChanged(double val)
{
	_ellipsoid->_n2 = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SuperEllipsoidEditor::on_doubleSpinBoxRad_valueChanged(double val)
{
	_ellipsoid->_radius = val;
	_ellipsoid->buildMesh(_ellipsoid->getSlices(), _ellipsoid->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}
