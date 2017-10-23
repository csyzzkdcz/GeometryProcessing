/********************************************************************************
** Form generated from reading UI file 'gpuprogramming.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GPUPROGRAMMING_H
#define UI_GPUPROGRAMMING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GPUProgrammingClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GPUProgrammingClass)
    {
        if (GPUProgrammingClass->objectName().isEmpty())
            GPUProgrammingClass->setObjectName(QStringLiteral("GPUProgrammingClass"));
        GPUProgrammingClass->resize(600, 400);
        menuBar = new QMenuBar(GPUProgrammingClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        GPUProgrammingClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GPUProgrammingClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        GPUProgrammingClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(GPUProgrammingClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        GPUProgrammingClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(GPUProgrammingClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GPUProgrammingClass->setStatusBar(statusBar);

        retranslateUi(GPUProgrammingClass);

        QMetaObject::connectSlotsByName(GPUProgrammingClass);
    } // setupUi

    void retranslateUi(QMainWindow *GPUProgrammingClass)
    {
        GPUProgrammingClass->setWindowTitle(QApplication::translate("GPUProgrammingClass", "GPUProgramming", 0));
    } // retranslateUi

};

namespace Ui {
    class GPUProgrammingClass: public Ui_GPUProgrammingClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPUPROGRAMMING_H
