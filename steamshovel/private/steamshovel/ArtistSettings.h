#ifndef I3_SHOVEL_ARTIST_SETTINGS_WIDGETS_H
#define I3_SHOVEL_ARTIST_SETTINGS_WIDGETS_H

#include <QComboBox>
#include <QFontComboBox>
#include <QPushButton>
#include <QWidget>
#include <QColor>
#include <QString>
#include <QStringList>
#include <QColorDialog>
#include <QFileDialog>
#include <vector>
#include <string>

class Scenario;
class QSlider;
class QDoubleSpinBox;

#include "shovelart/ColorVariant.h"
Q_DECLARE_METATYPE( ColorMapPtr );
#include "shovelart/OMKeySet.h"
Q_DECLARE_METATYPE( OMKeySet );
#include "shovelart/RangeSetting.h"
Q_DECLARE_METATYPE( RangeSetting );
#include "shovelart/ChoiceSetting.h"
Q_DECLARE_METATYPE( ChoiceSetting );
#include "shovelart/KeySetting.h"
Q_DECLARE_METATYPE( KeySetting );
#include "shovelart/FileSetting.h"
Q_DECLARE_METATYPE( FileSetting );

#include "shovelart/ArtistFwd.h"

#include "ShovelSlider.h"

// Struct to pass data around in the ScenarioModel
struct ArtistKeyData {
	QStringList keys;
	QString val;

	ArtistKeyData() {} // needed for QVariant 
	ArtistKeyData(const ArtistKeyData& other):
		keys(other.keys), val(other.val) {}

	ArtistKeyData& operator=(const ArtistKeyData & other){
		keys = other.keys;
		val = other.val;
		return *this;
	}

	ArtistKeyData( const std::vector<std::string>& kv,
	               const std::string& v )
    {
    	val = v.c_str();
    	keys.reserve( kv.size() );
    	BOOST_FOREACH( const std::string& k, kv ){
    		keys.push_back( k.c_str() );
    	}
	}
};
Q_DECLARE_METATYPE( ArtistKeyData );

#include "SteamshovelApp.h"
#include "FileService.h"

Qt::ItemFlags setting_QItemFlags( ArtistPtr artist, const std::string& setting_name );
QVariant setting_QItemData( Scenario*, ArtistPtr artist, const std::string& setting_name, int role );
bool setting_QItemSetData( Scenario*, ArtistPtr artist, const std::string& setting_name,
                           const QVariant& value, int role );

class ComboBoxEditor : public QComboBox{
	Q_OBJECT;
	Q_PROPERTY( QString value READ currentText USER true );

public:
	ComboBoxEditor( QWidget* parent, const QStringList& items, const QString& val ) :
		QComboBox(parent)
	{
		setFocusPolicy( Qt::StrongFocus );

		addItems(items);
		int idx = items.indexOf( val );
		if( idx == -1 ){
			// val not found in items
			if( val.length() ){
				// val is not empty, add at the end
				addItem( val );
				idx = count()-1;
			}
		}
		if( idx >= 0 )
			setCurrentIndex( idx );
	}
};

class ArtistKeyDataEditor : public ComboBoxEditor {
	Q_OBJECT;
public:
	ArtistKeyDataEditor( QWidget* parent, ArtistKeyData l ) :
		ComboBoxEditor(parent, l.keys, l.val)
	{}
};

class KeySettingEditor : public ComboBoxEditor {
	Q_OBJECT;
public:
	KeySettingEditor( QWidget* parent, KeySetting s ) :
		ComboBoxEditor(parent, s.validKeys(
		    SteamshovelApp::instance()->files().currentFrame()
	    ), s)
	{
		// blank string, meaning no selection; put it at the front of the list
		insertItem(0, QString());
	}
};

class ChoiceSettingEditor : public ComboBoxEditor {
	Q_OBJECT;
	Q_PROPERTY( int value READ currentIndex USER true );

public:
	ChoiceSettingEditor( QWidget* parent, ChoiceSetting s ) :
		ComboBoxEditor(parent, s.options(), s.options()[s])
	{}
};

class FontEditor : public QFontComboBox{
	Q_OBJECT;
	Q_PROPERTY( QFont font READ currentFont USER true );

public:
	FontEditor( QWidget* parent, QFont current ) :
		QFontComboBox( parent )
	{
		setFocusPolicy( Qt::StrongFocus );
		setCurrentFont( current );
	}
};

#if 1
	class FileSettingEditor : public QFileDialog{
		Q_OBJECT;
		Q_PROPERTY( QString value READ getFileName USER true );
		QString filename_;

	protected Q_SLOTS:
		void update(QString s) { filename_ = s; }

	public:
		FileSettingEditor( QWidget* parent, FileSetting filename ) :
			QFileDialog( parent )
		{

			log_trace_stream(filename_.toStdString().c_str());
			connect( this, SIGNAL(currentChanged(QString)), 
			         SLOT(update(QString)) );
		}

		QString getFileName() const { return filename_; }
	};

#else
	class FileSettingEditor : public QFileDialWidget{
		Q_OBJECT;
		Q_PROPERTY( QString value READ getFileName USER true );
		QString filename_;

	public:
		FileSettingEditor( QWidget* parent, FileSetting filename ) :
			QWidget( parent )
		{
			filename_ = QFileDialog::getOpenFileName();
			log_trace_stream(filename_.toStdString().c_str());
			Q_EMIT newFile();
			// connect( this, SIGNAL(clicked()), 
			//          SLOT(showDialog()) );
		}

		QString getFileName() const { return filename_; }

	Q_SIGNALS:
		void newFile();
	};
#endif

class ColorEditor : public QColorDialog{
	Q_OBJECT;
	Q_PROPERTY( QColor color READ currentColor USER true );

public:
	ColorEditor( QWidget* parent, QColor color ) :
		QColorDialog( color, parent )
	{
		setFocusPolicy( Qt::StrongFocus );
		setOption( QColorDialog::ShowAlphaChannel, true );
	}
};

class RangeSettingEditor : public ShovelSlider{
	Q_OBJECT;
	Q_PROPERTY( double value READ value USER true );

public:
	RangeSettingEditor( QWidget* parent, RangeSetting s );
};

class ColorMapEditor : public QComboBox{
	Q_OBJECT;
	Q_PROPERTY( ColorMapPtr colormap READ getColorMapValue USER true );

public:
	ColorMapEditor( QWidget* parent, ColorMapPtr current );

	ColorMapPtr getColorMapValue() const {
		return itemData(currentIndex()).value<ColorMapPtr>();
	}

protected Q_SLOTS:
	void handleIndexChanged( int );

Q_SIGNALS:
	void mapChanged();
};

#endif /* I3_SHOVEL_ARTIST_SETTINGS_WIDGETS_H */
