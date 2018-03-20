#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/icon.ico"));
	MainWindow w;
	if (argc > 1) {
        w.openFile(QString(argv[1]));
    }
	w.show();

	return a.exec();
}
