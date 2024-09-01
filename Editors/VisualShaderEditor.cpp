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

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), graph(nullptr), scene(nullptr), layout(nullptr), view(nullptr) {
//   this->setWindowIcon(QIcon(":/resources/visual_shader.png"));

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

    scene->setOrientation(Qt::Vertical);

    layout = new QHBoxLayout(this);

    view =  new GraphicsView(scene);

    layout->addWidget(view);

    QGroupBox *groupBox = new QGroupBox("Orientation");

    QRadioButton *radio1 = new QRadioButton("Vertical");
    QRadioButton *radio2 = new QRadioButton("Horizontal");

    QVBoxLayout *vbl = new QVBoxLayout;
    vbl->addWidget(radio1);
    vbl->addWidget(radio2);
    vbl->addStretch();
    groupBox->setLayout(vbl);

    QObject::connect(radio1, &QRadioButton::clicked, [this]() {
        scene->setOrientation(Qt::Vertical);
    });

    QObject::connect(radio2, &QRadioButton::clicked, [this]() {
        scene->setOrientation(Qt::Horizontal);
    });

    radio1->setChecked(true);

    layout->addWidget(groupBox);

    // Setup context menu for creating new nodes.
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction createNodeAction(QStringLiteral("Create Node"), view);
    QObject::connect(&createNodeAction, &QAction::triggered, [&]() {
        // Mouse position in scene coordinates.
        QPointF posView = view->mapToScene(view->mapFromGlobal(QCursor::pos()));

        NodeId const newId = graph->addNode();
        graph->setNodeData(newId, NodeRole::Position, posView);
    });
    view->insertAction(view->actions().front(), &createNodeAction);

    this->setLayout(layout);
}

VisualShaderEditor::~VisualShaderEditor() {
    if (view) delete view;
    if (layout) delete layout;
    if (scene) delete scene;
    if (graph) delete graph;
}

VisualShaderGraph::VisualShaderGraph()
    : _nextNodeId{0}
{}

VisualShaderGraph::~VisualShaderGraph()
{
    
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
