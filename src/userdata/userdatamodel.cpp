#include "userdatamodel.h"
#include "src/database/databasemanager.h"
#include <QDebug>
#include <QMessageBox>

UserDataModel::UserDataModel(QObject *parent) : QObject(parent) {}

void UserDataModel::initDataBase() {
    // 初始化数据库并加载数据到内存（假设DatabaseManager::loadAllData()返回QMap<QString, ClassInfo>）
    if (DatabaseManager::getInstance().initDatabase()) {
        m_classMap = DatabaseManager::getInstance().loadAllData(); // 适配新结构
        qDebug() << "数据库初始化成功，加载班级数：" << m_classMap.size();
    } else {
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败！");
    }
}

void UserDataModel::setUserData(const QString &className, const QString &userName, const QList<UserRecord> &records) {
    QMutexLocker locker(&m_mutex);
    // 查找或创建班级
    ClassInfo &classInfo = m_classMap[className];
    classInfo.className = className;
    
    // 查找或创建用户
    bool userExists = false;
    for (User &user : classInfo.users) {
        if (user.userName == userName) {
            user.records = records; // 更新记录
            userExists = true;
            break;
        }
    }
    if (!userExists) {
        classInfo.users.append({userName, records}); // 新增用户
    }
    
    // 同步数据库（需DatabaseManager适配QList<UserRecord>）
    bool success = DatabaseManager::getInstance().saveUserData(className, userName, records);
    qDebug() << "已存储数据 - 班级:" << className
             << "用户:" << userName
             << "数据条数:" << records.size()
             << "数据库同步:" << (success ? "成功" : "失败");
}

QList<UserRecord> UserDataModel::getUserData(const QString &className, const QString &userName) {
    QMutexLocker locker(&m_mutex);
    if (m_classMap.contains(className)) {
        const ClassInfo &classInfo = m_classMap[className];
        for (const User &user : classInfo.users) {
            if (user.userName == userName) {
                return user.records; // 返回用户的记录列表
            }
        }
    }
    return QList<UserRecord>(); // 空列表
}

QStringList UserDataModel::getMembersByClass(const QString &className) {
    QStringList memberList;
    if (m_classMap.contains(className)) {
        const ClassInfo &classInfo = m_classMap[className];
        for (const User &user : classInfo.users) {
            memberList.append(user.userName); // 提取用户名
        }
    }
    qDebug() << "班级" << className << "的成员：" << memberList;
    return memberList;
}

void UserDataModel::deleteUserData(const QString &className, const QString &userName, int row) {
    QMutexLocker locker(&m_mutex);
    if (!m_classMap.contains(className)) {
        QMessageBox::warning(nullptr, "提示", "班级不存在：" + className);
        return;
    }
    
    ClassInfo &classInfo = m_classMap[className];
    for (User &user : classInfo.users) {
        if (user.userName == userName) {
            // 按行索引删除记录（row为列表中的位置）
            if (row >= 0 && row < user.records.size()) {
                int deletedId = user.records[row].serialNumber; // 记录删除的序号
                user.records.removeAt(row);
                
                // 同步数据库（删除后重新编号可选）
                bool dbSuccess = DatabaseManager::getInstance().deleteUserData(className, userName, row);
                qDebug() << "删除条目 - 序号:" << deletedId
                         << "行号:" << row
                         << "数据库同步:" << (dbSuccess ? "成功" : "失败");
                
                if (!dbSuccess) {
                    QMessageBox::warning(nullptr, "警告", "数据库同步删除失败！");
                }
                return;
            } else {
                QMessageBox::information(nullptr, "提示", "行索引无效：" + QString::number(row));
                return;
            }
        }
    }
    
    QMessageBox::warning(nullptr, "提示", "用户不存在：" + userName);
}

void UserDataModel::ChangeUserData(int oldNum, int newNum, const QString &newInfo, const QString &newStatus) {
    QMutexLocker locker(&m_mutex);
    bool updated = false;
    
    // 遍历所有班级和用户
    for (ClassInfo &classInfo : m_classMap) {
        for (User &user : classInfo.users) {
            for (int i = 0; i < user.records.size(); ++i) {
                UserRecord &record = user.records[i];
                if (record.serialNumber == oldNum) {
                    // 更新记录属性
                    record.serialNumber = newNum;
                    record.interest = newInfo;
                    record.status = newStatus;
                    
                    // 同步数据库
                    bool dbSuccess = DatabaseManager::getInstance().updateUserData(
                        classInfo.className, user.userName, i, newNum, newInfo, newStatus);
                    
                    qDebug() << "更新数据 - 班级:" << classInfo.className
                             << "用户:" << user.userName
                             << "原序号:" << oldNum
                             << "新序号:" << newNum
                             << "新信息:" << newNum
                             << "新:" << newNum
                             << "数据库同步:" << (dbSuccess ? "成功" : "失败");
                    
                    updated = true;
                    return; // 假设序号唯一，找到后退出
                }
            }
        }
    }
    
    if (!updated) {
        qDebug() << "未找到序号为" << oldNum << "的条目";
    }
}

void UserDataModel::HttpChangeUserData(const QString &className, const QString &memberName, int id, const QString &newInfo, const QString &newStatus) {
    QMutexLocker locker(&m_mutex);
    if (!m_classMap.contains(className)) {
        qDebug() << "修改失败：班级" << className << "不存在";
        return;
    }
    
    ClassInfo &classInfo = m_classMap[className];
    for (User &user : classInfo.users) {
        if (user.userName == memberName) {
            // 按序号id查找记录
            for (int i = 0; i < user.records.size(); ++i) {
                UserRecord &record = user.records[i];
                if (record.serialNumber == id) {
                    record.interest = newInfo;
                    record.status = newStatus;
                    
                    // 同步数据库
                    bool dbSuccess = DatabaseManager::getInstance().updateUserData(
                        className, memberName, i, id, newInfo, newStatus);
                    
                    qDebug() << "HTTP更新数据 - 班级:" << className
                             << "用户:" << memberName
                             << "序号:" << id
                             << "数据库同步:" << (dbSuccess ? "成功" : "失败");
                    return;
                }
            }
            qDebug() << "未找到用户" << memberName << "中序号为" << id << "的记录";
            return;
        }
    }
    
    qDebug() << "修改失败：班级" << className << "中无成员" << memberName;
}

