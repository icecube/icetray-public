#include "ShellWidget.h"
#if QT_VERSION >= 0x050000
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QVBoxLayout>
#endif

ShellWidget::ShellWidget( QWidget* parent ) :
    QWidget( parent )
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    // Python code later adds a RichWidget from IPython as a child 
}
