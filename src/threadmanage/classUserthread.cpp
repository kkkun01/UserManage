#include <QStringList>
#include <QList>
#include <QPair>
#include <random>  // 包含C++标准随机数库
#include "classUserthread.h"
#include "userdatamodel.h"  // 包含 UserDataModel 类声明
#include "common.h"         // 包含 UserRecord 结构体定义
#include <QDebug>

ClassUserThread::ClassUserThread(const QString &className, const QString &userName, UserDataModel *model, QObject *parent)
    : QThread(parent), m_className(className), m_userName(userName), m_model(model) {}

void ClassUserThread::run()
{
    qDebug() << "FirstUserThread 当前线程ID: " << QThread::currentThreadId();
    
    QStringList infoList = {"喜欢玩", "喜欢学习", "喜欢运动"};
    QStringList statusList = {"学习", "休息", "娱乐"};
    QList<UserRecord> dataList;
    
    // 仅使用线程ID生成唯一种子（保持原有随机逻辑不变）
    std::thread::id threadId = std::this_thread::get_id();
    std::hash<std::thread::id> hasher;
    size_t seed = hasher(threadId);
    std::mt19937 gen(static_cast<unsigned int>(seed));
    
    // 定义分布器
    std::uniform_int_distribution<> infoDist(0, infoList.size() - 1);
    std::uniform_int_distribution<> statusDist(0, statusList.size() - 1);
    
    // 生成100条随机数据：构造 UserRecord 结构体
    for (int i = 0; i < 100; ++i) {
        UserRecord record;
        record.serialNumber = i;                  // 序号（对应 serialNumber）
        record.interest = infoList[infoDist(gen)]; // 兴趣（对应 interest）
        record.status = statusList[statusDist(gen)]; // 状态（对应 status）
        dataList.append(record);                  // 添加到结构化列表
    }
    
    qDebug() << "线程" << m_userName << "完成工作，第一条数据：" 
             << "序号=" << dataList.first().serialNumber 
             << "兴趣=" << dataList.first().interest 
             << "状态=" << dataList.first().status;
    
    m_model->setUserData(m_className, m_userName, dataList);
    emit sglDataGenerated(m_className, m_userName);
}
