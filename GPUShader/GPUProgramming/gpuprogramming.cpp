#include "gpuprogramming.h"
#include <QLayout>

GPUProgramming::GPUProgramming(QWidget *parent) : open(NULL),save(NULL),toolBar(NULL),slider(NULL),QMainWindow(parent)
{
	ui.setupUi(this);

	Init();

}

GPUProgramming::~GPUProgramming()
{
	if (open != NULL)
	{
		delete open;
		delete save;
		delete toolBar;
		delete slider;
		delete renderingWidget;

		open = NULL;
		save = NULL;
		toolBar = NULL;
		slider = NULL;
		renderingWidget = NULL;
	}
}

void GPUProgramming::Init()
{
	renderingWidget = new RenderingWidget(this);
	setGeometry(300, 150, 800, 600);
	CreateActions();
	CreateStatusBar();
	CreateToolBars();
	CreateSlider();


	QGridLayout *grl = new QGridLayout(this);
	grl->addWidget(slider,0,0,2,2);
	grl->addWidget(line_edit_slider_, 0, 0, 0, 0);
	grl->addWidget(renderingWidget,0,2,2,8);


	centralWidget()->setLayout(grl);
}

void GPUProgramming::CreateActions()
{
	open = new QAction("Open", this);
	open->setShortcut(tr("Ctrl+F"));
	open->setStatusTip("Open a obj file.");
	connect(open,&QAction::triggered,renderingWidget,&RenderingWidget::Open);

	save = new QAction("Save", this);
	open->setShortcut(QKeySequence::Save);
	open->setStatusTip("Open a obj file.");
	connect(save,&QAction::triggered,renderingWidget,&RenderingWidget::Save);

	DeNoisyshader = new QAction("Denoisyshader", this);
	DeNoisyshader->setStatusTip("Denoisyshader");
	connect(DeNoisyshader, &QAction::triggered, renderingWidget, &RenderingWidget::SetDeNoisyshader);

	NormalMapshader = new QAction("NormalMapshader", this);
	NormalMapshader->setStatusTip("NormalMapshader");
	connect(NormalMapshader, &QAction::triggered, renderingWidget, &RenderingWidget::setNormalMapshader);
}

void GPUProgramming::CreateStatusBar()
{
	statusBar();
}

void GPUProgramming::CreateToolBars()
{
	toolBar = addToolBar("ToolBar");
	toolBar->addAction(open);
	toolBar->addAction(save);
	toolBar->addAction(DeNoisyshader);
	toolBar->addAction(NormalMapshader);
}

void GPUProgramming::CreateSlider()
{
	slider = new QSlider(this);
	line_edit_slider_ = new QLineEdit("0");
	slider->setRange(0, MAX_SLIDER);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setweight(int)));
}

void GPUProgramming::setweight(int value)
{
	float weight_ = value*1.0 / MAX_SLIDER;
	QString str = QString("%1").arg(weight_);
	line_edit_slider_->setText(str);
	renderingWidget->SetWeight(weight_);
}