#include "ClippingPlanesEditor.h"
#include "ui_ClippingPlanesEditor.h"

#include "GLView.h"

#include <QKeyEvent>

ClippingPlanesEditor::ClippingPlanesEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClippingPlanesEditor),
	_glView(dynamic_cast<GLView*>(parent))
{
    ui->setupUi(this);
}

ClippingPlanesEditor::~ClippingPlanesEditor()
{
    delete ui;
}

void ClippingPlanesEditor::keyPressEvent(QKeyEvent *e)
{
	if (e->key() != Qt::Key_Escape)
		QDialog::keyPressEvent(e);
	else {/* minimize */ }
}

void ClippingPlanesEditor::on_toolButtonXY_toggled(bool checked)
{
	_glView->_clipXEnabled = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_toolButtonYZ_toggled(bool checked)
{
	_glView->_clipYEnabled = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_toolButtonZX_toggled(bool checked)
{
	_glView->_clipZEnabled = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_toolButtonFlipXY_toggled(bool checked)
{
	_glView->_clipXFlipped = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_toolButtonFlipYZ_toggled(bool checked)
{
	_glView->_clipYFlipped = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_toolButtonFlipZX_toggled(bool checked)
{
	_glView->_clipZFlipped = checked;
	_glView->update();
}

void ClippingPlanesEditor::on_doubleSpinBoxXYCoeff_valueChanged(double val)
{
	_glView->_clipXCoeff = val;
	_glView->update();
}

void ClippingPlanesEditor::on_doubleSpinBoxYZCoeff_valueChanged(double val)
{
	_glView->_clipYCoeff = val;
	_glView->update();
}

void ClippingPlanesEditor::on_doubleSpinBoxZXCoeff_valueChanged(double val)
{
	_glView->_clipZCoeff = val;
	_glView->update();
}
