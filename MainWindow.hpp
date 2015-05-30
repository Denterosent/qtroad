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
	private slots:
		void on_actionGenerate_triggered();
		void on_actionOpen_triggered();
};

class StructureChartDrawer
{
	private:
		QGraphicsScene* scene;
		StructureChart* chart;
		float rect_size_x;
		float rect_size_y;
		int top;
		int numberOfWays;
		int currentWay;
		int heightOfRects;
		int maxWidth;
		int loopOffset;

	public:
		StructureChartDrawer(QGraphicsScene* scene, StructureChart* chart);
		void drawHeadline();
		void drawDeclarations();
		void drawBody();
		void drawStructureChart();
		void drawSurroundingRectangle();
};
#endif // MAINWINDOW_HPP
