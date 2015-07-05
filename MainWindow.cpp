#include "MainWindow.hpp"
#include <QFileDialog>
#include <QTextStream>
#include "StructureChart.hpp"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QMessageBox>
#include <QtPrintSupport>
#include <QPainter>
#include <QPrinter>
#include "Parser.hpp"
#include "StructureChartDrawer.hpp"
#include "ClassChartDrawer.hpp"

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

	try {
		Parser parser(begin, end);

		resetGraphicsView();

		scene = new QGraphicsScene(this);
		this->actionDirect_Print->setEnabled(true);
		this->actionPrint_To_PDF->setEnabled(true);

		QPointF pos;

		ClassChartDrawer classChartDrawer;
		QGraphicsItem* classChart = classChartDrawer.drawClassChart(parser.getResult().classChart);
		scene->addItem(classChart);
		classChart->setPos(pos);
		pos = classChart->mapToParent(classChart->boundingRect().bottomLeft());

		for (const std::unique_ptr<StructureChart>& structureChartData : parser.getResult().structureCharts) {
			StructureChartDrawer drawer(scene);
			QGraphicsItem* structureChart = drawer.drawStructureChart(structureChartData.get());
			scene->addItem(structureChart);
			structureChart->setPos(pos);
			pos = structureChart->mapToParent(structureChart->boundingRect().bottomLeft());
		}

		//-------------create test environment for switchBlock-----------------------
/*		StructureChart* chart2 = new StructureChart;

		SimpleBlock* sb1 = new SimpleBlock("sb1");
		SimpleBlock* sb2 = new SimpleBlock("sb2\nsb2");
		SimpleBlock* sb3 = new SimpleBlock("sb3");
		SimpleBlock* sb4 = new SimpleBlock("sb4");
		SimpleBlock* sb11 = new SimpleBlock("sb11");
		SimpleBlock* sb21 = new SimpleBlock("sb21");
		SimpleBlock* sb31 = new SimpleBlock("sb31");
		SimpleBlock* sb41 = new SimpleBlock("sb41");

		std::map<std::string, BlockSequence> theMap;
		theMap["eins"].blocks.emplace_back(sb1);
		theMap["eins"].blocks.emplace_back(sb3);
		theMap["zwei"].blocks.emplace_back(sb2);
		theMap["drei"].blocks.emplace_back(sb11);
		theMap["vier"].blocks.emplace_back(sb21);
		theMap["vier"].blocks.emplace_back(sb31);
		theMap[""].blocks.emplace_back(sb41);

		SwitchBlock* switchBlock = new SwitchBlock("switch-expression", theMap);

		chart2->headline = "TestChart for Switch-Blocks";
		chart2->declarations.push_back(Declaration("testVar", "testtype"));
		chart2->declarations.push_back(Declaration("testVarddd", "int"));
		chart2->declarations.push_back(Declaration("testVar12", "human"));
		chart2->root.blocks.emplace_back(sb4);
		chart2->root.blocks.emplace_back(switchBlock);

		QGraphicsItem* structureChart2 = drawer.drawStructureChart(chart2);
		structureChart2->setPos(700.5, 400.5);
		scene->addItem(structureChart2);*/
		//--------------------------------------------------------------------------*/

		graphicsView->setScene(scene);
	} catch (std::runtime_error& e) {
		QMessageBox::warning(this, "Parser Error", e.what());
	}
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

void MainWindow::on_actionDirect_Print_triggered()
{
	QPrinter printer;
	if (QPrintDialog(&printer).exec() == QDialog::Accepted) { //Bug: while choosing the directory to print a pdf-file with Nitro PDF Creator, qtroad is marked with "no response"
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		scene->render(&painter);
	}
}

void MainWindow::on_actionPrint_To_PDF_triggered()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "PDF (*.pdf)");

	if (!fileName.isNull()) {
		printer.setOutputFileName(fileName);
		printer.setPageSize(QPrinter::A4);
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		scene->render(&painter);
	}
}
