#include "customserver.h"
#include "userdatamodel.h"
#include "QTimer"
CustomServer::CustomServer(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    ConnectInit();
    emit sglRequestClassList();
}

bool CustomServer::startServer(quint16 port)
{
    if (m_state == SERVER_STATE_RUNNING) {
        // 若已运行，先停止
        stopServer();
    }

    bool isListening = m_tcpServer->listen(QHostAddress::Any, port);
    if (isListening) {
        m_state = SERVER_STATE_RUNNING;
        qDebug()<<QString("服务器启动成功，监听端口：%1").arg(m_tcpServer->serverPort()) ; 
    } else {
        m_state = SERVER_STATE_STOPPED;
        qDebug()<<QString("服务器启动失败：%1").arg(m_tcpServer->errorString()) ; 
    }
    return isListening;
}

void CustomServer::stopServer()
{
    if (m_state == SERVER_STATE_RUNNING) {
        // 断开所有客户端连接
        for (QTcpSocket *socket : m_clientSockets) {
            socket->disconnectFromHost();
            socket->deleteLater();
        }
        m_clientSockets.clear();

        m_tcpServer->close();
        m_state = SERVER_STATE_STOPPED;
        emit serverStateChanged(SERVER_STATE_STOPPED, "服务器已停止");
    }
}

void CustomServer::setClassMemberMap(const QMap<QString, QStringList> &map)
{
    m_classMemberMap = map;  
}

void CustomServer::onNewConnection()
{
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    if (!clientSocket) return;
    
    // 客户端连接信息
    QString clientInfo = QString("%1:%2")
                             .arg(clientSocket->peerAddress().toString())
                             .arg(clientSocket->peerPort());
    m_clientSockets.append(clientSocket);
    
    // ==== 拼接“班级+成员”数据 ====
    QStringList classAndMembers; // 存储每个“班级|成员列表”字符串
    
    // 遍历班级-成员映射（m_classMemberMap是QMap<QString, QStringList>类型）
    for (auto it = m_classMemberMap.begin(); it != m_classMemberMap.end(); ++it) {
        QString className = it.key(); // 班级名
        QStringList members = it.value(); // 成员列表
        
        // 拼接当前班级的“班级名|成员1、成员2”
        QString item = QString("%1|%2").arg(className).arg(members.join("、"));
        classAndMembers << item;
    }
    
    // 整体格式：Class:班级1|成员1、成员2;班级2|成员3、成员4
    QString initMsg = QString("Class:%1").arg(classAndMembers.join(";"));
    
    // 发送数据
    QByteArray initData = initMsg.toUtf8();
    clientSocket->write(initData);
    clientSocket->flush();
    
    // 信号关联
    connect(clientSocket, &QTcpSocket::readyRead, this, &CustomServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &CustomServer::onDisconnected);
    
    emit clientConnected(clientInfo);
}

void CustomServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;
    
    // 1. 保留原有逻辑：读取数据并发射dataReceived信号
    QByteArray data = clientSocket->readAll();
    QString clientInfo = QString("%1:%2")
                             .arg(clientSocket->peerAddress().toString())
                             .arg(clientSocket->peerPort());
    emit dataReceived(clientInfo, data); // 保留原有功能
    
    
    // 2. 新增：解析客户端发送的updateInfo格式数据
    QString receivedMsg = QString::fromUtf8(data); // 转换为字符串
    if (receivedMsg.startsWith("updateInfo:")) {
        // 截取前缀后的内容（例如"班级一|小朱|1|喜欢玩|学习"）
        QString content = receivedMsg.section("updateInfo:", 1);
        // 按"|"分割字段
        QStringList fields = content.split("|");
        
        // 校验字段数量是否正确（必须为5个）
        if (fields.size() == 5) {
            // 提取各字段并处理
            QString className = fields[0].trimmed();       // 班级名
            QString memberName = fields[1].trimmed();      // 成员名
            int id = fields[2].trimmed().toInt();          // ID（转换为整数）
            QString newInfo = fields[3].trimmed();         // 兴趣（newInfo）
            QString newStatus = fields[4].trimmed();       // 状态（newStatus）
            
            // 过滤空字段（避免无效数据）
            if (!className.isEmpty() && !memberName.isEmpty() && id >= 0) {
                // 发射信号，传递解析后的字段
                emit sglHttpChangeUserData(className, memberName, id, newInfo, newStatus);
                qDebug() << "解析updateInfo成功，已发射信号：" 
                         << className << memberName << id << newInfo << newStatus;
            } else {
                qDebug() << "updateInfo字段为空或ID无效：" << receivedMsg;
            }
        } else {
            qDebug() << "updateInfo格式错误（字段数量不符）：" << receivedMsg;
        }
    }
}

void CustomServer::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString clientInfo = QString("%1:%2")
                         .arg(clientSocket->peerAddress().toString())
                         .arg(clientSocket->peerPort());
    m_clientSockets.removeOne(clientSocket);
    clientSocket->deleteLater();

    emit clientDisconnected(clientInfo);
}

void CustomServer::ConnectInit()
{
    connect(m_tcpServer, &QTcpServer::newConnection,
            this, &CustomServer::onNewConnection);
}
