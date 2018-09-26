#ifndef I3_SHOVEL_CHOICE_SETTING_H
#define I3_SHOVEL_CHOICE_SETTING_H

#include "ComplexSetting.h"
#include <QString>
#include <QStringList>
#include <stdexcept>

/**
 * A setting based on a options of strings. Can be casted to QString.
 * Carries options information as QStringList and options as index.
 */
class ChoiceSetting: public ComplexSetting<int> {
	const QStringList options_;

	void validate() const{
		if ( int(*this) < 0 || int(*this) >= options_.size() )
			throw std::runtime_error("ChoiceSetting: index outside of options range");
	}

public:
	typedef int Value;

	ChoiceSetting( const QStringList& options, const Value idx ) :
		ComplexSetting<int>( idx ),
		options_(options)
	{
		validate();
	}

	ChoiceSetting( const QString& options, const Value idx ) :
		ComplexSetting<int>( idx ),
		options_(options.split(";"))
	{
		validate();
	}

	// needed by QVariant
	ChoiceSetting() :
		ComplexSetting<int>( 0 ),
		options_()
	{}

	// overload base version to add validation
	void setValue( int val ){
		ComplexSetting<int>::setValue( val );
		validate();
	}

	const QStringList& options() const { return options_; }
};

#endif /* I3_SHOVEL_CHOICE_SETTING_H */
