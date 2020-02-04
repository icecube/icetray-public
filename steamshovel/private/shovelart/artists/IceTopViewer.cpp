#include "IceTopViewer.h"
#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "scripting/qpixmap_from_python.h"
#include "shovelart/timewindow.h"
#include "shovelart/ColorVariant.h"
#include "shovelart/pybindings/shovelart.h"

#include <boost/python.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>

#include <QPixmap>
#include <QWidget>
#include <QPainter>
#include <QScrollArea>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QRubberBand>

#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <icetray/name_of.h>
#include <dataclasses/physics/I3RecoPulse.h>

#include <stdexcept>

namespace {

    // assumes GIL is locked
    boost::python::object pySettings(const Artist& artist){
        using namespace boost::python;
        dict py_settings;
        BOOST_FOREACH( const Artist::SettingsMap::Item& item, artist.settings() )
        py_settings[ item.name ] =
            scripting::shovelart::settingValueToPython( artist,  item.name );
        return std::move(py_settings);
    }

}

class IceTopViewerWidget: public QWidget
{
    boost::python::object icetop_canvas_;
    QRubberBand* rubberBand_;
    QPoint mouseOrigin_;

    void mousePressEvent( QMouseEvent* ev ){
        mouseOrigin_ = ev->pos();
        if( ev->button() == Qt::LeftButton ){
            rubberBand_->setGeometry(QRect(mouseOrigin_, QSize()));
            rubberBand_->show();
        }
    }

    void mouseMoveEvent( QMouseEvent* ev ){
        rubberBand_->setGeometry(
            QRect(mouseOrigin_, ev->pos()).normalized()
        );
    }

    void mouseReleaseEvent( QMouseEvent *ev ){
        rubberBand_->hide();
        if( rubberBand_->size().width() > 3 && 
            rubberBand_->size().height() > 3 ){
            QRect box = rubberBand_->geometry();
            const int xdisp = box.left();
            const int ydisp = size().height() - box.bottom();
            pyUpdate( "zoom_event", xdisp, ydisp,
                      box.width(), box.height() );
        }else if( ev->button() == Qt::LeftButton ){
            // interpret as normal click
            const int xdisp = mouseOrigin_.x();
            const int ydisp = size().height() - mouseOrigin_.y();
            if( ev->button() == Qt::LeftButton ){
                pyUpdate( "button_press_event", xdisp, ydisp, 1 );
            }
        }
    }

    void wheelEvent( QWheelEvent* ev ){
        const int xdisp = ev->x();
        const int ydisp = size().height() - ev->y();
        const int steps = ev->delta() / 120; // from matplotlib's backend_qt4.py
        pyUpdate( "scroll_event", xdisp, ydisp, steps );
        QWidget::wheelEvent( ev );
    }

    void resizeEvent( QResizeEvent* ev ){
        pyUpdate( "resize_event", ev->size().width(),
                                  ev->size().height() );
        QWidget::resizeEvent( ev );
    }

    void paintEvent( QPaintEvent* ev ){
        using namespace boost::python;
        scripting::ScopedGIL gil;
        object obj = icetop_canvas_.attr("print_to_buffer")();
        PyObject* buf = object(obj[0]).ptr();
        object wh = obj[1];
        const int w = extract<int>( wh[0] );
        const int h = extract<int>( wh[1] );
        QPainter p( this );
        p.drawPixmap( 0, 0, w, h, scripting::qpixmap_from_python( buf, w, h ) );
    }

public:

    IceTopViewerWidget( boost::python::object py_settings ) :
        QWidget()
    {
        rubberBand_ = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand_->hide();
        setSizePolicy( QSizePolicy::Expanding,
                       QSizePolicy::Expanding );
        setBackgroundRole( QPalette::Dark );

        using namespace boost::python;
        scripting::ScopedGIL gil;
        object module = import(
            "icecube.steamshovel.util.icetop_widget"
        );
        icetop_canvas_ = module.attr("IceTopCanvas")(py_settings);
    }

    ~IceTopViewerWidget(){
    }

    template<typename T0>
    void pyUpdate( const char* func_name, T0 t0 ){
        using namespace boost::python;
        scripting::ScopedGIL gil;
        icetop_canvas_.attr( func_name )( t0 );
        update();
    }

    template<typename T0, typename T1>
    void pyUpdate( const char* func_name, T0 t0, T1 t1 ){
        using namespace boost::python;
        scripting::ScopedGIL gil;
        icetop_canvas_.attr( func_name )( t0, t1 );
        update();
    }

    template<typename T0, typename T1, typename T2>
    void pyUpdate( const char* func_name, T0 t0, T1 t1, T2 t2 ){
        using namespace boost::python;
        scripting::ScopedGIL gil;
        icetop_canvas_.attr( func_name )( t0, t1, t2 );
        update();
    }

    template<typename T0, typename T1, typename T2, typename T3>
    void pyUpdate( const char* func_name, T0 t0, T1 t1, T2 t2, T3 t3 ){
        using namespace boost::python;
        scripting::ScopedGIL gil;
        icetop_canvas_.attr( func_name )( t0, t1, t2, t3 );
        update();
    }
};


IceTopViewer::IceTopViewer(){
    std::vector<std::string> trace_types(2);
    trace_types[0] = "I3Map<OMKey, vector<I3DOMLaunch> >";
    trace_types[1] = "I3Map<OMKey, vector<I3Waveform> >";

    addSetting( "particle", KeySetting("I3Particle", "") );
    addSetting( "time trace", KeySetting(trace_types, "") );
    addSetting( "SLCs", KeySetting("I3Map<OMKey, vector<I3RecoPulse> >", "") );
    addSetting( "scint HLCs", KeySetting("I3Map<OMKey, vector<I3RecoPulse> >", "") );
    addSetting( "scint SLCs", KeySetting("I3Map<OMKey, vector<I3RecoPulse> >", "") );
    // addSetting( "show residuals", true );
    addSetting( "show no-hits", false );
    addSetting( "show FADC", false );
    addSetting( "show legends", true );
    addSetting( "log-radius", false );
    addSetting( "colormap", ColorMapPtr( new I3TimeColorMap() ) );
    addSetting( "scintillator colormap", ColorMapPtr( new I3TimeColorMap() ) );
    addSetting( "marker-size", RangeSetting(1.0, 15.0, 14, 8.0) );
    addSetting( "scale", RangeSetting(1.0, 30.0, 29, 15.0) );
    addSetting( "power", RangeSetting(0.0, 0.5, 50, 0.25) );
    addSetting( "2nd particle", KeySetting("I3Particle", "") );
    addSetting( "3rd particle", KeySetting("I3Particle", "") );
}

bool IceTopViewer::isValidKey( I3FramePtr frame, int key_idx, const std::string& key ){
    if( key_idx == 1 ){
        const std::string tname = frame->type_name( key );
        return tname == "I3RecoPulseSeriesMap" ||
               tname == "I3RecoPulseSeriesMapMask" ||
               tname == "I3Map<OMKey, vector<I3RecoPulse> >" ||
               tname == I3::name_of<I3RecoPulseSeriesMap>();
    }
    return Artist::isValidKey( frame, key_idx, key );
}

QWidget* IceTopViewer::create() const{
    scripting::ScopedGIL gil;
    using namespace boost::python;
    object m = import("matplotlib");
    if (extract<str>(m.attr("__version__"))() < str("1.4"))
        throw std::runtime_error("matplotlib version < 1.4 is too old");
    return new IceTopViewerWidget(pySettings(*this));
}

void IceTopViewer::updateFrame( I3FramePtr frame ){
    using namespace boost::python;
    scripting::ScopedGIL gil;

    list py_keys;
    BOOST_FOREACH( const std::string& key, keys() )
        py_keys.append( key );

    widget<IceTopViewerWidget>().pyUpdate(
        "update_frame", frame, py_keys, pySettings(*this)
    );
}

void IceTopViewer::updateColors( const TimeWindow& tw ){
    widget<IceTopViewerWidget>().pyUpdate(
        "update_colors", tw.colorStart(), tw.colorEnd()
    );
}
