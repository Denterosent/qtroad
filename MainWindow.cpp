#include "MainWindow.hpp"
#include <QFileDialog>
#include <QTextStream>
#include "StructureChart.hpp"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>

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

	scene = new QGraphicsScene(this);


	QGraphicsItemGroup* classDiagram1 = generateClassDiagram("TestClass", "#a: GZ\n#b: GZ", "+f(): GZ\n+g(): GZ");
	QGraphicsItemGroup* classDiagram2 = generateClassDiagram("AnotherTestClass", "-x: GZ", "+y(): GZ");
	classDiagram2->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topRight()) + QPointF(10.5, 0));
	classDiagram1->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topLeft()));
	scene->addItem(classDiagram1);
	scene->addItem(classDiagram2);

	/*===============================create testenvironment structure chart===========================*/
	StructureChart* chart = new StructureChart();
	SimpleBlock* firstSimpleBlock = new SimpleBlock("simple command") ;
	SimpleBlock* secondSimpleBlock = new SimpleBlock("this one too") ;
	SimpleBlock* thirdSimpleBlock = new SimpleBlock("me too :-)") ;
	SimpleBlock* fourthSimpleBlock = new SimpleBlock("this is a simple command.") ;
	SimpleBlock* fifthSimpleBlock = new SimpleBlock("this is a normal, simple Command.") ;
	SimpleBlock* firstLoopBlock = new SimpleBlock("I'm in a loop!");
	SimpleBlock* secondLoopBlock = new SimpleBlock("LoopBlock");
	BlockSequence loopBody;
	loopBody.blocks.push_back(firstLoopBlock);
	loopBody.blocks.push_back(secondLoopBlock);
	LoopBlock* theLoopBlock = new LoopBlock("forever young!!!", loopBody, true);
	BlockSequence yesBS;
	BlockSequence noBS;
//	yesBS.blocks.push_back( theLoopBlock );
	yesBS.blocks.push_back( thirdSimpleBlock );
	noBS.blocks.push_back( firstSimpleBlock );
	noBS.blocks.push_back( secondSimpleBlock );
	IfElseBlock* firstIfElseBlock = new IfElseBlock("are you stupid?", yesBS, noBS);
	chart->root.blocks.push_back( theLoopBlock );
	chart->root.blocks.push_back( fourthSimpleBlock );
	chart->root.blocks.push_back( firstIfElseBlock );
	chart->root.blocks.push_back( fifthSimpleBlock );

	//	PrimitiveType* firstType = new PrimitiveType;
	//	firstType->name = "first decl";
	//	chart->declarations.push_back(firstType);

	chart->headline = "This is a headline!!!";
	/*================================================================================================*/

	StructureChartDrawer* drawer = new StructureChartDrawer(scene, chart);
	drawer->drawStructureChart();

	graphicsView->setScene(scene);

	delete drawer;
	delete chart;
	delete firstSimpleBlock; //wieso tritt hier kein Fehler auf? durch das Löschen von chart wierden doch auch alle elemente von chart gelöscht. Oder?
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
		if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
			QPainter painter(&printer);
			painter.setRenderHint(QPainter::Antialiasing);
			scene->render(&painter);
		}
}

void MainWindow::on_actionPrint_To_PDF_triggered()
{
	//print graphics view to pdf
/*	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPageSize(QPrinter::A4);
	printer.setOutputFileName("prints/file.pdf");
//	printer.setDocName("structureAndClassChart.pdf");
	QPainter painter(&printer);
	graphicsView->render(&painter);
//	scene->render(printerPainter, structureChart->boundingRect(), structureChart->boundingRect(), Qt::KeepAspectRatio);
*/
	QMessageBox msgBox;
	msgBox.information(nullptr, "Sorry", "This function is currently not implemented.");
}

StructureChartDrawer::StructureChartDrawer(QGraphicsScene* pScene, StructureChart* pChart): scene(pScene), chart(pChart)
{
	/*Support for:
	 * Simple Blocks
	 * IfElseBlocks
	 * recursivity
	 * LoopBlocks
	 *
	 *no Support for:
	 * SwitchBlocks
	 * text auto-wrap
	 * space-managing
	 * declarations
	 */

	top = 50;
	left = 5;
	height = 20;
	ifElseBlockHeight = height;
	loopHeadingHeight = height;
	loopOffset = 20;
	maxWidth = 300;
	width = maxWidth;
	paddingLeft = 5;	//for every text
	paddingTop = 5;		//only for heading
}

int StructureChartDrawer::drawBody(QGraphicsItemGroup* group, boost::ptr_vector<Block>& vector){
	QGraphicsSimpleTextItem* commandBlock;
	QString text;
	QGraphicsRectItem* commandRect;
	unsigned int index;

	//draw body
	for(index = 0; index < vector.size(); index++){
		Block* block = &(vector[index]);
		SimpleBlock* simpleBlock = dynamic_cast<SimpleBlock*>(block);
		if (simpleBlock) {
			//draw text
			text = QString::fromStdString(simpleBlock->command);
			commandBlock = new QGraphicsSimpleTextItem(group);
			commandBlock->setText(text);
			commandBlock->setPos(left+paddingLeft,top+height*0.5-commandBlock->boundingRect().height()*0.5);

			//draw rect
			commandRect= new QGraphicsRectItem(group);
			commandRect->setRect(left,top,width,height);

			top += height;
		} else {
			IfElseBlock* ifElseBlock = dynamic_cast<IfElseBlock*>(block);
			if(ifElseBlock){
				//draw condition-rect
				QGraphicsRectItem* conditionRect = new QGraphicsRectItem(group);
				conditionRect->setRect(left, top, width, ifElseBlockHeight);

				//draw triangle-lines
				QGraphicsLineItem* leftLine = new QGraphicsLineItem(group);
				QGraphicsLineItem* rightLine = new QGraphicsLineItem(group);
				leftLine->setLine(left, top, width*0.5+left, top+ifElseBlockHeight);
				rightLine->setLine(left+width, top, width*0.5+left, top+ifElseBlockHeight);

				//draw condition and yes/no -text
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				QGraphicsSimpleTextItem* trueText = new QGraphicsSimpleTextItem(group);
				QGraphicsSimpleTextItem* falseText = new QGraphicsSimpleTextItem(group);
				text = QString::fromStdString(ifElseBlock->condition);
				conditionText->setText(text);
				trueText->setText("true");
				falseText->setText("false");
				conditionText->setPos(left+width*0.5-conditionText->boundingRect().width()*0.5, top);
				trueText->setPos(left+2, top+ifElseBlockHeight-trueText->boundingRect().height());
				falseText->setPos(left+width-falseText->boundingRect().width()-1, top+ifElseBlockHeight-falseText->boundingRect().height());

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
					spaceText->setText("%");
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
	int numberOfDrawnBlocks = index+1;
	std::cout << "number of drawn blocks from one call of drawBody(): " << numberOfDrawnBlocks << std::endl;
	return numberOfDrawnBlocks; //number of drawn blocks by this function call, may be usefull to know
}

void StructureChartDrawer::drawLoopHeading(QGraphicsItemGroup* group, LoopBlock* loopBlock){

	QGraphicsSimpleTextItem* loopHeading = new QGraphicsSimpleTextItem(group);
	loopHeading->setText(QString::fromStdString(loopBlock->condition));
	loopHeading->setPos(left+paddingLeft,top+loopHeadingHeight*0.5-loopHeading->boundingRect().height()*0.5);

	top += loopHeadingHeight;
}

void StructureChartDrawer::drawSurroundings(QGraphicsItemGroup* group){
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
}

void StructureChartDrawer::drawStructureChart()
{
	QGraphicsItemGroup* structureChart = new QGraphicsItemGroup();

	drawBody(structureChart, chart->root.blocks);
	drawSurroundings(structureChart);

	structureChart->setPos(0,100);
	scene->addItem(structureChart);
}
