#ifndef ARTMANAGER_H
#define ARTMANAGER_H

#include <QBrush>
#include <QHash>
#include <QIcon>

class ArtManager {
 public:
  static void Init();
  static const QIcon& GetIcon(const QString& name);
  static const QBrush& GetTransparenyBrush();
  static bool GetCachedPixmap(const QString& name, QPixmap &pm);
  static void clearCache();

 private:
  ArtManager();
  static QHash<QString, QIcon> icons;
  static QBrush transparenyBrush;
};

#endif  // ICONMANAGER_H
