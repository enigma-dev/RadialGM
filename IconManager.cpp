#include "IconManager.h"

#include <QDirIterator>

QHash<QString, QIcon> IconManager::icons;

void IconManager::Init() {
    QDirIterator it(":/resources", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QString name = path.mid(path.lastIndexOf("/")+1, path.lastIndexOf(".")-1 - path.lastIndexOf("/"));
        icons[name] = QIcon(path);
    }
}

IconManager::IconManager() {
}

const QIcon& IconManager::get_icon(const QString& name) {
    return icons[name];
}
