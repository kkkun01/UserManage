#ifndef FIRSTUSERTHREAD_H
#define FIRSTUSERTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>
// 1. 包含 common.h（确保 UserRecord 结构体可识别）
#include "common.h"
// 2. 包含 UserDataModel 头文件（使用其接口）
#include "userdatamodel.h"

class ClassUserThread : public QThread {
    Q_OBJECT
  public:

    explicit ClassUserThread(const QString &className, const QString &userName, UserDataModel *model, QObject *parent = nullptr);
    
    void run() override;
    
  signals:
    void sglDataGenerated(const QString &className, const QString &userName);
    
  private:
    QString m_userName;       // 用户名
    QString m_className;      // 班级名
    UserDataModel *m_model;   // 数据模型指针
};

#endif // FIRSTUSERTHREAD_H
