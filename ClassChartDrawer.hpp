#ifndef CLASSCHARTDRAWER_HPP
#define CLASSCHARTDRAWER_HPP

#include "ClassChart.hpp"
#include <QGraphicsItemGroup>

class ClassChartDrawer
{
	public:
		QGraphicsItemGroup* drawClassChart(const ClassChart& classChart);
	private:
		std::string visibilityToString(Visibility visibility);
		QGraphicsItemGroup* generateClassBox(Class* class_);
		QLineF calcArrow(QRectF r1, QRectF r2);
		QGraphicsItemGroup* drawArrow(QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym);
};

#endif // CLASSCHARTDRAWER_HPP
