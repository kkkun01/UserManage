#ifndef COMMON_H
#define COMMON_H
#include <QString>
#include<QVariantMap>
#define CLASS1_STR "班级一"
#define CLASS2_STR "班级二"

// 服务器状态
enum SERVER_STATE {
    SERVER_STATE_STOPPED, // 已停止
    SERVER_STATE_RUNNING  // 运行中（监听中）
};

struct UserRecord {
    int serialNumber;  // 序号（单条记录的序号，如1-100）
    QString interest;  // 兴趣
    QString status;    // 状态
    
    // 序列化
    QVariantMap toVariantMap() const {
        return {
            {"serialNumber", serialNumber},
            {"interest", interest},
            {"status", status}
        };
    }
    
    // 反序列化
    static UserRecord fromVariantMap(const QVariantMap& map) {
        return {
            map["serialNumber"].toInt(),
            map["interest"].toString(),
            map["status"].toString()
        };
    }
};

// 用户结构体
struct User {
    QString userName;               // 用户名
    QList<UserRecord> records;      // 该用户的所有信息
};

// 班级结构体
struct ClassInfo {
    QString className;              // 班级
    QList<User> users;              // 班级内所有用户
};





#endif // COMMON_H
