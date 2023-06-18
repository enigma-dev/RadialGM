#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/MessageModel.h"
#include "Models/ModelMapper.h"

#include <QObject>
#include <QWidget>

static const QHash<int, int> ResTypeFields = {
    {TypeCase::kFolder, TreeNode::kFolderFieldNumber},     {TypeCase::kSprite, TreeNode::kSpriteFieldNumber},
    {TypeCase::kSound, TreeNode::kSoundFieldNumber},       {TypeCase::kBackground, TreeNode::kBackgroundFieldNumber},
    {TypeCase::kPath, TreeNode::kPathFieldNumber},         {TypeCase::kFont, TreeNode::kFontFieldNumber},
    {TypeCase::kScript, TreeNode::kScriptFieldNumber},     {TypeCase::kTimeline, TreeNode::kTimelineFieldNumber},
    {TypeCase::kObject, TreeNode::kObjectFieldNumber},     {TypeCase::kEgmRoom, TreeNode::kEgmRoomFieldNumber},
    {TypeCase::kSettings, TreeNode::kSettingsFieldNumber}, {TypeCase::kShader, TreeNode::kShaderFieldNumber}};

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(MessageModel *treeNodeModel, QWidget *parent);
  ~BaseEditor();
  void ReplaceBuffer(google::protobuf::Message *buffer);

 public slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVariant &oldValue = QVariant(0), const QVector<int> &roles = QVector<int>());
  virtual void RebindSubModels();
  void OnSave();
  void MarkDeleted() { _deleted = true; }

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

  bool _reset_model_on_close = false;
  bool _deleted = false;
  MessageModel *_model;
  ModelMapper *_nodeMapper;
  ModelMapper *_resMapper;
};

#endif  // BASEEDTIOR_H
