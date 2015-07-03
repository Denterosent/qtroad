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
		int maxWidth;
		int width;
		int paddingLeft;
		int paddingTop;
		int paddingTopBlock;
		int paddingBody;
		int switchLineOffset;
		int maxEmtySignScale;
		float maxRelationIfElseBlock;
		float maxRelationSwitchBlock;

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
