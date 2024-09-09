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
#include "ResourceTransformations/VisualShader/visual_shader_nodes.h"
#include "ResourceTransformations/VisualShader/vs_noise_nodes.h"

using QtNodes::GraphicsView;
using QtNodes::NodeRole;

/*************************************/
/* VisualShaderEditor                */
/*************************************/

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), 
                                                                               visual_shader(nullptr),
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
                                                                               create_node_action(nullptr),
                                                                               create_node_dialog(nullptr) {
    visual_shader = new VisualShader();
    
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

    scene = new BasicGraphicsScene(*graph);
    scene->setOrientation(Qt::Horizontal);

    view = new GraphicsView(scene);
    view->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    scene_layer_layout->addWidget(view);

    // Setup context menu for creating new nodes.
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    create_node_action = new QAction(QStringLiteral("Create Node"), view);
    QObject::connect(create_node_action, &QAction::triggered, this, &VisualShaderEditor::on_create_node_action_triggered);
    view->insertAction(view->actions().front(), create_node_action);

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

    // Create the create node button.
    create_node_button = new QPushButton("Create Node", top_layer);
    create_node_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_node_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(create_node_button);
    QObject::connect(create_node_button, &QPushButton::pressed, this, &VisualShaderEditor::on_create_node_button_pressed);

    this->connect(this, &VisualShaderEditor::on_create_node_dialog_requested, this, &VisualShaderEditor::show_create_node_dialog);

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

    while (!items[i].type.empty()) {
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
        node_item->setData(0, Qt::UserRole, QString::fromStdString(item.type));
        node_item->setData(0, Qt::UserRole + 1, QString::fromStdString(item.description));

        i++;
    }

    //////////////// Start of Footer ////////////////

    graph->register_visual_shader(visual_shader);
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
    if (create_node_action) delete create_node_action;
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
    if (visual_shader) delete visual_shader;
}

const VisualShaderEditor::CreateNodeDialogNodesTreeItem VisualShaderEditor::create_node_dialog_nodes_tree_items[] = {

	// Input

    {"Input", "Input/Basic", "VisualShaderNodeInput", "Input parameter."},

	{"ColorConstant", "Input/Basic", "VisualShaderNodeColorConstant", "Color constant."},
	{"BooleanConstant", "Input/Basic", "VisualShaderNodeBooleanConstant", "Boolean constant."},
	{"FloatConstant", "Input/Basic", "VisualShaderNodeFloatConstant", "Scalar floating-point constant."},
	{"IntConstant", "Input/Basic", "VisualShaderNodeIntConstant", "Scalar integer constant."},
	{"UIntConstant", "Input/Basic", "VisualShaderNodeUIntConstant", "Scalar unsigned integer constant."},
	{"Vector2Constant", "Input/Basic", "VisualShaderNodeVec2Constant", "2D vector constant."},
	{"Vector3Constant", "Input/Basic", "VisualShaderNodeVec3Constant", "3D vector constant."},
	{"Vector4Constant", "Input/Basic", "VisualShaderNodeVec4Constant", "4D vector constant."},

	// Functions

	{"FloatFunc", "Functions/Scalar", "VisualShaderNodeFloatFunc", "Float function."},
	{"IntFunc", "Functions/Scalar", "VisualShaderNodeIntFunc", "Integer function."},
	{"UIntFunc", "Functions/Scalar", "VisualShaderNodeUIntFunc", "Unsigned integer function."},
	{"VectorFunc", "Functions/Vector", "VisualShaderNodeVectorFunc", "Vector function."},
	{"DerivativeFunc", "Functions/Others", "VisualShaderNodeDerivativeFunc", "Derivative function."},

	// Operators

	{"FloatOp", "Operators/Scalar", "VisualShaderNodeFloatOp", "Float operator."},
	{"IntOp", "Operators/Scalar", "VisualShaderNodeIntOp", "Integer operator."},
	{"UIntOp", "Operators/Scalar", "VisualShaderNodeUIntOp", "Unsigned integer operator."},
	{"VectorOp", "Operators/Vector", "VisualShaderNodeVectorOp", "Vector operator."},
	{"VectorCompose", "Operators/Vector", "VisualShaderNodeVectorCompose", "Composes vector from scalars."},
	{"VectorDecompose", "Operators/Vector", "VisualShaderNodeVectorDecompose", "Decomposes vector to scalars."},

	// Procedural

    {"ValueNoise", "Procedural/Noise", "VisualShaderNodeValueNoise", "Generates a simple, or Value, noise based on input 'UV'. The scale of the generated noise is controlled by input 'Scale'."},

    // Utility

	{"Compare", "Utility/Logic", "VisualShaderNodeCompare", "Returns the boolean result of the comparison between two parameters."},
    {"If", "Utility/Logic", "VisualShaderNodeIf", "Returns the value of the 'True' or 'False' input based on the value of the 'Condition' input."},
    {"Switch", "Utility/Logic", "VisualShaderNodeSwitch", "Returns an associated scalar if the provided boolean value is true or false."},
	{"Is", "Utility/Logic", "VisualShaderNodeIs", "Returns the boolean result of the comparison between INF (or NaN) and a scalar parameter."},

    {"", "", "", ""},
};

void VisualShaderEditor::create_node(const QPointF& pos) {
    QTreeWidgetItem* selected_item = create_node_dialog->get_selected_item();

    if (!selected_item) {
        return;
    }

    VisualShaderEditor::add_node(selected_item, pos);
}

void VisualShaderEditor::add_node(QTreeWidgetItem* selected_item, const QPointF& pos) {
    std::string type = selected_item->data(0, Qt::UserRole).toString().toStdString();

    if (type.empty()) {
        return;
    }

    // Instantiate the node based on the type
    std::shared_ptr<VisualShaderNode> node;

    if (type == "VisualShaderNodeInput") {
        node = std::make_shared<VisualShaderNodeInput>();
    } else if (type == "VisualShaderNodeColorConstant") {
        node = std::make_shared<VisualShaderNodeColorConstant>();
    } else if (type == "VisualShaderNodeBooleanConstant") {
        node = std::make_shared<VisualShaderNodeBooleanConstant>();
    } else if (type == "VisualShaderNodeFloatConstant") {
        node = std::make_shared<VisualShaderNodeFloatConstant>();
    } else if (type == "VisualShaderNodeIntConstant") {
        node = std::make_shared<VisualShaderNodeIntConstant>();
    } else if (type == "VisualShaderNodeUIntConstant") {
        node = std::make_shared<VisualShaderNodeUIntConstant>();
    } else if (type == "VisualShaderNodeVec2Constant") {
        node = std::make_shared<VisualShaderNodeVec2Constant>();
    } else if (type == "VisualShaderNodeVec3Constant") {
        node = std::make_shared<VisualShaderNodeVec3Constant>();
    } else if (type == "VisualShaderNodeVec4Constant") {
        node = std::make_shared<VisualShaderNodeVec4Constant>();
    } else if (type == "VisualShaderNodeFloatFunc") {
        node = std::make_shared<VisualShaderNodeFloatFunc>();
    } else if (type == "VisualShaderNodeIntFunc") {
        node = std::make_shared<VisualShaderNodeIntFunc>();
    } else if (type == "VisualShaderNodeUIntFunc") {
        node = std::make_shared<VisualShaderNodeUIntFunc>();
    } else if (type == "VisualShaderNodeDerivativeFunc") {
        node = std::make_shared<VisualShaderNodeDerivativeFunc>();
    } else if (type == "VisualShaderNodeFloatOp") {
        node = std::make_shared<VisualShaderNodeFloatOp>();
    } else if (type == "VisualShaderNodeIntOp") {
        node = std::make_shared<VisualShaderNodeIntOp>();
    } else if (type == "VisualShaderNodeUIntOp") {
        node = std::make_shared<VisualShaderNodeUIntOp>();
    } else if (type == "VisualShaderNodeValueNoise") {
        node = std::make_shared<VisualShaderNodeValueNoise>();
    } else if (type == "VisualShaderNodeCompare") {
        node = std::make_shared<VisualShaderNodeCompare>();
    } else if (type == "VisualShaderNodeIf") {
        node = std::make_shared<VisualShaderNodeIf>();
    } else if (type == "VisualShaderNodeIs") {
        node = std::make_shared<VisualShaderNodeIs>();
    } else if (type == "VisualShaderNodeSwitch") {
        node = std::make_shared<VisualShaderNodeSwitch>();
    } else {
        std::cout << "Unknown node type: " << type << std::endl;
    }

    if (!node) {
        std::cout << "Failed to create node of type: " << type << std::endl;
        return;
    }

    QPointF offset {view->mapToScene(pos.toPoint())}; // Top-left corner of the view

    graph->add_node_custom(node, offset);
}

void VisualShaderEditor::show_create_node_dialog(const QPointF& pos) {
    int status {create_node_dialog->exec()};
    switch (status) {
        case QDialog::Accepted:
            std::cout << "Create node dialog accepted" << std::endl;
            VisualShaderEditor::create_node(pos);
            break;
        case QDialog::Rejected:
            std::cout << "Create node dialog rejected" << std::endl;
            break;
        default:
            std::cout << "Create node dialog unknown status" << std::endl;
            break;
    }
}

void VisualShaderEditor::on_create_node_button_pressed() {
    Q_EMIT on_create_node_dialog_requested();
}

void VisualShaderEditor::on_create_node_action_triggered() {
    QPointF pos {view->mapToScene(view->mapFromGlobal(QCursor::pos()))};
    Q_EMIT on_create_node_dialog_requested(pos);
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
                                                      cancel_button(nullptr),
                                                      selected_item(nullptr) {
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
    this->connect(create_node_dialog_nodes_tree, &QTreeWidget::itemSelectionChanged, this, &CreateNodeDialog::update_selected_item);

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
    QObject::connect(create_button, &QPushButton::pressed, this, &CreateNodeDialog::on_create_node_button_pressed);
    create_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    create_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom

    cancel_button = new QPushButton("Cancel");
    QObject::connect(cancel_button, &QPushButton::pressed, this, &CreateNodeDialog::on_cancel_node_creation_button_pressed);
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

void CreateNodeDialog::on_create_node_button_pressed() {
    this->accept();
}

void CreateNodeDialog::on_cancel_node_creation_button_pressed() {
    this->reject();
}

void CreateNodeDialog::update_selected_item() {
    QTreeWidgetItem* item = create_node_dialog_nodes_tree->currentItem();
    if (item) {
        selected_item = item;
        create_node_dialog_nodes_description->setText(item->data(0, Qt::UserRole + 1).toString());
    } else {
        selected_item = nullptr;
        create_node_dialog_nodes_description->setText("");
    }
}

/*************************************/
/* VisualShaderGraph                 */
/*************************************/

VisualShaderGraph::VisualShaderGraph() {

}

VisualShaderGraph::~VisualShaderGraph() {

}

std::unordered_set<NodeId> VisualShaderGraph::allNodeIds() const {
    return _node_ids;
}

std::unordered_set<ConnectionId> VisualShaderGraph::allConnectionIds(NodeId const node_id) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&node_id](ConnectionId const &cid) {
                     return cid.inNodeId == node_id || cid.outNodeId == node_id;
                 });

    return result;
}

std::unordered_set<ConnectionId> VisualShaderGraph::connections(NodeId node_id,
                                                                PortType port_type,
                                                                PortIndex port_index) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&port_type, &port_index, &node_id](ConnectionId const &cid) {
                     return (getNodeId(port_type, cid) == node_id
                             && getPortIndex(port_type, cid) == port_index);
                 });

    return result;
}

bool VisualShaderGraph::connectionExists(ConnectionId const connection_id) const {
    return (_connectivity.find(connection_id) != _connectivity.end());
}

void VisualShaderGraph::add_node_custom(const std::shared_ptr<VisualShaderNode>& node, const QPointF& offset) {
    NodeId new_id {newNodeId()};

    // Create new node.
    _node_ids.insert(new_id);

    // Add the node to the graph.
    visual_shader->add_node(node, {(float)offset.x(), (float)offset.y()}, (int)new_id);

    this->setNodeData(new_id, NodeRole::Position, offset);

    Q_EMIT nodeCreated(new_id);
}

NodeId VisualShaderGraph::addNode(QString const node_type) {
    std::cerr << "Unsupported operation: addNode" << std::endl;
    return -1;
}

bool VisualShaderGraph::connectionPossible(ConnectionId const connection_id) const {
    return _connectivity.find(connection_id) == _connectivity.end();
}

void VisualShaderGraph::addConnection(ConnectionId const connection_id) {
    _connectivity.insert(connection_id);

    Q_EMIT connectionCreated(connection_id);
}

bool VisualShaderGraph::nodeExists(NodeId const node_id) const {
    return (_node_ids.find(node_id) != _node_ids.end());
}

QVariant VisualShaderGraph::nodeData(NodeId node_id, NodeRole role) const {
    const std::shared_ptr<VisualShaderNode> n{visual_shader->get_node((int)node_id)};
    
    QVariant result;

    // Make sure the node exists.
    if (!n) {
        return result;
    }

    switch (role) {
    case NodeRole::Type:
        result = QString::fromStdString(n->get_caption());
        break;

    case NodeRole::Position:
        result = _node_geometry_data[node_id].pos;
        break;

    case NodeRole::Size:
        result = _node_geometry_data[node_id].size;
        break;

    case NodeRole::CaptionVisible:
        result = true;
        break;

    case NodeRole::Caption:
        result = QString::fromStdString(n->get_caption());
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        result = n->get_input_port_count();
        break;

    case NodeRole::OutPortCount:
        result = n->get_output_port_count();
        break;

    case NodeRole::Widget:
        result = QVariant();
        break;
    }

    return result;
}

bool VisualShaderGraph::setNodeData(NodeId node_id, NodeRole role, QVariant value) {
    bool result {false};

    switch (role) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        _node_geometry_data[node_id].pos = value.value<QPointF>();

        Q_EMIT nodePositionUpdated(node_id);

        result = true;
    } break;

    case NodeRole::Size: {
        _node_geometry_data[node_id].size = value.value<QSize>();
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

QVariant VisualShaderGraph::portData(NodeId node_id,
                                     PortType port_type,
                                     PortIndex port_index,
                                     PortRole role) const {
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
        if (port_type == PortType::In)
            return QString::fromUtf8("Port In");
        else
            return QString::fromUtf8("Port Out");

        break;
    }

    return QVariant();
}

bool VisualShaderGraph::setPortData(NodeId node_id, 
                                    PortType port_type, 
                                    PortIndex port_index, 
                                    QVariant const &value, 
                                    PortRole role) {
    Q_UNUSED(node_id);
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

bool VisualShaderGraph::deleteConnection(ConnectionId const connection_id) {
    bool disconnected = false;

    auto it = _connectivity.find(connection_id);

    if (it != _connectivity.end()) {
        disconnected = true;

        _connectivity.erase(it);
    }

    if (disconnected)
        Q_EMIT connectionDeleted(connection_id);

    return disconnected;
}

bool VisualShaderGraph::deleteNode(NodeId const node_id) {
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(node_id);
    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _node_ids.erase(node_id);
    _node_geometry_data.erase(node_id);

    Q_EMIT nodeDeleted(node_id);

    return true;
}
