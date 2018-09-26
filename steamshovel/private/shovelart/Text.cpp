#include <icetray/I3Logging.h>

#include <QPainter>

#include "Text.h"

QPixmap makeStringPixmap( const std::string& message, QColor color,
                          QColor bg_color, QFont font, double scaling_factor ){

	// the QFontMetrics::boundingRect function will be used to compute the necessary max width
	// and height of the output pixmap.
	QFontMetrics fm(font);

	QString str( message.c_str() );
	QStringList lines = str.split( QRegExp("\\n") );
	QList<QRect> spacings;
	int nlines = lines.length();
	int height = 4;
	int width = 1;
	for( int i = 0; i < nlines; ++i ){
		QRect bound = fm.boundingRect( lines[i] );
		width = std::max( width, bound.width() + 4 );
		height += bound.height();
		spacings.append( bound );

	}

	QPixmap pixmap(width*scaling_factor, height*scaling_factor);
	if( bg_color.isValid() ){
		pixmap.fill( QColor::fromRgbF( bg_color.redF(), bg_color.blueF(), bg_color.greenF(), 0. ) );
	}
	else{
		// old default behavior matches the default steamshovel background color
		pixmap.fill( QColor::fromRgbF(.15, .15, .15, 0.) );
	}
	QPainter p(&pixmap);
	p.setFont(font);
	p.setPen(color);
	p.scale(scaling_factor, scaling_factor);
	int spacing = 0;
	for( int i = 0; i < nlines; ++i ){
		spacing += -spacings[i].top();
		p.drawText( 2, spacing-2, lines[i] );
		spacing += spacings[i].bottom();
	}
	return pixmap;

}
