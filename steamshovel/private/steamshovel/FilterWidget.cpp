#include "FilterWidget.h"
#include "moc_FilterWidget.cpp"

#include "SteamshovelApp.h"
#include "FileService.h"

#include "scripting/gil.h"

#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QMetaObject>

#include <boost/python.hpp>
#include <boost/python/handle.hpp>

#include <sstream>
#include <cassert>

FilterWidget::FilterWidget( QWidget* parent ):
    QWidget( parent),
    header_( new QLabel("def filter(index, frame):", this) ),
    edit_( new QTextEdit(this) ),
    message_( new QLabel(this) ),
    apply_( new QPushButton("Apply", this) )
{
    header_->setIndent( -5 );

    QHBoxLayout* headerLayout = new QHBoxLayout;
    headerLayout->addWidget( header_ );
    headerLayout->addStretch( 1 );
    headerLayout->addWidget( apply_ );

    QVBoxLayout* verticalLayout = new QVBoxLayout( this );
    verticalLayout->setContentsMargins( 2, 2, 2, 2 );
    verticalLayout->addLayout( headerLayout );
    verticalLayout->addWidget( edit_ );
    verticalLayout->addWidget( message_ );

    connect( edit_, SIGNAL(textChanged()),
             this, SLOT(processText()) );
    connect( apply_, SIGNAL(clicked()),
             this, SLOT(emitFilter()) );
}

void
FilterWidget::showEvent( QShowEvent* event ){
    processText();
}

void
FilterWidget::emitFilter(){
    Q_EMIT sendFilter( filter_to_send_ );
}

QString
FilterWidget::getCode()const{
    return edit_->toPlainText();
}

void
FilterWidget::setCode(QString code){
    edit_->setPlainText( code );
    processText();
    emitFilter();
}

void
FilterWidget::processText(){
    using namespace boost::python;
    using boost::python::handle;

    filter_to_send_ = object();
    QString code = getCode();

    if(code.isEmpty()){
        message_->setStyleSheet( "QLabel { font: italic }" );
        message_->setText( "No filter code, write some!" );
        return;
    }

    // auto-indent body
    code.prepend( header_->text() + "\n" );
    code.replace( "\n", "\n  " );
#if QT_VERSION >= 0x050000
    log_debug_stream( "code to be tested:\n"
                      << code.toLatin1().constData() << "\n" );
#else
    log_debug_stream( "code to be tested:\n"
                      << code.toAscii().constData() << "\n" );
#endif

    scripting::ScopedGIL gil;
    try {
        // compile code, see if that raises anything
        object main = import( "__main__" );
        object global( main.attr( "__dict__" ) );
        // make I3Frame available in function scope
        object icetray = import( "icecube.icetray" );
        global["I3Frame"] = icetray.attr( "I3Frame" );
        object dataclasses = import( "icecube.dataclasses" );
        global["I3Particle"] = dataclasses.attr( "I3Particle" );
#if QT_VERSION >= 0x050000
        exec( code.toLatin1().constData(), global, global );
#else
        exec( code.toAscii().constData(), global, global );
#endif
        filter_to_send_ = global["filter"];
    }
    catch (error_already_set& e) {
        // show Python exception in footer
        PyObject *exc_ptr, *val_ptr, *tb_ptr;
        PyErr_Fetch( &exc_ptr, &val_ptr, &tb_ptr );
        assert( exc_ptr != NULL );

        handle<> exc_h( exc_ptr );
        handle<> val_h( allow_null(val_ptr) );
        object exc( exc_h );
        object val( val_h );

        object s = exc.attr("__name__") + "\n";
        // exception format is different for exec: val is tuple
        s += PyTuple_Check( val_ptr ) ? str( val[0] ) : str( val );

        const char* c_s = extract<const char*>( s );
        message_->setStyleSheet( "QLabel { color: red; }" );
        message_->setText( c_s );
        message_->show();
    }

    // good, code compiles, now apply to current frame
    processFrame();
}

void
FilterWidget::processFrame(){
    using namespace boost::python;
    using boost::python::handle;

    if( !isVisible() )
        return;

    I3FramePtr frame = SteamshovelApp::instance()->files().currentFrame();
    unsigned frame_index = SteamshovelApp::instance()->files().currentIndex();
    if( !frame || !filter_to_send_ )
        return;

    scripting::ScopedGIL gil;
    try{
        bool result = extract<bool>( filter_to_send_( frame_index, frame ) );
        log_debug_stream( "filter applied on current frame: " << result );
        if( result ){
            message_->setStyleSheet( "QLabel { color: green; }" );
            message_->setText( "True [on current frame]" );
        }else{
            message_->setStyleSheet( "QLabel { color: orange; }" );
            message_->setText( "False [on current frame]" );
        }
    }
    catch( error_already_set& ){
        // show Python exception in footer
        PyObject *exc_ptr, *val_ptr, *tb_ptr;
        PyErr_Fetch( &exc_ptr, &val_ptr, &tb_ptr );
        assert( exc_ptr != NULL );

        handle<> exc_h( exc_ptr );
        handle<> val_h( allow_null(val_ptr) );
        object exc( exc_h );
        object val( val_h );

        object s = exc.attr("__name__") + "\n";
        // exception format is different for exec: val is tuple
        s += PyTuple_Check( val_ptr ) ? str( val[0] ) : str( val );

        const char* c_s = extract<const char*>( s );
        message_->setStyleSheet( "QLabel { color: red; }" );
        message_->setText( c_s );
        message_->show();
    }
}
