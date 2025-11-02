#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customcombobox.h"
#include "firstuserthread.h"
#include "userdatamodel.h"
//#include "classthreadmanage.h"
#include <QTreeWidgetItem>
#include "customserver.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    
    
private slots:
    void on_btn_Dlenum_clicked();

    void on_btn_changenum_clicked();

    void on_btn_HttpStatus_clicked();
    
    void refreshClientConnected(const QString &clientInfo);
    
    void refreshClientDisconnected(const QString &clientInfo);
    
    void refreshDataReceived(const QString &clientInfo, const QString &data);
    
   void slotHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus);

private:
    Ui::MainWindow *ui;
//    CustomComboBox *m_CmbClassA = nullptr;
//    CustomComboBox *m_CmbClassB = nullptr;
    UserDataModel *m_model = nullptr;
    FirstUserThread *m_UserThread = nullptr;
    QTreeWidgetItem * m_currentItem = nullptr;
    QTreeWidgetItem * m_userDataItem = nullptr;
    QList<QVariantList>m_UserCurrdata;
    QString m_CurClassStr = "";
    QString m_CurUserStr = "";
    QString m_CurindexStr = "";
    bool m_rowAllow = false;
    bool m_httpText = true;
    CustomServer* m_Server;
private:
    void InitMyCombox();
    void ConnectSlot();
    void ShowUserData(const QString &className, const QString &userName);
    void setClassMemberMap();
};

#endif // MAINWINDOW_H
