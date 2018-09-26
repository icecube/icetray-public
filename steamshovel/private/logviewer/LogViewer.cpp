#include "LogViewer.h"
#include "moc_LogViewer.cpp"

#include <icetray/I3Logging.h>

#include <QString>
#include <QLayout>
#include <QKeyEvent>
#include <QRect>

#include <sstream>
#include <stdexcept>

LogViewer::LogViewer( QString filename ) :
	file_( filename ),
	textview_( new QTextEdit(this) ),
	timer_( new QTimer(this) )
{
	if ( !file_.open(QIODevice::ReadOnly | QIODevice::Text) )
        throw std::runtime_error("Cannot open input as text file");

	QRect rect( 50, 50, 500, 500 );
	setGeometry( rect );
	setWindowTitle( "Steamshovel: Message Log" );

	QFont font;
	textview_->setCurrentFont( font );
	textview_->setReadOnly( true );
	textview_->setWordWrapMode( QTextOption::NoWrap );

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget( textview_ );
	setLayout( mainLayout );

	updateView();
	timer_->start(500);
	connect( timer_, SIGNAL(timeout()), SLOT(updateView()) );
}

void LogViewer::keyPressEvent( QKeyEvent* event )
{
	if( event->matches( QKeySequence::Close ) )
		close();
	else
		QWidget::keyPressEvent( event );
}

void LogViewer::updateView()
{
	QString lines = file_.readAll();
	if (lines.contains("<!-- Steamshovel Normal Exit -->"))
		close();
	textview_->insertHtml( lines );
}
