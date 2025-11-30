#include "databasemanager.h"
#include "common.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    if (m_db.isValid() && m_db.isOpen()) {
        m_db.close(); // 先关闭连接
    }
    // 移除连接（避免内存泄漏和连接残留）
    QSqlDatabase::removeDatabase(m_db.connectionName());
}

bool DatabaseManager::initDatabase(const QString& dbPath) {
    // 检查连接是否已存在：如果已创建且路径相同，直接返回成功
    if (m_db.isValid() && m_db.connectionName() == "userDataConnection") {
        if (m_db.databaseName() == dbPath) {
            return m_db.isOpen() || m_db.open();
        } else {
            // 路径不同：先关闭旧连接，再重新创建
            m_db.close();
            QSqlDatabase::removeDatabase("userDataConnection");
        }
    }
    
    // 创建新连接（确保连接名唯一）
    m_db = QSqlDatabase::addDatabase("QSQLITE", "userDataConnection");
    m_db.setDatabaseName(dbPath);
    
    if (!m_db.open()) {
        qDebug() << "数据库打开失败：" << m_db.lastError().text();
        return false;
    }
    
    // 创建表
    QSqlQuery query(m_db);
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS user_data (
            class_name TEXT NOT NULL,
            user_name TEXT NOT NULL,
            row_num INTEGER NOT NULL,
            num INTEGER NOT NULL,       -- 对应 UserRecord::serialNumber
            info TEXT NOT NULL,         -- 对应 UserRecord::interest
            status TEXT NOT NULL,       -- 对应 UserRecord::status
            PRIMARY KEY (class_name, user_name, row_num)
        );)"; // 关键修复：添加分号
    
    if (!query.exec(createTableSql)) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::saveUserData(const QString& className, const QString& userName,
                                   const QList<UserRecord>& records) {
    if (!m_db.isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }
    
    QSqlQuery query(m_db);
    m_db.transaction();
    
    query.prepare("DELETE FROM user_data WHERE class_name = ? AND user_name = ?");
    query.addBindValue(className);
    query.addBindValue(userName);
    if (!query.exec()) {
        qDebug() << "删除旧数据失败：" << query.lastError().text();
        m_db.rollback();
        return false;
    }
    
    query.prepare(R"(
        INSERT INTO user_data (class_name, user_name, row_num, num, info, status)
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    
    for (int row = 0; row < records.size(); ++row) {
        const UserRecord& record = records[row];
        query.addBindValue(className);
        query.addBindValue(userName);
        query.addBindValue(row);
        query.addBindValue(record.serialNumber);
        query.addBindValue(record.interest);
        query.addBindValue(record.status);
        
        if (!query.exec()) {
            qDebug() << "插入数据失败：" << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }
    
    return m_db.commit();
}

bool DatabaseManager::deleteUserData(const QString& className, const QString& userName, int rowNum) {
    if (!m_db.isOpen()) return false;
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM user_data WHERE class_name = ? AND user_name = ? AND row_num = ?");
    query.addBindValue(className);
    query.addBindValue(userName);
    query.addBindValue(rowNum);
    
    if (query.exec()) {
        return query.numRowsAffected() > 0;
    } else {
        qDebug() << "删除失败：" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::updateUserData(const QString& className, const QString& userName, int rowNum,
                                     int num, const QString& info, const QString& status) {
    if (!m_db.isOpen()) return false;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE user_data SET num = ?, info = ?, status = ?
        WHERE class_name = ? AND user_name = ? AND row_num = ?
    )");
    query.addBindValue(num);
    query.addBindValue(info);
    query.addBindValue(status);
    query.addBindValue(className);
    query.addBindValue(userName);
    query.addBindValue(rowNum);
    
    if (query.exec()) {
        qDebug() << "更新成功：" << className << userName << rowNum << info << status;
        return query.numRowsAffected() > 0;
    } else {
        qDebug() << "更新失败：" << query.lastError().text();
        return false;
    }
}

QMap<QString, ClassInfo> DatabaseManager::loadAllData() {
    QMap<QString, ClassInfo> allData;
    if (!m_db.isOpen()) return allData;
    
    QSqlQuery query("SELECT * FROM user_data ORDER BY class_name, user_name, row_num", m_db);
    while (query.next()) {
        QString className = query.value("class_name").toString();
        QString userName = query.value("user_name").toString();
        int rowNum = query.value("row_num").toInt();
        int serialNumber = query.value("num").toInt();
        QString interest = query.value("info").toString();
        QString status = query.value("status").toString();
        
        UserRecord record;
        record.serialNumber = serialNumber;
        record.interest = interest;
        record.status = status;
        
        ClassInfo& classInfo = allData[className];
        classInfo.className = className;
        
        bool userFound = false;
        for (User& user : classInfo.users) {
            if (user.userName == userName) {
                user.records.insert(rowNum, record);
                userFound = true;
                break;
            }
        }
        if (!userFound) {
            User newUser;
            newUser.userName = userName;
            newUser.records.insert(rowNum, record);
            classInfo.users.append(newUser);
        }
    }
    return allData;
}
