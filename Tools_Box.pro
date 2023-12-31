QT       += core gui
QT       += network
QT       += sql

# 大于Qt4版本 才包含widget模块
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 应用程序名  生成的.exe程序名称
TARGET = tools_box

# 模板类型   应用程序模板
# 模板变量告诉qmake为这个应用程序生成哪种makefile
# 默认是app,但是也有其他的(lib/vcapp/vclib/...)
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    login.cpp \
    my_mysql.cpp \
    tcp_thread.cpp \
    udp_thread.cpp \
    widget.cpp

HEADERS += \
    login.h \
    my_mysql.h \
    tcp_thread.h \
    udp_thread.h \
    widget.h

FORMS += \
    login.ui \
    widget.ui

# OpenCV配置
# 详见:https://blog.csdn.net/weixin_43763292/article/details/112975207
INCLUDEPATH += F:\Project_C++\OpenCV-4.5.4\opencv\opencv-build\install\include
LIBS += F:\Project_C++\OpenCV-4.5.4\opencv\opencv-build\lib\libopencv_*.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
