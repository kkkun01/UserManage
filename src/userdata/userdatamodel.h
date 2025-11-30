#ifndef USERDATAMODEL_H
#define USERDATAMODEL_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QVariantMap>
#include "common.h"

class UserDataModel : public QObject {
    Q_OBJECT
  public:
    UserDataModel(QObject *parent = nullptr);
    
    // 设置指定班级、用户的记录数据
    void setUserData(const QString &className, const QString &userName, const QList<UserRecord> &records);
    
    // 获取指定班级、用户的所有记录
    QList<UserRecord> getUserData(const QString &className, const QString &userName);
    
    // 获取指定班级的所有成员用户名
    QStringList getMembersByClass(const QString &className);
    
    // 更新指定班级、用户的某条记录的属性
    void updateUserData(const QString &className, const QString &userName, int row, 
                       const QString &property, const QString &value);
    
    // 删除指定班级、用户的某条记录
    void deleteUserData(const QString &className, const QString &userName, int row);
    
    // 根据记录序号修改数据
    bool changeUserRecords(const QString& className,const QString& userName,const QList<UserRecord>& records);
    
    // 通过HTTP接口修改指定班级、用户的某条记录
    void HttpChangeUserData(const QString &className, const QString &memberName, 
                           int id, const QString &newInfo, const QString &newStatus);
    
    // 初始化数据库
    void initDataBase();
    
  private:
    // key=班级名称，value=班级信息
    QMap<QString, ClassInfo> m_classMap;
    QMutex m_mutex;
};

#endif // USERDATAMODEL_H
