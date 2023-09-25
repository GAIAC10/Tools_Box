#include "widget.h"
#include "ui_widget.h"
#include "tcp_thread.h"

// : QWidget(parent)(继承)表示调用父类(Qwidget)中的构造函数(没有构造函数那就默认构造函数)
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    /*
    https://www.jianshu.com/p/48f007c2de09
    */

    /* 界面操作 */
    // 设置widget名字
    this->setWindowTitle("Tools_Box");

    QString current_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
    ui->textbrowser->append(current_time + "\t开机");

    // 退出即析构
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    // ComboBox切换StackWidget
    // 函数currentIndexChanged需要重载(接收int/QString)
    void (QComboBox::*currentIndexChangedSingal)(int page) = &QComboBox::currentIndexChanged;
    connect(ui->combobox_main,currentIndexChangedSingal,this,&Widget::combobox_set_index);
    // 默认是第一个窗口
    ui->stackedwidget_main->setCurrentIndex(0);

    /*
    lambda表达式 [](){}();
    [] =和this是局部所有的值传递/&是局部所有的引用传递/自定义值(x,y,...)
    () 1.操作符重载函数参数 -> (a,b)/(&a,&b) 2.()mutable -> {}中操作的值通过浅拷贝的方式输出
    {} 函数体
    () 由信号带不带()决定
    ex1:
    直接设置按钮文字(不需要connect)
    [=](){
        btn-> setText("text");
    }();
    ex2:
    返回值数据类型{}
    int ret = []()->int{return 1000;}();
    */

    // 历史数据
    connect(ui->push_history, &QPushButton::clicked, [this]()->void{ui->combobox_main->setCurrentIndex(0);ui->stackedwidget_main->setCurrentIndex(2);});
    // 视频传输
    connect(ui->push_video, &QPushButton::clicked, [this]()->void{ui->combobox_main->setCurrentIndex(0);ui->stackedwidget_main->setCurrentIndex(3);});
    // 图片传输
    connect(ui->push_image, &QPushButton::clicked, [this]()->void{ui->combobox_main->setCurrentIndex(0);ui->stackedwidget_main->setCurrentIndex(4);});
    // 获取数据
//    connect(ui->push_dataget, &QPushButton::clicked, this, []()
//    {
//        // TCP获取数据
//    });
    // 数据库
    connect(ui->push_database, &QPushButton::clicked, [this]()
    {
        ui->stackedwidget_main->setCurrentIndex(5);
    }
    );

    /* 网络传输 */
    // 获取本机IP地址 设置PORT
    QHostAddress local_ip = get_local_host_ip();
    QString udp_port = "8888";
    QString tcp_port = "9999";

    // 显示项目路径
    QString dir = QDir::currentPath() + "/";
    ui->lineedit_path_change->setText(dir);
    // 显示网络信息
    ui->label_ip_inf->setText(local_ip.toString());
    ui->label_udp_port_inf->setText(udp_port);
    ui->label_tcp_port_inf->setText(tcp_port);

    is_ui_visiable(false);

    // 新建TCP对象和Thread对象(主要传输数据)
    tcp_thread_ = new tcp_thread(local_ip,tcp_port);
    connect(tcp_thread_,&tcp_thread::signal_msg,this,&Widget::print_tcp_msg);

    // 新建UDP对象和Thread对象(主要传输视频)
    udp_thread_ = new udp_thread(local_ip,udp_port);
    // 接收线程信号
    connect(udp_thread_, &udp_thread::signal_img, this, &Widget::display_udp_frame);

    connect(this, &Widget::destroyed, this, &Widget::close_thread); //线程跟随窗口退出

    tcp_thread_->start();
    udp_thread_->start();
    // 上锁
    tcp_thread_->stop();
    // 上锁
    udp_thread_->stop();
    ui->textBrowser_2->setText("等待客户端连接...");

    /* 数据库查询 */

    /*
    创建数据库 -> 创建表 -> 添加数据
    创建数据库
    create database qt_mysql charset utf8;
    创建表
    create table student
    (
        id integer unsigned primary key,
        name varchar(16) not null,
        birth date
    )charset utf8;
    查看数据
    select * from student;
    插入数据
    insert into student value(1,"tom","1999-9-9");
    */

    // 初始化数据库
    // 加载mysql数据库
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("qt_mysql");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("root");
    db.setPort(3306);
    // 手动编译动态库
    if(db.open())
    {
        QMessageBox::information(this,"连接提示","连接成功");
        // QSqlTableModel表示数据上的模型
        m = new QSqlTableModel;
        // 连接"student"数据表
        m->setTable("student");
        // setModel表示界面上的模型
        ui->tableView->setModel(m);
    }
    else
    {
        QMessageBox::warning(this,"连接提示","连接失败");
        // 打印错误结果
        qDebug() << db.lastError().text();
    }
}

Widget::~Widget()
{
    delete ui;
}

// 获得本地IP
QHostAddress Widget::get_local_host_ip()
{
    /*
    获取主机名
    QString localhostName=QHostInfo::localHostName();
    */

    /*
    QHostInfo表示IP总信息的集合
    QHostInfo hostinfo = QHostInfo::fromName(localhostName);
    QList<QHostAddress> addrlist = hostinfo.addresses();
    for(int i=0; i<addrlist.count(); i++)
    {
        QHostAddress host = addrlist.at(i);
    }
    */
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    QHostAddress result;
    foreach(QHostAddress address, AddressList)
    {
        // 使用IPV4
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::Null && address != QHostAddress::LocalHost)
        {
            if(address.toString().contains("127.0."))
            {
                continue;
            }
            if(address.toString().contains("192.168.1."))
            {
                result = address;
                break;
            }
        }
    }
    qDebug() << "get_local_host_ip:" << result;
    return result;
}

// 按钮使能
void Widget::is_ui_visiable(bool state)
{
    ui->push_image_cat->setEnabled(state);
    ui->push_video_cat->setEnabled(state);
    ui->lineedit_path_change->setEnabled(state);
    ui->push_path_change->setEnabled(state);
    ui->radio_jpg_select->setEnabled(state);
    ui->radio_png_select->setEnabled(state);
    ui->text_edit_msg->setEnabled(state);
    ui->push_log_clear->setEnabled(state);
    ui->push_msg_send->setEnabled(state);
}

// class tcp_thread类发送msg_queue的emit对应slot
void Widget::print_tcp_msg()
{
    if(!tcp_thread_->msg_queue.isEmpty())
    {
        ui->textBrowser_2->setTextColor(QColor::fromRgb(255,0,0));
        ui->textBrowser_2->append("Client:" + tcp_thread_->msg_queue.dequeue());
        ui->textBrowser_2->setTextColor(QColor::fromRgb(0,0,0));
        // 当tcp连接断开,需要打印"已断开!"时
        if(!tcp_thread_->connect_flag)
        {
            on_push_listen_begin_clicked();
            ui->textBrowser_2->setText("等待客户端连接...");
        }
    }
}

// 点击"开始监听"按钮
void Widget::on_push_listen_begin_clicked()
{
    // 点击文字为"开始监听"的按钮
    if(ui->push_listen_begin->text().toUtf8() == "开始监听")
    {
        // 有tcp客户端连接时
        if(tcp_thread_->connect_flag)
        {
            ui->push_listen_begin->setText("结束监听");
            // 控件使能
            is_ui_visiable(true);
            // 解锁
            tcp_thread_->go_on();
            // 解锁
            udp_thread_->go_on();
            udp_thread_->recv_flag = true;
            tcp_thread_->recv_flag = true;
        }
        else
        {
            ui->textBrowser_2->append("无客户端连接!");
        }
    }
    else if(ui->push_listen_begin->text().toUtf8() == "结束监听")
    {
        ui->push_listen_begin->setText("开始监听");
        udp_thread_->recv_flag = false;
        tcp_thread_->recv_flag = false;
        // 控件失能
        is_ui_visiable(false);
        // 暂停udp线程
        udp_thread_->stop();
        // 暂停udp线程
        tcp_thread_->stop();
        ui->textBrowser_2->clear();
        // 清空视频页面
        ui->label_video_2->clear();
    }
}

// 关闭所有线程
void Widget::close_thread()
{
    tcp_thread_->go_on();
    // 停止线程
    tcp_thread_->start_flag = false;
    tcp_thread_->quit();
    // 等待线程处理完手头工作
    tcp_thread_->wait();
    if(tcp_thread_->connect_flag)
    {
        tcp_thread_->socket->abort();
        tcp_thread_->server->close();
    }
    udp_thread_->go_on();
    // 停止线程
    udp_thread_->start_flag = false;
    udp_thread_->quit();
    // 等待线程处理完手头工作
    udp_thread_->wait();
}

// 清空接受区
void Widget::on_push_log_clear_clicked()
{
    ui->textBrowser_2->clear();
}

// tcp发送消息
void Widget::on_push_msg_send_clicked()
{
    // 获得TextEdit信息
    QString msg = ui->text_edit_msg->toPlainText();
    tcp_thread_->socket->write(msg.toUtf8());
    // 将缓存区中的数据存入socket强制发送
    tcp_thread_->socket->flush();

    ui->textBrowser_2->setTextColor(QColor::fromRgb(0,0,255));
    ui->textBrowser_2->append("Server:"+msg);
    ui->textBrowser_2->setTextColor(QColor::fromRgb(0,0,0));
}

// udp_thread要求显示图片、视频(传输为QPixmap数据,在pixmap_queue中)
void Widget::display_udp_frame()
{
    if(!udp_thread_->pixmap_queue.isEmpty())
    {
        QPixmap pix = udp_thread_->pixmap_queue.dequeue();
        ui->label_video_2->setPixmap(pix);
        // 点击截屏
        if(!udp_thread_->save_img_result.isEmpty())
        {
            ui->textBrowser_2->append("已保存!"+udp_thread_->save_img_result);
            udp_thread_->save_img_result.clear();
        }
        // 点击录屏
        else if(udp_thread_->save_video_end_flag)
        {
            ui->textBrowser_2->append("已保存!"+udp_thread_->save_video_result);
            udp_thread_->save_video_end_flag = false;
        }
    }
}

//截屏
void Widget::on_push_image_cat_clicked()
{
    // 设置图片保存方式
    if(ui->radio_jpg_select->isChecked())
    {
        udp_thread_->save_img_type = ".jpg";
    }
    else if(ui->radio_png_select->isChecked())
    {
        udp_thread_->save_img_type = ".png";
    }
    // 设置图片保存路径
    udp_thread_->save_img_path = ui->lineedit_path_change->text().toUtf8();
    udp_thread_->save_img_flag = true;
}

// 录屏
void Widget::on_push_video_cat_clicked()
{
    if(ui->push_video_cat->text().toUtf8() == "录屏")
    {
        ui->push_video_cat->setText("停止录屏");
        // 视频保存路径
        udp_thread_->save_video_path = ui->lineedit_path_change->text().toUtf8();
        udp_thread_->save_video_start_flag = true;
    }
    else if(ui->push_video_cat->text().toUtf8() == "停止录屏")
    {
        ui->push_video_cat->setText("录屏");
        udp_thread_->save_video_end_flag = true;
    }
}

// 更改文件保存路径
void Widget::on_push_path_change_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "请选择文件保存路径...", "./");
    if(filePath.isEmpty())
    {
        filePath = ".";
    }
    ui->lineedit_path_change->setText(filePath+"/");
}

// 通过ComboBox选择StackWidget窗口
void Widget::combobox_set_index(int page)
{
    ui->stackedwidget_main->setCurrentIndex(page--);
}

// 数据库查询并打印
void Widget::on_push_select_clicked()
{
    QSqlQuery query(db);
    QString sql = QString("select * from student;");
    query.exec(sql);
    while(query.next())
    {
        qDebug() << query.value(0);
        qDebug() << query.value(1);
        qDebug() << query.value(2);
    }
}

// 数据库插入
void Widget::on_push_insert_clicked()
{
    QString id = ui->lineedit_id->text();
    QString name = ui->lineedit_name->text();
    QString birth = ui->lineedit_bir->text();
    // 插入语句
    QString sql = QString("insert into student value(%1,'%2','%3');").arg(id).arg(name).arg(birth);
    // 执行插入
    QSqlQuery query(db);
    if(query.exec(sql))
    {
        QMessageBox::information(this,"插入提示","插入成功");
    }
    else
    {
        QMessageBox::information(this,"插入提示","插入失败");
    }
}

// 数据库查询并显示到TabelView上
void Widget::on_push_insert_view_clicked()
{
    // 对数据库进行增删改查->sql语句/TableView
    m->select();
}
