#include "my_mysql.h"

bool my_mysql(QSqlDatabase db)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("qt_mysql");
    db.setUserName("root");
    db.setPassword("root");
    if(!db.open())
    {
        qDebug() << "连接失败";
        return false;
    }
    else
    {
        qDebug() << "连接成功";
        return true;
    }

}
