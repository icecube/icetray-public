#ifndef I3_SHOVEL_LOGVIEWER_H
#define I3_SHOVEL_LOGVIEWER_H

#include <QWidget>
#include <QTextEdit>
#include <QTimer>
#include <QFile>

class LogViewer : public QWidget {
	Q_OBJECT;

	QFile file_;
	QTextEdit* textview_;
	QTimer* timer_;

	void keyPressEvent( QKeyEvent* );

protected Q_SLOTS:
	void updateView();

public:
	LogViewer( QString filename );
};

#endif
