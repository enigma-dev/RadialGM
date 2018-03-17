#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QHash>
#include <QIcon>

class IconManager
{
public:
    static void Init();
    static const QIcon& get_icon(const QString& name);

private:
    IconManager();
    static QHash<QString, QIcon> icons;
};

#endif // ICONMANAGER_H
