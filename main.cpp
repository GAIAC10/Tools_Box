#include "widget.h"
#include "login.h"
#include <QObject>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();
    login *login_ = new login;
    login_->show();
    Widget *w = new Widget;
    QObject::connect(login_, &login::send_login_success, w, &Widget::show);

    return a.exec();
}
