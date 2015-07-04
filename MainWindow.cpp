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
#include <iostream>
#include <sstream>
#include "Parser.hpp"
#include "StructureChartDrawer.hpp"

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

QGraphicsItemGroup* generateClassDiagram(std::string str1, std::string str2, std::string str3)
{
	QGraphicsItemGroup* group = new QGraphicsItemGroup();
	group->setHandlesChildEvents(false);

	QGraphicsSimpleTextItem* text1 = new QGraphicsSimpleTextItem();
	text1->setPos(2., 2.);
	text1->setText(QString::fromStdString(str1));
	group->addToGroup(text1);

	QGraphicsSimpleTextItem* text2 = new QGraphicsSimpleTextItem();
	text2->setPos(text1->mapToParent(text1->boundingRect().bottomLeft()) + QPointF(0.,1.));
	if (str2.empty()) {
		text2->setText("\n");
		text2->setFont(QFont("", 2));
	} else {
		text2->setText(QString::fromStdString(str2));
	}
	group->addToGroup(text2);

	QGraphicsSimpleTextItem* text3 = new QGraphicsSimpleTextItem();
	text3->setPos(text2->mapToParent(text2->boundingRect().bottomLeft()) + QPointF(0.,1.));
	if (str3.empty()) {
		text3->setText("\n");
		text3->setFont(QFont("", 2));
	} else {
		text3->setText(QString::fromStdString(str3));
	}
	group->addToGroup(text3);

	QRectF textRect = group->boundingRect().adjusted(-1., -1., 0., 0.);
	textRect.setWidth(std::ceil(textRect.width()));

	QGraphicsRectItem* rect = new QGraphicsRectItem();
	rect->setRect(textRect);
	group->addToGroup(rect);

	QGraphicsLineItem* line1 = new QGraphicsLineItem();
	line1->setLine(QLineF(text1->mapToParent(text1->boundingRect().bottomLeft()),
						  QPointF(textRect.right(), text1->mapToParent(0., text1->boundingRect().bottom()).y())));
	group->addToGroup(line1);

	QGraphicsLineItem* line2 = new QGraphicsLineItem();
	line2->setLine(QLineF(text2->mapToParent(text2->boundingRect().bottomLeft()),
						  QPointF(textRect.right(), text2->mapToParent(0., text2->boundingRect().bottom()).y())));
	group->addToGroup(line2);

	return group;
}

std::string visibilityToString(Visibility visibility)
{
	switch (visibility) {
		case Visibility::private_:
			return "-";
		case Visibility::protected_:
			return "#";
		case Visibility::public_:
			return "+";
	}
}

QGraphicsItemGroup* drawClassChart(const ClassChart& classChart)
{
	QGraphicsItemGroup* group = new QGraphicsItemGroup();
	int left = 0;
	for (const std::unique_ptr<Class>& class_ : classChart.classes) {
		std::ostringstream attrstr;
		bool first = false;
		for (const Attribute& attr : class_->attributes) {
			if (first++) attrstr << "\n";
			attrstr << visibilityToString(attr.visibility) << attr.name << ": " << attr.type->umlName();
		}
		std::ostringstream opstr;
		first = false;
		for (const Operation& op : class_->operations) {
			if (first++) opstr << "\n";
			opstr << visibilityToString(op.visibility) << op.name << "(";
			if (!op.arguments.empty()) {
				bool first;
				for (const Argument& arg : op.arguments) {
					if (first++) opstr << "; ";
					opstr << arg.name << ": " << arg.type->umlName();
				}
			}
			opstr << ")";
			if (op.returnType) {
				opstr << ": " << op.returnType->umlName();
			}
		}
		QGraphicsItemGroup* classBox = generateClassDiagram(class_->name, attrstr.str(), opstr.str());
		classBox->setPos(group->boundingRect().topRight());

		group->addToGroup(classBox);

		if (&class_ != &classChart.classes.back()) {
			QGraphicsLineItem* line = new QGraphicsLineItem();
			line->setLine({group->boundingRect().topRight() + QPointF(0,group->boundingRect().height()/2), group->boundingRect().topRight() + QPointF(30,group->boundingRect().height()/2)});
			group->addToGroup(line);

			const QPolygonF poly({{0,5*2},{5*sqrt(2),0},{-5*sqrt(2),0}});
			QGraphicsPolygonItem* polygon = new QGraphicsPolygonItem(poly);
			polygon->setRotation(-90);
			polygon->setPos(group->boundingRect().topRight() + QPointF(0,group->boundingRect().height()/2));
			group->addToGroup(polygon);
		}
	}
	return group;
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

		ClassChart demoClassChart;
		demoClassChart.classes.emplace_back(new Class{"A", {}, {}});
		demoClassChart.classes.back()->attributes.emplace_back("x", Type::createFromUmlName("Int"), Visibility::private_);
		demoClassChart.classes.back()->operations.emplace_back("getX", Type::createFromUmlName("Int"), Visibility::public_);
		demoClassChart.classes.back()->operations.emplace_back("resetX", nullptr, Visibility::public_);
		demoClassChart.classes.emplace_back(new Class{"B", {}, {}});
		demoClassChart.classes.back()->attributes.emplace_back("y", Type::createFromUmlName("Int"), Visibility::private_);
		scene->addItem(drawClassChart(demoClassChart));

		//draw StructureCharts
		StructureChartDrawer drawer(scene);
		QGraphicsItem* structureChart = drawer.drawStructureChart(parser.getResult().structureCharts.front().get());
		structureChart->setPos(0.5, 100.5);
		scene->addItem(structureChart);

		//-------------create test environment for switchBlock-----------------------
		StructureChart* chart2 = new StructureChart;

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
		theMap["vier"].blocks.emplace_back(sb41);

		SwitchBlock* switchBlock = new SwitchBlock("switch-expression\ntest\ntest\n...\n...", theMap);

		chart2->headline = "TestChart for Switch-Blocks";
		chart2->declarations.push_back(Declaration("testVar", "testtype"));
		chart2->declarations.push_back(Declaration("testVarddd", "int"));
		chart2->declarations.push_back(Declaration("testVar12", "human"));
		chart2->root.blocks.emplace_back(sb4);
		chart2->root.blocks.emplace_back(switchBlock);

		QGraphicsItem* structureChart2 = drawer.drawStructureChart(chart2);
		structureChart2->setPos(700.5, 400.5);
		scene->addItem(structureChart2);
		//--------------------------------------------------------------------------

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
