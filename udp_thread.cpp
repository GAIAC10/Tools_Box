#include "udp_thread.h"

udp_thread::udp_thread(QHostAddress ip, QString port, QObject *parent) : QThread(parent)
{
    // 默认开启线程
    start_flag = true;
    recv_flag = false;
    // 默认不保存图片
    save_img_flag = false;
    // 默认不保存视频
    save_video_start_flag = false;
    save_video_end_flag = false;

    // 新建一个udp套接字对象
    socket = new QUdpSocket();
    // 绑定IP地址并监听端口
    socket->bind(ip,port.toInt());
    connect(socket,SIGNAL(readyRead()),this,SLOT(recv_udp()));
}

// UDP接收图像数据
void udp_thread::recv_udp()
{
    // QByteArray保存二进制byte数组(序列化)
    QByteArray buff_temp;
    // 将缓存区大小调整成udp可用字节数大小
    buff_temp.resize(socket->bytesAvailable());
    // 读取udp数据大小并且数据保存到缓存区中
    socket->readDatagram(buff_temp.data(),buff_temp.size());
    if(recv_flag)
    {
        // 加入队列，等待线程处理
        bytes_queue.enqueue(buff_temp);
    }
}

void udp_thread::run()
{
    QByteArray img_bytes;
    QString current_time;
    while(start_flag)
    {
        mutex.lock();
        if(!bytes_queue.isEmpty())
        {
            img_bytes = bytes_queue.dequeue();
            // 使用QBuffer管理img_bytes二进制数据
            QBuffer img_buffer(&img_bytes);
            // 从img_buffer中使用JPEG格式读取QImage(QImageReader类内置生成QImage)图片
            QImageReader img_reader(&img_buffer,"JPEG");
            // 一般使用QImage处理完图片数据之后,使用QPixmap显示图片
            QPixmap pix = QPixmap::fromImage(img_reader.read());
            pixmap_queue.enqueue(pix);
            emit signal_img();
            img_bytes.clear();
            // 点击截屏
            if(save_img_flag)
            {
                current_time = QDateTime::currentDateTime().toString("MM-dd-hh-mm-ss");
                save_img_result = save_img_path + current_time + save_img_type;
                // 保存图片
                pix.save(save_img_result);
                save_img_flag = false;
            }
            // 点击开始录屏
            if(save_video_start_flag)
            {
                current_time = QDateTime::currentDateTime().toString("MM-dd-hh-mm-ss");
                save_video_result = save_video_path + current_time + ".avi";
                // VideoWriter创建一个视频写入器对象,用于保存视频文件
                // VideoWriter(const string& filename, int fourcc, double fps,Size frameSize, bool isColor=true);
                // 文件名 压缩帧的codec(视频编解码器的四字符代码) 帧率 视频文件宽高 视频帧是否将按照指定的编解码器进行压缩保存
                save_video_writer.open(save_video_result.toStdString(),VideoWriter::fourcc('M','J','P','G'), 30.0, Size(640, 480), true);
                save_video_start_flag = false;
            }
            // 有save_video_result数据 点击结束录屏
            else if(save_video_end_flag && !save_video_result.isEmpty())
            {
                save_video_writer.release();
            }
            // 录屏中
            if(save_video_writer.isOpened() && !save_video_end_flag)
            {
                save_video_writer.write(QImageToMat(pix.toImage()));
            }
        }
        mutex.unlock();
    }
}

// Qt和OpenCV之间图像数据转换,QImage转Mat，使用opencv库函数进行保存视频
Mat udp_thread::QImageToMat(QImage image)
{
    Mat mat, mat_out;
    switch (image.format())
    {
        case QImage::Format_ARGB32:
        // Qt读入本地彩色图后一般为此格式
        case QImage::Format_RGB32:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            // 转3通道,OpenCV一般使用3通道
            cv::cvtColor(mat, mat_out, CV_BGRA2BGR);
            break;
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat_out, CV_BGR2RGB);
            break;
        case QImage::Format_Grayscale8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        default:
            break;
    }
    return mat_out;
}

void udp_thread::stop()
{
    mutex.lock();
}

void udp_thread::go_on()
{
    mutex.unlock();
}

udp_thread::~udp_thread(){}
