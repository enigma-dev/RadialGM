#include "ArtManager.h"

#include <QDirIterator>

QHash<QString, QIcon> ArtManager::icons;
QBrush ArtManager::transparenyBrush;

void ArtManager::Init() {
  QDirIterator it(":/resources", QDirIterator::Subdirectories);
  while (it.hasNext()) {
    QString path = it.next();
    QString name = path.mid(path.lastIndexOf("/") + 1, path.lastIndexOf(".") - 1 - path.lastIndexOf("/"));
    icons[name] = QIcon(path);
  }

  transparenyBrush = QBrush(Qt::black, QPixmap(":/transparent.png"));
}

ArtManager::ArtManager() {}

const QIcon& ArtManager::get_icon(const QString& name) { return icons[name]; }

const QBrush& ArtManager::get_transpareny_brush() { return transparenyBrush; }
