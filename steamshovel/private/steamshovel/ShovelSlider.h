#ifndef I3_SHOVEL_DoubleSliderSpinner_H
#define I3_SHOVEL_DoubleSliderSpinner_H

#include <QWidget>

class QDoubleSpinBox;
class QSlider;

class ShovelSlider: public QWidget {
	Q_OBJECT;
	Q_PROPERTY( double value READ value USER true );

	QSlider* slider_;
	QDoubleSpinBox* spinbox_;
	double min_;
	double max_;

protected Q_SLOTS:
	void toSlider( double );
	void toSpinBox( int );

public:
	ShovelSlider( QWidget* parent=0 );
	
	void setRange( double min, double max,
	               unsigned nsteps, unsigned decimals=2 );
	void setSpinBoxRange( double min, double max );
	double value() const;

public Q_SLOTS:
	void setValue( double );
	void setValueSilent( double );

Q_SIGNALS:
	void valueChanged( double );
};

#endif
