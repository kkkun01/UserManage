#ifndef CUSTOMSERVER_H
#define CUSTOMSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include "common/common.h"

class CustomServer : public QObject
{
    Q_OBJECT
public:
    explicit CustomServer(QObject *parent = nullptr);

    // 启动服务器（返回是否启动成功）
    bool startServer(quint16 port);
    // 停止服务器
    void stopServer();

    // 获取当前服务器状态（供外部查询）
    SERVER_STATE getServerState() const { return m_state; }
    // 获取当前监听的端口（若运行中）
    quint16 getListeningPort() const {
        return m_tcpServer->isListening() ? m_tcpServer->serverPort() : 0;
    }
    void setClassMemberMap(const QMap<QString, QStringList> &map);

signals:
    // 服务器状态变化信号（状态 + 描述信息）
    void serverStateChanged(SERVER_STATE state, const QString &statusInfo);
    // 其他原有信号...
    void clientConnected(const QString &clientInfo);
    void clientDisconnected(const QString &clientInfo);
    void dataReceived(const QString &clientInfo, const QString &data);
    // 定义请求信号：向MainWindow请求班级列表
    void sglRequestClassList();
    
    void sglHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus);
    
  public slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    
private:
  void ConnectInit();

private:
    QTcpServer *m_tcpServer;
    QList<QTcpSocket*> m_clientSockets;
    SERVER_STATE m_state = SERVER_STATE_STOPPED; // 服务器当前状态（默认停止）
    QMap<QString, QStringList> m_classMemberMap;
    QStringList m_classUserList;
};

#endif // CUSTOMSERVER_H
