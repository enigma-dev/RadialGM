#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/EditorModel.h"
#include "Models/EditorMapper.h"
#include "treenode.pb.h"

#include <QObject>
#include <QWidget>

using TreeNode = buffers::TreeNode;
using TypeCase = TreeNode::TypeCase;

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
  explicit BaseEditor(EditorModel *model, QWidget *parent);

  bool HasFocus();

 signals:
  void FocusGained();
  void FocusLost();

 public slots:
  void OnSave();

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

  EditorMapper *_mapper;
  EditorModel *_model;
  bool _hasFocus = false;
};

#endif  // BASEEDTIOR_H
