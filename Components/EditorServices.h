#ifndef EDITORSERVICES_H
#define EDITORSERVICES_H

#include <QModelIndex>

// TODO: fundies
// this class is LIKE QDesktopServices but for RGM instead
// maybe even subclass it and override url handler too to point to egm
// https://doc.qt.io/qt-5/qdesktopservices.html#setUrlHandler
// it should use the external edit preferences and stuff
// watch some egm files and reload then inform protomodel
// also maybe make this a static class with private constructor?
class EditorServices
{
public:
  EditorServices();

  //TODO: Finish this or something for plugins & preferences
  void RegisterExternalEditor();

public slots:
  //NOTE: when the indexes are tree nodes and not fields of a
  //resource then they should open the metadata/entire resource
  //and when they are the root of the resource rather than a field
  //they should just do some other kind of default or something
  //thats how the treeview can provide a context option for these too
  // instead of just the buttons in the editors

  // requests the resource be opened in the file explorer
  static void OpenExternally(const QModelIndex &index);
  // requests the resource be edited using the preference command
  static void EditExternally(const QModelIndex &index);
};

#endif // EDITORSERVICES_H
