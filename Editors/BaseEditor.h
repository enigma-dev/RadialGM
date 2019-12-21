#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/ModelMapper.h"

#include <QObject>
#include <QWidget>

static const QHash<int, int> ResTypeFields = {
    {TypeCase::kFolder, TreeNode::kFolderFieldNumber},     {TypeCase::kSprite, TreeNode::kSpriteFieldNumber},
    {TypeCase::kSound, TreeNode::kSoundFieldNumber},       {TypeCase::kBackground, TreeNode::kBackgroundFieldNumber},
    {TypeCase::kPath, TreeNode::kPathFieldNumber},         {TypeCase::kFont, TreeNode::kFontFieldNumber},
    {TypeCase::kScript, TreeNode::kScriptFieldNumber},     {TypeCase::kTimeline, TreeNode::kTimelineFieldNumber},
    {TypeCase::kObject, TreeNode::kObjectFieldNumber},     {TypeCase::kRoom, TreeNode::kRoomFieldNumber},
    {TypeCase::kSettings, TreeNode::kSettingsFieldNumber}, {TypeCase::kShader, TreeNode::kShaderFieldNumber}};

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(ProtoModelPtr treeNodeModel, QWidget *parent);

  virtual void closeEvent(QCloseEvent *event);
  void ReplaceBuffer(google::protobuf::Message *buffer);
  void SetModelData(int index, const QVariant &value);
  QVariant GetModelData(int index);

 signals:
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);

 public slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVariant &oldValue = QVariant(0), const QVector<int> &roles = QVector<int>());
  virtual void RebindSubModels();
  void OnSave();

 protected:
  ModelMapper *nodeMapper;
  ModelMapper *resMapper;
  ProtoModelPtr _model;
};

#endif  // BASEEDTIOR_H
