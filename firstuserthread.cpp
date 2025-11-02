#include <QStringList>
#include <QList>
#include <QPair>
#include <random>  // 包含C++标准随机数库
#include "firstuserthread.h"
#include "QDebug"

FirstUserThread::FirstUserThread(const QString &className, const QString &userName, UserDataModel *model, QObject *parent)
    : QThread(parent), m_className(className), m_userName(userName), m_model(model) {}

void FirstUserThread::run()
{
    qDebug() << "FirstUserThread 当前线程ID: " << QThread::currentThreadId();

    QStringList infoList = {"喜欢玩", "喜欢学习", "喜欢运动"};
    QStringList statusList = {"学习", "休息", "娱乐"};
    QList<QVariantList> dataList;

    // 仅使用线程ID生成唯一种子
    // 1. 获取当前线程ID
    std::thread::id threadId = std::this_thread::get_id();

    // 2. 将线程ID转换为哈希值（可作为整数种子使用）
    std::hash<std::thread::id> hasher;
    size_t seed = hasher(threadId); // 线程ID的哈希值作为种子

    // 使用线程唯一的种子初始化随机数生成器
    std::mt19937 gen(static_cast<unsigned int>(seed));

    // 定义分布器
    std::uniform_int_distribution<> infoDist(0, infoList.size() - 1);
    std::uniform_int_distribution<> statusDist(0, statusList.size() - 1);

    // 生成100条随机数据
    for (int i = 0; i < 100; ++i) {
        QString info = infoList[infoDist(gen)];
        QString status = statusList[statusDist(gen)];
        QVariantList dataItem;
        dataItem << i << info << status;
        dataList.append(dataItem);
    }

    qDebug() << "线程" << m_userName << "完成工作" << dataList.first();
    m_model->setUserData(m_className, m_userName, dataList);
    emit sglDataGenerated(m_className, m_userName);
}

