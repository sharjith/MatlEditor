
#ifndef __MATLEDITOR_H__
#define __MATLEDITOR_H__

#include "ui_MatlEditor.h"

class GLView;
class SphericalHarmonicsEditor;
class MatlEditor : public QWidget, private Ui::MatlEditor
{
	Q_OBJECT
public:
	MatlEditor(QWidget* parent = 0);
	~MatlEditor();

	GLView* const getGLView() const { return _glView; }

private slots:	
	void on_modelButtonNext_clicked();
	void on_modelButtonPrev_clicked();
	void on_checkTexture_toggled(bool checked);
	void on_checkShaded_toggled(bool checked);
	void on_textureButton_clicked();
	void on_defaultButton_clicked();

	void on_toolButtonFitAll_clicked(bool checked);
	void on_toolButtonWindowZoom_clicked(bool checked);
	void on_toolButtonTopView_clicked(bool checked);
	void on_toolButtonBottomView_clicked(bool checked);
	void on_toolButtonLeftView_clicked(bool checked);
	void on_toolButtonRightView_clicked(bool checked);
	void on_toolButtonFrontView_clicked(bool checked);
	void on_toolButtonBackView_clicked(bool checked);
	void on_toolButtonProjection_toggled(bool checked);
	void on_toolButtonSectionView_toggled(bool checked);
	void on_toolButtonMultiView_toggled(bool checked);
	
	void on_isometricView_triggered(bool checked);
	void on_dimetricView_triggered(bool checked);
	void on_trimetricView_triggered(bool checked);
	
	void on_pushButtonLightAmbient_clicked();
	void on_pushButtonLightDiffuse_clicked();
	void on_pushButtonLightSpecular_clicked();

	void on_pushButtonMaterialAmbient_clicked();
	void on_pushButtonMaterialDiffuse_clicked();
	void on_pushButtonMaterialSpecular_clicked();
	void on_pushButtonMaterialEmissive_clicked();

	void on_sliderLightPosX_valueChanged(int value);
	void on_sliderLightPosY_valueChanged(int value);
	void on_sliderLightPosZ_valueChanged(int value);

	void on_sliderTransparency_valueChanged(int value);
	void on_sliderShine_valueChanged(int value);

	void on_pushButtonBrass_clicked();
	void on_pushButtonBronze_clicked();
	void on_pushButtonCopper_clicked();
	void on_pushButtonGold_clicked();
	void on_pushButtonSilver_clicked();
	void on_pushButtonRuby_clicked();
	void on_pushButtonEmerald_clicked();
	void on_pushButtonTurquoise_clicked();
	void on_pushButtonJade_clicked();
	void on_pushButtonObsidian_clicked();
	void on_pushButtonPearl_clicked();
	void on_pushButtonChrome_clicked();
	void on_pushButtonBlackPlastic_clicked();
	void on_pushButtonCyanPlastic_clicked();
	void on_pushButtonGreenPlastic_clicked();
	void on_pushButtonRedPlastic_clicked();
	void on_pushButtonWhitePlastic_clicked();
	void on_pushButtonYellowPlastic_clicked();
	void on_pushButtonBlackRubber_clicked();
	void on_pushButtonCyanRubber_clicked();
	void on_pushButtonGreenRubber_clicked();
	void on_pushButtonRedRubber_clicked();
	void on_pushButtonWhiteRubber_clicked();
	void on_pushButtonYellowRubber_clicked();
	void on_comboBoxModel_currentIndexChanged(int index);
	
protected:
	void showEvent(QShowEvent *event);
	void moveEvent(QMoveEvent *event);

private:
	GLView* _glView;	

	QAction* isometricView;
	QAction* dimetricView;
	QAction* trimetricView;

private:
	void updateControls();
	void updateComboBox();
};

#endif

