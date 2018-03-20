#ifndef ARTMANAGER_H
#define ARTMANAGER_H

#include <QHash>
#include <QIcon>
#include <QBrush>

class ArtManager
{
public:
    static void Init();
    static const QIcon& get_icon(const QString& name);
    static const QBrush& get_transpareny_brush();

private:
    ArtManager();
    static QHash<QString, QIcon> icons;
    static QBrush transparenyBrush;
};

#endif // ICONMANAGER_H
