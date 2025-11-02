#ifndef USERDATAMODEL_H
#define USERDATAMODEL_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPair>
#include<QMutexLocker>

class UserDataModel : public QObject {
    Q_OBJECT
public:
    UserDataModel(QObject *parent = nullptr);
    void setUserData(const QString &className, const QString &userName, const QList<QVariantList> &data);
    QList<QVariantList> getUserData(const QString &className, const QString &userName);
    QStringList getMembersByClass(const QString &className);
    void updateUserData(const QString &className, const QString &userName, int row, int col, const QString &value);
    void deleteUserData(const QString &className, const QString &userName, int row);
    void ChangeUserData(int oldNum, int newNum, QString newInfo, QString newStatus);
    void HttpChangeUserData(QString className, QString  memberNname,int id, QString newInfo, QString newStatus);
    void initDataBase();

private:
    QMap<QString, QMap<QString, QList<QVariantList>>> m_userData;
    QMutex m_mutex; // 互斥锁，保护共享数据m_userData
};
#endif // USERDATAMODEL_H
