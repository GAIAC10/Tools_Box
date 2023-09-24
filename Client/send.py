"""
作为Tools_Box -> Tools_box.pro的Client
TCP : 循环定时向Server发送本地时间
UDP : 向Server传输视频
"""

import cv2 as cv2
import socket
import threading
import datetime
import time

udp = ''
tcp_connect_flag = False


class UDP_SOCKET(object):
    # 未自定义时默认IP地址和端口号
    def __init__(self, server_addr=('127.0.0.1', 8888)):
        self.addr = server_addr
        self.udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send_frame(self, frame, addr=()):
        if addr:
            self.addr = addr
        self.udp.sendto(frame, self.addr)

    def close(self):
        self.udp.close()


class IMAGE_FUNC(object):
    def __init__(self, compress_quality=89):
        # 使用JPEG编码格式并自定义quality(cv2.IMWRITE_JPEG_QUALITY本质是一个int值,表示编码程度[0:低质量,高压缩...])
        self.encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), compress_quality]

    def compress(self, img, quality=89):
        self.encode_param[1] = quality
        times = 0
        while True:
            # img(要编码的图像) -> byte
            _, encode_img = cv2.imencode('.jpg', img, self.encode_param)
            times += 1
            if len(encode_img) <= 65500:
                return encode_img
            # 寻找quality使len<65500
            if times > 3:
                self.encode_param[1] -= 1
                times = 0

    def send_video(self):
        global udp, tcp_connect_flag
        cam = cv2.VideoCapture(0)
        if cam.isOpened():
            while True:
                try:
                    _, frame = cam.read()
                    cv2.imshow('frame', frame)  # h:480 w:640 s:3
                    udp.send_frame(self.compress(frame))
                except Exception as e:
                    print(e)
                    break
                key = cv2.waitKey(20)
                if key == ord('q'):
                    break
                elif tcp_connect_flag is False:
                    print('tcp 已断开连接！')
                    break
        else:
            print('cam error!')
        cam.release()


class TCP_SOCKET(object):
    # 未自定义时默认IP地址和端口号
    def __init__(self, server_addr=('127.0.0.1', 9999)):
        global tcp_connect_flag
        self.addr = server_addr
        self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp.settimeout(60)
        print('正在连接TCP服务器...')
        while True:
            try:
                self.tcp.connect(server_addr)
                print('tcp connect success!')
                tcp_connect_flag = True
                break
            except Exception:
                pass

    def send_data(self):
        global tcp_connect_flag
        while True:
            if tcp_connect_flag is False:
                print('TCP已断开连接！')
                break
            current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            # 发送时间
            self.tcp.send(current_time.encode())
            time.sleep(2)

    def recv_data(self):
        global tcp_connect_flag
        while True:
            data = self.tcp.recv(1024)
            if len(data) != 0:
                print('server: '+str(data.decode()))
            else:
                print('TCP服务端已退出!')
                tcp_connect_flag = False
                break

    def close(self):
        self.tcp.close()


# 设置TCP/UDP的IP地址和端口号
tcp = TCP_SOCKET(('192.168.1.3', 9999))
udp = UDP_SOCKET(('192.168.1.3', 8888))
img = IMAGE_FUNC()

tcp_recv_data = threading.Thread(target=tcp.recv_data)
tcp_send_data = threading.Thread(target=tcp.send_data)
udp_video = threading.Thread(target=img.send_video)

tcp_recv_data.start()
udp_video.start()
tcp_send_data.start()

tcp_recv_data.join()
udp_video.join()
tcp_send_data.join()

udp.close()
tcp.close()
cv2.destroyAllWindows()
