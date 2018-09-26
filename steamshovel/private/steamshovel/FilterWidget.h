#ifndef I3_SHOVEL_FILTERWIDGET_H
#define I3_SHOVEL_FILTERWIDGET_H

#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>

#include <boost/python/object.hpp>

#include <QWidget>
#include <QString>
#include <QTimer>

class QLabel;
class QTextEdit;
class QPushButton;
class QShowEvent;

class FilterWidget: public QWidget {
    friend class ShovelMainWindow;

    Q_OBJECT;
    Q_PROPERTY(QString code READ getCode WRITE setCode);

    SET_LOGGER("FilterWidget");

    QLabel* header_;
    QTextEdit* edit_;
    QLabel* message_;
    QPushButton* apply_;

    boost::python::object filter_to_send_;

protected:
    void showEvent( QShowEvent* );

protected Q_SLOTS:
    void emitFilter();
    void processText();
    void processFrame();

public:
    FilterWidget(QWidget* parent = NULL);

    QString getCode() const;
    void setCode(QString);

Q_SIGNALS:
    void sendFilter(boost::python::object);
};

#endif
