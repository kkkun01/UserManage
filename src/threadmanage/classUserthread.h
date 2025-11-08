#ifndef FIRSTUSERTHREAD_H
#define FIRSTUSERTHREAD_H

#include <QObject>
#include <QThread>
#include "userdatamodel.h"

class ClassUserThread:public QThread
{
    Q_OBJECT
public:
    explicit ClassUserThread(const QString &className, const QString &userName, UserDataModel *model, QObject *parent = nullptr);

    virtual void run();

    void SetClassInfo( QString username);

signals:
    void sglDataGenerated(const QString &className, const QString &userName);
private:
    QString m_userName = "";
    QString m_className = "";
    UserDataModel *m_model;

};

#endif // FIRSTUSERTHREAD_H
