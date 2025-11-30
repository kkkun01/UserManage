#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include <QStylePainter>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidgetItem>
 #include <QStack>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_model = new UserDataModel(this);
    m_Server = new CustomServer(this);
    m_model->initDataBase();
    ConnectSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitMyCombox()
{

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
        if (m_isEditEnabled) {
            disableAllItemsEdit(); 
            ui->btn_edit->setText("开放所有编辑权限");
            m_isEditEnabled = false;
        }
        m_currentItem = ui->classtreeWidget->currentItem();
        if (!m_currentItem || !m_currentItem->parent()) {
            ui->datatreeWidget->clear();
            return; // 点击班级节点时清空数据展示
        }
        m_CurClassStr = m_currentItem->parent()->text(0);
        m_CurUserStr = m_currentItem->text(0);
        ShowUserData(m_CurClassStr, m_CurUserStr);
    });
    connect(ui->datatreeWidget, &QTreeWidget::itemClicked, this,[this](){
        if(!m_isEditEnabled)
        {
            QMessageBox::information(this,"提示","请点击编辑按钮",QMessageBox::Ok);
        }
    });
    connect(m_Server, &CustomServer::sglHttpChangeUserData,this, &MainWindow::slotHttpChangeUserData, Qt::DirectConnection);
    connect(m_Server, &CustomServer::clientConnected,this, &MainWindow::refreshClientConnected);
    connect(m_Server, &CustomServer::clientDisconnected,this, &MainWindow::refreshClientDisconnected);
    connect(m_Server, &CustomServer::dataReceived,this, &MainWindow::refreshDataReceived);
}

void MainWindow::ShowUserData(const QString &className, const QString &userName)
{
    QList<UserRecord> userRecords = m_model->getUserData(className, userName);
    if(userRecords.isEmpty())
    {
        qDebug()<<"用户"<<userName<<"无数据（返回空列表）";
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

void MainWindow::on_btn_edit_clicked()
{
    if (!m_isEditEnabled) {
        // 状态1：未编辑 → 开放编辑权限
        enableAllItemsEdit();
        ui->btn_edit->setText(QStringLiteral("取消编辑"));
        m_isEditEnabled = true;
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("已开放所有项编辑权限（双击数据进行修改）"));
    } else {
        // 状态2：已编辑 → 取消编辑 + 回滚数据
        disableAllItemsEdit();
        ui->btn_edit->setText(QStringLiteral("编辑"));
        m_isEditEnabled = false;
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("已取消编辑权限，数据已回滚"));
    }
}
void MainWindow::traverseAllItems(std::function<void(QTreeWidgetItem*)> callback)
{
    if (!ui->datatreeWidget || !callback) return;
    
    QStack<QTreeWidgetItem*> itemStack;
    // 压入所有顶层项
    for (int i = 0; i < ui->datatreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *topItem = ui->datatreeWidget->topLevelItem(i);
        if (topItem) itemStack.push(topItem);
    }
    
    // 栈遍历（模拟递归，避免栈溢出）
    while (!itemStack.isEmpty()) {
        QTreeWidgetItem *currentItem = itemStack.pop();
        callback(currentItem); // 执行自定义操作（启用/禁用编辑）
        
        // 逆序压入子项（保证遍历顺序与递归一致）
        for (int i = currentItem->childCount() - 1; i >= 0; --i) {
            QTreeWidgetItem *childItem = currentItem->child(i);
            if (childItem) itemStack.push(childItem);
        }
    }
}

// 新增：开放所有项编辑权限
void MainWindow::enableAllItemsEdit()
{
    traverseAllItems([](QTreeWidgetItem *item) {
        // 启用编辑权限（保留原有标志，仅叠加可编辑属性）
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    });
    
    // 启用树控件编辑触发方式
    ui->datatreeWidget->setEditTriggers(QTreeWidget::AllEditTriggers);
}

void MainWindow::disableAllItemsEdit()
{
    ShowUserData(m_CurClassStr, m_CurUserStr);
    
    // 2. 禁用所有项的编辑权限
    traverseAllItems([](QTreeWidgetItem *item) {
        // 移除可编辑标志（保留其他原有属性）
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    });
    
    // 3. 禁用树控件编辑触发
    ui->datatreeWidget->setEditTriggers(QTreeWidget::NoEditTriggers);
}

QList<UserRecord> MainWindow::readSelectedUserRecords()
{
    QList<UserRecord> records;
    if (!ui->datatreeWidget) {
        qDebug() << "QTreeWidget 未初始化";
        return records;
    }
    
    qDebug() << "开始读取 QTreeWidget 数据（单层结构，共" << ui->datatreeWidget->topLevelItemCount() << "行）";
    
    // 单层结构：遍历所有顶层项（记录行）
    for (int i = 0; i < ui->datatreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *recordItem = ui->datatreeWidget->topLevelItem(i);
        if (!recordItem) {
            qDebug() << "第" << i << "行：记录项为空，跳过";
            continue;
        }
        
        // 按列索引读取数据
        UserRecord record;
        record.serialNumber = recordItem->text(0).toInt();
        record.interest = recordItem->text(1);
        record.status = recordItem->text(2);
        records.append(record);
        
        // 打印当前行的完整数据（调试用）
        qDebug() << "第" << i + 1 << "行数据："
                 << "序号=" << record.serialNumber
                 << "，兴趣=" << record.interest
                 << "，状态=" << record.status
                 << "（原始列文本：列0=" << recordItem->text(0)
                 << "，列1=" << recordItem->text(1)
                 << "，列2=" << recordItem->text(2) << "）";
    }
    
    // 最终统计打印
    qDebug() << "数据读取完成，共读取到" << records.size() << "条记录";
    return records;
}

// 确认修改按钮槽函数（核心逻辑）
void MainWindow::on_btn_change_clicked()
{
    // 1. 先关闭编辑模式（可选，避免后续误操作）
    if (m_isEditEnabled) {
        ui->btn_edit->setText("开放所有编辑权限");
        m_isEditEnabled = false;
    }
    
    // 2. 校验已有成员变量（班级名/用户名）是否有效
    if (m_CurClassStr.isEmpty() || m_CurUserStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "未选中有效用户（班级名/用户名为空）！");
        return;
    }
    
    // 3. 读取选中用户的修改后记录
    QList<UserRecord> modifiedRecords = readSelectedUserRecords();
    if (modifiedRecords.isEmpty()) {
        QMessageBox::warning(this, "警告", "该用户无任何记录可修改！");
        return;
    }
    
    // 4. 传入 UserDataModel 执行修改（使用已有成员变量定位，传入新记录）
    bool modifySuccess = m_model->changeUserRecords(
        m_CurClassStr,  
        m_CurUserStr,   
        modifiedRecords 
        );
    
    // 5. 提示操作结果
    if (modifySuccess) {
        QMessageBox::information(this, "成功", 
            QString("已成功修改「%1-%2」的记录！").arg(m_CurClassStr).arg(m_CurUserStr));
    } else {
        QMessageBox::critical(this, "失败", 
            QString("修改「%1-%2」的记录失败！").arg(m_CurClassStr).arg(m_CurUserStr));
    }
}
