#include "gpuprogramming.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GPUProgramming w;
	w.show();
	return a.exec();
}
