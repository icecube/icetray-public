#ifndef I3_SHOVEL_MOVIE_DIALOG_H
#define I3_SHOVEL_MOVIE_DIALOG_H

#include "ui_movie.h"
#include "shovelart/timewindow.h"
#include <QDialog>

namespace Ui{ class MovieDialog; }
class I3GLWidget;
class QGraphicsScene;
class MovieEngine;

class MovieDialog : public QDialog {

Q_OBJECT;

public:
	MovieDialog( I3GLWidget* gl, const TimeWindow& window, MovieEngine* engine );
	~MovieDialog();

	double getDuration();
	double getRotPerFrame();

public Q_SLOTS:
	void updatePreview();
	void accept();
	void updateDurationLabel();
	void updateOutputLabel();


protected:
	void resizeEvent( QResizeEvent* e );
	void showEvent( QShowEvent* e );


private:
	Ui::MovieDialog ui;
	I3GLWidget* gl_;
	MovieEngine* engine_;
	TimeWindow initial_window_;
	QGraphicsScene *scenes_[3];
	QGraphicsView *views_[3];
	QSize viewsize_;

};

#endif /* I3_SHOVEL_MOVIE_DIALOG_H */
