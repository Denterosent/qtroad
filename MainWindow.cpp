#include "MainWindow.hpp"
#include <QFileDialog>
#include <QTextStream>
#include "StructureChart.hpp"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QDebug>

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

QGraphicsItemGroup* generateClassDiagram(QString name, QString attribs, QString ops)
{
	QGraphicsItemGroup* group = new QGraphicsItemGroup();

	QGraphicsSimpleTextItem* text1 = new QGraphicsSimpleTextItem(group);
	text1->setText(name);

	QGraphicsSimpleTextItem* text2 = new QGraphicsSimpleTextItem(group);
	text2->setText(attribs);
	text2->setPos(text1->mapToParent(text1->boundingRect().bottomLeft()));

	QGraphicsSimpleTextItem* text3 = new QGraphicsSimpleTextItem(group);
	text3->setText(ops);
	text3->setPos(text2->mapToParent(text2->boundingRect().bottomLeft()));

	QGraphicsRectItem* rect = new QGraphicsRectItem(group);
	rect->setRect(group->childrenBoundingRect());

	QGraphicsLineItem* line1 = new QGraphicsLineItem(group);
	line1->setLine(QLineF(text1->mapToParent(text1->boundingRect().bottomLeft()),
						  text1->mapToParent(rect->boundingRect().right(), text1->boundingRect().bottom())));

	QGraphicsLineItem* line2 = new QGraphicsLineItem(group);
	line2->setLine(QLineF(text2->mapToParent(text2->boundingRect().bottomLeft()),
						  text2->mapToParent(rect->boundingRect().right(), text2->boundingRect().bottom())));

	return group;
}

void MainWindow::on_actionGenerate_triggered()
{
	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();

	resetGraphicsView();
	QGraphicsScene* scene = new QGraphicsScene(this);

	QGraphicsItemGroup* classDiagram1 = generateClassDiagram("TestClass", "#a: GZ\n#b: GZ", "+f(): GZ\n+g(): GZ");
	QGraphicsItemGroup* classDiagram2 = generateClassDiagram("AnotherTestClass", "-x: GZ", "+y(): GZ");
	classDiagram2->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topRight()) + QPointF(10.5, 0));
	classDiagram1->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topLeft()));
	scene->addItem(classDiagram1);
	scene->addItem(classDiagram2);
	
	/*create testenvironment========================================================================*/
	StructureChart* chart = new StructureChart();
	//chart->declarations.emplace_back(new ClassType(), 'v');
	chart->headline = "This is a headline!!!";
/*
	Declaration dec1;
	dec1.varName = "spielBrettDaten[0..4][0..4]";
	PrimitiveType type1;


	dec1.type = type1.createFromUmlName("GZ");

	chart->declarations.push_back(dec1);
*/
	StructureChartDrawer* drawer = new StructureChartDrawer(scene, chart);
	//drawer->drawHeadline();
	//drawer->drawSurroundingRectangle();
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

