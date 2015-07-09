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
		QGraphicsItemGroup* drawClassBox(Class* class_);
		QLineF calculateArrowLine(QRectF r1, QRectF r2);
		QGraphicsItemGroup* drawArrow(QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym);
		std::string attributeToString(const Attribute& attribute);
		std::string operationToString(const Operation& operation);
};

#endif // CLASSCHARTDRAWER_HPP
