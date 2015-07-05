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
	const float marginClassStructure = 20.f;
	const float marginStructure = 10.f;

	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();

	try {
		Parser parser(begin, end);

		resetGraphicsView();

		scene = new QGraphicsScene(this);
		this->actionDirect_Print->setEnabled(true);
		this->actionPrint_To_PDF->setEnabled(true);

		float height = 0.;

		ClassChartDrawer classChartDrawer;
		QGraphicsItem* classChart = classChartDrawer.drawClassChart(parser.getResult().classChart);
		scene->addItem(classChart);
		classChart->setPos(0,0);
		height += scene->sceneRect().height() + marginClassStructure;

		for (const std::unique_ptr<StructureChart>& structureChartData : parser.getResult().structureCharts) {
			StructureChartDrawer drawer(scene);
			QGraphicsItem* structureChart = drawer.drawStructureChart(structureChartData.get());
			scene->addItem(structureChart);
			structureChart->setPos(0, height);
			height += structureChart->childrenBoundingRect().height() + marginStructure;
		}

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
