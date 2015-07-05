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
		bool drawArrowInternal(QGraphicsItemGroup* ret, QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym);
		QGraphicsItemGroup* drawArrow(QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym);
};

#endif // CLASSCHARTDRAWER_HPP
