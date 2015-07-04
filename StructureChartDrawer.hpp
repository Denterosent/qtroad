#ifndef STRUCTURECHARTDRAWER_H
#define STRUCTURECHARTDRAWER_H

#include "StructureChart.hpp"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QFont>
#include <iostream>


class StructureChartDrawer
{
	private:
		QGraphicsScene* scene;
		StructureChart* chart;
		int top;
		int left;
		int loopOffset;
		int initialWidth;
		int width;
		int paddingLeft;
		int paddingTop;
		int paddingTopBlock;
		int paddingBody;
		int paddingVariablelist;
		int switchLineOffset;
		int maxEmtySignScale;
		float relationOfBodiesIfElseBlock;
		float maxRelationIfElseBlock;
		float maxRelationSwitchBlock;
		std::string errorTag;
		QString variablelistLabel;
		QString ifElseTrueLabel;
		QString ifElseFalseLabel;
		QString switchElseLabel;

	public:
		StructureChartDrawer(QGraphicsScene* scene);
		void drawBody(QGraphicsItem* group, const std::vector<std::unique_ptr<Block>>& vector);
		QGraphicsItem* drawStructureChart(StructureChart* pChart);
		void drawHead(QGraphicsItem* group);
		void wrapText(QGraphicsSimpleTextItem* inputItem, int maximumWidth);
		void drawEmtySign(QGraphicsRectItem* rect, QGraphicsItem* group, int maxScale);
		void drawLoopHeading(QGraphicsSimpleTextItem* loopHeading);
		void drawSurroundingRect(QGraphicsItem* group);
};
#endif // STRUCTURECHARTDRAWER_H
