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
//		QGraphicsItem* structureChart2 = drawer.drawStructureChart(parser.getResult().structureCharts.front().get());
//		structureChart2->setPos(700.5, 400.5);
//		scene->addItem(structureChart2);

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

StructureChartDrawer::StructureChartDrawer(QGraphicsScene* pScene)
{
	scene = pScene; //for easier handling of parser

	/*Support for:
	 * Simple Blocks
	 * IfElseBlocks
	 * recursivity
	 * LoopBlocks
	 * space-filling blocks
	 * AutoHeight of blocks
	 * text auto-wrap
	 * multiple Structure-Charts
	 *
	 *no Support for:
	 * SwitchBlocks
	 * AutoWidth of blocks
	 * declarations - in testing phase
	 */

	/*buglist:
	 * autowrap gets into a endless loop if not stopped
	 * autowrap splits at unwanted signs
	 */

	maxWidth = 0;
	width = 500;
	top = 0;
	loopOffset = 20;
	paddingLeft = 5;	//for every text relative to block, also padding for rigth
	paddingTop = 5;		//only for heading, also padding bottom
	paddingTopBlock = 3;//is also a padding to bottom and used in every block
	paddingBody = 5;	//set it to 0, if you don't like the extra margin
	left = paddingBody;
	maxEmtySignScale = 10;
	maximumHeightOfIfElseBlock = width; //IfElseBlock doesn't get bigger than a square
}

void StructureChartDrawer::drawBody(QGraphicsItem* group, const std::vector<std::unique_ptr<Block>>& vector)
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
			int blockWidth = commandBlock->boundingRect().width()+paddingLeft*2;
			if(blockWidth > maxWidth){
				maxWidth = blockWidth;
				std::cout << blockWidth << std::endl;
			}

			//draw rect
			QGraphicsRectItem* commandRect= new QGraphicsRectItem(group);
			commandRect->setRect(left, top, width, blockHeight);

			top += blockHeight;
		} else {
			IfElseBlock* ifElseBlock = dynamic_cast<IfElseBlock*>(block);
			if(ifElseBlock){
				//width of left BlockSequence is calculated with "ceil", width of right one with "floor", this affects condition text and triangle lines too

				//draw condition text
				QString text = QString::fromStdString(ifElseBlock->condition);
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				conditionText->setText(text);
				wrapText(conditionText, std::round(width*0.7));
				conditionText->setPos(left + std::ceil(width*0.5-conditionText->boundingRect().width()*0.5), top);

				//calculate the height of the condition block
				int textHeight = conditionText->boundingRect().height();
				int textWidth = conditionText->boundingRect().width();
				int ifElseBlockHeight = (width*textHeight)/(width-textWidth);
				if((ifElseBlockHeight > maximumHeightOfIfElseBlock) or (ifElseBlockHeight < 0))
				{
					ifElseBlockHeight = maximumHeightOfIfElseBlock;
				}

				//draw condition-rect
				QGraphicsRectItem* conditionRect = new QGraphicsRectItem(group);
				conditionRect->setRect(left, top, width, ifElseBlockHeight);

				//draw triangle-lines
				QGraphicsLineItem* leftLine = new QGraphicsLineItem(group);
				QGraphicsLineItem* rightLine = new QGraphicsLineItem(group);
				leftLine->setLine(left, top, std::ceil(width*0.5+left), top+ifElseBlockHeight);
				rightLine->setLine(left+width, top, std::ceil(width*0.5+left), top+ifElseBlockHeight);

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
				width = std::ceil(width*0.5);
				drawBody(group, ifElseBlock->yes.blocks);
				leftTop = top;
				top = saveTop;
				left += width;
				width = std::floor(saveWidth*0.5);
				drawBody(group, ifElseBlock->no.blocks);
				rightTop = top;
				left = saveLeft;
				width = saveWidth;
				if(leftTop != rightTop){
					top = std::max(leftTop, rightTop); //to make sure, that next blocks continue at max top of both if-bodies
					QGraphicsSimpleTextItem* spaceText = new QGraphicsSimpleTextItem(group);
					spaceText->setText("∅");
					QGraphicsRectItem* spaceRect = new QGraphicsRectItem(group);
					if(leftTop < rightTop){
						//add spacefiller left
						spaceRect->setRect(left, leftTop, std::ceil(width*0.5), top-leftTop);
					}else{
						//add spacefiller right
						spaceRect->setRect(left+std::ceil(width*0.5), rightTop, std::floor(width*0.5), top-rightTop);
					}
					//scale the "∅" and position it
					int multiplicator = 0, xMultiplicator = 0, yMultiplicator = 0;
					xMultiplicator = (spaceRect->boundingRect().width())/(spaceText->boundingRect().width());
					yMultiplicator = (spaceRect->boundingRect().height())/(spaceText->boundingRect().height());
					multiplicator = std::min(xMultiplicator, yMultiplicator);
					multiplicator = std::min(multiplicator, maxEmtySignScale);
					spaceText->setScale(multiplicator);
					spaceText->setPos((spaceRect->boundingRect().left()+spaceRect->boundingRect().width()*0.5)-(spaceText->boundingRect().width()*multiplicator*0.5),
									  (spaceRect->boundingRect().top()+spaceRect->boundingRect().height()*0.5)-(spaceText->boundingRect().height()*multiplicator*0.5));
				}
			}else{
				LoopBlock* loopBlock = dynamic_cast<LoopBlock*>(block);
				if(loopBlock){
					QGraphicsSimpleTextItem* loopHeading = new QGraphicsSimpleTextItem(group);
					loopHeading->setText(QString::fromStdString(loopBlock->condition));

					int saveTop = top;
					if(loopBlock->headControlled){drawLoopHeading(loopHeading);}
					left += loopOffset;
					width -= loopOffset;
					drawBody(group, loopBlock->body.blocks);
					width += loopOffset;
					left -= loopOffset;
					if(!loopBlock->headControlled){drawLoopHeading(loopHeading);}

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

//						switchBlock->sequences.size();

					}else{
						std::cout << "Error: no valid block" << std::endl;
						throw std::runtime_error(std::string("Block is invalid"));
					}
				}
			}
		}
	}
}

void StructureChartDrawer::drawLoopHeading(QGraphicsSimpleTextItem* loopHeading)
{
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
//		int widthText = inputItem->boundingRect().width();
//		std::cout << "width: " << widthText << std::endl;
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
//					std::cout << index[i] << std::endl;

					diffVector = std::abs(aimedPosition - index[i]);
					diffSave = std::abs(aimedPosition - positionToUse);
					if(diffVector < diffSave){
						positionToUse = index[i];
					}
				}

//				std::cout << "size of text: " << sizeOfString << std::endl;
//				std::cout << "chosen one: " << positionToUse << std::endl << std::endl;

				newText = newText.insert(positionToUse+1, "\n");
			}
			inputItem->setText(newText);
		}
		numberOfInserts++;
	}
}

void StructureChartDrawer::drawHead(QGraphicsItem* group)
{
	//draw headline
	QGraphicsSimpleTextItem* headline = new QGraphicsSimpleTextItem(group);
	headline->setText(QString::fromStdString(chart->headline));
	headline->setPos(paddingLeft, paddingTop);
	QFont font = headline->font();
	font.setBold(true);
	headline->setFont(font);

	top += 2*paddingTop + headline->boundingRect().height();

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

void StructureChartDrawer::drawSurroundingRect(QGraphicsItem* group)
{
	QGraphicsRectItem* surroundingRect = new QGraphicsRectItem(group);
	surroundingRect->setRect(std::floor(group->childrenBoundingRect().left() + 1), std::floor(group->childrenBoundingRect().top() + 1),
							 std::floor(group->childrenBoundingRect().width() + paddingBody - 1), std::floor(group->childrenBoundingRect().height() + paddingBody - 1));
	//Why the +1 and -1? - Because without it, Qt would make the line at paddingBody = 0 around the rects, with this constants, line is drawn on the rect-lines of body when paddingBody = 0
}

QGraphicsItem* StructureChartDrawer::drawStructureChart(StructureChart* pChart)
{
	//init or reset attributes:
	chart = pChart;
	maxWidth = 0;
	width = 500;
	top = 0;

	QGraphicsItem* structureChart = new QGraphicsItemGroup();

	drawHead(structureChart);
	drawBody(structureChart, chart->root.blocks);
	drawSurroundingRect(structureChart);

	return structureChart;
}
