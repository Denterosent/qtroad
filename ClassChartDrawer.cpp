#include "ClassChartDrawer.hpp"
#include <sstream>
#include <iostream>
#include <QFont>

static const QPolygonF poly({{0,5*2},{5*sqrt(2),0},{-5*sqrt(2),0}});

std::string ClassChartDrawer::visibilityToString(Visibility visibility)
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

QGraphicsItemGroup* ClassChartDrawer::generateClassBox(Class* class_)
{
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

	std::string str1 = class_->name;
	std::string str2 = attrstr.str();
	std::string str3 = opstr.str();

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

bool ClassChartDrawer::drawArrowInternal(QGraphicsItemGroup* ret, QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym)
{
	QRectF r1 = tail->mapToParent(tail->boundingRect()).boundingRect();
	QRectF r2 = head->mapToParent(head->boundingRect()).boundingRect();
	if (r1.right() < r2.left()) {
		QGraphicsLineItem* line = new QGraphicsLineItem();
		line->setLine(r1.right(), r1.top() + .5 * r1.height(),
					  r2.left(), r2.top() + .5 * r2.height());
		ret->addToGroup(line);
		return true;
	}

	return false;
}

QGraphicsItemGroup*ClassChartDrawer::drawArrow(QGraphicsItem* tail, QGraphicsItem* head, QGraphicsItem* tailsym, QGraphicsItem* headsym)
{
	QGraphicsItemGroup* ret = new QGraphicsItemGroup();
	if (!drawArrowInternal(ret, tail, head, tailsym, headsym)) {
		drawArrowInternal(ret, head, tail, tailsym, headsym);
	}
	return ret;
}

QGraphicsItemGroup*ClassChartDrawer::drawClassChart(const ClassChart& classChart)
{
	QGraphicsItemGroup* group = new QGraphicsItemGroup();
	std::map<Class*, QGraphicsItemGroup*> classBoxes;
	for (const std::unique_ptr<Class>& class_ : classChart.classes) {
		QGraphicsItemGroup* classBox = generateClassBox(class_.get());
		group->addToGroup(classBox);
		classBoxes[class_.get()] = classBox;
	}

	float maxdim = 0.f;

	for (const std::pair<const Class*, QGraphicsItemGroup*>& p : classBoxes) {
		QRectF rect = p.second->boundingRect();

		if (maxdim < rect.width())
			maxdim = rect.width();

		if (maxdim < rect.height())
			maxdim = rect.height();
	}

	float radius = 2.f * maxdim;

	int counter = 0;
	for (const std::pair<const Class*, QGraphicsItemGroup*>& p : classBoxes) {
		p.second->setPos(radius + std::cos(2 * M_PI * (counter/float(classBoxes.size()))) * radius,
						 radius + std::sin(2 * M_PI * (counter/float(classBoxes.size()))) * radius);
		counter++;
	}

	std::map<Edge*, QGraphicsItemGroup*> edgeLines;

	for (const std::unique_ptr<Edge>& edge : classChart.edges) {
		QGraphicsItemGroup* a = drawArrow(classBoxes[edge->tail], classBoxes[edge->head], nullptr, nullptr);
		edgeLines[edge.get()] = a;
		group->addToGroup(a);
	}

	return group;
}
