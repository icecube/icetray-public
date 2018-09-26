#include "ShovelSlider.h"
#include "moc_ShovelSlider.cpp"

#include <QSlider>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QSizePolicy>
#include <boost/math/special_functions/round.hpp>

ShovelSlider::ShovelSlider( QWidget* parent ) :
	QWidget( parent ),
	slider_( new QSlider( Qt::Horizontal, this ) ),
	spinbox_( new QDoubleSpinBox( this ) )
{
	setRange( 0.0, 100.0, 100 );

	slider_->setFocusPolicy( Qt::WheelFocus );
	slider_->setAutoFillBackground( true );
	slider_->setContentsMargins(0, 0, 1, 0);
	spinbox_->setFocusPolicy( Qt::WheelFocus );
	spinbox_->setAutoFillBackground( true );
	spinbox_->setContentsMargins(1, 0, 0, 0);

	QGridLayout* l = new QGridLayout( this );
	l->setContentsMargins( 0, 0, 0, 0 );
	l->addWidget( slider_, 0, 0, 1, 3 );
	l->addWidget( spinbox_, 0, 3 );

	connect( slider_, SIGNAL(valueChanged(int)),
	         SLOT(toSpinBox(int)) );
	connect( spinbox_, SIGNAL(valueChanged(double)),
	         SLOT(toSlider(double)) );
}

void ShovelSlider::setRange( double min, double max,
                             unsigned nsteps,
                             unsigned decimals ){
	min_ = min;
	max_ = max;
	spinbox_->setRange( min, max );
	slider_->setRange( 0, nsteps );
	spinbox_->setSingleStep( (max - min) / nsteps );
	spinbox_->setDecimals( decimals );
}

void ShovelSlider::setSpinBoxRange( double min, double max ){
	spinbox_->setRange( std::min(min_, min), 
	                    std::max(max_, max) );
}

void ShovelSlider::toSlider( double x ){
	double z = (x - min_) / (max_ - min_);
	z = std::max(z, 0.0);
	z = std::min(z, 1.0);
	const int k = boost::math::round<int>( z * slider_->maximum() );
	slider_->blockSignals(true);
	slider_->setValue( k );
	slider_->blockSignals(false);
	Q_EMIT valueChanged( x );
}

void ShovelSlider::toSpinBox( int k ){
	double z = double( k ) / double( slider_->maximum() );
	z = std::max( z, 0.0 );
	z = std::min( z, 1.0 );
	const double x = (1.0 - z) * min_ + z * max_;
	spinbox_->blockSignals(true);
	spinbox_->setValue( x );
	spinbox_->blockSignals(false);
	Q_EMIT valueChanged( x );
}

double ShovelSlider::value()const{
	return spinbox_->value();
}

void ShovelSlider::setValue( double x ){
	spinbox_->blockSignals(true);
	spinbox_->setValue( x );
	spinbox_->blockSignals(false);
	toSlider( x );
}

void ShovelSlider::setValueSilent( double x ){
	blockSignals(true);
	setValue( x );
	blockSignals(false);
}
