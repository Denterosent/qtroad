#include "StructureChartDrawer.hpp"

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
	 * declarations - in testing phase
	 * SwitchBlocks
	 *
	 *no Support for:
	 * AutoWidth of blocks
	 * Resizing of SwitchBlock line for case strings
	 * Else-Case of SwitchBlock
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
	switchLineOffset = 10;

	maxEmtySignScale = 10;
	relationOfBodiesIfElseBlock = 0.8; //leftBodyWidth = relation * width
	maxRelationIfElseBlock = 1; //maxHeight = maxRelation * width
	maxRelationSwitchBlock = 0.5;

	errorTag = "Error from Drawer: ";
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

			//first point for auto width
			int blockHeight = commandBlock->boundingRect().height()+paddingTopBlock*2;
			int blockWidth = commandBlock->boundingRect().width()+paddingLeft*2;
			if(blockWidth > maxWidth){
				maxWidth = blockWidth;
//				std::cout << blockWidth << std::endl;
			}

			//draw rect
			QGraphicsRectItem* commandRect= new QGraphicsRectItem(group);
			commandRect->setRect(left, top, width, blockHeight);

			top += blockHeight;
		} else {
			IfElseBlock* ifElseBlock = dynamic_cast<IfElseBlock*>(block);
			if(ifElseBlock){
				//calc the width of the bodies
				float relation = 0.5;
				if(ifElseBlock->no.blocks.size() == 0){relation = relationOfBodiesIfElseBlock;}
				int leftWidth = std::round(width * relation);
				int rightWidth = width - leftWidth;

				//draw condition text
				QString text = QString::fromStdString(ifElseBlock->condition);
				QGraphicsSimpleTextItem* conditionText = new QGraphicsSimpleTextItem(group);
				conditionText->setText(text);
				wrapText(conditionText, std::round(width*0.7));
				conditionText->setPos(left + std::round(leftWidth - conditionText->boundingRect().width() * relation), top);

				//calculate the height of the condition block
				int textHeight = conditionText->boundingRect().height();
				int textWidth = conditionText->boundingRect().width();
				int ifElseBlockHeight = (width*textHeight)/(width-textWidth);
				if((ifElseBlockHeight > maxRelationIfElseBlock*width) or (ifElseBlockHeight < 0))
				{
					ifElseBlockHeight = maxRelationIfElseBlock*width;
				}

				//draw condition-rect
				QGraphicsRectItem* conditionRect = new QGraphicsRectItem(group);
				conditionRect->setRect(left, top, width, ifElseBlockHeight);

				//draw triangle-lines
				QGraphicsLineItem* leftLine = new QGraphicsLineItem(group);
				QGraphicsLineItem* rightLine = new QGraphicsLineItem(group);
				leftLine->setLine(left, top, leftWidth + left, top+ifElseBlockHeight);
				rightLine->setLine(left+width, top, leftWidth + left, top+ifElseBlockHeight);

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
				width = leftWidth;
				drawBody(group, ifElseBlock->yes.blocks);
				leftTop = top;
				top = saveTop;
				left += width;
				width = rightWidth;
				drawBody(group, ifElseBlock->no.blocks);
				rightTop = top;
				left = saveLeft;
				width = saveWidth;
				if(leftTop != rightTop){
					top = std::max(leftTop, rightTop); //to make sure, that next blocks continue at max top of both if-bodies
					QGraphicsRectItem* spaceRect = new QGraphicsRectItem(group);
					if(leftTop < rightTop){
						//add spacefiller left
						spaceRect->setRect(left, leftTop, leftWidth, top-leftTop);
					}else{
						//add spacefiller right
						spaceRect->setRect(left + leftWidth, rightTop, rightWidth, top-rightTop);
					}
					drawEmtySign(spaceRect, group, maxEmtySignScale);
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
						QGraphicsSimpleTextItem* switchExpressionTextItem = new QGraphicsSimpleTextItem(group);
						switchExpressionTextItem->setText(switchExpression);

						//calc height of switch-expression block
						int x = switchExpressionTextItem->boundingRect().width() + 2*paddingLeft;
						int y = switchExpressionTextItem->boundingRect().height() + 2*paddingTopBlock;
						int heightOfSwitchExpressionBlock = switchLineOffset + (width * y)/(width - x);
						if((heightOfSwitchExpressionBlock > maxRelationSwitchBlock*width) or (heightOfSwitchExpressionBlock < 0))
						{
							heightOfSwitchExpressionBlock = maxRelationSwitchBlock*width;
						}

						switchExpressionTextItem->setPos(left + width - switchExpressionTextItem->boundingRect().width() - paddingLeft, top + paddingTopBlock);

						//draw the big line
						QGraphicsLineItem* switchLine = new QGraphicsLineItem(group);
						switchLine->setLine(left, top, left + width, top + heightOfSwitchExpressionBlock - switchLineOffset);

						//draw the rectangle around the expression block
						QGraphicsRectItem* switchExpressionRectItem = new QGraphicsRectItem(group);
						switchExpressionRectItem->setRect(left, top, width, heightOfSwitchExpressionBlock);

						top += heightOfSwitchExpressionBlock;

						int saveTop = top, saveWidth = width, saveLeft = left;
						int widthForEachElement = std::round(width / switchBlock->sequences.size());
						int heightOfSwitchLine;
						int maxTop = 0;
						width = widthForEachElement;
						std::vector<int> topValues;

						for (const std::pair<const std::string, BlockSequence>& pair : switchBlock->sequences){
							QString caseText = QString::fromStdString(pair.first);
							QGraphicsSimpleTextItem* caseTextItem = new QGraphicsSimpleTextItem(group);
							caseTextItem->setText(caseText);
							caseTextItem->setPos(left + paddingLeft, saveTop - caseTextItem->boundingRect().height() - paddingTopBlock);

							//draw the small vertical lines
							heightOfSwitchLine = heightOfSwitchExpressionBlock - ((left - saveLeft)  * (heightOfSwitchExpressionBlock - switchLineOffset) / saveWidth) - 1;
							QGraphicsLineItem* caseLine = new QGraphicsLineItem(group);
							caseLine->setLine(left, top, left, top - heightOfSwitchLine);

							drawBody(group, pair.second.blocks);

							left += widthForEachElement;
							topValues.push_back(top);
							maxTop = std::max(top, maxTop);
							top = saveTop;
						}

						top = maxTop;
						width = saveWidth;
						left = saveLeft;

						//add spacefillers where needed
						int topValue = 0;
						for (unsigned int i = 0; i < topValues.size(); i++){
							topValue = topValues[i];
							if(topValue < maxTop){
								//add spacefiller
								QGraphicsRectItem* spaceRect = new QGraphicsRectItem(group);
								spaceRect->setRect(left + i * widthForEachElement, topValue, widthForEachElement, maxTop - topValue);
								drawEmtySign(spaceRect, group, maxEmtySignScale);
							}
						}
					}else{
						throw std::runtime_error(errorTag + "Block is invalid.");
					}
				}
			}
		}
	}
}

void StructureChartDrawer::drawEmtySign(QGraphicsRectItem* rect, QGraphicsItem* group, int maxScale){
	//scale the "∅" and position it
	QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(group);
	text->setText("∅");

	int multiplicator = 0, xMultiplicator = 0, yMultiplicator = 0;
	xMultiplicator = (rect->boundingRect().width())/(text->boundingRect().width());
	yMultiplicator = (rect->boundingRect().height())/(text->boundingRect().height());
	multiplicator = std::min(xMultiplicator, yMultiplicator);
	multiplicator = std::min(multiplicator, maxScale);
	text->setScale(multiplicator);
	text->setPos((rect->boundingRect().left()+rect->boundingRect().width()*0.5)-(text->boundingRect().width()*multiplicator*0.5),
					  (rect->boundingRect().top()+rect->boundingRect().height()*0.5)-(text->boundingRect().height()*multiplicator*0.5));

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
	QGraphicsSimpleTextItem* declarationTextItem = new QGraphicsSimpleTextItem(group);
	QString declarationText = "";

	for (Declaration& decl : chart->declarations){
		declarationText += QString::fromStdString(decl.varName+": "+decl.type->umlName()+"\n");
	}
	declarationTextItem->setText(declarationText);

	top += 2*paddingTop + declarationTextItem->boundingRect().height();

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
	//init or reset attributes;
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
