#include "tcp_thread.h"

tcp_thread::tcp_thread(QHostAddress ip, QString port, QObject *parent) : QThread(parent)
{
    // 线程工作标志位
    start_flag = true;
    // tcp连接标志位
    connect_flag = false;
    // 保存接收数据到队列标志位
    recv_flag = false;

    // 新建一个对象
    server = new QTcpServer();
    // 监听端口，等待客户端连接
    server->listen(ip, port.toInt());
    connect(server,&QTcpServer::newConnection,this,&tcp_thread::new_client_connect);
}

void tcp_thread::new_client_connect()
{
    // 获取客户端套接字
    socket = server->nextPendingConnection();
    client_ip = socket->peerAddress();
    client_port = socket->peerPort();
    connect(socket,SIGNAL(readyRead()),this,SLOT(read_tcp()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(socket_disconnect()));
    connect_flag = true;
    msg_queue.enqueue(QString("上线->[%1]:[%2]").arg(client_ip.toString()).arg(client_port));
    emit signal_msg();
}

// 读取客户端发送来的数据
void tcp_thread::read_tcp()
{
    QByteArray buffer;
    buffer = socket->readAll();
    // 在点击"开始监听"之后recv_flag = True
    if(recv_flag)
    {
        bytes_queue.enqueue(buffer);
    }
}

// 客户端断开
void tcp_thread::socket_disconnect()
{
    connect_flag = false;
    msg_queue.enqueue(QString("已断开!"));
    emit signal_msg();
}

// 保证只有一个Thread对QQueue进行数据提取
void tcp_thread::run()
{
    QByteArray data_temp;
    while(start_flag)
    {
        mutex.lock();
        if(!bytes_queue.isEmpty())
        {
            data_temp = bytes_queue.dequeue();
            msg_queue.enqueue(QString(data_temp));
            emit signal_msg();
        }
        mutex.unlock();
    }
}

void tcp_thread::go_on()
{
    mutex.unlock();
}

void tcp_thread::stop()
{
    mutex.lock();
}

tcp_thread::~tcp_thread(){}
