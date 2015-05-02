#include "MainWindow.hpp"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setupUi(this);
}

void MainWindow::resetGraphicsView()
{
	QGraphicsScene* oldScene = graphicsView->scene();
	if (oldScene) {
		graphicsView->setScene(nullptr);
		delete oldScene;
	}
}

void MainWindow::on_btnGenerate_clicked()
{
	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();

	resetGraphicsView();
	QGraphicsScene* scene = new QGraphicsScene(this);
	scene->setSceneRect(0.f, 0.f, 200.f, 200.f);
	scene->addLine(0.f, 0.f, 100.f, 100.f);
	graphicsView->setScene(scene);
}
