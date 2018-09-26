#ifndef I3_SHOVEL_SESSION_MENU_H
#define I3_SHOVEL_SESSION_MENU_H

#include <QMenu>
#include <QList>
class QWidget;
class QAction;

class SessionMenu: public QMenu {
    Q_OBJECT;

public:
    SessionMenu(QWidget* parent = 0);
    ~SessionMenu();

    void addToRecentList(const QString& file);

private:
    void insertStandardSessions();

    QAction* menuStandard;
    QList<QAction*> recent() const;
};

#endif
