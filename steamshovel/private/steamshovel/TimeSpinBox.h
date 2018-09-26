#ifndef I3_SHOVEL_TIMESPINBOX_H
#define I3_SHOVEL_TIMESPINBOX_H

#include <QDoubleSpinBox>
#include <QSize>
#include <limits>

class TimeSpinBox : public QDoubleSpinBox {
	Q_OBJECT;

public:
	TimeSpinBox(QWidget* parent) : QDoubleSpinBox(parent)
	{
		setMinimum( -std::numeric_limits<double>::max() );
		setMaximum( std::numeric_limits<double>::max() );
		setDecimals( 0 );
		setSuffix( " ns" );
	}

	QSize sizeHint() const { return QSize(120, 27); }
};

#endif
