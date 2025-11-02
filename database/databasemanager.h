#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QDebug>
#include <QMap>
#include <QString>

class DatabaseManager {
public:
    // 单例模式，确保全局唯一数据库连接
    static DatabaseManager& getInstance();

    // 初始化数据库（创建表）
    bool initDatabase(const QString& dbPath = "user_data.db");

    // 插入/更新数据（对应setUserData）
    bool saveUserData(const QString& className, const QString& userName,
                     const QList<QVariantList>& data);

    // 删除数据（对应deleteUserData）
    bool deleteUserData(const QString& className, const QString& userName, int rowNum);

    // 更新数据（对应ChangeUserData）
    bool updateUserData(const QString& className, const QString& userName, int rowNum,
                       int num, const QString& info, const QString& status);

    // 从数据库加载数据到内存（程序启动时用）
    QMap<QString, QMap<QString, QList<QVariantList>>> loadAllData();

private:
    QSqlDatabase m_db;

    // 私有构造函数，防止外部实例化
    DatabaseManager();
    // 禁止拷贝
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
