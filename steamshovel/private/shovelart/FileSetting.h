#ifndef I3_SHOVEL_RENDER_FILE_SETTING_H
#define I3_SHOVEL_RENDER_FILE_SETTING_H

#include "ComplexSetting.h"
#include <QString>

// Type used as an Artist setting representing a filename.
class FileSetting: public ComplexSetting<QString> {
    QString filter_;
public:
    typedef QString Value;

    FileSetting(QString val, QString filter = QString()):
        ComplexSetting<QString>( val ),
        filter_(filter)
    {}

    // Default constructor is needed by QVariant
    FileSetting():
        ComplexSetting<QString>( QString() )
    {}

    QString filter() const { return filter_; }
};

#endif
