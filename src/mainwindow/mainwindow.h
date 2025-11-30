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
    
    void on_btn_HttpStatus_clicked();
    
    void refreshClientConnected(const QString &clientInfo);
    
    void refreshClientDisconnected(const QString &clientInfo);
    
    void refreshDataReceived(const QString &clientInfo, const QString &data);
    
    // HTTP修改数据的槽函数
    void slotHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus);
    
    void on_btn_change_clicked();
    
    void on_btn_edit_clicked();
    
  private:
    Ui::MainWindow *ui;
    UserDataModel *m_model = nullptr;
    ClassUserThread *m_UserThread = nullptr;
    QTreeWidgetItem *m_currentItem = nullptr;
    QTreeWidgetItem *m_userDataItem = nullptr;
   
    QList<UserRecord> m_CurUserRecords;
    
    QString m_CurClassStr = "";   // 当前选中班级
    QString m_CurUserStr = "";    // 当前选中用户
    QString m_CurindexStr = "";   // 当前选中记录序号
    bool m_httpText = true;
    CustomServer* m_Server;
    
    bool m_isEditEnabled = false;
    
  private:
    void InitMyCombox();
    void ConnectSlot();
    void ShowUserData(const QString &className, const QString &userName);
    void setClassMemberMap();
    void enableAllItemsEdit();  // 开放所有项编辑权限
    void disableAllItemsEdit(); // 取消所有项编辑权限
    void traverseAllItems(std::function<void(QTreeWidgetItem*)> callback); // 非递归遍历工具
    QList<UserRecord> readSelectedUserRecords(); 
    
};

#endif // MAINWINDOW_H
