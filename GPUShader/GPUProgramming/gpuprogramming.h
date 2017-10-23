#ifndef GPUPROGRAMMING_H
#define GPUPROGRAMMING_H
#ifndef MAX_SLIDER
#define MAX_SLIDER 100
#endif

#include <QtWidgets/QMainWindow>
#include "ui_gpuprogramming.h"
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QSlider>
#include <QLineEdit>
#include "RenderingWidget.h"

class GPUProgramming : public QMainWindow
{
	Q_OBJECT

public:
	GPUProgramming(QWidget *parent = 0);
	~GPUProgramming();
	public slots:
	void setweight(int value);

private:
	void Init();
	void CreateActions();
	void CreateStatusBar();
	void CreateToolBars();
	void CreateSlider();

private:
	Ui::GPUProgrammingClass ui;
	QAction *open;
	QAction *save;
	QAction *DeNoisyshader;
	QAction *NormalMapshader;

	QToolBar *toolBar;

	QSlider *slider;
	QLineEdit *line_edit_slider_;

	RenderingWidget *renderingWidget;
};

#endif // GPUPROGRAMMING_H
