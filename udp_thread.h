#ifndef UDP_THREAD_H
#define UDP_THREAD_H

#include <QThread>
#include <QHostAddress>
#include <QQueue>
#include <QUdpSocket>
#include <QMutex>
#include <QBuffer>
#include <QImageReader>
#include <QPixmap>
#include <QDateTime>
// OpenCV配置
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// 使用OpenCV内置函数
#include <opencv2/imgproc/types_c.h>
// OpenCV命名空间
using namespace cv;


class udp_thread : public QThread
{
    Q_OBJECT

public:
    explicit udp_thread(QHostAddress ip,QString port,QObject *parent = nullptr);
    ~udp_thread();

    // 线程工作标志位
    bool start_flag;
    // 保存udp数据到队列标志位
    bool recv_flag;
    // 图像原始数据队列
    QQueue<QByteArray> bytes_queue;
    // 图像数据队列
    QQueue<QPixmap> pixmap_queue;

    // 保存图片相关
    bool save_img_flag;
    QString save_img_path;
    QString save_img_type;
    QString save_img_result;

    // 保存视频相关
    bool save_video_start_flag;
    bool save_video_end_flag;
    QString save_video_path;
    QString save_video_result;
    // OpenCV方式保存视频
    VideoWriter save_video_writer;

    // udp套接字
    QUdpSocket *socket;

    // 暂停线程
    void stop();
    // 继续线程
    void go_on();

protected:
    void run();

private:
    // 互斥量
    QMutex mutex;
    Mat QImageToMat(QImage image);

signals:
   void signal_img();

private slots:
    void recv_udp();

};

#endif // UDP_THREAD_H
