/********************************************************************************\
 **                                                                              **
 **  Copyright (C) 2024 Saif Kandil (k0T0z)                                      **
 **                                                                              **
 **  This file is a part of the ENIGMA Development Environment.                  **
 **                                                                              **
 **                                                                              **
 **  ENIGMA is free software: you can redistribute it and/or modify it under the **
 **  terms of the GNU General Public License as published by the Free Software   **
 **  Foundation, version 3 of the license or any later version.                  **
 **                                                                              **
 **  This application and its source code is distributed AS-IS, WITHOUT ANY      **
 **  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS   **
 **  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more       **
 **  details.                                                                    **
 **                                                                              **
 **  You should have recieved a copy of the GNU General Public License along     **
 **  with this code. If not, see <http://www.gnu.org/licenses/>                  **
 **                                                                              **
 **  ENIGMA is an environment designed to create games and other programs with a **
 **  high-level, fully compilable language. Developers of ENIGMA or anything     **
 **  associated with ENIGMA are in no way responsible for its users or           **
 **  applications created by its users, or damages caused by the environment     **
 **  or programs made in the environment.                                        **
 **                                                                              **
 \********************************************************************************/

#ifndef ENIGMA_VISUAL_SHADER_EDITOR_H
#define ENIGMA_VISUAL_SHADER_EDITOR_H

#include <QtCore/QJsonObject>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtWidgets/QHBoxLayout>
#include <QPushButton>
#include <QStackedLayout>

#include <QtNodes/AbstractGraphModel>
#include <QtNodes/ConnectionIdUtils>
#include <QtNodes/StyleCollection>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/GraphicsView>

#include "ResourceTransformations/VisualShader/visual_shader.h"
#include "BaseEditor.h"

using ConnectionId = QtNodes::ConnectionId;
using ConnectionPolicy = QtNodes::ConnectionPolicy;
using NodeFlag = QtNodes::NodeFlag;
using NodeId = QtNodes::NodeId;
using NodeRole = QtNodes::NodeRole;
using PortIndex = QtNodes::PortIndex;
using PortRole = QtNodes::PortRole;
using PortType = QtNodes::PortType;
using StyleCollection = QtNodes::StyleCollection;
using QtNodes::InvalidNodeId;

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;

class VisualShaderGraph;

class VisualShaderEditor : public BaseEditor {
    Q_OBJECT

 public:
  VisualShaderEditor(MessageModel* model, QWidget* parent = nullptr);
  ~VisualShaderEditor() override;

  void create_node();

  void add_node();

 private:
  QHBoxLayout* layout;
  QStackedLayout* layers_stack;

  QWidget* scene_layer; // Layer having the scene.
  QHBoxLayout* scene_layer_layout;
  VisualShaderGraph* graph;
  BasicGraphicsScene* scene;
  GraphicsView* view;

  QWidget* top_layer; // Layer having the menu bar.
  QHBoxLayout* menu_bar;

  QPushButton* add_node_button;
  QPushButton* preview_shader_button;
};

/**
 * The class implements a bare minimum required to demonstrate a model-based
 * graph.
 */
class VisualShaderGraph : public QtNodes::AbstractGraphModel
{
    Q_OBJECT

public:
    struct NodeGeometryData
    {
        QSize size;
        QPointF pos;
    };

    VisualShaderGraph();

    ~VisualShaderGraph() override;

    std::unordered_set<NodeId> allNodeIds() const override;

    std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override;

    std::unordered_set<ConnectionId> connections(NodeId nodeId,
                                                 PortType portType,
                                                 PortIndex portIndex) const override;

    bool connectionExists(ConnectionId const connectionId) const override;

    NodeId addNode(QString const nodeType = QString()) override;

    /**
   * Connection is possible when graph contains no connectivity data
   * in both directions `Out -> In` and `In -> Out`.
   */
    bool connectionPossible(ConnectionId const connectionId) const override;

    void addConnection(ConnectionId const connectionId) override;

    bool nodeExists(NodeId const nodeId) const override;

    QVariant nodeData(NodeId nodeId, NodeRole role) const override;

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    QVariant portData(NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      PortRole role) const override;

    bool setPortData(NodeId nodeId,
                     PortType portType,
                     PortIndex portIndex,
                     QVariant const &value,
                     PortRole role = PortRole::Data) override;

    bool deleteConnection(ConnectionId const connectionId) override;

    bool deleteNode(NodeId const nodeId) override;

    QJsonObject saveNode(NodeId const) const override;

    /// @brief Creates a new node based on the informatoin in `nodeJson`.
    /**
   * @param nodeJson conains a `NodeId`, node's position, internal node
   * information.
   */
    void loadNode(QJsonObject const &nodeJson) override;

private:
    std::unordered_set<NodeId> _nodeIds;

    /// [Important] This is a user defined data structure backing your model.
    /// In your case it could be anything else representing a graph, for example, a
    /// table. Or a collection of structs with pointers to each other. Or an
    /// abstract syntax tree, you name it.
    ///
    /// This data structure contains the graph connectivity information in both
    /// directions, i.e. from Node1 to Node2 and from Node2 to Node1.
    std::unordered_set<ConnectionId> _connectivity;

    mutable std::unordered_map<NodeId, NodeGeometryData> _nodeGeometryData;

    /// A convenience variable needed for generating unique node ids.
    NodeId _nextNodeId;

    NodeId newNodeId() override { return _nextNodeId++; }
};

#endif // ENIGMA_VISUAL_SHADER_EDITOR_H
