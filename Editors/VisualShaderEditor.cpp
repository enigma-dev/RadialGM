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

#include <unordered_map>

#include <QtNodes/GraphicsView>

#include "VisualShader.pb.h"

using QtNodes::GraphicsView;
using QtNodes::NodeRole;

/*************************************/
/* VisualShaderEditor                */
/*************************************/

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), 
                                                                               layout(nullptr),
                                                                               scene_layer_layout(nullptr), 
                                                                               scene_layer(nullptr), 
                                                                               graph(nullptr), 
                                                                               scene(nullptr), 
                                                                               view(nullptr), 
                                                                               top_layer(nullptr),
                                                                               menu_bar(nullptr),
                                                                               create_node_button(nullptr),
                                                                               preview_shader_button(nullptr),
                                                                               create_node_dialog(nullptr) {
    // Create the main layout.
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    //////////////// End of Header ////////////////

    // Create the scene layer.
    scene_layer = new QWidget();
    scene_layer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scene_layer->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom

    // Create the scene layer layout.
    scene_layer_layout = new QHBoxLayout(scene_layer);
    scene_layer_layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    scene_layer_layout->setSpacing(0);
    scene_layer_layout->setSizeConstraint(QLayout::SetNoConstraint);
    scene_layer_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

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
    view->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    scene_layer_layout->addWidget(view);

    // Set the scene layer layout.
    scene_layer->setLayout(scene_layer_layout);

    // Create the menu bar layer on top of the scene layer.
    top_layer = new QWidget(view);
    top_layer->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    top_layer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Create the menu bar layout.
    menu_bar = new QHBoxLayout(top_layer);
    menu_bar->setContentsMargins(10, 10, 10, 10); // Left, top, right, bottom
    menu_bar->setSpacing(5); // Adjust spacing as needed
    menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    menu_bar->setSizeConstraint(QLayout::SetMinimumSize);

    // Create the add node button.
    create_node_button = new QPushButton("Add Node", top_layer);
    create_node_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_node_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(create_node_button);
    QObject::connect(create_node_button, &QPushButton::clicked, this, &VisualShaderEditor::show_create_node_dialog);

    // Create the preview shader button.
    preview_shader_button = new QPushButton("Preview Shader", top_layer);
    preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    preview_shader_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(preview_shader_button);

    // Set the top layer layout.
    top_layer->setLayout(menu_bar);

    // Add the scene layer to the main layout.
    layout->addWidget(scene_layer);

    ////////////////////////////////////
    // CreateNodeDialog Nodes Tree
    ////////////////////////////////////

    // Create the create node dialog under the main layout.
    create_node_dialog = new CreateNodeDialog(this);
    create_node_dialog->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    create_node_dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    //////////////////////////////////////////
    // CreateNodeDialog Nodes Tree Children
    //////////////////////////////////////////

    const VisualShaderEditor::CreateNodeDialogNodesTreeItem* items {VisualShaderEditor::create_node_dialog_nodes_tree_items};

    // Map to store category items
    std::unordered_map<std::string, QTreeWidgetItem*> category_path_map;

    int i{0};

    while (items[i].return_type != VisualShaderNode::PortType::PORT_TYPE_ENUM_SIZE) {
        const CreateNodeDialogNodesTreeItem& item {items[i]};

        // Parse the category string into a vector of strings
        std::vector<std::string> categories {pasre_node_category_path(item.category_path)};
        QTreeWidgetItem* parent {nullptr}; // Start from the root

        std::string current_category_path;
        // Create/find each level of categories
        for (const std::string& category : categories) {
            if (!current_category_path.empty()) {
                current_category_path += "/";
            }

            current_category_path += category;

            parent = find_or_create_category_item(parent, category, current_category_path, create_node_dialog->get_nodes_tree(), category_path_map);
        }

        // Now add the item to its corresponding parent category
        QTreeWidgetItem *node_item = new QTreeWidgetItem(parent);
        node_item->setText(0, QString::fromStdString(item.name));

        i++;
    }

    //////////////// Start of Footer ////////////////

    graph->set_visual_shader_editor(this);

    this->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom

    // Set the window title and icon.
    this->setWindowTitle("Visual Shader Editor");
    // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
    this->setLayout(layout);
}

VisualShaderEditor::~VisualShaderEditor() {
    // TODO: We don't need to delete the pointers as they are destroyed when the parent is destroyed.
    if (create_node_dialog) delete create_node_dialog;
    if (preview_shader_button) delete preview_shader_button;
    if (create_node_button) delete create_node_button;
    if (menu_bar) delete menu_bar;
    if (top_layer) delete top_layer;
    if (view) delete view;
    if (scene) delete scene;
    if (graph) delete graph;
    if (scene_layer) delete scene_layer;
    if (scene_layer_layout) delete scene_layer_layout;
    if (layout) delete layout;
}

const VisualShaderEditor::CreateNodeDialogNodesTreeItem VisualShaderEditor::create_node_dialog_nodes_tree_items[] = {
    {"Color", "Input/All", "VisualShaderNodeInput", "Color", { "color" }, VisualShaderNode::PortType::PORT_TYPE_VECTOR_4D},
    {"Time", "Input/All", "VisualShaderNodeInput", "Time", { "time" }, VisualShaderNode::PortType::PORT_TYPE_SCALAR},
    {"UV", "Input/All", "VisualShaderNodeInput", "UV", { "uv" }, VisualShaderNode::PortType::PORT_TYPE_VECTOR_2D},

    {"", "", "", "", {}, VisualShaderNode::PortType::PORT_TYPE_ENUM_SIZE},  // End of list.
};

void VisualShaderEditor::create_node() {
    std::cout << "Creating node" << std::endl;
    VisualShaderEditor::add_node();
}

void VisualShaderEditor::add_node() {
    
}

void VisualShaderEditor::show_create_node_dialog(const bool& custom_mouse_pos) {
    int status = create_node_dialog->exec();
    switch (status) {
        case QDialog::Accepted:
            std::cout << "Create node dialog accepted" << std::endl;
            break;
        case QDialog::Rejected:
            std::cout << "Create node dialog rejected" << std::endl;
            break;
        default:
            std::cout << "Create node dialog unknown status" << std::endl;
            break;
    }
}

std::vector<std::string> VisualShaderEditor::pasre_node_category_path(const std::string& node_category_path) {
    std::vector<std::string> tokens;
    std::stringstream ss(node_category_path);
    std::string token;
    while (std::getline(ss, token, '/')) {
        tokens.push_back(token);
    }
    return tokens;
}

QTreeWidgetItem* VisualShaderEditor::find_or_create_category_item(QTreeWidgetItem* parent, const std::string& category, const std::string& category_path, QTreeWidget* create_node_dialog_nodes_tree, std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map) {
    // Check if category already exists under parent
    if (category_path_map.find(category_path) != category_path_map.end()) {
        return category_path_map[category_path];
    }

    // Create a new QTreeWidgetItem
    QTreeWidgetItem* new_item;

    if (parent) {
        new_item = new QTreeWidgetItem(parent);
    } else {
        new_item = new QTreeWidgetItem(create_node_dialog_nodes_tree);
    }

    new_item->setText(0, QString::fromStdString(category));

    // Add the new category to the map
    category_path_map[category_path] = new_item;
    
    return new_item;
}

/*************************************/
/* CreateNodeDialog                  */
/*************************************/

CreateNodeDialog::CreateNodeDialog(QWidget* parent) : QDialog(parent),
                                                      layout(nullptr),
                                                      create_node_dialog_nodes_tree_layout(nullptr),
                                                      create_node_dialog_nodes_tree(nullptr),
                                                      create_node_dialog_nodes_description(nullptr),
                                                      buttons_layout(nullptr),
                                                      create_button(nullptr),
                                                      cancel_button(nullptr) {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    //////////////// End of Header ////////////////

    // Create the nodes tree layout.
    create_node_dialog_nodes_tree_layout = new QVBoxLayout();
    create_node_dialog_nodes_tree_layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    create_node_dialog_nodes_tree_layout->setSpacing(0); // Adjust spacing as needed
    create_node_dialog_nodes_tree_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    create_node_dialog_nodes_tree_layout->setSizeConstraint(QLayout::SetMinimumSize);

    // Add the nodes tree layout to the main layout.
    layout->addLayout(create_node_dialog_nodes_tree_layout);

    // Create the nodes tree.
    create_node_dialog_nodes_tree = new QTreeWidget();
    create_node_dialog_nodes_tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_node_dialog_nodes_tree->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    create_node_dialog_nodes_tree->setColumnCount(1);
    create_node_dialog_nodes_tree->setHeaderHidden(true);

    // Add the nodes tree to the nodes tree layout.
    create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_tree, 2); // 2x the size of the nodes description.

    // Create the nodes description.
    create_node_dialog_nodes_description = new QTextEdit();
    create_node_dialog_nodes_description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_node_dialog_nodes_description->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    create_node_dialog_nodes_description->setReadOnly(true);
    create_node_dialog_nodes_description->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Add the nodes description to the nodes tree layout.
    create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_description, 1);

    // Create the buttons layout.
    buttons_layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    create_button = new QPushButton("Create");
    QObject::connect(create_button, &QPushButton::clicked, this, &CreateNodeDialog::on_CreateButtonTriggered);
    create_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom

    cancel_button = new QPushButton("Cancel");
    QObject::connect(cancel_button, &QPushButton::clicked, this, &CreateNodeDialog::on_CancelButtonTriggered);
    cancel_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    cancel_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom

    // Add the buttons to the buttons layout.
    buttons_layout->addWidget(create_button);
    buttons_layout->addWidget(cancel_button);

    // Add the buttons layout to the main layout.
    layout->addLayout(buttons_layout);

    //////////////// Start of Footer ////////////////

    this->setWindowTitle("Create Shader Node");

    this->setLayout(layout);
}

CreateNodeDialog::~CreateNodeDialog() {
    if (cancel_button) delete cancel_button;
    if (create_button) delete create_button;
    if (buttons_layout) delete buttons_layout;
    if (create_node_dialog_nodes_tree) delete create_node_dialog_nodes_tree;
    if (create_node_dialog_nodes_description) delete create_node_dialog_nodes_description;
    if (create_node_dialog_nodes_tree_layout) delete create_node_dialog_nodes_tree_layout;
    if (layout) delete layout;
}

void CreateNodeDialog::on_CreateButtonTriggered() {
    this->accept();
}

void CreateNodeDialog::on_CancelButtonTriggered() {
    this->reject();
}

/*************************************/
/* VisualShaderGraph                 */
/*************************************/

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
