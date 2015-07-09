#include "ClassChartDrawer.hpp"
#include <sstream>
#include <iostream>
#include <QFont>
#include <QBrush>

static const QPolygonF triangleArrowhead({{5*sqrt(2),-5*2},{0,0},{-5*sqrt(2),-5*2}});

std::string ClassChartDrawer::visibilityToString(Visibility visibility)
{
	switch (visibility) {
		case Visibility::private_: {
			return "-";
		}
		case Visibility::protected_: {
			return "#";
		}
		case Visibility::public_: {
			return "+";
		}
	}
}

QGraphicsItemGroup* ClassChartDrawer::drawClassBox(Class* class_)
{
	std::ostringstream attrstr;
	bool first = true;
	for (const Attribute& attribute : class_->getAttributes()) {
		if (!first) {
			attrstr << "\n";
		}
		first = false;
		attrstr << attributeToString(attribute);
	}
	std::ostringstream opstr;
	first = true;
	for (const Operation& operation : class_->getOperations()) {
		if (!first) {
			opstr << "\n";
		}
		first = false;
		opstr << operationToString(operation);
	}

	std::string str1 = class_->getName();
	std::string str2 = attrstr.str();
	std::string str3 = opstr.str();

	if (class_->isAbstract()) {
		str1 += " {abstract}";
	}

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

QLineF ClassChartDrawer::calculateArrowLine(QRectF r1, QRectF r2)
{
	if (r1.right() < r2.left() && r1.bottom() < r2.top()) {
		return QLineF(r1.bottomRight(), r2.topLeft());
	} else if (r2.right() < r1.left() && r2.bottom() < r1.top()) {
		return QLineF(r1.topLeft(), r2.bottomRight());
	} else if (r1.right() < r2.left() && r2.bottom() < r1.top()) {
		return QLineF(r1.topRight(), r2.bottomLeft());
	} else if (r2.right() < r1.left() && r1.bottom() < r2.top()) {
		return QLineF(r1.bottomLeft(), r2.topRight());
	} else if (r1.right() < r2.left()) {
		return QLineF(r1.right(), r1.top() + .5 * r1.height(), r2.left(), r2.top() + .5 * r2.height());
	} else if (r2.right() < r1.left()) {
		return QLineF(r1.left(), r1.top() + .5 * r1.height(), r2.right(), r2.top() + .5 * r2.height());
	} else if (r1.bottom() < r2.top()) {
		return QLineF(r1.left() + .5 * r1.width(), r1.bottom(), r2.left() + .5 * r2.width(), r2.top());
	} else {
		return QLineF(r1.left() + .5 * r1.width(), r1.top(), r2.left() + .5 * r2.width(), r2.bottom());
	}
}

QGraphicsItemGroup* ClassChartDrawer::drawArrow(QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym)
{
	QGraphicsItemGroup* ret = new QGraphicsItemGroup();

	QRectF r1 = tail->mapToParent(tail->boundingRect()).boundingRect();
	QRectF r2 = head->mapToParent(head->boundingRect()).boundingRect();

	QGraphicsLineItem* line = new QGraphicsLineItem(calculateArrowLine(r1, r2));
	ret->addToGroup(line);

	if (headsym) {
		headsym->setPos(line->line().p2());
		headsym->setRotation(-line->line().angle() - 90);
		ret->addToGroup(headsym);
	}

	return ret;
}

std::string ClassChartDrawer::attributeToString(const Attribute& attribute)
{
	std::ostringstream str;
	str << visibilityToString(attribute.getVisibility()) << attribute.getName() << ": " << attribute.getType()->umlName();
	return str.str();
}

std::string ClassChartDrawer::operationToString(const Operation& operation)
{
	std::ostringstream str;
	bool first = false;
	str << visibilityToString(operation.getVisibility()) << operation.getName() << "(";
	if (!operation.getArguments().empty()) {
		bool first = true;
		for (const Argument& arg : operation.getArguments()) {
			if (!first) {
				str << "; ";
			}
			first = false;
			str << arg.getName() << ": " << arg.getType()->umlName();
		}
	}
	str << ")";
	if (operation.getReturnType()) {
		str << ": " << operation.getReturnType()->umlName();
	}
	if (operation.getStereotype() == Operation::constructor) {
		str << " «constructor»";
	}
	if (operation.getStereotype() == Operation::destructor) {
		str << " «destructor»";
	}
	if (operation.isAbstract()) {
		str << " {abstract}";
	}
	return str.str();
}

QGraphicsItemGroup* ClassChartDrawer::drawClassChart(const ClassChart& classChart)
{
	QGraphicsItemGroup* group = new QGraphicsItemGroup();
	std::map<Class*, QGraphicsItemGroup*> classBoxes;
	for (const std::unique_ptr<Class>& class_ : classChart.getClasses()) {
		QGraphicsItemGroup* classBox = drawClassBox(class_.get());
		group->addToGroup(classBox);
		classBoxes[class_.get()] = classBox;
	}

	int radius = 0;
	bool intersection = false;

	do {
		radius += 100;
		int counter = 0;
		for (const std::unique_ptr<Class>& class_ : classChart.getClasses()) {
			classBoxes[class_.get()]->setPos(radius + std::cos(2 * 3.14159 * (counter/float(classBoxes.size()))) * radius,
											 radius + std::sin(2 * 3.14159 * (counter/float(classBoxes.size()))) * radius);
			counter++;
		}
		intersection = false;
		for (std::map<Class*, QGraphicsItemGroup*>::iterator it = classBoxes.begin(); it != classBoxes.end() && !intersection; it++) {
			for (std::map<Class*, QGraphicsItemGroup*>::iterator jt = classBoxes.begin(); it != jt && !intersection; jt++) {
				QRectF r1 = it->second->mapToParent(it->second->boundingRect()).boundingRect();
				QRectF r2 = jt->second->mapToParent(jt->second->boundingRect()).boundingRect();
				intersection |= r1.intersects(r2);
			}
		}
	} while (intersection);

	std::map<Edge*, QGraphicsItemGroup*> edgeLines;

	for (const std::unique_ptr<Edge>& edge : classChart.getEdges()) {
		QGraphicsItem* arrowhead = nullptr;
		if (dynamic_cast<Inheritance*>(edge.get())) {
			QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(triangleArrowhead);
			polygonItem->setBrush(QBrush(Qt::white));
			arrowhead = polygonItem;
		}
		if (dynamic_cast<Association*>(edge.get())) {
			QPainterPath painterPath;
			painterPath.addPolygon(triangleArrowhead);
			arrowhead = new QGraphicsPathItem(painterPath);
		}
		QGraphicsItemGroup* a = drawArrow(classBoxes[edge->getTail()], classBoxes[edge->getHead()], nullptr, arrowhead);
		edgeLines[edge.get()] = a;
		group->addToGroup(a);
	}

	return group;
}
