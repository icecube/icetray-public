#include "SessionMenu.h"
#include "moc_SessionMenu.cpp"
#include <QVariant>
#include <QSettings>
#include <QDir>
#include <icetray/I3Logging.h>
#include "scripting/gil.h"
#include "shovelart/pybindings/shovelart.h"

SessionMenu::SessionMenu(QWidget* parent):
    QMenu(parent)
{
    insertStandardSessions();

    // load settings
    QSettings settings;
    int size = settings.beginReadArray( "session/recent" );
    for (int i = size - 1; i >= 0; --i) {
        settings.setArrayIndex(i);
        QString file = settings.value( "file" ).toString();
        addToRecentList(file);
    }
    settings.endArray();
}

SessionMenu::~SessionMenu()
{
    log_debug( "Saving settings of Session Menu" );
    QSettings settings;
    QList<QAction*> r = recent();
    settings.beginWriteArray( "session/recent" );
    for (int i = 0; i < r.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue( "file", r[i]->data().toString() );
    }
    settings.endArray();
}

void
SessionMenu::insertStandardSessions()
{
    QMenu* menu = addMenu("Standard sessions");
    menuStandard = menu->menuAction();

    QString fileName;
    {
        using namespace boost::python;
        scripting::ScopedGIL gil;
        object sessions = import("icecube.steamshovel.sessions");
        fileName = extract<QString>(sessions.attr("__file__"));
    }

    QDir session_dir(fileName.left(fileName.lastIndexOf("/")));
    for (int i = 0, n = session_dir.count(); i < n; ++i) {
        QString fileName = session_dir[i];
        if (!fileName.endsWith(".py") || fileName.endsWith("__init__.py"))
            continue;

        QString name = fileName.left(fileName.lastIndexOf('.'));
        QAction* a = new QAction(name, this);
        a->setData(session_dir.path() + "/" + fileName);
        menu->addAction(a);

#if QT_VERSION >= 0x050000
        log_debug("Adding to standard sessions: %s, %s",
                  name.toLatin1().data(),
                  fileName.toLatin1().data());
#else
        log_debug("Adding to standard sessions: %s, %s",
                  name.toAscii().data(),
                  fileName.toAscii().data());
#endif
    }
}

void
SessionMenu::addToRecentList(const QString& file)
{
    // adds session on top of menu
    QList<QAction*> r = recent();
    QAction* top;
    if (r.isEmpty()) {
        top = insertSeparator(actions()[0]);
    } else {
        top = r[0];
    }

    // if entry already exists and not on top, float to top
    for (int i = 0; i < r.size(); ++i) {
        if (r[i]->data().toString() == file) {
            if (i == 0) // already on top
                return;
            QAction* a = r[i];
            removeAction(a);
            insertAction(top, a);
            return;
        }
    }

    // add new recent session, limit entries to 10
    if (r.size() == 10) {
        QAction* a = r.back();
        removeAction(a);
        delete a;
    }

    QString name = file.section('/', -1);
    name = name.left(name.lastIndexOf('.'));

#if QT_VERSION >= 0x050000
    log_debug("Adding to recent sessions: %s, %s",
              name.toLatin1().data(), file.toLatin1().data());
#else
    log_debug("Adding to recent sessions: %s, %s",
              name.toAscii().data(), file.toAscii().data());
#endif

    QAction* a = new QAction(name, this);
    a->setData(file);
    insertAction(top, a);
}

QList<QAction*>
SessionMenu::recent()
    const
{
    QList<QAction*> a = actions();
    QList<QAction*> recent;
    for (int i = 0;
         i < a.size() &&
         !a[i]->isSeparator() &&
         a[i] != menuStandard;
         ++i) {
        recent.push_back(a[i]);
    }
    return recent;
}
