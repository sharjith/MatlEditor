#ifndef CLIPPINGPLANESEDITOR_H
#define CLIPPINGPLANESEDITOR_H

#include <QDialog>

namespace Ui {
class ClippingPlanesEditor;
}
class GLView;
class ClippingPlanesEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ClippingPlanesEditor(QWidget *parent = nullptr);
    ~ClippingPlanesEditor();

protected slots:
	void keyPressEvent(QKeyEvent *e);
	void on_toolButtonXY_toggled(bool checked);
	void on_toolButtonYZ_toggled(bool checked);
	void on_toolButtonZX_toggled(bool checked);
	void on_toolButtonFlipXY_toggled(bool checked);
	void on_toolButtonFlipYZ_toggled(bool checked);
	void on_toolButtonFlipZX_toggled(bool checked);
	void on_doubleSpinBoxXYCoeff_valueChanged(double val);
	void on_doubleSpinBoxYZCoeff_valueChanged(double val);
	void on_doubleSpinBoxZXCoeff_valueChanged(double val);

private:
    Ui::ClippingPlanesEditor *ui;

	GLView* _glView;
};

#endif // CLIPPINGPLANESEDITOR_H
