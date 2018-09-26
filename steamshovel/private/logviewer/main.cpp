#include "LogViewer.h"
#include <QApplication>
#include <QDir>
#include <QString>
#include <iostream>
#include <fcntl.h>

int main(int argc, char* argv[]){
	QApplication app(argc, argv);
	if( argc != 2 )
		return 1;
	QString filename = argv[1];
	LogViewer v( filename );
	v.show();
	v.raise();
	return app.exec();
}
