#ifndef BASEEDTIOR_H
#define BASEEDTIOR_H

#include "Models/ModelMapper.h"

#include "codegen/treenode.pb.h"

#include <QObject>
#include <QWidget>

using TypeCase = buffers::TreeNode::TypeCase;
using TreeNode = buffers::TreeNode;
using Background = buffers::resources::Background;
using Font =  buffers::resources::Font;
using Object = buffers::resources::Object;
using Path = buffers::resources::Path;
using Room = buffers::resources::Room;
using Sound = buffers::resources::Sound;
using Sprite = buffers::resources::Sprite;
using Timeline = buffers::resources::Timeline;

static const QHash<int, int> ResTypeFields = {
    { TypeCase::kSprite,     TreeNode::kSpriteFieldNumber     },
    { TypeCase::kSound,      TreeNode::kSoundFieldNumber      },
    { TypeCase::kBackground, TreeNode::kBackgroundFieldNumber },
    { TypeCase::kPath,       TreeNode::kPathFieldNumber       },
    { TypeCase::kFont,       TreeNode::kFontFieldNumber       },
    { TypeCase::kTimeline,   TreeNode::kTimelineFieldNumber   },
    { TypeCase::kObject,     TreeNode::kObjectFieldNumber     },
    { TypeCase::kRoom,       TreeNode::kRoomFieldNumber       }
};

class BaseEditor : public QWidget {
  Q_OBJECT

 public:
  explicit BaseEditor(ProtoModel *treeNodeModel, QWidget *parent);

  virtual void closeEvent(QCloseEvent *event);
  void ReplaceBuffer(google::protobuf::Message *buffer);
  void SetModelData(int index, const QVariant &value);
  QVariant GetModelData(int index);

 public slots:
  virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVector<int> &roles = QVector<int>());

 protected:
  ModelMapper* nodeMapper;
  ModelMapper* resMapper;
};

#endif  // BASEEDTIOR_H
