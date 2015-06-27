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

	public:
		StructureChartDrawer(QGraphicsScene* scene, StructureChart* chart);
		void drawBody(QGraphicsItemGroup* group, const std::vector<std::unique_ptr<Block>>& vector);
		void drawStructureChart();
		void drawHead(QGraphicsItemGroup* group);
		void wrapText(QGraphicsSimpleTextItem* inputItem, int maximumWidth);
		void drawLoopHeading(QGraphicsItemGroup* group, LoopBlock* loopBlock);
		void drawSurroundingRect(QGraphicsItemGroup* group);
		int getMaxWidth();
};
#endif // MAINWINDOW_HPP
