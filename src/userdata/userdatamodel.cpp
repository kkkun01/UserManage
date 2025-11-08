#include "userdatamodel.h"
#include "src/database/databasemanager.h"
#include <QDebug>
#include <QMessageBox>

UserDataModel::UserDataModel(QObject *parent)
{

}

void UserDataModel::initDataBase() {
    // 初始化数据库并加载数据到内存
    if (DatabaseManager::getInstance().initDatabase()) {
        m_userData = DatabaseManager::getInstance().loadAllData();
        qDebug() << "数据库初始化成功，加载数据条数：" << m_userData.size();
    } else {
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败！");
    }
}

void UserDataModel::setUserData(const QString &className, const QString &userName, const QList<QVariantList> &data)
{
//    QMutexLocker locker(&m_mutex); // 自动加锁，离开作用域自动解锁
    QMap<QString, QList<QVariantList>>& userMap = m_userData[className];
    userMap[userName] = data;
    bool success = DatabaseManager::getInstance().saveUserData(className, userName, data);
        qDebug() << "已存储数据 - 班级:" << className
                 << "用户:" << userName
                 << "数据条数:" << data.size()
                 << "setUserData 数据库同步:" << (success ? "成功" : "失败");
}

QList<QVariantList> UserDataModel::getUserData(const QString &className, const QString &userName)
{
//    QMutexLocker locker(&m_mutex); // 自动加锁
    // 先判断班级是否存在
    if (m_userData.contains(className)) {
        const QMap<QString, QList<QVariantList>>& userMap = m_userData[className];
        // 再判断用户是否存在
        if (userMap.contains(userName)) {
            return userMap[userName]; // 返回对应数据
        }
    }
    return QList<QVariantList>(); // 不存在时返回空列表
}

QStringList UserDataModel::getMembersByClass(const QString &className)
{
    QStringList memberList; // 存储成员名列表
    qDebug()<<"m_userData:"<<m_userData;
    
    // 1. 检查该班级是否存在于外层QMap中
    if (!m_userData.contains(className)) {
        return memberList; // 班级不存在，返回空列表
    }
    
    // 2. 获取该班级对应的中层QMap（key为成员名，value为成员数据）
    const QMap<QString, QList<QVariantList>> &classMembers = m_userData[className];
    
    // 3. 提取中层QMap的所有key（即成员名），转换为QStringList
    memberList = classMembers.keys();
    
    return memberList;
}


void UserDataModel::deleteUserData(const QString &className, const QString &userName, int row)
    {
        QMutexLocker locker(&m_mutex); // 添加互斥锁，确保线程安全

        // 1. 检查班级是否存在
        if (!m_userData.contains(className)) {
            QMessageBox::warning(nullptr, "提示", "班级不存在：" + className);
            return;
        }
        QMap<QString, QList<QVariantList>> &userMap = m_userData[className];

        // 2. 检查用户是否存在
        if (!userMap.contains(userName)) {
            QMessageBox::warning(nullptr, "提示", "用户不存在：" + userName);
            return;
        }
        QList<QVariantList> &dataList = userMap[userName];

        // 3. 遍历数据列表 查找并删除序号匹配的条目
        bool deleted = false;
        int targetRowNum = -1; // 记录要删除的行号（数据库中存储的row_num）

        auto it = dataList.begin();
        while (it != dataList.end()) {
            if (it->size() < 1) { // 跳过格式错误的条目
                ++it;
                continue;
            }

            // 提取条目中的序号（可能已被修改）
            int itemId = (*it)[0].toInt();

            if (itemId == row) {
                // 记录当前条目在列表中的索引（即数据库中的row_num）
                targetRowNum = it - dataList.begin();
                // 从内存中删除条目
                it = dataList.erase(it);
                deleted = true;
                qDebug() << "内存中删除条目 - 序号:" << row << "行号:" << targetRowNum;
                break; // 假设序号唯一，找到后直接退出循环
            } else {
                ++it;
            }
        }

        // 4. 处理删除结果
        if (!deleted) {
            QMessageBox::information(nullptr, "提示", QString("未找到序号为%1的条目").arg(row));
            return;
        }

        // 5. 同步删除数据库中的对应行（使用记录的行号）
        if (targetRowNum != -1) {
            bool dbSuccess = DatabaseManager::getInstance().deleteUserData(className, userName, targetRowNum);
            qDebug() << "删除条目 - 序号:" << row
                     << "数据库行号:" << targetRowNum
                     << "同步结果:" << (dbSuccess ? "成功" : "失败");

            if (!dbSuccess) {
                QMessageBox::warning(nullptr, "警告", "数据库同步删除失败！");
            }
        }
    }


void UserDataModel::ChangeUserData(int oldNum, int newNum, QString newInfo, QString newStatus)
{
    QMutexLocker locker(&m_mutex);
    bool updated = false;

    // 1. 遍历所有班级
    QMapIterator<QString, QMap<QString, QList<QVariantList>>> classIt(m_userData);
    while (classIt.hasNext()) {
        classIt.next();
        const QString &className = classIt.key();
        QMap<QString, QList<QVariantList>> &userMap = m_userData[className];

        // 2. 遍历当前班级的所有用户
        QMapIterator<QString, QList<QVariantList>> userIt(userMap);
        while (userIt.hasNext()) {
            userIt.next();
            const QString &userName = userIt.key();
            QList<QVariantList> &dataList = userMap[userName];

            // 3. 遍历用户数据列表，根据序号查找目标条目
            for (int rowNum = 0; rowNum < dataList.size(); ++rowNum) {
                QVariantList &dataItem = dataList[rowNum];

                // 验证条目格式
                if (dataItem.size() < 1) {
                    continue;
                }

                // 匹配旧序号
                int currentNum = dataItem[0].toInt();
                if (currentNum == oldNum) {
                    // 更新内存中的数据
                    dataItem[0] = newNum;    // 更新序号
                    dataItem[1] = newInfo;   // 更新信息
                    dataItem[2] = newStatus; // 更新状态

                    // 同步到数据库（使用行号rowNum）
                    bool dbSuccess = DatabaseManager::getInstance().updateUserData(
                        className, userName, rowNum, newNum, newInfo, newStatus);

                    qDebug() << "更新数据 - 班级:" << className
                             << "用户:" << userName
                             << "原序号:" << oldNum
                             << "新序号:" << newNum
                             << "数据库同步:" << (dbSuccess ? "成功" : "失败");

                    updated = true;
                    // 如果只需修改第一个匹配的序号，可以在这里加break
                    // break;
                }
            }
        }
    }

    // 处理未找到条目的情况
    if (!updated) {
//        QMessageBox::information(nullptr, "提示",
//                                QString("未找到序号为%1的条目").arg(oldNum));
    }
}

void UserDataModel::HttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus)
{
    // 1. 校验核心 key 是否存在（班级 + 成员）
    if (!m_userData.contains(className)) {
        qDebug() << "修改失败：班级" << className << "不存在";
        return;
    }
    QMap<QString, QList<QVariantList>>& memberDataMap = m_userData[className];
    if (!memberDataMap.contains(memberName)) {
        qDebug() << "修改失败：班级" << className << "中无成员" << memberName;
        return;
    }
    
    // 2. 获取该成员的数据列表，按 id 匹配目标项
    QList<QVariantList>& dataList = memberDataMap[memberName];
    bool isUpdated = false;
    
    for (int i = 0; i < dataList.size(); ++i) {
        QVariantList& dataItem = dataList[i];
        // 实际结构：第0列是 id（正确匹配）
        if (dataItem.size() > 0 && dataItem[0].toInt() == id) {
            // 关键修正：按实际列索引更新（第1列=兴趣，第2列=状态）
            if (dataItem.size() >= 2) {
                dataItem[1] = newInfo;    // 更新兴趣（newInfo）到第1列
            }
            if (dataItem.size() >= 3) {
                dataItem[2] = newStatus;  // 更新状态（newStatus）到第2列
            }
            isUpdated = true;
            break;
        }
    }
    
    // 3. 数据库同步（保持原有）
    bool dbSuccess = DatabaseManager::getInstance().updateUserData(
        className, memberName, id, id, newInfo, newStatus);
    
    // 4. 结果日志（添加数据项打印，验证更新）
    if (isUpdated) {
        qDebug() << "内存数据更新成功！更新后的数据项：" << className<<memberName<<id<<newInfo<<newStatus; // 打印更新后的列表
    }
    if (isUpdated && dbSuccess) {
        qDebug() << "数据更新成功（内存+数据库）";
    } else {
        qDebug() << "修改失败：" << (isUpdated ? "数据库同步失败" : "未找到匹配数据项");
    }
}
