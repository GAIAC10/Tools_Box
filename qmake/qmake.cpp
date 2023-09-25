#include <QApplication>

// 基本控件
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// 基本布局(布局也是对象)
// 水平布局
#include <QHBoxLayout>
// 垂直布局
#include <QVBoxLayout>

// 窗口
#include <QWidget>

int main(int argc, char *argv[])
{
	// 本程序就是一个对象
	QApplication app(argc, argv);

	QLabel *infoLabel = new QLabel;
	QLabel *openLabel = new QLabel;
	QLineEdit *cmdLineEdit = new QLineEdit;
	QPushButton *commitbutton = new QPushButton;
	QPushButton *cancelbutton = new QPushButton;
	QPushButton *browsebutton = new QPushButton;

	infoLabel->setText("input cmd:");
	openLabel->setText("open");
	commitbutton->setText("commit");
	cancelbutton->setText("cancel");
	browsebutton->setText("browse");

	// 水平布局对象
	QHBoxLayout *cmdLayout = new QHBoxLayout;
	cmdLayout->addWidget(openLabel);
	cmdLayout->addWidget(cmdLineEdit);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(commitbutton);
	buttonLayout->addWidget(cancelbutton);
	buttonLayout->addWidget(browsebutton);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(infoLabel);
	mainLayout->addLayout(cmdLayout);
	mainLayout->addLayout(buttonLayout);

	QWidget w;
	w.setLayout(mainLayout);
	w.show();
	w.setWindowTitle("running");

	// 运行之后一直执行
	return app.exec();

//生成.exe
//1.配置环境
//2.打开qt的cmd(不是电脑cmd)
//3.切换到cpp文件的目录下
//4.qmake -project生成pro文件
//5.qmake生成makefile
//6.mingw32-make生成.exe
//7.运行.exe
}