#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/MessageModel.h"
#include "Models/ModelMapper.h"
#include "Models/EditorMapper.h"

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
  explicit BaseEditor(MessageModel *treeNodeModel, QWidget *parent);

  void ReplaceBuffer(google::protobuf::Message *buffer);
  bool HasFocus();

 signals:
  void ResourceRenamed(TypeCase type, const QString &oldName, const QString &newName);
  void FocusGained();
  void FocusLost();

 public slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVariant &oldValue = QVariant(0), const QVector<int> &roles = QVector<int>());
  virtual void RebindSubModels();
  void OnSave();

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

  ModelMapper *_nodeMapper;
  ModelMapper *_resMapper;
  MessageModel *_model;
  bool _hasFocus = false;
};

#endif  // BASEEDTIOR_H
