/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTreeWidget *classtreeWidget;
    QTreeWidget *datatreeWidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_makenum;
    QPushButton *btn_Dlenum;
    QPushButton *btn_changenum;
    QPushButton *btn_HttpStatus;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(817, 620);
        MainWindow->setLayoutDirection(Qt::LeftToRight);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        classtreeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(classtreeWidget);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(classtreeWidget);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        classtreeWidget->setObjectName(QString::fromUtf8("classtreeWidget"));
        classtreeWidget->setGeometry(QRect(60, 40, 141, 301));
        datatreeWidget = new QTreeWidget(centralWidget);
        new QTreeWidgetItem(datatreeWidget);
        datatreeWidget->setObjectName(QString::fromUtf8("datatreeWidget"));
        datatreeWidget->setGeometry(QRect(250, 50, 431, 291));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(61, 380, 571, 30));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(25);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btn_makenum = new QPushButton(layoutWidget);
        btn_makenum->setObjectName(QString::fromUtf8("btn_makenum"));

        horizontalLayout->addWidget(btn_makenum);

        btn_Dlenum = new QPushButton(layoutWidget);
        btn_Dlenum->setObjectName(QString::fromUtf8("btn_Dlenum"));

        horizontalLayout->addWidget(btn_Dlenum);

        btn_changenum = new QPushButton(layoutWidget);
        btn_changenum->setObjectName(QString::fromUtf8("btn_changenum"));

        horizontalLayout->addWidget(btn_changenum);

        btn_HttpStatus = new QPushButton(layoutWidget);
        btn_HttpStatus->setObjectName(QString::fromUtf8("btn_HttpStatus"));

        horizontalLayout->addWidget(btn_HttpStatus);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 817, 26));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = classtreeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "\347\224\250\346\210\267\345\210\227\350\241\250", nullptr));

        const bool __sortingEnabled = classtreeWidget->isSortingEnabled();
        classtreeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = classtreeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "\347\217\255\347\272\247\344\270\200", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem1->child(0);
        ___qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "\345\260\217\346\230\216", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem1->child(1);
        ___qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "\345\260\217\345\215\216", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem1->child(2);
        ___qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "\345\260\217\344\271\246", nullptr));
        QTreeWidgetItem *___qtreewidgetitem5 = classtreeWidget->topLevelItem(1);
        ___qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "\347\217\255\347\272\247\344\272\214", nullptr));
        QTreeWidgetItem *___qtreewidgetitem6 = ___qtreewidgetitem5->child(0);
        ___qtreewidgetitem6->setText(0, QApplication::translate("MainWindow", "\345\260\217\346\234\261", nullptr));
        QTreeWidgetItem *___qtreewidgetitem7 = ___qtreewidgetitem5->child(1);
        ___qtreewidgetitem7->setText(0, QApplication::translate("MainWindow", "\345\260\217\347\216\213", nullptr));
        QTreeWidgetItem *___qtreewidgetitem8 = ___qtreewidgetitem5->child(2);
        ___qtreewidgetitem8->setText(0, QApplication::translate("MainWindow", "\345\260\217\346\255\206", nullptr));
        classtreeWidget->setSortingEnabled(__sortingEnabled);

        QTreeWidgetItem *___qtreewidgetitem9 = datatreeWidget->headerItem();
        ___qtreewidgetitem9->setText(2, QApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        ___qtreewidgetitem9->setText(1, QApplication::translate("MainWindow", "\344\277\241\346\201\257", nullptr));
        ___qtreewidgetitem9->setText(0, QApplication::translate("MainWindow", "\345\272\217\345\217\267", nullptr));

        const bool __sortingEnabled1 = datatreeWidget->isSortingEnabled();
        datatreeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem10 = datatreeWidget->topLevelItem(0);
        ___qtreewidgetitem10->setText(2, QApplication::translate("MainWindow", "\345\255\246\344\271\240", nullptr));
        ___qtreewidgetitem10->setText(1, QApplication::translate("MainWindow", "\345\226\234\346\254\242\347\216\251", nullptr));
        ___qtreewidgetitem10->setText(0, QApplication::translate("MainWindow", "1", nullptr));
        datatreeWidget->setSortingEnabled(__sortingEnabled1);

        btn_makenum->setText(QApplication::translate("MainWindow", "\347\224\237\346\210\220\351\232\217\346\234\272\346\225\260", nullptr));
        btn_Dlenum->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
        btn_changenum->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271", nullptr));
        btn_HttpStatus->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200Http", nullptr));
        menu->setTitle(QApplication::translate("MainWindow", "\351\246\226\351\241\265", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
