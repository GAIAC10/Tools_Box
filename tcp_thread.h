#ifndef TCP_THREAD_H
#define TCP_THREAD_H

#include <QThread>
#include <QHostAddress>
#include <QQueue>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMutex>
#include <QDebug>

class tcp_thread : public QThread
{
    Q_OBJECT

public:
    explicit tcp_thread(QHostAddress ip, QString port, QObject *parent = nullptr);
    ~tcp_thread();

    // 线程工作标志位
    bool start_flag;
    // tcp连接标志位
    bool connect_flag;
    // 保存接收数据到队列标志位
    bool recv_flag;

    // 客户端消息
    QQueue<QByteArray> bytes_queue;
    // 日志信息
    QQueue<QString> msg_queue;

    // tcp套接字
    QTcpSocket *socket;
    // tcp服务器
    QTcpServer *server;
    QHostAddress client_ip;
    qint16 client_port;

    // 暂停线程
    void stop();
    // 继续线程
    void go_on();
    void new_client_connect();

protected:
    void run();

private:
    //互斥量
    QMutex mutex;

signals:
    void signal_msg();

private slots:
    void read_tcp();
    void socket_disconnect();
};

#endif // TCP_THREAD_H
