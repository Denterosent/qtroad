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

	/*===============================create testenvironment structure chart===========================*/
	StructureChart* chart = new StructureChart();
	SimpleBlock* firstSimpleBlock = new SimpleBlock("simple command") ;
	SimpleBlock* secondSimpleBlock = new SimpleBlock("this one too") ;
	SimpleBlock* thirdSimpleBlock = new SimpleBlock("me too :-)") ;
	SimpleBlock* fourthSimpleBlock = new SimpleBlock("this is a simple command.") ;
	SimpleBlock* fifthSimpleBlock = new SimpleBlock("this is a normal, simple Command.") ;
	BlockSequence yesBS;
	BlockSequence noBS;
	yesBS.blocks.push_back( thirdSimpleBlock );
	noBS.blocks.push_back( firstSimpleBlock );
	noBS.blocks.push_back( secondSimpleBlock );
	IfElseBlock* firstIfElseBlock = new IfElseBlock("are you stupid???", yesBS, noBS);
	chart->root.blocks.push_back( fourthSimpleBlock );
	chart->root.blocks.push_back( firstIfElseBlock );
	chart->root.blocks.push_back( fifthSimpleBlock );

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

StructureChartDrawer::StructureChartDrawer(QGraphicsScene* pScene, StructureChart* pChart): scene(pScene), chart(pChart)
{
	/*Support for:
	 * Simple Blocks
	 * IfElseBlocks
	 * recursivity
	 *
	 *no Support for:
	 * LoopBlocks
	 * SwitchBlocks
	 * text auto-wrap
	 * space-filling
	 */

	top = 50;
	left = 5;
	height = 20;
	ifElseBlockHeight = height;
	maxWidth = 170;
	width = maxWidth;
	paddingLeft = 5;	//for every text
	paddingTop = 5;		//only for heading
}

void StructureChartDrawer::drawHeadline(QGraphicsItemGroup* group)
{
	QGraphicsSimpleTextItem* headline = new QGraphicsSimpleTextItem(group);
	headline->setText(QString::fromStdString(chart->headline));
	headline->setPos(paddingLeft, paddingTop);
}

void StructureChartDrawer::drawDeclarations(QGraphicsItemGroup* group)
{
	for (Declaration& decl : chart->declarations){
		scene->addSimpleText(QString::fromStdString(decl.varName+": "+decl.type->umlName()));
	}
}

void StructureChartDrawer::drawTestBody(QGraphicsItemGroup* group, boost::ptr_vector<Block>& vector){
	QGraphicsSimpleTextItem* commandBlock;
	QString text;
	QGraphicsRectItem* commandRect;

	//draw body
	for(unsigned int i = 0; i < vector.size(); i++){
		Block* block = &(vector[i]);
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
				leftLine->setLine(left, top+1, width*0.5, top+ifElseBlockHeight);
				rightLine->setLine(left+width, top+1, width*0.5, top+ifElseBlockHeight);

				//draw condition and yes/no -text
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				QGraphicsSimpleTextItem* trueText = new QGraphicsSimpleTextItem(group);
				QGraphicsSimpleTextItem* falseText = new QGraphicsSimpleTextItem(group);
				text = QString::fromStdString(ifElseBlock->condition);
				conditionText->setText(text);
				trueText->setText("true");
				falseText->setText("false");
				conditionText->setPos(left+width*0.5-conditionText->boundingRect().width()*0.5, top);
				trueText->setPos(left+1, top+ifElseBlockHeight-trueText->boundingRect().height());
				falseText->setPos(left+width-falseText->boundingRect().width(), top+ifElseBlockHeight-falseText->boundingRect().height());

				top += ifElseBlockHeight;

				//draw both bodies by calling this function
				int saveTop = top;
				int saveLeft = left;
				int saveWidth = width;
				width = width*0.5;
				drawTestBody(group, ifElseBlock->yes.blocks);
				top = saveTop;
				left += width;
				drawTestBody(group, ifElseBlock->no.blocks);
				width = saveWidth;
				left = saveLeft;
			}else{
				std::cout << "Error: Block is neither a simple Block nor an IfElseBlock!\nOnly these Blocktypes are implemented.";
			}
		}
	}
}

/*void StructureChartDrawer::drawBody(Block* ptrVectorOfBlockSequence)
{
	for(int i = 0; i < anzElemente; i++){
		zuPruefendesElement = ptrVectorOfBlockSequence[i];
		int loopOffset = 0;
			switch(zuPruefendesElement){
			case SimpleBlock:
				drawRectWithCommandInside(SimpleBlock.command, (maxWidth/numberOfWays.f)*(currentWay - 1)+ loopOffset, top, maxWidth/numberOfWays.f - loopOffset, heightOfRects );
				//ToDo: implement function above
				break;
			case IfElseBlock:
				drawHeading();
				numberOfWays++;
				drawBody(IfElseBlock.yes*);
				currentWay++;
				drawBody(IfElseBlock.no*);
				currentWay--;
				numberOfWays--;
				break;
			case LoopBlock:
				if(LoopBlock.headControlled){
					drawHeading();
				}
				loopOffset = 20;
				drawBody(LoopBlock.body*);
				loopOffset = 0;
				if(!LoopBlock.headControlled){
					drawHeading();
				}
				break;
			case SwitchBlock:
				//ToDo
				break;
			}
	top += heightOfRects;
	}
}*/

void StructureChartDrawer::drawStructureChart()
{
	QGraphicsItemGroup* structureChart = new QGraphicsItemGroup();
	drawTestBody(structureChart, chart->root.blocks);
	QGraphicsRectItem* boundingRect = new QGraphicsRectItem(structureChart);
	boundingRect->setRect(structureChart->childrenBoundingRect());
	drawHeadline(structureChart);
	structureChart->setPos(0,100);
	scene->addItem(structureChart);
}
