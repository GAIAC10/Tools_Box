#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    :QWidget(parent)
    ,ui(new Ui::login)
    /*
    添加新的UI界面
    https://www.cnblogs.com/edgarli/p/16164687.html
    */
{
    ui->setupUi(this);
    // 设置窗体无边框,不可拖动拖拽拉伸
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 退出即析构
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    // 添加信息清除按钮
    ui->lineEdit_username->setClearButtonEnabled(true);
    ui->lineEdit_passwd->setClearButtonEnabled(true);
    // QKeySequence 快捷键
    ui->pushButton_login->setShortcut(QKeySequence::InsertParagraphSeparator);
    // 过滤器
    ui->lineEdit_username->installEventFilter(this);
    ui->lineEdit_passwd->installEventFilter(this);
    // 美化:去边框
    ui->lineEdit_username->setStyleSheet("QLineEdit{background-color:transparent}""QLineEdit{border-width:0;border-style:outset}");
    ui->lineEdit_passwd->setStyleSheet("QLineEdit{background-color:transparent}""QLineEdit{border-width:0;border-style:outset}");
}

login::~login()
{
    delete ui;
}

// 记住密码
QString login::remeber_passwd(QString name) const
{
    QFile from_passwd("../Users/passwd.json");
    from_passwd.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray array = from_passwd.readAll();
    from_passwd.close();
    int flag = 0;
    QJsonDocument document = QJsonDocument::fromJson(array);
    QJsonObject jsonobject = document.object();
    // json迭代器
    QJsonObject::Iterator iterjson = jsonobject.begin();
    while(iterjson != jsonobject.end())
    {
        if(iterjson.key() == name)
        {
            flag = 1;
            break;
        }
        iterjson++;
    }
    if(flag == 1)
    {
        // 有name返回密码
        return iterjson.value().toString();
    }
    else
    {
        // 没name返回空字符串
        return QString();
    }
}

// 按压鼠标
void login::mousePressEvent(QMouseEvent *e)
{
    mouse_press = true;
    begin_point = e->pos();
    // 继续基类事件循环
    QWidget::mousePressEvent(e);
}

// 移动鼠标
void login::mouseMoveEvent(QMouseEvent *e)
{
    if(mouse_press)
    {
        // QCursor::pos()鼠标光标的位置
        QPoint relaPos(QCursor::pos() - begin_point);
        move(relaPos);
    }
    // 继续基类事件循环
    QWidget::mouseMoveEvent(e);
}

// 用户名和密码的过滤器函数
bool login::eventFilter(QObject *watcher, QEvent *e)
{
    if(watcher == ui->lineEdit_username)
    {
        QString name = ui->lineEdit_username->text();
        // 失焦(输入完毕,且没有光标之后)
        if(e->type() == QEvent::FocusOut)
        {
            ui->lineEdit_username->clearFocus();
            QString passwd = this->remeber_passwd(name);
            // 自动填写密码
            ui->lineEdit_passwd->setText(passwd);
            return true;
        }
    }
    // 最后默认返回watcher,e
    return QWidget::eventFilter(watcher,e);
}

// 用户登录,向数据库中查询数据并判断
void login::on_pushButton_login_clicked()
{
    /*
    // 获取用户名
    QString username = ui->lineEdit_username->text();
    // 获取密码
    QString password = ui->lineEdit_passwd->text();
    // 加密
    QByteArray mm_utf8 = password.toUtf8();
    for(int i = 0; i < mm_utf8.size(); i++)
    {
        mm_utf8[i] = mm_utf8[i] + key;
    }
    QString password_md = QString(mm_utf8);
    // 查询语句
    QString sql = QString("select * from student where username = '%1' and password = '%2';").arg(username).arg(password_md);
    // 执行查询
    QSqlQuery query(db);
    // 没有查询到数据
    if(!query.exec(sql))
    {
        qDebug() << db.lastError().text();
    }
    // 查询到数据
    if(query.next())
    {
        QMessageBox::information(this,"登录提示","登录成功");
        qDebug() << "登录成功";
    // 记住密码 勾选框选中
    // if(ui->checkBox->isChecked())
    // {
    // }
    }
    else
    {
        QMessageBox::information(this,"登录提示","登录失败");
        qDebug() << "登录失败";
    }
    */
    emit send_login_success();
    this->hide();
}

// 用户注册,向数据库中插入数据
void login::on_pushButton_register_clicked()
{
    /*
    // 获取用户名
    QString username = ui->lineEdit_username->text();
    // 获取密码
    QString password = ui->lineEdit_passwd->text();
    // 加密
    QByteArray mm_utf8 = password.toUtf8();
    for(int i = 0; i < mm_utf8.size(); i++)
    {
        // 密钥
        mm_utf8[i] = mm_utf8[i] + key;
    }
    QString password_md = QString(mm_utf8);
    // 插入语句
    QString sql = QString("insert into student(username,password) value('%1','%2');").arg(username).arg(password_md);
    // 执行插入
    QSqlQuery query(db);
    if(query.exec(sql))
    {
        QMessageBox::information(this,"注册提示","注册成功");
        qDebug() << "注册成功";
    }
    else
    {
        QMessageBox::information(this,"注册提示","注册失败");
        qDebug() << "注册失败";
        qDebug() << db.lastError().text();
    }
    */
}
