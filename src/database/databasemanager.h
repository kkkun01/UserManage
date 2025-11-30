#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QString>
#include <QList>
#include "common.h"

class DatabaseManager {
  public:
    // 单例模式，确保全局唯一数据库连接
    static DatabaseManager& getInstance();
    
    // 初始化数据库（创建表），默认数据库路径为 user_data.db
    bool initDatabase(const QString& dbPath = "user_data.db");
    
    // 插入/更新数据（接收 UserRecord 列表）
    bool saveUserData(const QString& className, const QString& userName,const QList<UserRecord>& records);
    
    // 删除指定行数据
    bool deleteUserData(const QString& className, const QString& userName, int rowNum);
    
    // 更新指定行数据(http模块)
    bool updateUserData(const QString& className, const QString& userName, int rowNum,
                        int num, const QString& info, const QString& status);
    
    QMap<QString, ClassInfo> loadAllData();
    
    void printAllData(const QMap<QString, ClassInfo>& allData);

  private:
    QSqlDatabase m_db;
    
    DatabaseManager();
    ~DatabaseManager(); 
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
