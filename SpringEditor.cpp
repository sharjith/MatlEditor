#include "SpringEditor.h"
#include "ui_SpringEditor.h"
#include "Spring.h"
#include "GLView.h"

SpringEditor::SpringEditor(Spring* spring, QWidget *parent)
	: QDialog(parent),
	_spring(spring)
{
	ui = new Ui::SpringEditor();
	ui->setupUi(this);

	ui->doubleSpinBoxSecRad->setValue(_spring->_sectionRadius);
	ui->doubleSpinBoxCoilRad->setValue(_spring->_coilRadius);
	ui->doubleSpinBoxPitch->setValue(_spring->_pitch);
	ui->doubleSpinBoxTurns->setValue(_spring->_turns);
	
}

SpringEditor::~SpringEditor()
{
	delete ui;
}

void SpringEditor::on_doubleSpinBoxSecRad_valueChanged(double val)
{
	_spring->_sectionRadius = val;
	_spring->buildMesh(_spring->getSlices(), _spring->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SpringEditor::on_doubleSpinBoxCoilRad_valueChanged(double val)
{
	_spring->_coilRadius = val;
	_spring->buildMesh(_spring->getSlices(), _spring->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SpringEditor::on_doubleSpinBoxPitch_valueChanged(double val)
{
	_spring->_pitch = val;
	_spring->buildMesh(_spring->getSlices(), _spring->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}

void SpringEditor::on_doubleSpinBoxTurns_valueChanged(double val)
{
	_spring->_turns = val;
	_spring->buildMesh(_spring->getSlices(), _spring->getStacks());
	dynamic_cast<GLView*>(parent())->update();
}