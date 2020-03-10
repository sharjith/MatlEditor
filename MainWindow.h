#pragma once

#include <QMainWindow>
namespace Ui { class MainWindow; };
class MatlEditor;
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

protected:
	void showEvent(QShowEvent *event);
	void moveEvent(QMoveEvent *event);

protected slots:
	void on_actionExit_triggered(bool checked = false);
	void on_actionAbout_triggered(bool checked = false);
	void on_actionAbout_Qt_triggered(bool checked = false);


private:
	Ui::MainWindow *ui;
	MatlEditor* _editor;

	bool _bFirstTime;
};
