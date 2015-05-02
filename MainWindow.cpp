#include "MainWindow.hpp"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setupUi(this);
}

void MainWindow::on_btnGenerate_clicked()
{
	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();
}
