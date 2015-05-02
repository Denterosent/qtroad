#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
	private:
		void resetGraphicsView();
	private slots:
		void on_actionGenerate_triggered();
		void on_actionOpen_triggered();
};

#endif // MAINWINDOW_HPP
