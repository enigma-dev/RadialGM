#ifndef EDITORMAPPER_H
#define EDITORMAPPER_H

#include "ModelMapper.h"

#include <QObject>
#include <QStack>
#include <QAbstractItemView>

class MessageModel;
class BaseEditor;

/**
 * @brief The EditorMapper class used to map widgets into the model.
 * This class encapsulates the hierarchical data binding needed by
 * the various editors. The organization of the internal source model
 * and protocol buffer is abstracted behind the compile-time generated
 * constants and field numbers of resources.
 */
// NOTE: There is intentionally no API to remove individual mappings
// in this class so that your tiny brain doesn't butcher what is
// otherwise a very elegant execution of basic data structures.
class EditorMapper : public QObject {
  Q_OBJECT

  MessageModel* _rootModel; // << the root model
  ModelMapper* _rootMapper; // << mapper on the root
  QStack<ModelMapper*> _mappers; // << current mappers in progress
  QList<ModelMapper*> _groups; // << created mappers

 public:
  explicit EditorMapper(MessageModel *model, BaseEditor *parent);

  // the fact this is a ModelMapper* is only an implementation detail
  // that you should not rely on at all
  using MapGroup = ModelMapper*;

  /**
   * @brief mapField Maps a proto field in the model to a widget property.
   * @param fieldNumber The compile-time generated field number constant.
   * @param object The Qt widget we want to map.
   * @param property The property name of the object we want to map. The
   *                 default, an empty string, will instead look up the
   *                 USER property on the object, which is the one QWidgets
   *                 are associated with for editing. For example, the
   *                 QLineEdit USER property is the "text" property.
   */
  void mapField(int fieldNumber, QWidget *widget, const QByteArray& property="");
  /**
   * @brief mapName Convenience for mapping the name field of the resource.
   *
   * This is an abstraction to keep the base editor and subclasses from relying
   * on the structure of tree nodes and knowing about all other resource types
   * which would slow down compilation.
   *
   * @param object The object to map the name to
   * @param property The property name of the object to map. Same as
   *                 mapField parameter.
   */
  void mapName(QWidget *widget, const QByteArray& property="");
  /**
   * @brief clear Removes all mappings and moves back to the root of the model.
   */
  void clear();
  /**
   * @brief pushField Moves current index into a field.
   *
   * This can be used to map messages within a repeated field and easily remap
   * the entire group later on. It's particularly useful for master-detail
   * oriented views like the room editor's background settings which change
   * when the selected background changes.
   *
   * @param fieldNumber The compile-time constant of the field number.
   * @param index The index of the repeated field value, 0 by default.
   * @return MapGroup An index which you can later use to remap the fields.
   */
  MapGroup pushField(int fieldNumber, int index = 0);
  /**
   * @brief pushAsset Moves current index into the resource field.
   *
   * Like mapName this is also a convenience function that keeps editors
   * loosely coupled from the structure of the tree node buffer. It's a
   * consequence of the name being in the tree node but not the resource.
   * This also resets the current index back to the root to find the
   * currently set resource of the tree node. That's why there is no
   * popResource, because you can just call this instead!
   */
  void pushAsset();
  /**
   * @brief pushView Moves current index into a field view.
   *
   * This can be used to create master-detail mappings in the editors.
   * The view's selection change will automatically remap all of the
   * fields that are mapped after this call.
   *
   * @param fieldNumber The compile-time constant of the field number.
   * @param view The Qt view to map to the field.
   */
  void pushView(int fieldNumber, QAbstractItemView *view);
  /**
   * @brief popField Moves current index back to the previous parent.
   */
  void popField();
  /**
   * @brief popRoot Moves back to the root of the model.
   *
   * Existing mappings are not cleared but future mappings will be made relative
   * to the root as they initially were.
   */
  void popRoot();
  /**
   * @brief load Reverts all of the widgets in the group to the model values.
   * @param group A specific group of mapped widgets to load. Can be null,
   *              meaning load all mapped groups.
   */
  void load(MapGroup group = nullptr);
};

#endif  // EDITORMAPPER_H
