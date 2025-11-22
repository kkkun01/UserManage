#include "databasemanager.h"
#include "common.h" // 包含 UserRecord、ClassInfo 等结构体定义

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() = default;

bool DatabaseManager::initDatabase(const QString& dbPath) {
    // 保持原有逻辑（数据库驱动、表结构不变，因为表字段与 UserRecord 属性对应）
    m_db = QSqlDatabase::addDatabase("QSQLITE", "userDataConnection");
    m_db.setDatabaseName(dbPath);
    
    if (!m_db.open()) {
        qDebug() << "数据库打开失败：" << m_db.lastError().text();
        return false;
    }
    
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
        )
    )";
    
    if (!query.exec(createTableSql)) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }
    return true;
}

// 保存用户数据：接收 QList<UserRecord> 而非 QList<QVariantList>
bool DatabaseManager::saveUserData(const QString& className, const QString& userName,
                                   const QList<UserRecord>& records) {
    if (!m_db.isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }
    
    QSqlQuery query(m_db);
    m_db.transaction();
    
    // 先删除旧数据（逻辑不变）
    query.prepare("DELETE FROM user_data WHERE class_name = ? AND user_name = ?");
    query.addBindValue(className);
    query.addBindValue(userName);
    if (!query.exec()) {
        qDebug() << "删除旧数据失败：" << query.lastError().text();
        m_db.rollback();
        return false;
    }
    
    // 插入新数据：从 UserRecord 提取属性
    query.prepare(R"(
        INSERT INTO user_data (class_name, user_name, row_num, num, info, status)
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    
    for (int row = 0; row < records.size(); ++row) {
        const UserRecord& record = records[row]; // 遍历 UserRecord 列表
        query.addBindValue(className);
        query.addBindValue(userName);
        query.addBindValue(row);                 // 行号（用于定位）
        query.addBindValue(record.serialNumber); // num 对应 serialNumber
        query.addBindValue(record.interest);     // info 对应 interest
        query.addBindValue(record.status);       // status 对应 status
        
        if (!query.exec()) {
            qDebug() << "插入数据失败：" << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }
    
    return m_db.commit();
}

// 保持不变（删除逻辑与 row_num 绑定，不受结构体影响）
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

// 更新逻辑：直接接收 UserRecord 相关属性
bool DatabaseManager::updateUserData(const QString& className, const QString& userName, int rowNum,
                                     int num, const QString& info, const QString& status) {
    // 保持不变（参数与数据库字段直接对应，num 对应 serialNumber，info 对应 interest）
    if (!m_db.isOpen()) return false;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE user_data SET num = ?, info = ?, status = ?
        WHERE class_name = ? AND user_name = ? AND row_num = ?
    )");
    query.addBindValue(num);       // serialNumber
    query.addBindValue(info);      // interest
    query.addBindValue(status);    // status
    query.addBindValue(className);
    query.addBindValue(userName);
    query.addBindValue(rowNum);
    
    if (query.exec()) {
        qDebug() << "更新成功：" << className << userName << rowNum<<info<<status;
        return query.numRowsAffected() > 0;
    } else {
        qDebug() << "更新失败：" << query.lastError().text();
        return false;
    }
}

// 加载数据：返回 QMap<QString, ClassInfo> 而非嵌套 QVariantList 映射
QMap<QString, ClassInfo> DatabaseManager::loadAllData() {
    QMap<QString, ClassInfo> allData; // 存储所有班级数据
    if (!m_db.isOpen()) return allData;
    
    QSqlQuery query("SELECT * FROM user_data ORDER BY class_name, user_name, row_num", m_db);
    while (query.next()) {
        // 提取数据库字段
        QString className = query.value("class_name").toString();
        QString userName = query.value("user_name").toString();
        int rowNum = query.value("row_num").toInt();
        int serialNumber = query.value("num").toInt();
        QString interest = query.value("info").toString();
        QString status = query.value("status").toString();
        
        // 构造 UserRecord
        UserRecord record;
        record.serialNumber = serialNumber;
        record.interest = interest;
        record.status = status;
        
        // 插入到 ClassInfo 结构中
        ClassInfo& classInfo = allData[className]; // 查找或创建班级
        classInfo.className = className;
        
        // 查找班级中的用户，若不存在则新增
        bool userFound = false;
        for (User& user : classInfo.users) {
            if (user.userName == userName) {
                user.records.insert(rowNum, record); // 插入到用户的记录列表
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
