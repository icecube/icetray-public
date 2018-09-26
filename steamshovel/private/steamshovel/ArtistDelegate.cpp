#include <icetray/I3Logging.h>

#include "ArtistSettings.h"
#include "ArtistDelegate.h"
#include "moc_ArtistDelegate.cpp"
#include "shovelart/BindSignal.h"
#include "shovelart/Artist.h"

ArtistDelegate::ArtistDelegate( QObject* parent ) :
	QStyledItemDelegate( parent )
{}

QWidget* ArtistDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
	QVariant data = index.data(Qt::EditRole);
	QWidget* w = NULL;
	// arrange for this->commitData to be emitted whenever a widget changes value
	// so that changes are reflected in the scenario immediately.
	// In the binds below, g++ insists on having a non-const type, though clang does not
	ArtistDelegate* ncthis = const_cast<ArtistDelegate*>(this);
	if( data.type() == QVariant::Int ){
		// borrow the default implementation's spinner widget
		w = QStyledItemDelegate::createEditor( parent, option, index );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( valueChanged(int)), bs, SLOT( call() ) );
	}
	else if( data.type() == QVariant::String ){
		return QStyledItemDelegate::createEditor( parent, option, index );
	}
	else if( data.canConvert<ArtistKeyData>() ){
		const ArtistKeyData l = data.value<ArtistKeyData>();
		w = new ArtistKeyDataEditor( parent, l );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentIndexChanged(int) ), bs, SLOT( call() ) );
	}
	else if( data.canConvert<KeySetting>() ){
		const KeySetting s = data.value<KeySetting>();
		w = new KeySettingEditor( parent, s );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentIndexChanged(int) ), bs, SLOT( call() ) );
	}
	else if( data.canConvert<ChoiceSetting>() ){
		const ChoiceSetting s = data.value<ChoiceSetting>();
		w = new ChoiceSettingEditor( parent, s );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentIndexChanged(int) ), bs, SLOT( call() ) );
	}
	else if( data.canConvert<RangeSetting>() ){
		const RangeSetting s = data.value<RangeSetting>();
		w = new RangeSettingEditor( parent, s );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( valueChanged(double) ), bs, SLOT( call() ) );
	}
	else if( data.canConvert<FileSetting>() ){
		const FileSetting s = data.value<FileSetting>();
		w = new FileSettingEditor( parent, s );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentChanged(QString) ), bs, SLOT( call() ) );
	}
	else if( data.type() == QVariant::Font ){
		w = new FontEditor( parent, data.value<QFont>() );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentFontChanged(QFont)), bs, SLOT(call()) );
	}
	else if( data.type() == QVariant::Color ){
		w = new ColorEditor( parent, data.value<QColor>() );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( currentColorChanged(QColor)), bs, SLOT(call()) );
	}
	else if( data.canConvert<ColorMapPtr>() ){
		w = new ColorMapEditor( parent, data.value<ColorMapPtr>() );
		BindSignal* bs = new BindSignal( boost::bind( &ArtistDelegate::commitData, ncthis, w ), w );
		QObject::connect( w, SIGNAL( mapChanged()), bs, SLOT( call() ) );
	}
	else{
		log_warn( "No customized editor for this setting type" );
		w = QStyledItemDelegate::createEditor( parent, option, index );
	}
	return w;

}


void ArtistDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const {
	QStyleOptionViewItem new_option = option;
	if( dynamic_cast< ColorMapEditor* >( editor ) ){
		// For color maps, which create a combo box editor, move the combo box to the left side
		// of the delegate region, directly over the colored label (instead of to its right).
		// we could set the width using the sizeHint, but this seems tidier: it puts the editor
		// widget (fairly) neatly over the colored label.  We exploit the magic number 128,
		// which is the expected width of color icons for these color maps.
		new_option.rect.setWidth( 132 );
		new_option.rect.setX( option.rect.x()-128 );
	} else
	if( dynamic_cast< RangeSettingEditor* >( editor ) ){
		// For the RangeSettingEditor, we need to make extra space for the QSpinBox,
		// which does not fit into the standard space in case of some widged styles (Mac!)
		new_option.rect.adjust( 0, -20, 0, 20 );
	} else
	if( dynamic_cast< ComboBoxEditor* >( editor ) ){
		new_option.rect.adjust( 0, -5, 0, 5 );
		if( new_option.rect.width() < 200 )
			new_option.rect.setWidth( 200 );
	}
	QStyledItemDelegate::updateEditorGeometry( editor, new_option, index );
};
