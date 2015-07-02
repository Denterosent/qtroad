#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_MainWindow.h"
#include "StructureChart.hpp"


class MainWindow : public QMainWindow, private Ui::MainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
	private:
		void resetGraphicsView();
		QGraphicsScene* scene;
	private slots:
		void on_actionGenerate_triggered();
		void on_actionOpen_triggered();
		void on_actionDirect_Print_triggered();
		void on_actionPrint_To_PDF_triggered();
};

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
		int maxEmtySignScale;
		int maximumHeightOfIfElseBlock;
		int maximumHeightOfSwitchBlock;

	public:
		StructureChartDrawer(QGraphicsScene* scene);
		void drawBody(QGraphicsItem* group, const std::vector<std::unique_ptr<Block>>& vector);
		QGraphicsItem* drawStructureChart(StructureChart* pChart);
		void drawHead(QGraphicsItem* group);
		void wrapText(QGraphicsSimpleTextItem* inputItem, int maximumWidth);
		void drawLoopHeading(QGraphicsSimpleTextItem* loopHeading);
		void drawSurroundingRect(QGraphicsItem* group);
};
#endif // MAINWINDOW_HPP
