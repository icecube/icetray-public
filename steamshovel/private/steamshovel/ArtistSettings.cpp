#include "ArtistSettings.h"
#include "moc_ArtistSettings.cpp"

#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"
#include "shovelart/ColorVariant.h"

#include <QBrush>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QColorDialog>
#include <QMetaType>
#include <QSizePolicy>

#include <typeinfo>

Qt::ItemFlags setting_QItemFlags( ArtistPtr artist, const std::string& name ){
	const Artist::Setting& setting = artist->settings()[ name ];
	if( setting.is<bool>() ){
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
	}
	if( setting.is<OMKeySet>() ){
		return Qt::ItemIsEnabled; // no interaction
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant setting_QItemData( Scenario* scenario, ArtistPtr artist, const std::string& name, int role ){
	const Artist::Setting& setting = artist->settings()[ name ];

	if( setting.is<bool>() ){
		if( role == Qt::CheckStateRole ){
			return setting.value<bool>() ? Qt::Checked : Qt::Unchecked;
		}
		return QVariant();
	}

	else if( setting.is<int>() ){
		switch( role ){
			case Qt::DisplayRole:
				return QString::number( setting.value<int>() );
			case Qt::EditRole:
				return setting.value<int>();
			default: return QVariant();
		}
	}

	else if( setting.is<RangeSetting>()){
		switch( role ){
			case Qt::DisplayRole:
				return QString::number( setting.value<RangeSetting>() );
			case Qt::EditRole:
				return QVariant::fromValue( setting.value<RangeSetting>() );
			default: return QVariant();
		}
	}

	else if( setting.is<ChoiceSetting>() ){
		switch( role ){
			case Qt::DisplayRole:{
				const ChoiceSetting& s = setting.value<ChoiceSetting>();
				return s.options()[s];
			}
			case Qt::EditRole:
				return QVariant::fromValue( setting.value<ChoiceSetting>() );
			default: return QVariant();
		}
	}

	else if( setting.is<FileSetting>() ){
		switch( role ){
			case Qt::DisplayRole:{
				QString s = setting.valueFromComplex<FileSetting>();
				return s.isEmpty() ? "No file selected" : s;
			}
			case Qt::EditRole:
				return QVariant::fromValue( setting.value<FileSetting>() );
			default: return QVariant();
		}
	}

	else if( setting.is<KeySetting>() ){
		const QString& val = setting.valueFromComplex<KeySetting>();
		switch( role ){
			case Qt::DisplayRole:
				return val.isEmpty() ? "Click to select..." : val;
			case Qt::EditRole:
				return QVariant::fromValue( setting.value<KeySetting>() );
			case Qt::FontRole:
				if ( val.isEmpty() ) {
					QFont f;
					f.setItalic(true);
					return f;
				}
			default: return QVariant();
		}
	}

	else if( setting.is<QFont>() ){
		switch( role ){
			case Qt::DisplayRole:
				return setting.value<QFont>().family();
			case Qt::FontRole:
			case Qt::EditRole:
				return setting.value<QFont>();
			default: return QVariant();
		}
	}

	else if( setting.is<QColor>() ){
		switch( role ){
			case Qt::DisplayRole:
				return setting.value<QColor>().name();
			case Qt::DecorationRole:
			case Qt::EditRole:
				return setting.value<QColor>();
			// case Qt::BackgroundRole:
			// 	return QBrush( QColor::fromRgbF(.15,.15,.15) );
			default: return QVariant();
		}
	}

	else if( setting.is<QString>() ){
		switch( role ){
			case Qt::DisplayRole:
			case Qt::EditRole:
				return setting.value<QString>();
			default: return QVariant();
		}
	}

	else if( setting.is<ColorMapPtr>() ){
		QSize iconsize(64, 18);
		ColorMapPtr map = setting.value<ColorMapPtr>();
		switch( role ){
			case Qt::DecorationRole:
				return QIcon( map->image( iconsize ) );
			case Qt::DisplayRole:
				return QVariant();
				//return QString::fromStdString(ColorMap::toString(map));
			case Qt::EditRole:
				return QVariant::fromValue( map );	
			default: return QVariant();
		}
	}

	else if( setting.is<OMKeySet>() ){
		switch( role ){
			case Qt::DisplayRole:
				return QString("Hint: Select DOMs in GL Widget");
			case Qt::FontRole: {
				QFont font;
				font.setItalic(true);
				return font;
			}
			default: return QVariant();
		}
	}

	// unknown type, gracefully display error message
	if( role == Qt::DisplayRole )
		return QString("Unknown setting type");

	return QVariant();
}


bool setting_QItemSetData( Scenario* scenario, ArtistPtr artist, const std::string& name,
                           const QVariant& value, int role )
{
	const Artist::Setting& setting = artist->settings()[ name ];
	if( setting.is<bool>() && role == Qt::CheckStateRole ){
		scenario->changeSetting<bool>( artist, name, value.value<int>() );
	}
	else if( setting.is<int>() && role == Qt::EditRole ){
		scenario->changeSetting<int>( artist, name, value.value<int>() );
	}
	else if( setting.is<QFont>() && role == Qt::EditRole ){
		scenario->changeSetting<QFont>( artist, name, value.value<QFont>() );
	}
	else if( setting.is<QColor>() && role == Qt::EditRole ){
		scenario->changeSetting<QColor>( artist, name, value.value<QColor>() );
	}
	else if( setting.is<QString>() && role == Qt::EditRole ){
		scenario->changeSetting<QString>( artist, name, value.toString() );
	}
	else if( setting.is<ColorMapPtr>() && role == Qt::EditRole ){
		scenario->changeSetting<ColorMapPtr>( artist, name, value.value<ColorMapPtr>() );
	}
	else if( setting.is<FileSetting>() && role == Qt::EditRole ){
		scenario->changeSetting<FileSetting>( artist, name, value.toString() );
	}
	else if( setting.is<RangeSetting>() && role == Qt::EditRole ){
		scenario->changeComplexSetting<RangeSetting>( artist, name, value.value<float>() );
	}
	else if( setting.is<ChoiceSetting>() && role == Qt::EditRole ){
		scenario->changeComplexSetting<ChoiceSetting>( artist, name, value.toInt() );
	}
	else if( setting.is<KeySetting>() && role == Qt::EditRole ){
		scenario->changeComplexSetting<KeySetting>( artist, name, value.toString() );
	}
	return false;
}

RangeSettingEditor::RangeSettingEditor( QWidget* parent, RangeSetting s ) :
	ShovelSlider( parent )
{
	// setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	setContentsMargins(0, 0, 0, 0);
	const double step = double(s.vmax_ - s.vmin_) / s.nmax_;
	const int decimals = std::max( int(std::ceil(-std::log10(step))), 0 );
	setRange( s.vmin_, s.vmax_, s.nmax_, decimals );
	setValue( float(s) );
}

ColorMapEditor::ColorMapEditor( QWidget* parent, ColorMapPtr map ):
	QComboBox(parent)
{
	setIconSize(QSize(64,18));
	int idx = 0, set_idx = 0;
	const ColorMap::mapList mlist = ColorMap::availableNativeMaps();
	for( ColorMap::mapList::const_iterator i = mlist.begin(); i!=mlist.end(); ++i, ++idx ){
		ColorMapPtr thisMap = i->map;
		ColorMap& thisMapRef = *thisMap.get();
		ColorMap& mapRef = *map.get();
#if __cplusplus < 201103L
		if( typeid( thisMapRef ) == typeid( mapRef ) ){
#else
		if( typeid( thisMapRef ).hash_code() == typeid( mapRef ).hash_code() ){
#endif
			thisMap = map;
			set_idx = idx;
		}
		addItem( QIcon(thisMap->image(iconSize())),
		         i->name.c_str(),
		         QVariant::fromValue(thisMap) );
	}

	const ColorMap::mapList& plist = ColorMap::availablePythonMaps();
	for( ColorMap::mapList::const_iterator i = plist.begin(); i!=plist.end(); ++i, ++idx ){
		ColorMapPtr thisMap = i->map;
		if( thisMap == map ){
			set_idx = idx;
		}
		addItem( QIcon(thisMap->image(iconSize())),
		         i->name.c_str(),
		         QVariant::fromValue(thisMap) );
	}
	setCurrentIndex(set_idx);

	QObject::connect( this, SIGNAL( currentIndexChanged(int) ), this, SLOT( handleIndexChanged(int) ) );
}

void ColorMapEditor::handleIndexChanged( int idx )
{
	ColorMapPtr map = itemData(idx).value<ColorMapPtr>();
	ColorMap& mapRef = *map.get();
#if __cplusplus < 201103L
	if( typeid( mapRef ) == typeid( ConstantColorMap ) ){
#else
	if( typeid( mapRef ).hash_code() == typeid( ConstantColorMap ).hash_code() ){
#endif
		ConstantColorMap* ccm = dynamic_cast<ConstantColorMap*>(map.get());
		QColor c = QColorDialog::getColor( ccm->value(0), this );
		if( c.isValid() ){
			ccm->setColor(c);
		}
	}
	Q_EMIT mapChanged();
}
