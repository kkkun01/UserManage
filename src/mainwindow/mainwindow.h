#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customcombobox.h"
#include "classUserthread.h"
#include "userdatamodel.h"
#include "common.h"  // 引入结构体定义（UserRecord）
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
    
    // HTTP修改数据的槽函数（保持原有参数，内部适配结构体）
    void slotHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus);
    
//    // 新增：接收线程数据生成完成的信号（刷新界面数据）
//    void slotDataGenerated(const QString &className, const QString &userName);
    
  private:
    Ui::MainWindow *ui;
    UserDataModel *m_model = nullptr;
    ClassUserThread *m_UserThread = nullptr;
    QTreeWidgetItem *m_currentItem = nullptr;
    QTreeWidgetItem *m_userDataItem = nullptr;
    
    // 关键修改：将 QList<QVariantList> 改为 QList<UserRecord>（结构化存储当前用户数据）
    QList<UserRecord> m_CurUserRecords;
    
    QString m_CurClassStr = "";   // 当前选中班级
    QString m_CurUserStr = "";    // 当前选中用户
    QString m_CurindexStr = "";   // 当前选中记录序号
    bool m_rowAllow = false;
    bool m_httpText = true;
    CustomServer* m_Server;
    
  private:
    void InitMyCombox();
    void ConnectSlot();
    // 关键修改：参数不变，但内部使用 QList<UserRecord> 展示数据
    void ShowUserData(const QString &className, const QString &userName);
    void setClassMemberMap();
};

#endif // MAINWINDOW_H
