#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QString>
#include <QList>
// 引入包含结构体定义的头文件（假设为common.h）
#include "common.h"

class DatabaseManager {
  public:
    // 单例模式，确保全局唯一数据库连接
    static DatabaseManager& getInstance();
    
    // 初始化数据库（创建表）
    bool initDatabase(const QString& dbPath = "user_data.db");
    
    // 插入/更新数据（参数改为QList<UserRecord>）
    bool saveUserData(const QString& className, const QString& userName,
                      const QList<UserRecord>& records);
    
    // 删除数据（保持不变，rowNum为行索引）
    bool deleteUserData(const QString& className, const QString& userName, int rowNum);
    
    // 更新数据（保持参数，对应UserRecord的属性）
    bool updateUserData(const QString& className, const QString& userName, int rowNum,
                        int num, const QString& info, const QString& status);
    
    // 从数据库加载数据到内存（返回值改为QMap<QString, ClassInfo>）
    QMap<QString, ClassInfo> loadAllData();
    
  private:
    QSqlDatabase m_db;
    
    // 私有构造函数，防止外部实例化
    DatabaseManager();
    // 禁止拷贝
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
