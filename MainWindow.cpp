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

void MainWindow::on_actionGenerate_triggered()
{
	std::string input = plainTextEdit->toPlainText().toStdString();
	const char* begin = &*input.cbegin();
	const char* end = &*input.cend();

	resetGraphicsView();

	scene = new QGraphicsScene(this);
	this->actionDirect_Print->setEnabled(true);
	this->actionPrint_To_PDF->setEnabled(true);

	QGraphicsItemGroup* classDiagram1 = generateClassDiagram("TestClass", "#a: GZ\n#b: GZ", "+f(): GZ\n+g(): GZ");
	QGraphicsItemGroup* classDiagram2 = generateClassDiagram("AnotherTestClass", "-x: GZ", "+y(): GZ");
	classDiagram2->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topRight()) + QPointF(10, 0));
	classDiagram1->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topLeft()));
	scene->addItem(classDiagram1);
	scene->addItem(classDiagram2);

	/*===============================create testenvironment structure chart===========================*/
	StructureChart* chart = new StructureChart();
	SimpleBlock* firstSimpleBlock = new SimpleBlock("simple command") ;
	SimpleBlock* secondSimpleBlock = new SimpleBlock("this one too") ;
	SimpleBlock* thirdSimpleBlock = new SimpleBlock("me too :-)\nReally????\nAre you serious?\nhmm") ;
	SimpleBlock* fourthSimpleBlock = new SimpleBlock("this is a simple command.") ;
	SimpleBlock* fifthSimpleBlock = new SimpleBlock("this is a normal, simple Command.") ;
	SimpleBlock* firstLoopBlock = new SimpleBlock("I'm in a loop!\nYolo!!!");
	SimpleBlock* secondLoopBlock = new SimpleBlock("LoopBlock");
	BlockSequence noBS2;
	Block* yesBlock = new IfElseBlock("positiveeeee", noBS2, noBS2);

	BlockSequence yesBS2;
	yesBS2.blocks.push_back(std::unique_ptr<Block>(yesBlock));
	IfElseBlock* secondIfElseBlock = new IfElseBlock("condition\ncontinuedCondition12345", yesBS2, noBS2);

	BlockSequence loopBody;
	loopBody.blocks.push_back(std::unique_ptr<Block>(firstLoopBlock));
	loopBody.blocks.push_back(std::unique_ptr<Block>(secondIfElseBlock));
	loopBody.blocks.push_back(std::unique_ptr<Block>(secondLoopBlock));
	LoopBlock* theLoopBlock = new LoopBlock("foreve\n young!!!\n.\n.\n.", loopBody, true);

	BlockSequence yesBS;
	BlockSequence noBS;
	yesBS.blocks.push_back(std::unique_ptr<Block>(theLoopBlock));
	yesBS.blocks.push_back(std::unique_ptr<Block>(thirdSimpleBlock));
	noBS.blocks.push_back(std::unique_ptr<Block>(firstSimpleBlock));
	noBS.blocks.push_back(std::unique_ptr<Block>(secondSimpleBlock));
	IfElseBlock* firstIfElseBlock = new IfElseBlock("i", yesBS, noBS);
/*
	BlockSequence firstCase;
	std::map<std::string, BlockSequence> cases;
	SwitchBlock* switchBlock = new SwitchBlock("switch value",);*/
//	chart->root.blocks.push_back( theLoopBlock );
	chart->root.blocks.push_back(std::unique_ptr<Block>(fourthSimpleBlock));
	chart->root.blocks.push_back(std::unique_ptr<Block>(firstIfElseBlock));
	chart->root.blocks.push_back(std::unique_ptr<Block>(fifthSimpleBlock));
//	chart->root.blocks.push_back( secondIfElseBlock );

	//	PrimitiveType* firstType = new PrimitiveType;
	//	firstType->name = "first decl";
	//	chart->declarations.push_back(firstType);

	chart->headline = "This is a headline!!!";
	/*================================================================================================*/

	StructureChartDrawer drawer(scene, chart);
	drawer.drawStructureChart();

	graphicsView->setScene(scene);

	delete chart;
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
	printer.setOutputFileName(QFileDialog::getSaveFileName(this, "Save File", "", "PDF (*.pdf)"));
	printer.setPageSize(QPrinter::A4);

	QPainter painter(&printer);
	painter.setRenderHint(QPainter::Antialiasing);
	scene->render(&painter);
}

StructureChartDrawer::StructureChartDrawer(QGraphicsScene* pScene, StructureChart* pChart): scene(pScene), chart(pChart)
{
	/*Support for:
	 * Simple Blocks
	 * IfElseBlocks
	 * recursivity
	 * LoopBlocks
	 * space-filling blocks
	 * AutoHeight of Blocks
	 *
	 *no Support for:
	 * SwitchBlocks
	 * text auto-wrap
	 * declarations
	 */

	top = 50;
	left = 5;
	loopOffset = 20;
	maxWidth = 300;
	width = maxWidth;
	paddingLeft = 5;	//for every text
	paddingTop = 5;		//only for heading
	paddingTopBlock = 3;//is also a padding to bottom and used in every block
}

int StructureChartDrawer::drawBody(QGraphicsItemGroup* group, const std::vector<std::unique_ptr<Block>>& vector)
{
	//draw body
	for(unsigned int index = 0; index < vector.size(); index++){
		Block* block = vector[index].get();
		SimpleBlock* simpleBlock = dynamic_cast<SimpleBlock*>(block);
		if (simpleBlock) {
			//draw text
			QString text = QString::fromStdString(simpleBlock->command);
			QGraphicsSimpleTextItem* commandBlock = new QGraphicsSimpleTextItem(group);
			commandBlock->setText(text);
			commandBlock->setPos(left+paddingLeft,top+paddingTopBlock);

			int blockHeight = commandBlock->boundingRect().height()+paddingTopBlock*2;

			//draw rect
			QGraphicsRectItem* commandRect= new QGraphicsRectItem(group);
			commandRect->setRect(left,top,width,blockHeight);

			top += blockHeight;
		} else {
			IfElseBlock* ifElseBlock = dynamic_cast<IfElseBlock*>(block);
			if(ifElseBlock){
				//draw condition text
				QString text = QString::fromStdString(ifElseBlock->condition);
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				conditionText->setText(text);
				conditionText->setPos(left+width*0.5-conditionText->boundingRect().width()*0.5, top);

				//calculate the height of the condition block
				int textHeight = conditionText->boundingRect().height();
				int textWidth = conditionText->boundingRect().width();
				int ifElseBlockHeight = (width*textHeight)/(width-textWidth);
				int maximumHeight = width;
				if((ifElseBlockHeight > maximumHeight) or (ifElseBlockHeight < 0))
				{
					ifElseBlockHeight = maximumHeight;
				}

				//draw condition-rect
				QGraphicsRectItem* conditionRect = new QGraphicsRectItem(group);
				conditionRect->setRect(left, top, width, ifElseBlockHeight);

				//draw triangle-lines
				QGraphicsLineItem* leftLine = new QGraphicsLineItem(group);
				QGraphicsLineItem* rightLine = new QGraphicsLineItem(group);
				leftLine->setLine(left, top, width*0.5+left, top+ifElseBlockHeight);
				rightLine->setLine(left+width, top, width*0.5+left, top+ifElseBlockHeight);

				//draw yes/no -text
				QGraphicsSimpleTextItem* trueText = new QGraphicsSimpleTextItem(group);
				QGraphicsSimpleTextItem* falseText = new QGraphicsSimpleTextItem(group);
				trueText->setText("true");
				falseText->setText("false");
				trueText->setPos(left+2, top+ifElseBlockHeight-trueText->boundingRect().height());
				falseText->setPos(left+width-falseText->boundingRect().width()-2, top+ifElseBlockHeight-falseText->boundingRect().height());

				top += ifElseBlockHeight;

				//draw both bodies by calling this function recursively
				int saveTop = top, saveLeft = left, saveWidth = width, leftTop, rightTop;
				width = width*0.5;
				drawBody(group, ifElseBlock->yes.blocks);
				leftTop = top;
				top = saveTop;
				left += width;
				drawBody(group, ifElseBlock->no.blocks);
				rightTop = top;
				left = saveLeft;
				if(leftTop != rightTop){
					top = std::max(leftTop, rightTop); //to make sure, that next blocks continue at max top of both if-bodies
					QGraphicsSimpleTextItem* spaceText = new QGraphicsSimpleTextItem(group);
					spaceText->setText("∅");
					QGraphicsRectItem* spaceRect = new QGraphicsRectItem(group);
					if(leftTop < rightTop){//add spacefiller left
						spaceRect->setRect(left, leftTop, width, top-leftTop);
					}else{//addspacefiller right
						spaceRect->setRect(left+width, rightTop, width, top-rightTop);
					}
					spaceText->setPos(spaceRect->boundingRect().left()+spaceRect->boundingRect().width()*0.5-spaceText->boundingRect().width()*0.5,
									  spaceRect->boundingRect().top()+spaceRect->boundingRect().height()*0.5-spaceText->boundingRect().height()*0.5);
				}
				width = saveWidth;

			}else{
				LoopBlock* loopBlock = dynamic_cast<LoopBlock*>(block);
				if(loopBlock){
					int saveTop = top;
					if(loopBlock->headControlled){drawLoopHeading(group, loopBlock);}
					left += loopOffset;
					width -= loopOffset;
					drawBody(group, loopBlock->body.blocks);
					width += loopOffset;
					left -= loopOffset;
					if(!loopBlock->headControlled){drawLoopHeading(group, loopBlock);}

					QGraphicsRectItem* leftBorder = new QGraphicsRectItem(group);
					leftBorder->setRect(left, saveTop, width, top-saveTop);
				}else{
					std::cout << "Error: Block is neither a simple Block, loop-Block, nor an IfElseBlock!\nOnly these Blocktypes are implemented.";
				}
			}
		}
	}
// commented this ou because some peolpe din't like it:
// ====================================================
//	int numberOfDrawnBlocks = vector.size();
//	std::cout << "number of drawn blocks from one call of drawBody(): " << numberOfDrawnBlocks << std::endl;
//	return numberOfDrawnBlocks; //number of drawn blocks by this function call, may be usefull to know
}

void StructureChartDrawer::drawLoopHeading(QGraphicsItemGroup* group, LoopBlock* loopBlock)
{
	QGraphicsSimpleTextItem* loopHeading = new QGraphicsSimpleTextItem(group);
	loopHeading->setText(QString::fromStdString(loopBlock->condition));
	loopHeading->setPos(left+paddingLeft, top + paddingTopBlock);

	int loopHeadingHeight = loopHeading->boundingRect().height() + 2*paddingTopBlock;
	top += loopHeadingHeight;
}

void StructureChartDrawer::drawSurroundings(QGraphicsItemGroup* group)
{
	//draw surrounding rectangle
	int bottom = left;
	QGraphicsRectItem* surroundingRect = new QGraphicsRectItem(group);
	surroundingRect->setRect(group->childrenBoundingRect().left(), group->childrenBoundingRect().top(),
							 group->childrenBoundingRect().width() + left, group->childrenBoundingRect().height() + bottom);
	//draw declarations
	//	for (Declaration& decl : chart->declarations){
	//		scene->addSimpleText(QString::fromStdString(decl.varName+": "+decl.type->umlName()));
	//	}
	//draw headline
	QGraphicsSimpleTextItem* headline = new QGraphicsSimpleTextItem(group);
	headline->setText(QString::fromStdString(chart->headline));
	headline->setPos(paddingLeft, paddingTop);
	QFont font = headline->font();
	font.setBold(true);
	headline->setFont(font);
}

void StructureChartDrawer::drawStructureChart()
{
	QGraphicsItemGroup* structureChart = new QGraphicsItemGroup();

	drawBody(structureChart, chart->root.blocks);
	drawSurroundings(structureChart);

	structureChart->setPos(0.5,100.5);
	scene->addItem(structureChart);
}
