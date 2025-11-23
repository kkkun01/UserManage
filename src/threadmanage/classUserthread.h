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
    // 构造函数：传入班级名、用户名、数据模型，父对象默认 nullptr
    explicit ClassUserThread(const QString &className, const QString &userName, 
                             UserDataModel *model, QObject *parent = nullptr);
    
    // 重写 run 函数（线程执行逻辑）
    void run() override; // 建议用 override 明确重写父类方法，编译时会检查正确性
    
    // 设置用户名（原 SetClassInfo 命名与功能不符，建议修正命名，可选）
    void setUserName(const QString &userName);
    
  signals:
    // 数据生成完成信号：通知外部（如 UI 层）更新
    void sglDataGenerated(const QString &className, const QString &userName);
    
  private:
    QString m_userName;       // 用户名
    QString m_className;      // 班级名
    UserDataModel *m_model;   // 数据模型指针（用于调用 setUserData）
};

#endif // FIRSTUSERTHREAD_H
