#ifndef WIDGET_H
#define WIDGET_H

#include <tcp_thread.h>
#include <udp_thread.h>
#include <QWidget>
// 主机地址和ip地址
#include <QHostAddress>
// 检索所有本地网络接口信息
#include <QNetworkInterface>
#include <QDebug>
// 获取项目目录
#include <QDir>
#include <QFileDialog>
#include<QDateTime>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
// 在QTabelView上显示数据库数据
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    // QWidget *parent 新创建一个Widget并指定其父类, =0 表示是一个单独的Window
    // 有explicit时就必须要通过 对象后面加上括号 来调用有参构造
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    // tcp_thread实例化类
    tcp_thread *tcp_thread_;
    // udp_thread实例化类
    udp_thread *udp_thread_;

    QHostAddress get_local_host_ip();
    void is_ui_visiable(bool state);

    QSqlDatabase db;
    QSqlTableModel *m;

private slots:
    // 传输视频和文字
    void print_tcp_msg();
    void close_thread();
    void on_push_listen_begin_clicked();
    void on_push_log_clear_clicked();
    void display_udp_frame();
    void on_push_msg_send_clicked();
    void on_push_image_cat_clicked();
    void on_push_video_cat_clicked();
    void on_push_path_change_clicked();

    // 显示主页控件
    void combobox_set_index(int page);

    // 数据库
    void on_push_select_clicked();
    void on_push_insert_clicked();
    void on_push_insert_view_clicked();
};
#endif // WIDGET_H
