#include "MainWindow.hpp"
#include <QFileDialog>
#include <QTextStream>
#include "StructureChart.hpp"

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

void MainWindow::on_actionGenerate_triggered()
{
	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();

	resetGraphicsView();
	/*create testenvironment========================================================================*/
	QGraphicsScene* scene = new QGraphicsScene(this);

	StructureChart* chart;
	chart->headline = "This is a headline!!!";
/*
	Declaration dec1;
	dec1.varName = "spielBrettDaten[0..4][0..4]";
	PrimitiveType type1;


	dec1.type = type1.createFromUmlName("GZ");

	chart->declarations.push_back(dec1);
*/
	StructureChartDrawer* drawer = new StructureChartDrawer(scene, chart);
	drawer->drawHeadline();
	drawer->drawSurroundingRectangle();
	//===============================================================================================
	graphicsView->setScene(scene);
}

void MainWindow::on_actionOpen_triggered()
{
	QStringList files = QFileDialog::getOpenFileNames(this, "Select code files", "../qtroad", "C/C++ (*.c *.h *.cpp *.hpp)");
	QString content;
	for (QString fileName : files) {
		QFile file(fileName);
		file.open(QIODevice::ReadOnly);
		content += QTextStream(&file).readAll() + "\n";
	}
	plainTextEdit->setPlainText(content);
}

//===========================================================================================================
StructureChartDrawer::StructureChartDrawer(QGraphicsScene* pScene, StructureChart* pChart)
{
	scene = pScene;
	chart = pChart;
	rect_size_x = 100;
	rect_size_y = 200;
}

void StructureChartDrawer::drawHeadline()
{
	scene->addSimpleText(QString::fromStdString(chart->headline));
}

void StructureChartDrawer::drawDeclarations()
{
	for (Declaration& decl : chart->declarations)
		scene->addSimpleText(QString::fromStdString(decl.varName+": "+decl.type->umlName()));
}

void StructureChartDrawer::drawBody()
{

}

void StructureChartDrawer::drawStructureChart()
{
	drawHeadline();
	drawDeclarations();
}

void StructureChartDrawer::drawSurroundingRectangle()
{
	scene->setSceneRect(0.f, 0.f, rect_size_x, rect_size_y);
	scene->addRect(0.f, 0.f, rect_size_x, rect_size_y);
}

