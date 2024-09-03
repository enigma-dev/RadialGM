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

#include "VisualShaderEditor.h"

#include <QAction>
#include <QScreen>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>

#include <QtNodes/GraphicsView>

#include "VisualShader.pb.h"

using QtNodes::GraphicsView;
using QtNodes::NodeRole;

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), 
                                                                               layout(nullptr),
                                                                               layers_stack(nullptr),
                                                                               scene_layer(nullptr), 
                                                                               scene_layer_layout(nullptr), 
                                                                               graph(nullptr), 
                                                                               scene(nullptr), 
                                                                               view(nullptr), 
                                                                               menu_bar(nullptr) {
    // Create the main layout.
    layout = new QHBoxLayout(this);

    // Create the layers widget.
    layers_stack = new QStackedLayout();
    layers_stack->setStackingMode(QStackedLayout::StackAll); // See https://doc.qt.io/qt-5/qstackedlayout.html#stackingMode-prop

    // Create the scene layer.
    scene_layer = new QWidget();

    // Create the scene layer layout.
    scene_layer_layout = new QHBoxLayout(scene_layer);

    graph = new VisualShaderGraph();

    // Initialize and connect two nodes.
    {
        NodeId id1 = graph->addNode();
        graph->setNodeData(id1, NodeRole::Position, QPointF(0, 0));

        NodeId id2 = graph->addNode();
        graph->setNodeData(id2, NodeRole::Position, QPointF(300, 300));

        graph->addConnection(ConnectionId{id1, 0, id2, 0});
    }

    scene = new BasicGraphicsScene(*graph);
    scene->setOrientation(Qt::Horizontal);

    view = new GraphicsView(scene);

    scene_layer_layout->addWidget(view);

    // Set the scene layer layout.
    scene_layer->setLayout(scene_layer_layout);

    // Add the scene layer to the layers widget.
    layers_stack->addWidget(scene_layer); // Scene layer is added first so it has index 0.

    // Create the menu bar layer.
    top_layer = new QWidget();

    // Create the menu bar layout.
    menu_bar = new QHBoxLayout(top_layer);

    // Create the add node button.
    add_node_button = new QPushButton("Add Node", top_layer);
    menu_bar->addWidget(add_node_button);
    QObject::connect(add_node_button, &QPushButton::clicked, this, &VisualShaderEditor::create_node);

    // Create the preview shader button.
    preview_shader_button = new QPushButton("Preview Shader", top_layer);
    menu_bar->addWidget(preview_shader_button);

    // Align the buttons to the top-left corner.
    menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Set the top layer layout.
    top_layer->setLayout(menu_bar);

    // Add the top layer to the layers widget and set it as the current widget.
    layers_stack->addWidget(top_layer); // Top layer is added second so it has index 1.

    layout->addLayout(layers_stack);

    // Set the window title and icon.
    this->setWindowTitle("Visual Shader Editor");
    // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
    this->setLayout(layout);
}

VisualShaderEditor::~VisualShaderEditor() {
    if (preview_shader_button) delete preview_shader_button;
    if (add_node_button) delete add_node_button;
    if (menu_bar) delete menu_bar;
    if (top_layer) delete top_layer;
    if (view) delete view;
    if (scene) delete scene;
    if (graph) delete graph;
    if (scene_layer_layout) delete scene_layer_layout;
    if (scene_layer) delete scene_layer;
    if (layers_stack) delete layers_stack;
    if (layout) delete layout;
}

void VisualShaderEditor::create_node() {
    std::cout << "Creating node" << std::endl;
    VisualShaderEditor::add_node();
}

void VisualShaderEditor::add_node() {
    
}

VisualShaderGraph::VisualShaderGraph() : _nextNodeId{0} {

}

VisualShaderGraph::~VisualShaderGraph() {

}

std::unordered_set<NodeId> VisualShaderGraph::allNodeIds() const
{
    return _nodeIds;
}

std::unordered_set<ConnectionId> VisualShaderGraph::allConnectionIds(NodeId const nodeId) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&nodeId](ConnectionId const &cid) {
                     return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                 });

    return result;
}

std::unordered_set<ConnectionId> VisualShaderGraph::connections(NodeId nodeId,
                                                               PortType portType,
                                                               PortIndex portIndex) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&portType, &portIndex, &nodeId](ConnectionId const &cid) {
                     return (getNodeId(portType, cid) == nodeId
                             && getPortIndex(portType, cid) == portIndex);
                 });

    return result;
}

bool VisualShaderGraph::connectionExists(ConnectionId const connectionId) const
{
    return (_connectivity.find(connectionId) != _connectivity.end());
}

NodeId VisualShaderGraph::addNode(QString const nodeType)
{
    NodeId newId = _nextNodeId++;
    // Create new node.
    _nodeIds.insert(newId);

    Q_EMIT nodeCreated(newId);

    return newId;
}

bool VisualShaderGraph::connectionPossible(ConnectionId const connectionId) const
{
    return _connectivity.find(connectionId) == _connectivity.end();
}

void VisualShaderGraph::addConnection(ConnectionId const connectionId)
{
    _connectivity.insert(connectionId);

    Q_EMIT connectionCreated(connectionId);
}

bool VisualShaderGraph::nodeExists(NodeId const nodeId) const
{
    return (_nodeIds.find(nodeId) != _nodeIds.end());
}

QVariant VisualShaderGraph::nodeData(NodeId nodeId, NodeRole role) const
{
    Q_UNUSED(nodeId);

    QVariant result;

    switch (role) {
    case NodeRole::Type:
        result = QString("Default Node Type");
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = true;
        break;

    case NodeRole::Caption:
        result = QString("Node");
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        result = 5u;
        break;

    case NodeRole::OutPortCount:
        result = 3u;
        break;

    case NodeRole::Widget:
        result = QVariant();
        break;
    }

    return result;
}

bool VisualShaderGraph::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    bool result = false;

    switch (role) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        _nodeGeometryData[nodeId].pos = value.value<QPointF>();

        Q_EMIT nodePositionUpdated(nodeId);

        result = true;
    } break;

    case NodeRole::Size: {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result = true;
    } break;

    case NodeRole::CaptionVisible:
        break;

    case NodeRole::Caption:
        break;

    case NodeRole::Style:
        break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        break;

    case NodeRole::OutPortCount:
        break;

    case NodeRole::Widget:
        break;
    }

    return result;
}

QVariant VisualShaderGraph::portData(NodeId nodeId,
                                    PortType portType,
                                    PortIndex portIndex,
                                    PortRole role) const
{
    switch (role) {
    case PortRole::Data:
        return QVariant();
        break;

    case PortRole::DataType:
        return QVariant();
        break;

    case PortRole::ConnectionPolicyRole:
        return QVariant::fromValue(ConnectionPolicy::One);
        break;

    case PortRole::CaptionVisible:
        return true;
        break;

    case PortRole::Caption:
        if (portType == PortType::In)
            return QString::fromUtf8("Port In");
        else
            return QString::fromUtf8("Port Out");

        break;
    }

    return QVariant();
}

bool VisualShaderGraph::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    Q_UNUSED(nodeId);
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

bool VisualShaderGraph::deleteConnection(ConnectionId const connectionId)
{
    bool disconnected = false;

    auto it = _connectivity.find(connectionId);

    if (it != _connectivity.end()) {
        disconnected = true;

        _connectivity.erase(it);
    }

    if (disconnected)
        Q_EMIT connectionDeleted(connectionId);

    return disconnected;
}

bool VisualShaderGraph::deleteNode(NodeId const nodeId)
{
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(nodeId);
    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _nodeIds.erase(nodeId);
    _nodeGeometryData.erase(nodeId);

    Q_EMIT nodeDeleted(nodeId);

    return true;
}

QJsonObject VisualShaderGraph::saveNode(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

void VisualShaderGraph::loadNode(QJsonObject const &nodeJson)
{
    NodeId restoredNodeId = nodeJson["id"].toInt();

    // Next NodeId must be larger that any id existing in the graph
    _nextNodeId = std::max(restoredNodeId + 1, _nextNodeId);

    // Create new node.
    _nodeIds.insert(restoredNodeId);

    Q_EMIT nodeCreated(restoredNodeId);

    {
        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

        setNodeData(restoredNodeId, NodeRole::Position, pos);
    }
}
