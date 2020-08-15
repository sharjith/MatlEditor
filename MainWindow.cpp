#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MatlEditor.h"
#include "GLView.h"
#include <QtOpenGL>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	Q_INIT_RESOURCE(MatlEditor);
	_editor = new MatlEditor(this);
	setCentralWidget(_editor);

	_bFirstTime = true;
}

MainWindow::~MainWindow()
{
	if (_editor)
		delete _editor;
	delete ui;
}

void MainWindow::on_actionExit_triggered(bool /*checked*/)
{
	close();
	qApp->exit();
}

void MainWindow::on_actionAbout_triggered(bool /*checked*/)
{
    QMessageBox::about(this, "About Material Editor", "Application to demonstrate Modern OpenGL");
}

void MainWindow::on_actionAbout_Qt_triggered(bool /*checked*/)
{
	QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
	if (_bFirstTime)
	{
		_editor->getGLView()->setModelNum(6);
		_bFirstTime = false;
	}

}

void MainWindow::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	QApplication::sendEvent(_editor, event);
}
