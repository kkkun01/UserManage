#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include <QStylePainter>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_model = new UserDataModel(this);
    m_Server = new CustomServer(this);
    // 初始化数据库（新增：程序启动时加载数据）
    m_model->initDataBase();
    ConnectSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitMyCombox()
{
    // 保留原有逻辑（若需使用可解锁）
}

void MainWindow::ConnectSlot()
{
    connect(ui->btn_makenum,&QPushButton::clicked,this,[&](){
        m_currentItem = ui->classtreeWidget->currentItem();
        if (!m_currentItem || !m_currentItem->parent()) {
            QMessageBox::warning(this, "警告", "请选中具体用户（非班级节点）");
            return;
        }
        m_CurClassStr = m_currentItem->parent()->text(0);
        m_CurUserStr = m_currentItem->text(0);
        qDebug()<<"classname"<<m_CurClassStr<<"username"<<m_CurUserStr;
        
        // 避免重复创建线程
        if (m_UserThread) {
            m_UserThread->quit();
            m_UserThread->wait();
            delete m_UserThread;
            m_UserThread = nullptr;
        }
        m_UserThread = new ClassUserThread(m_CurClassStr, m_CurUserStr, m_model);
        connect(m_UserThread,&ClassUserThread::sglDataGenerated,this,&MainWindow::ShowUserData);
        m_UserThread->start();
    });
    
    connect(ui->classtreeWidget, &QTreeWidget::itemClicked, this,[this](){
        m_currentItem = ui->classtreeWidget->currentItem();
        if (!m_currentItem || !m_currentItem->parent()) {
            ui->datatreeWidget->clear();
            return; // 点击班级节点时清空数据展示
        }
        m_CurClassStr = m_currentItem->parent()->text(0);
        m_CurUserStr = m_currentItem->text(0);
        ShowUserData(m_CurClassStr, m_CurUserStr);
    });
    
//    connect(ui->datatreeWidget, &QTreeWidget::itemClicked, this,[this](){
//        for (int col = 0; col < 3; ++col) {
//            ui->datatreeWidget->resizeColumnToContents(col);
//        }
//        m_userDataItem = ui->datatreeWidget->currentItem();
//        if(m_rowAllow && m_userDataItem)
//        {
//            // 从树节点获取修改后的数据（序号、兴趣、状态）
//            int newNum = m_userDataItem->text(0).toInt();
//            QString newInfo = m_userDataItem->text(1);
//            QString newStatus = m_userDataItem->text(2);
//            // 调用模型接口修改数据（按原序号匹配）
//            m_model->ChangeUserData(m_CurindexStr.toInt(), newNum, newInfo, newStatus);
//            qDebug()<<"oldnum"<<m_CurindexStr.toInt()<<"new num"<<newNum<<newInfo<<newStatus;
//            m_CurindexStr = QString::number(newNum);
//            m_userDataItem->setFlags(m_userDataItem->flags() & ~Qt::ItemIsEditable);
//        }else if(!m_rowAllow)
//        {
//            QMessageBox::information(this, "提示", "请点击编辑按钮");
//        }
//        });
        connect(ui->datatreeWidget, &QTreeWidget::itemClicked, this,[this](){
            for (int col = 0; col < 3; ++col) {
                ui->datatreeWidget->resizeColumnToContents(col);
            }
            if(m_rowAllow)
            {
                m_model->ChangeUserData(m_CurindexStr.toInt(), m_userDataItem->text(0).toInt(),m_userDataItem->text(1),m_userDataItem->text(2));
                qDebug()<<"oldnum"<<m_CurindexStr.toInt()<<"new num"<<m_userDataItem->text(0).toInt()<<m_userDataItem->text(1)<<m_userDataItem->text(2);
                m_CurindexStr = m_userDataItem->text(0);
            }else
            {
                QMessageBox::information(this,
                                         "提示",
                                         "请点击编辑按钮",
                                         QMessageBox::Ok);
            }
        });
        
    connect(m_Server, &CustomServer::sglHttpChangeUserData,this, &MainWindow::slotHttpChangeUserData, Qt::DirectConnection);
    connect(m_Server, &CustomServer::clientConnected,this, &MainWindow::refreshClientConnected);
    connect(m_Server, &CustomServer::clientDisconnected,this, &MainWindow::refreshClientDisconnected);
    connect(m_Server, &CustomServer::dataReceived,this, &MainWindow::refreshDataReceived);
}

void MainWindow::ShowUserData(const QString &className, const QString &userName)
{
    // 关键修改：获取结构化数据（QList<UserRecord>）
    QList<UserRecord> userRecords = m_model->getUserData(className, userName);
    if(userRecords.isEmpty())
    {
        qDebug()<<"用户"<<userName<<"无数据";
        QMessageBox::information(this, "提示", QString("用户%1暂无数据").arg(userName));
        ui->datatreeWidget->clear();
        return;
    }
    
    qDebug()<<"用户"<<userName<<"第一条数据："<<userRecords.first().serialNumber<<userRecords.first().interest<<userRecords.first().status;
    ui->datatreeWidget->clear();
    
    // 遍历结构化记录，展示到树控件
    for (int i = 0; i < userRecords.size(); ++i) {
        const UserRecord& record = userRecords[i];
        QTreeWidgetItem* treeItem = new QTreeWidgetItem();
        
        // 直接通过结构体成员赋值，无需解析QVariant
        treeItem->setText(0, QString::number(record.serialNumber)); // 序号
        treeItem->setData(0, Qt::UserRole, record.serialNumber);    // 存储原始整数
        treeItem->setText(1, record.interest);                     // 兴趣
        treeItem->setText(2, record.status);                       // 状态
        
        ui->datatreeWidget->addTopLevelItem(treeItem);
    }
    
    // 自动调整列宽
    for (int col = 0; col < 3; ++col) {
        ui->datatreeWidget->resizeColumnToContents(col);
    }
    
    qDebug() << QString("数据插入完成，共插入%1条有效数据").arg(ui->datatreeWidget->topLevelItemCount());
}

void MainWindow::setClassMemberMap()
{
    QMap<QString, QStringList> classMemberMap;
    
    if (!ui->classtreeWidget) return;
    
    // 遍历所有顶层节点（班级节点）
    for (int i = 0; i < ui->classtreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *classItem = ui->classtreeWidget->topLevelItem(i);
        if (!classItem) continue;
        
        QString className = classItem->text(0);
        if (className.isEmpty()) continue;
        
        // 遍历班级子节点（成员）
        QStringList memberList;
        for (int j = 0; j < classItem->childCount(); ++j) {
            QTreeWidgetItem *memberItem = classItem->child(j);
            if (memberItem) {
                memberList << memberItem->text(0);
            }
        }
        
        classMemberMap[className] = memberList;
    }
    
    m_Server->setClassMemberMap(classMemberMap);
    qDebug()<<"classMemberMap:"<<classMemberMap;
}

void MainWindow::on_btn_Dlenum_clicked()
{
    m_userDataItem = ui->datatreeWidget->currentItem();
    if (!m_userDataItem) {
        QMessageBox::warning(nullptr, "警告", "请先选中要删除的行");
        return;
    }
    
    // 关键修改：获取记录序号和行索引（数据库按行索引删除）
    int serialNumber = m_userDataItem->text(0).toInt();
    int rowNum = ui->datatreeWidget->indexOfTopLevelItem(m_userDataItem);
    qDebug()<<"删除 行号: "<<rowNum<<"序列号： "<<serialNumber;
    
    // 从界面和模型中删除
    ui->datatreeWidget->takeTopLevelItem(rowNum);
    m_model->deleteUserData(m_CurClassStr, m_CurUserStr, rowNum); // 传入行索引，而非序号
}

void MainWindow::on_btn_changenum_clicked()
{
    m_userDataItem = ui->datatreeWidget->currentItem();
    if (!m_userDataItem) {
        QMessageBox::warning(nullptr, "警告", "请先选中要修改的行");
        return;
    }
    
    if(!m_rowAllow)
    {
        m_CurindexStr = m_userDataItem->text(0); // 记录原始序号
    }
    m_rowAllow = true;
    m_userDataItem->setFlags(m_userDataItem->flags() | Qt::ItemIsEditable);
    qDebug()<<"可编辑行： "<<m_userDataItem->text(0)<<m_userDataItem->text(1)<<m_userDataItem->text(2);
}

void MainWindow::on_btn_HttpStatus_clicked()
{
    if(m_httpText)
    {
        if (m_Server->getServerState() == SERVER_STATE_STOPPED)
        {
            setClassMemberMap();
            bool startSuccess = m_Server->startServer(9999);
            if (startSuccess) {
                m_httpText = false;
                ui->btn_HttpStatus->setText("关闭HTTP");
            } else {
                QMessageBox::critical(nullptr, "错误", "打开HTTP服务失败");
            }
        }else
        {
            QMessageBox::warning(nullptr, "提示", "HTTP服务已在运行");
        }
    }else
    {
        if(m_Server->getServerState() == SERVER_STATE_RUNNING)
        {
            m_Server->stopServer();
            m_httpText = true;
            ui->btn_HttpStatus->setText("打开HTTP");
        }else
        {
            QMessageBox::critical(nullptr, "错误", "关闭HTTP服务失败");
        }
    }
}

void MainWindow::refreshClientConnected(const QString &clientInfo)
{
    qDebug()<<"客户端连接："<<clientInfo;
}

void MainWindow::refreshClientDisconnected(const QString &clientInfo)
{
    qDebug()<<"客户端断开："<<clientInfo;
}

void MainWindow::refreshDataReceived(const QString &clientInfo, const QString &data)
{
    qDebug()<<"收到客户端"<<clientInfo<<"数据："<<data;
}

void MainWindow::slotHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus)
{
    m_model->HttpChangeUserData(className, memberName, id, newInfo, newStatus);
    // 若当前展示的是该用户数据，刷新界面
    if (m_CurClassStr == className && m_CurUserStr == memberName) {
        ShowUserData(className, memberName);
    }
}
