#include "userdatamodel.h"
#include "src/database/databasemanager.h"
#include <QDebug>
#include <QMessageBox>
#include "common.h"

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
    
    // 同步数据库
    bool success = DatabaseManager::getInstance().saveUserData(className, userName, records);
    qDebug() << "已存储数据 - 班级:" << className
             << "用户:" << userName
             << "数据条数:" << records.size()
             << "数据库同步:" << (success ? "成功" : "失败");
}

QList<UserRecord> UserDataModel::getUserData(const QString &className, const QString &userName) {
    QMutexLocker locker(&m_mutex);
    qDebug() << "查找条件：className=" << className << " | userName=" << userName;
    if (m_classMap.contains(className)) {
        qDebug() << "找到班级：" << className;
        const ClassInfo &classInfo = m_classMap[className];
        
        QStringList userNames;
        for (const User &user : classInfo.users) {
            userNames.append(user.userName);
        }
        qDebug() << "该班级下所有用户：" << userNames;
        
        // 查找目标用户
        for (const User &user : classInfo.users) {
            if (user.userName == userName) {
                qDebug() << "找到用户：" << userName << "，记录数：" << user.records.size();
                return user.records;
            }
        }
        qDebug() << "班级" << className << "中无用户：" << userName;
    } else {
        qDebug() << "未找到班级：" << className;
    }
    qDebug() << "===== getUserData 返回空列表 =====";
    return QList<UserRecord>();
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

bool UserDataModel::changeUserRecords(const QString &className, const QString &userName, const QList<UserRecord> &records)
{
    QMutexLocker locker(&m_mutex);  // 保证线程安全
    
    // 1. 输入参数校验
    if (className.isEmpty() || userName.isEmpty() || records.isEmpty()) {
        qDebug() << "changeUserRecords: 输入参数无效（班级名/用户名空或记录列表空）";
        return false;
    }
    
    // 2. 定位目标班级（利用 QMap 的 key 快速查找）
    if (!m_classMap.contains(className)) {
        qDebug() << "changeUserRecords: 未找到班级" << className;
        return false;
    }
    ClassInfo& targetClass = m_classMap[className];  // 引用，直接修改原数据
    
    // 3. 定位目标用户（遍历班级内的用户列表）
    User* targetUser = nullptr;
    for (User& user : targetClass.users) {
        if (user.userName == userName) {
            targetUser = &user;
            break;
        }
    }
    if (!targetUser) {
        qDebug() << "changeUserRecords: 未找到班级[" << className << "]中的用户" << userName;
        return false;
    }
    
    // 4. 替换内存中的记录（核心逻辑）
    targetUser->records = records;
    
    // 5. 同步数据库（复用 DatabaseManager 的 saveUserData，避免冗余）
    bool dbSuccess = DatabaseManager::getInstance().saveUserData(
        className,  // 目标班级
        userName,   // 目标用户
        records     // 新记录列表
        );
    
    // 6. 结果处理与日志
    if (dbSuccess) {
        qDebug() << "changeUserRecords: 成功更新班级[" << className << "]-用户[" << userName << "]的记录（共" << records.size() << "条）";
        return true;
    } else {
        qDebug() << "changeUserRecords: 数据库同步失败（班级：" << className << "，用户：" << userName << "）";
        // 可选：数据库同步失败时，回滚内存数据（恢复修改前的记录）
        // 需提前缓存旧记录：QList<UserRecord> oldRecords = targetUser->records;
        // targetUser->records = oldRecords;
        return false;
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

