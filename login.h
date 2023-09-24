#ifndef LOGIN_H
#define LOGIN_H

#include "my_mysql.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>

namespace Ui {class login;}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    QString remeber_passwd(QString name) const;

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    bool eventFilter(QObject *watcher, QEvent *e);

private:
    Ui::login *ui;
    QSqlDatabase db;
    QFile css_file;
    bool mouse_press;
    QPoint begin_point;

signals:
    void send_login_success();

private slots:
    void on_pushButton_login_clicked();
};

#endif // LOGIN_H
