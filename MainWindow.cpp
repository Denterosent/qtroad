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
#include "Parser.hpp"

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

	try {
		Parser parser(begin, end);

		resetGraphicsView();

		scene = new QGraphicsScene(this);
		this->actionDirect_Print->setEnabled(true);
		this->actionPrint_To_PDF->setEnabled(true);

		QGraphicsItemGroup* classDiagram1 = generateClassDiagram("TestClass", "#a: GZ\n#b: GZ", "+f(): GZ\n+g(): GZ");
		QGraphicsItemGroup* classDiagram2 = generateClassDiagram("AnotherTestClass", "-x: GZ", "+y(): GZ");
		classDiagram2->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topRight()) + QPointF(50, 0));
		classDiagram1->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topLeft()));
		scene->addItem(classDiagram1);
		scene->addItem(classDiagram2);
		QGraphicsPolygonItem* polygon = new QGraphicsPolygonItem();
		QPolygonF poly;
		poly.append(QPointF(0,10));
		poly.append(QPointF(10*sqrt(2)/2,0));
		poly.append(QPointF(-10*sqrt(2)/2,0));
		polygon->setPolygon(poly);
		polygon->setBrush(QBrush(Qt::SolidPattern));
		polygon->setRotation(-90);
		QGraphicsLineItem* line = new QGraphicsLineItem();
		line->setLine(0,0,38,0);
		line->setPos(classDiagram1->mapToScene(classDiagram1->childrenBoundingRect().topRight()) + QPointF(.5,20.5));
		polygon->setPos(line->mapToScene(line->boundingRect().topRight()) + QPointF(.5,.5));
		scene->addItem(line);
		scene->addItem(polygon);

		StructureChartDrawer drawer(scene, parser.getResult().structureCharts.front().get());
		drawer.drawStructureChart();

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
	 * AutoHeight of blocks
	 * text auto-wrap
	 *
	 *no Support for:
	 * SwitchBlocks
	 * AutoWidth of blocks
	 * declarations
	 */

	top = 50;
	loopOffset = 20;
	maxWidth = 500;
	width = maxWidth;
	paddingLeft = 5;	//for every text relative to block
	paddingTop = 5;		//only for heading
	paddingTopBlock = 3;//is also a padding to bottom and used in every block
	paddingBody = 0;	//set it to 0, if you don't like the extra margin
	left = paddingBody;
}

void StructureChartDrawer::drawBody(QGraphicsItemGroup* group, const std::vector<std::unique_ptr<Block>>& vector)
{
	for(unsigned int index = 0; index < vector.size(); index++){
		Block* block = vector[index].get();
		SimpleBlock* simpleBlock = dynamic_cast<SimpleBlock*>(block);
		if (simpleBlock) {
			//draw text
			QString text = QString::fromStdString(simpleBlock->command);
			QGraphicsSimpleTextItem* commandBlock = new QGraphicsSimpleTextItem(group);
			commandBlock->setText(text);
			wrapText(commandBlock, width-2*paddingLeft);
			commandBlock->setPos(left+paddingLeft,top+paddingTopBlock);

			int blockHeight = commandBlock->boundingRect().height()+paddingTopBlock*2;

			//draw rect
			QGraphicsRectItem* commandRect= new QGraphicsRectItem(group);
			commandRect->setRect(left, top, width, blockHeight);

			top += blockHeight;
		} else {
			IfElseBlock* ifElseBlock = dynamic_cast<IfElseBlock*>(block);
			if(ifElseBlock){
				//draw condition text
				QString text = QString::fromStdString(ifElseBlock->condition);
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				conditionText->setText(text);
				wrapText(conditionText, width-width*0.3);
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

					QGraphicsRectItem* border = new QGraphicsRectItem(group);
					border->setRect(left, saveTop, width, top-saveTop);
				}else{
					SwitchBlock* switchBlock = dynamic_cast<SwitchBlock*>(block);
					if(switchBlock){
						//draw condition
						QString switchExpression = QString::fromStdString(switchBlock->expression);

						for (std::map<std::string, BlockSequence>::iterator itr = switchBlock->sequences.begin(); itr != switchBlock->sequences.end(); itr++){
							//draw switchValue
//							QString::fromStdString(*itr);

							//draw Blocks
//							drawBody(group, *itr.second.blocks);
						}

						switchBlock->sequences.size();

					}else{
						std::cout << "Error: no valid block";
						throw std::runtime_error(std::string("Block is invalid"));
					}
				}
			}
		}
	}
}

void StructureChartDrawer::drawLoopHeading(QGraphicsItemGroup* group, LoopBlock* loopBlock)
{
	QGraphicsSimpleTextItem* loopHeading = new QGraphicsSimpleTextItem(group);
	loopHeading->setText(QString::fromStdString(loopBlock->condition));
	wrapText(loopHeading, width-2*paddingLeft);
	loopHeading->setPos(left+paddingLeft, top+paddingTopBlock);

	int loopHeadingHeight = loopHeading->boundingRect().height() + 2*paddingTopBlock;
	top += loopHeadingHeight;
}

void StructureChartDrawer::wrapText(QGraphicsSimpleTextItem* inputItem, int maximumWidth)
{
	std::vector<int> index;
	int startingPos = 0, occurencePos = 0, startingPosSave = -1;
	const QString originalText = inputItem->text();
	QString newText = originalText;
	bool thereAreOccurences = false;
	int numberOfInserts = 1;

	//unexpected behavior: endless loop
	while((inputItem->boundingRect().width() > maximumWidth) && (numberOfInserts <= 2)){
		int widthText = inputItem->boundingRect().width();
		std::cout << "width: " << widthText << std::endl;
		//fill vector with positions of spaces
		while(startingPos > startingPosSave){
			occurencePos = originalText.indexOf(QRegExp("[ ,+->]"), startingPos+1);
			if(occurencePos != -1){
				index.push_back(occurencePos);
				thereAreOccurences = true;
			}
			startingPosSave = startingPos;
			startingPos = occurencePos;
		}
		if(thereAreOccurences){
			//add \n's after occurence
			int sizeOfString = originalText.size();
			int sizeOfSubsting = std::round(sizeOfString/(numberOfInserts+1));
			int positionToUse = 0;
			int diffVector, diffSave;
			int aimedPosition = 0;

			for(int x = 0; x < numberOfInserts; x++){
				aimedPosition += sizeOfSubsting;

				//calc pos for one insert
				for(unsigned int i = 0; i < index.size(); i++){
					std::cout << index[i] << std::endl;

					diffVector = std::abs(aimedPosition - index[i]);
					diffSave = std::abs(aimedPosition - positionToUse);
					if(diffVector < diffSave){
						positionToUse = index[i];
					}
				}

				std::cout << "size of text: " << sizeOfString << std::endl;
				std::cout << "chosen one: " << positionToUse << std::endl << std::endl;

				newText = newText.insert(positionToUse+1, "\n");
			}
			inputItem->setText(newText);
		}
		numberOfInserts++;
	}
}

void StructureChartDrawer::drawHead(QGraphicsItemGroup* group)
{

	//draw headline
	QGraphicsSimpleTextItem* headline = new QGraphicsSimpleTextItem(group);
	headline->setText(QString::fromStdString(chart->headline));
	headline->setPos(paddingLeft, paddingTop);
	QFont font = headline->font();
	font.setBold(true);
	headline->setFont(font);

	//draw declarations - still testing
//	std::vector<QGraphicsSimpleTextItem*> vecDecl;
	for (Declaration& decl : chart->declarations){
		scene->addSimpleText(QString::fromStdString(decl.varName+": "+decl.type->umlName()));
	}
		//	for (int i = 0; i < chart->declarations.size(); i++){
//		vecDecl.push_back(new QGraphicsSimpleTextItem(group));
//		vecDecl[i].setText(QString::fromStdString(chart->declarations[i].varName+": "+chart->declarations[i].type->umlName()));
//	}
}

void StructureChartDrawer::drawSurroundingRect(QGraphicsItemGroup* group)
{
	QGraphicsRectItem* surroundingRect = new QGraphicsRectItem(group);
	surroundingRect->setRect(group->childrenBoundingRect().left() + 1, group->childrenBoundingRect().top() + 1,
							 group->childrenBoundingRect().width() + paddingBody - 2, group->childrenBoundingRect().height() + paddingBody - 2);
	//Why the +1 and -2? - Because without it, Qt would make the line at paddingBody = 0 around the rects, with this constants, line is drawn on the rect-lines of body when paddingBody = 0
}

void StructureChartDrawer::drawStructureChart()
{
	QGraphicsItemGroup* structureChart = new QGraphicsItemGroup();

	drawHead(structureChart);
	drawBody(structureChart, chart->root.blocks);
	drawSurroundingRect(structureChart);

	structureChart->setPos(0.5, 100.5);
	scene->addItem(structureChart);
}
