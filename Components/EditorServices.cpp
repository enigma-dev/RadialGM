#include "EditorServices.h"

#include "Models/ProtoModel.h"

#include <QDesktopServices>
#include <QUrl>

EditorServices::EditorServices()
{

}

//NOTE: if the indexes are added to a list for tracking
//they should be held as QPersistentModelIndex so that
//they will still refer to the same resource later
//we may want to create special AssetIndex types for this
void EditorServices::OpenExternally(const QModelIndex &index) {
  QDesktopServices::openUrl(QUrl::fromLocalFile(index.data().toString()));
}

void EditorServices::EditExternally(const QModelIndex &index) {
  QDesktopServices::openUrl(QUrl::fromLocalFile(index.data().toString()));
}
