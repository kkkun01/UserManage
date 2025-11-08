#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include <QStylePainter>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    m_CmbClassA = new CustomComboBox(this);
//    m_CmbClassB = new CustomComboBox(this);
//    InitMyCombox();
    m_model = new UserDataModel(this);
    m_Server = new CustomServer(this);
    ConnectSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitMyCombox()
{
//    // 添加选项
//    QStringList itemsA = {"小明", "小华", "小张"};
//    m_CmbClassA->addItems(itemsA);
//    m_CmbClassA->setCustomCbxTitle(CLASS1_STR);
//    QStringList itemsB = {"查理", "小歆", "小朱"};
//    m_CmbClassB->addItems(itemsB);
//    m_CmbClassB->setCustomCbxTitle(CLASS2_STR);
//    QIcon icon(":/image/file.jpeg");
//    for(int i = 0; i < m_CmbClassA->count(); ++i) {
//        m_CmbClassA->setItemIcon(i, icon);
//        m_CmbClassB->setItemIcon(i, icon);
//    }
//    m_CmbClassA->setGeometry(50,50,100,20);
//    m_CmbClassB->setGeometry(50,150,100,20);

//    m_CmbClassA->setLayoutDirection(Qt::RightToLeft);
//    m_CmbClassB->setLayoutDirection(Qt::RightToLeft);
}

void MainWindow::ConnectSlot()
{
    connect(ui->btn_makenum,&QPushButton::clicked,this,[&](){
        m_currentItem = ui->classtreeWidget->currentItem();
        m_CurClassStr = m_currentItem->parent()->text(0);
        m_CurUserStr = m_currentItem->text(0);
        qDebug()<<"classname"<<m_CurClassStr<<"username"<<m_CurUserStr;
        m_UserThread = new ClassUserThread(m_CurClassStr,m_CurUserStr,m_model);
        if(!m_UserThread->isRunning())
        {
             m_UserThread->start();
        }
        connect(m_UserThread,&ClassUserThread::sglDataGenerated,this,&MainWindow::ShowUserData);
    });

    connect(ui->classtreeWidget, &QTreeWidget::itemClicked, this,[this](){
        m_currentItem = ui->classtreeWidget->currentItem();
        m_CurClassStr = m_currentItem->parent()->text(0);
        m_CurUserStr = m_currentItem->text(0);
        ShowUserData(m_CurClassStr,m_CurUserStr);
    });

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
    
   connect(m_Server, &CustomServer::sglHttpChangeUserData,this, &MainWindow::slotHttpChangeUserData,Qt::DirectConnection);
    
    connect(m_Server, &CustomServer::clientConnected,this, &MainWindow::refreshClientConnected);
    connect(m_Server, &CustomServer::clientDisconnected,this, &MainWindow::refreshClientDisconnected);
    connect(m_Server, &CustomServer::dataReceived,this, &MainWindow::refreshDataReceived);
    

    
    

}

void MainWindow::ShowUserData(const QString &className, const QString &userName)
{
    m_UserCurrdata = m_model->getUserData(className,userName);
    if(m_UserCurrdata.isEmpty())
    {
        qDebug()<<"class userName is NUll";
        QMessageBox::information(this,
                                 "提示",
                                 "class userName is NULL",
                                 QMessageBox::Ok);
        ui->datatreeWidget->clear();
        return;
    }
    qDebug()<<m_UserCurrdata.first();
    ui->datatreeWidget->clear();
    for (int i = 0; i < m_UserCurrdata.size(); ++i) {
           // 获取当前行的数据（一条包含3个元素的QVariantList）
           const QVariantList& currentItem = m_UserCurrdata[i];

           // 安全检查：确保当前行数据包含至少3个元素
           if (currentItem.size() < 3) {
               // 输出警告信息，提示哪条数据不完整
               qWarning() << QString("第%1条数据不完整（少于3个元素），已跳过").arg(i);
               continue; // 跳过不完整的数据，处理下一条
           }

           // 创建树节点项，用于存储当前行的数据
           QTreeWidgetItem* treeItem = new QTreeWidgetItem();

           // 设置第一列数据（序号，整数类型）
           // 将QVariant(int)转换为字符串显示
           treeItem->setText(0, currentItem[0].toString());
           // 存储原始整数类型到UserRole，用于后续可能的排序或数据处理
           treeItem->setData(0, Qt::UserRole, currentItem[0]);

           // 设置第二列数据（信息，字符串类型）
           treeItem->setText(1, currentItem[1].toString());

           // 设置第三列数据（状态，字符串类型）
           treeItem->setText(2, currentItem[2].toString());

           // 将创建好的节点添加到树控件中作为顶级节点
           ui->datatreeWidget->addTopLevelItem(treeItem);
       }

       // 自动调整每列的宽度，使其刚好能容纳该列中最长的内容
       for (int col = 0; col < 3; ++col) {
           ui->datatreeWidget->resizeColumnToContents(col);
       }

       // 输出调试信息，提示成功插入的数据数量
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
        
        // 获取班级名称（顶层节点的文本）
        QString className = classItem->text(0);
        if (className.isEmpty()) continue;
        
        // 遍历当前班级节点的子节点（成员节点），收集成员名称
        QStringList memberList;
        for (int j = 0; j < classItem->childCount(); ++j) {
            QTreeWidgetItem *memberItem = classItem->child(j);
            if (memberItem) {
                // 添加成员节点的文本（假设成员名称在第0列）
                memberList << memberItem->text(0);
            }
        }
        
        // 将“班级名-成员列表”存入映射
        classMemberMap[className] = memberList;
    }
    
    // 将映射传递给服务器（需要在CustomServer中添加对应的接口）
    m_Server->setClassMemberMap(classMemberMap);
    qDebug()<<"classMemberMap:"<<classMemberMap;
}

void MainWindow::on_btn_Dlenum_clicked()
{
     m_userDataItem = ui->datatreeWidget->currentItem();
     if (m_userDataItem)
     {
        int index =m_userDataItem->text(0).toInt();
        qDebug()<<"delete 行号: "<< ui->datatreeWidget->indexOfTopLevelItem(m_userDataItem)<<"序列号： "<<index;
        ui->datatreeWidget->takeTopLevelItem( ui->datatreeWidget->indexOfTopLevelItem(m_userDataItem));
        m_model->deleteUserData(m_CurClassStr,m_CurUserStr,index);
     }else
     {
         QMessageBox::warning(nullptr, "警告", "请先选中要删除的行");
     }
}

//void LeftIconComboBox::paintEvent(QPaintEvent *e)
//{
//    QStylePainter painter(this);
//    painter.setPen(palette().color(QPalette::Text));

//    // 绘制基础控件
//    QStyleOptionComboBox opt;
//    initStyleOption(&opt);
//    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

//    // 手动绘制左侧图标
//    if (currentIndex() >= 0) {
//        QIcon icon = itemIcon(currentIndex());
//        if (!icon.isNull()) {
//            QRect iconRect = opt.rect.adjusted(4, 0, -opt.rect.width() + 24, 0); // 左对齐坐标
//            icon.paint(&painter, iconRect, Qt::AlignLeft);
//        }
//    }

//    // 绘制文本（向右偏移避开图标）
//    opt.rect.adjust(30, 0, 0, 0);
//    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
//}



void MainWindow::on_btn_changenum_clicked()
{
    m_userDataItem = ui->datatreeWidget->currentItem();
    if (m_userDataItem)
    {
        if(!m_rowAllow)
        {
            m_CurindexStr = m_userDataItem->text(0);
        }
        m_rowAllow = true;
        m_userDataItem->setFlags(m_userDataItem->flags() | Qt::ItemIsEditable);
        qDebug()<<"changeable: "<<m_userDataItem->text(0)<<m_userDataItem->text(1)<<m_userDataItem->text(2);
    }else
    {
        QMessageBox::warning(nullptr, "警告", "请先选中要修改的行");
    }
}

void MainWindow::on_btn_HttpStatus_clicked()
{
    if(m_httpText)
    {
        if (m_Server->getServerState() == SERVER_STATE_STOPPED)
        {
            setClassMemberMap();
            m_Server->startServer(9999);
            m_httpText = false;
            ui->btn_HttpStatus->setText("关闭Https");
        }else
        {
            QMessageBox::critical(nullptr, "错误", "打开http服务失败");
        }
    }else
    {
        if(m_Server->getServerState() == SERVER_STATE_RUNNING)
        {
            m_Server->stopServer();
            m_httpText = true;
            ui->btn_HttpStatus->setText("打开Https");
        }else
        {
            QMessageBox::critical(nullptr, "错误", "关闭http服务失败");
        }
    }
}

void MainWindow::refreshClientConnected(const QString &clientInfo)
{
    
}

void MainWindow::refreshClientDisconnected(const QString &clientInfo)
{
    
}

void MainWindow::refreshDataReceived(const QString &clientInfo, const QString &data)
{
    
}

void MainWindow::slotHttpChangeUserData(QString className, QString memberName, int id, QString newInfo, QString newStatus)
{
    m_model->HttpChangeUserData(className, memberName, id, newInfo, newStatus);
    QTreeWidgetItem * curClassItem = ui->classtreeWidget->currentItem();
    if(curClassItem!=nullptr)
    {
        if(curClassItem->parent()->text(0) == className && curClassItem->text(0) == memberName)
        {
           ShowUserData(className,memberName);
        }
    }

}

