#ifndef I3_SHOVEL_MOVIE_ENGINE_H
#define I3_SHOVEL_MOVIE_ENGINE_H

#include <boost/python/object.hpp>

#include <QObject>

class I3GLWidget;

class MovieEngine : public QObject{
	Q_OBJECT;

public:
	// default values
	MovieEngine( QObject* parent, I3GLWidget* w ) : QObject(parent), gl_(w),
		width_(640), height_(480), fps_(25), nframes_(500),
		rotation_(5.), starttime_(0), endtime_(40000),
		scale_factor_(1.0), rescale_flag_(false),
		GUI_progress_(true)
	{}

// read/writeable properties.  Write functions are given in slots, below.
	int width() const{ return width_; }
	Q_PROPERTY( int width READ width WRITE setWidth NOTIFY widthChanged );

	int height() const{ return height_; }
	Q_PROPERTY( int height READ height WRITE setHeight NOTIFY heightChanged );

	int fps() const{ return fps_; }
	Q_PROPERTY( int fps READ fps WRITE setFps NOTIFY fpsChanged );

	int nframes() const{ return nframes_; }
	Q_PROPERTY( int nframes READ nframes WRITE setNframes NOTIFY nframesChanged );

	double starttime() const{ return starttime_; }
	Q_PROPERTY( double starttime READ starttime WRITE setStarttime NOTIFY starttimeChanged );

	double endtime() const{ return endtime_; }
	Q_PROPERTY( double endtime READ endtime WRITE setEndtime NOTIFY endtimeChanged );

	double rotation() const { return rotation_; }
	Q_PROPERTY( double rotation READ rotation WRITE setRotation NOTIFY rotationChanged );

	double scaleFactor() const { return scale_factor_; }
	Q_PROPERTY( double scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged );

	bool rescaleFlag() const { return rescale_flag_; }
	Q_PROPERTY( bool rescaleFlag READ rescaleFlag WRITE setRescaleFlag NOTIFY rescaleFlagChanged );

	double duration() const {
		return double(nframes_) / fps_;
	}
	Q_PROPERTY( double duration READ duration STORED false );

	double rotPerFrame() const { return rotation_ / fps_; }
	Q_PROPERTY( double rotPerFrame READ rotPerFrame STORED false );

	double timeStepPerFrame(){ return ( endtime_ - starttime_ ) / nframes_; }
	Q_PROPERTY( double timeStepPerFrame READ timeStepPerFrame STORED false );

	QString imageOutputDir() const { return output_dir_; }
	Q_PROPERTY( QString imageOutputDir READ imageOutputDir WRITE setImageOutputDir );

	QString movieFileName() const { return movie_filename_; }
	Q_PROPERTY( QString movieFileName READ movieFileName );

	bool guiProgress() const { return GUI_progress_; }
	void setGuiProgress( bool b ) { GUI_progress_ = b; }
	Q_PROPERTY( bool guiProgress READ guiProgress WRITE setGuiProgress );

	QString encoder() const { return encoder_; }
	void setEncoder( QString encoder ) { encoder_ = encoder; }
	Q_PROPERTY( QString encoder READ encoder WRITE setEncoder );

	Q_CLASSINFO( "qmeta_scriptable_slots", "cancelCurrentProduction,produceStills,produceMovie,setPreframeHook" );

// callable operations
public Q_SLOTS:
	void setWidth( int w ){ width_ = w; Q_EMIT widthChanged(width_); }
	void setHeight( int h ){ height_ = h; Q_EMIT heightChanged(height_); }
	void setFps( int h ){ fps_ = h; Q_EMIT fpsChanged(fps_); }
	void setNframes( int h ){ nframes_ = h; Q_EMIT nframesChanged(nframes_); }
	void setStarttime( double h ){ starttime_ = h; Q_EMIT starttimeChanged(starttime_); }
	void setEndtime( double h ){ endtime_ = h; Q_EMIT endtimeChanged(endtime_); }
	void setRotation( double r ) { rotation_ = r; Q_EMIT rotationChanged(rotation_); }
	void setScaleFactor( double s ) { scale_factor_ = s; Q_EMIT scaleFactorChanged(scale_factor_); }
	void setRescaleFlag( bool f ) { rescale_flag_ = f; Q_EMIT rescaleFlagChanged(rescale_flag_); }

	void setImageOutputDir( const QString& s ){ output_dir_ = s; }
	void setMovieFileName( const QString& s ){ movie_filename_ = s; }
	void produce();
	void cancelCurrentProduction();

	// these slots are intended for use with qmeta and so use a different string type
	void produceStills( const std::string& dir ){ output_dir_ = QString::fromStdString(dir); movie_filename_ = ""; produce(); }
	void produceMovie( const std::string& name ){ movie_filename_ = QString::fromStdString(name); produce(); }

	void setPreframeHook( boost::python::object );

Q_SIGNALS:
	void widthChanged(int);
	void heightChanged(int);
	void fpsChanged(int);
	void nframesChanged(int);
	void starttimeChanged(double);
	void endtimeChanged(double);
	void rotationChanged(double);
	void scaleFactorChanged(double);
	void rescaleFlagChanged(bool);

protected:
	I3GLWidget* gl_;
	int width_, height_;
	int fps_, nframes_;
	double rotation_;
	double starttime_, endtime_;

	double scale_factor_;
	bool rescale_flag_;

	QString output_dir_;
	QString movie_filename_;
	QString encoder_;

	bool cancelflag_;

	bool GUI_progress_;

	boost::python::object preframe_hook_;
};

class QImage;
class QString;

class MovieProgress {
public:
	virtual ~MovieProgress(){}
	virtual void start( int max ) = 0;
	virtual void step( int k, const QString& path, const QImage& image ) = 0;
	virtual void converting() = 0;
	virtual void done() = 0;
};

class NullMovieProgress : public MovieProgress {
public:
	virtual void start( int max ){}
	virtual void step( int k, const QString& path, const QImage& image ){}
	virtual void converting(){}
	virtual void done(){}
};

#endif /* I3_SHOVEL_MOVIE_ENGINE_H */
