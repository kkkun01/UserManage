#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() = default;

bool DatabaseManager::initDatabase(const QString& dbPath) {
    // 添加SQLite数据库驱动
    m_db = QSqlDatabase::addDatabase("QSQLITE", "userDataConnection");
    m_db.setDatabaseName(dbPath);

    // 打开数据库
    if (!m_db.open()) {
        qDebug() << "数据库打开失败：" << m_db.lastError().text();
        return false;
    }

    // 创建数据表
    QSqlQuery query(m_db);
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS user_data (
            class_name TEXT NOT NULL,
            user_name TEXT NOT NULL,
            row_num INTEGER NOT NULL,
            num INTEGER NOT NULL,
            info TEXT NOT NULL,
            status TEXT NOT NULL,
            PRIMARY KEY (class_name, user_name, row_num)
        )
    )";

    if (!query.exec(createTableSql)) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::saveUserData(const QString& className, const QString& userName,
                                   const QList<QVariantList>& data) {
    if (!m_db.isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }

    QSqlQuery query(m_db);
    // 开启事务提高批量操作效率
    m_db.transaction();

    // 先删除该用户的旧数据，再插入新数据（实现更新效果）
    query.prepare("DELETE FROM user_data WHERE class_name = ? AND user_name = ?");
    query.addBindValue(className);
    query.addBindValue(userName);
    if (!query.exec()) {
        qDebug() << "删除旧数据失败：" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 插入新数据
    query.prepare(R"(
        INSERT INTO user_data (class_name, user_name, row_num, num, info, status)
        VALUES (?, ?, ?, ?, ?, ?)
    )");

    for (int row = 0; row < data.size(); ++row) {
        const QVariantList& item = data[row];
        if (item.size() < 3) { // 确保数据格式正确
            qDebug() << "数据格式错误，至少需要3个元素";
            m_db.rollback();
            return false;
        }

        query.addBindValue(className);
        query.addBindValue(userName);
        query.addBindValue(row); // 行号（从0开始）
        query.addBindValue(item[0]); // num
        query.addBindValue(item[1]); // info
        query.addBindValue(item[2]); // status

        if (!query.exec()) {
            qDebug() << "插入数据失败：" << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }

    // 提交事务
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
        return query.numRowsAffected() > 0; // 判断是否有数据被删除
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
         qDebug() << "更新成功："<<className<<rowNum<<rowNum<<num<<info<<status;
        return query.numRowsAffected() > 0; // 判断是否有数据被更新
    } else {
        qDebug() << "更新失败：" << query.lastError().text();
        return false;
    }
}

QMap<QString, QMap<QString, QList<QVariantList>>> DatabaseManager::loadAllData() {
    QMap<QString, QMap<QString, QList<QVariantList>>> data;
    if (!m_db.isOpen()) return data;

    QSqlQuery query("SELECT * FROM user_data ORDER BY class_name, user_name, row_num", m_db);
    while (query.next()) {
        QString className = query.value("class_name").toString();
        QString userName = query.value("user_name").toString();
        int rowNum = query.value("row_num").toInt();
        QVariantList item;
        item << query.value("num")
             << query.value("info")
             << query.value("status");

        // 插入到内存数据结构
        data[className][userName].insert(rowNum, item);
    }
    return data;
}
