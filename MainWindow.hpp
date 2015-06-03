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
		int top;
		int left;
		int height;
		int ifElseBlockHeight;
		int maxWidth;
		int width;
		int paddingLeft;
		int paddingTop;

	public:
		StructureChartDrawer(QGraphicsScene* scene, StructureChart* chart);
		void drawHeadline(QGraphicsItemGroup* structureChart);
		void drawDeclarations();
		void drawBody();
		QGraphicsItemGroup* drawTestBody(boost::ptr_vector<Block>& vector);
		void drawStructureChart();
};
#endif // MAINWINDOW_HPP
