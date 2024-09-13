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

#include "VisualShader.pb.h"
#include "ResourceTransformations/VisualShader/visual_shader_nodes.h"
#include "ResourceTransformations/VisualShader/vs_noise_nodes.h"

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderEditor                           *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent) : BaseEditor(model, parent), 
                                                                               visual_shader(nullptr),
                                                                               layout(nullptr),
                                                                               scene_layer_layout(nullptr), 
                                                                               scene_layer(nullptr), 
                                                                               scene(nullptr), 
                                                                               view(nullptr), 
                                                                               top_layer(nullptr),
                                                                               menu_bar(nullptr),
                                                                               create_node_button(nullptr),
                                                                               preview_shader_button(nullptr),
                                                                               create_node_action(nullptr),
                                                                               zoom_in_button(nullptr),
                                                                               reset_zoom_button(nullptr),
                                                                               zoom_out_button(nullptr),
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

    scene = new VisualShaderGraphicsScene(visual_shader);

    view = new VisualShaderGraphicsView(scene, scene_layer);
    view->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    scene_layer_layout->addWidget(view);

    // // Setup context menu for creating new nodes.
    // view->setContextMenuPolicy(Qt::ActionsContextMenu);
    // create_node_action = new QAction(QStringLiteral("Create Node"), view);
    // QObject::connect(create_node_action, &QAction::triggered, this, &VisualShaderEditor::on_create_node_action_triggered);
    // view->insertAction(view->actions().front(), create_node_action);

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

    zoom_in_button = new QPushButton("Zoom In", scene_layer);
    zoom_in_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    zoom_in_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(zoom_in_button);
    QObject::connect(zoom_in_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_in);

    reset_zoom_button = new QPushButton("Reset Zoom", scene_layer);
    reset_zoom_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    reset_zoom_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(reset_zoom_button);
    QObject::connect(reset_zoom_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::reset_zoom);

    zoom_out_button = new QPushButton("Zoom Out", scene_layer);
    zoom_out_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    zoom_out_button->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    menu_bar->addWidget(zoom_out_button);
    QObject::connect(zoom_out_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_out);

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

    this->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Set the window title and icon.
    this->setWindowTitle("Visual Shader Editor");
    // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
    this->setLayout(layout);
}

VisualShaderEditor::~VisualShaderEditor() {
    // TODO: We don't need to delete the pointers as they are destroyed when the parent is destroyed.
    if (create_node_dialog) delete create_node_dialog;
    if (zoom_out_button) delete zoom_out_button;
    if (reset_zoom_button) delete reset_zoom_button;
    if (zoom_in_button) delete zoom_in_button;
    if (create_node_action) delete create_node_action;
    if (preview_shader_button) delete preview_shader_button;
    if (create_node_button) delete create_node_button;
    if (menu_bar) delete menu_bar;
    if (top_layer) delete top_layer;
    if (view) delete view;
    if (scene) delete scene;
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

void VisualShaderEditor::create_node(const QPointF& coordinate) {
    QTreeWidgetItem* selected_item {create_node_dialog->get_selected_item()};

    if (!selected_item) {
        return;
    }

    VisualShaderEditor::add_node(selected_item, coordinate);
}

void VisualShaderEditor::add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate) {
    std::string type {selected_item->data(0, Qt::UserRole).toString().toStdString()};

    if (type.empty()) {
        return;
    }

    int new_node_id {visual_shader->get_valid_node_id()};

    if (new_node_id == VisualShader::NODE_ID_INVALID) {
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

    bool result {visual_shader->add_node(node, {(float)coordinate.x(), (float)coordinate.y()}, new_node_id)};

    if (!result) {
        return;
    }

    scene->add_node(new_node_id);
}

void VisualShaderEditor::show_create_node_dialog(const QPointF& coordinate) {
    int status {create_node_dialog->exec()};
    switch (status) {
        case QDialog::Accepted:
            std::cout << "Create node dialog accepted" << std::endl;
            VisualShaderEditor::create_node(coordinate);
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
    QPointF coordinate {view->mapToScene(view->mapFromGlobal(QCursor::pos()))};
    Q_EMIT on_create_node_dialog_requested(coordinate);
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

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               CreateNodeDialog                             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

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
    QTreeWidgetItem* item {create_node_dialog_nodes_tree->currentItem()};
    if (item) {
        selected_item = item;
        create_node_dialog_nodes_description->setText(item->data(0, Qt::UserRole + 1).toString());
    } else {
        selected_item = nullptr;
        create_node_dialog_nodes_description->setText("");
    }
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsScene                    *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

//////////////////////////////
// Public functions
//////////////////////////////

VisualShaderGraphicsScene::VisualShaderGraphicsScene(VisualShader* vs, QObject* parent) : QGraphicsScene(parent), vs(vs) {
	setItemIndexMethod(QGraphicsScene::NoIndex); // https://doc.qt.io/qt-6/qgraphicsscene.html#ItemIndexMethod-enum

    // Populate the scene with nodes
    std::vector<int> nodes {vs->get_nodes()};

    for (const int& node_id : nodes) {
        const std::shared_ptr<VisualShaderNode> node {vs->get_node(node_id)};

        if (!node) {
            continue;
        }

        VisualShaderNodeGraphicsObject* n_o {new VisualShaderNodeGraphicsObject(vs, node_id)};
        node_graphics_objects[node_id] = n_o;
        this->addItem(n_o);
        this->update();
    }

    // Populate the scene with connections
    std::vector<int> connections {vs->get_connections()};

    for (const int& connection_id : connections) {
        const VisualShader::Connection connection {vs->get_connection(connection_id)};

        VisualShaderConnectionGraphicsObject* c_o {new VisualShaderConnectionGraphicsObject()};
        connection_graphics_objects[connection_id] = c_o;
        this->addItem(c_o);
        this->update();
    }
}

VisualShaderGraphicsScene::~VisualShaderGraphicsScene() {
    
}

bool VisualShaderGraphicsScene::add_node(const int& n_id) {
    // Make sure the node doesn't already exist, we don't want to overwrite a node.
    if (node_graphics_objects.find(n_id) != node_graphics_objects.end()) {
        vs->remove_node(n_id);
        return false;
    }

    const std::shared_ptr<VisualShaderNode> node {vs->get_node(n_id)};
    
    if (!node) {
        vs->remove_node(n_id);
        return false;
    }

    VisualShaderNodeGraphicsObject* n_o {new VisualShaderNodeGraphicsObject(vs, n_id)};

    node_graphics_objects[n_id] = n_o;

    addItem(n_o);

    return true;
}

bool VisualShaderGraphicsScene::delete_node() {
    return false;
}

bool VisualShaderGraphicsScene::add_connection() {
    return false;
}

bool VisualShaderGraphicsScene::delete_connection() {
    return false;
}

VisualShaderNodeGraphicsObject* VisualShaderGraphicsScene::get_node_graphics_object(const int& n_id) const {
    VisualShaderNodeGraphicsObject* n_o {nullptr};

    auto it {node_graphics_objects.find(n_id)};
    if (it != node_graphics_objects.end()) {
        n_o = it->second;
    }

    return n_o;
}

VisualShaderConnectionGraphicsObject* VisualShaderGraphicsScene::get_connection_graphics_object(const int& c_id) const {
    VisualShaderConnectionGraphicsObject* c_o {nullptr};

    auto it {connection_graphics_objects.find(c_id)};
    if (it != connection_graphics_objects.end()) {
        c_o = it->second;
    }

    return c_o;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsView                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

//////////////////////////////
// Public functions
//////////////////////////////

VisualShaderGraphicsView::VisualShaderGraphicsView(VisualShaderGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent), 
                                                                                                        context_menu(nullptr),
                                                                                                        create_node_action(nullptr),
                                                                                                        delete_node_action(nullptr),
                                                                                                        zoom_in_action(nullptr),
                                                                                                        reset_zoom_action(nullptr),
                                                                                                        zoom_out_action(nullptr) {
    setDragMode(QGraphicsView::ScrollHandDrag);
	setRenderHint(QPainter::Antialiasing);

	setBackgroundBrush(this->background_color);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	setCacheMode(QGraphicsView::CacheBackground);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    // Allow dezooming 8 times from the default zoom level.
	zoom_min = (1.0f / std::pow(zoom_step, 8.0f));
	// Allow zooming 4 times from the default zoom level.
	zoom_max = (1.0f * std::pow(zoom_step, 4.0f));

	setSceneRect(rect_x, rect_y, rect_width, rect_height);

    reset_zoom();

    // Set the context menu
    context_menu = new QMenu(this);
    create_node_action = new QAction(QStringLiteral("Create Node"), context_menu);
    QObject::connect(create_node_action, &QAction::triggered, this, &VisualShaderGraphicsView::on_create_node_action_triggered);
    context_menu->addAction(create_node_action);

    delete_node_action = new QAction(QStringLiteral("Delete Node"), context_menu);
    delete_node_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
	delete_node_action->setShortcut(QKeySequence(QKeySequence::Delete));
    QObject::connect(delete_node_action, &QAction::triggered, this, &VisualShaderGraphicsView::on_delete_node_action_triggered);
    context_menu->addAction(delete_node_action);

    zoom_in_action = new QAction(QStringLiteral("Zoom In"), context_menu);
    zoom_in_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    zoom_in_action->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    QObject::connect(zoom_in_action, &QAction::triggered, this, &VisualShaderGraphicsView::zoom_in);
    context_menu->addAction(zoom_in_action);

    reset_zoom_action = new QAction(QStringLiteral("Reset Zoom"), context_menu);
    QObject::connect(reset_zoom_action, &QAction::triggered, this, &VisualShaderGraphicsView::reset_zoom);
    context_menu->addAction(reset_zoom_action);

    zoom_out_action = new QAction(QStringLiteral("Zoom Out"), context_menu);
    zoom_out_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    zoom_out_action->setShortcut(QKeySequence(QKeySequence::ZoomOut));
    QObject::connect(zoom_out_action, &QAction::triggered, this, &VisualShaderGraphicsView::zoom_out);
    context_menu->addAction(zoom_out_action);
}

VisualShaderGraphicsView::~VisualShaderGraphicsView() {
    if (zoom_out_action) delete zoom_out_action;
    if (reset_zoom_action) delete reset_zoom_action;
    if (zoom_in_action) delete zoom_in_action;
    if (create_node_action) delete create_node_action;
    if (context_menu) delete context_menu;
}

//////////////////////////////
// Private slots
//////////////////////////////

void VisualShaderGraphicsView::on_create_node_action_triggered() {
    
}

void VisualShaderGraphicsView::on_delete_node_action_triggered() {
    
}

void VisualShaderGraphicsView::zoom_in() {
	const float factor {std::pow(zoom_step, zoom)};

	QTransform t {transform()};
    t.scale(factor, factor);
    if (t.m11() >= zoom_max) {
        return;
    }

	scale(factor, factor);
	Q_EMIT zoom_changed(transform().m11());
}

void VisualShaderGraphicsView::reset_zoom() {
    if ((float)transform().m11() == zoom) {
        return;
    }

    resetTransform(); // Reset the zoom level to 1.0f
    Q_EMIT zoom_changed(transform().m11());
}

void VisualShaderGraphicsView::zoom_out() {
	const float factor {std::pow(zoom_step, -1.0f * zoom)};

	QTransform t {transform()};
    t.scale(factor, factor);
    if (t.m11() <= zoom_min) {
        return;
    }

	scale(factor, factor);
	Q_EMIT zoom_changed(transform().m11());
}

//////////////////////////////
// Private functions
//////////////////////////////

void VisualShaderGraphicsView::drawBackground(QPainter *painter, const QRectF &r) {
    QGraphicsView::drawBackground(painter, r);

	std::function<void(float)> draw_grid = [&](float grid_step) {
		QRect window_rect {this->rect()};

		QPointF tl {mapToScene(window_rect.topLeft())};
		QPointF br {mapToScene(window_rect.bottomRight())};

        float left {std::floor((float)tl.x() / grid_step)};
        float right {std::ceil((float)br.x() / grid_step)};
        float bottom {std::floor((float)tl.y() / grid_step)};
        float top {std::ceil((float)br.y() / grid_step)};

		// Vertical lines
		for (int xi {(int)left}; xi <= (int)right; ++xi) {
			QLineF line(xi * grid_step, bottom * grid_step, xi * grid_step, top * grid_step);
			painter->drawLine(line);
		}

		// Horizontal lines
		for (int yi {(int)bottom}; yi <= (int)top; ++yi) {
			QLineF line(left * grid_step, yi * grid_step, right * grid_step, yi * grid_step);
			painter->drawLine(line);
		}
	};

	QPen fine_pen(this->fine_grid_color, 1.0f);
    painter->setPen(fine_pen);
	draw_grid(15.0f);

    QPen coarse_pen(this->coarse_grid_color, 1.0f);
    painter->setPen(coarse_pen);
    draw_grid(150.0f);
}

void VisualShaderGraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    QGraphicsView::contextMenuEvent(event);

    if (itemAt(event->pos())) {
		return;
	}

	QPointF scene_coordinate {mapToScene(event->pos())};

	context_menu->exec(event->globalPos());
}

void VisualShaderGraphicsView::wheelEvent(QWheelEvent *event) {
    float t_zoom {(float)transform().m11()};

	const QPoint delta {event->angleDelta()};

	if (delta.y() == 0) {
		event->ignore();
		return;
	}

    if (delta.y() > 0 && (std::abs(t_zoom - zoom_max) > std::numeric_limits<double>::epsilon())) {
        zoom_in();
    } else if (delta.y() < 0 && (std::abs(t_zoom - zoom_min) > std::numeric_limits<double>::epsilon())) {
        zoom_out();
    } else {
        event->ignore();
    }
}

void VisualShaderGraphicsView::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

    switch (event->button()) {
        case Qt::LeftButton:
            last_click_coordinate = mapToScene(event->pos());
            break;
        default:
            break;
    }
}

void VisualShaderGraphicsView::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

    switch (event->buttons()) {
        case Qt::LeftButton:
            {
                QPointF current_coordinate {mapToScene(event->pos())};
                QPointF difference {last_click_coordinate - current_coordinate};
                setSceneRect(sceneRect().translated(difference.x(), difference.y()));
                last_click_coordinate = current_coordinate;
            }
            break;
        default:
            break;
    }
}

void VisualShaderGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

void VisualShaderGraphicsView::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

    if (!scene()) {
        return;
    }

    if (scene()->items().isEmpty()) {
        return;
    }

    std::cout << "Changing view port to fit items..." << std::endl;

	move_view_to_fit_items();
}

void VisualShaderGraphicsView::move_view_to_fit_items() {
    QRectF items_bounding_rect {scene()->itemsBoundingRect()};
    items_bounding_rect.adjust(-fit_in_view_margin, -fit_in_view_margin, fit_in_view_margin, fit_in_view_margin);

    QPointF scene_tl {this->scene()->sceneRect().topLeft()};
    QPointF scene_br {this->scene()->sceneRect().bottomRight()};
    QPointF items_tl {items_bounding_rect.topLeft()};
    QPointF items_br {items_bounding_rect.bottomRight()};

    // Make sure the items bounding rect is inside the scene rect
    if (items_tl.x() < scene_tl.x()) {
        items_bounding_rect.setLeft(scene_tl.x());
    }

    if (items_tl.y() > scene_tl.y()) {
        items_bounding_rect.setTop(scene_tl.y());
    }

    if (items_br.x() > scene_br.x()) {
        items_bounding_rect.setRight(scene_br.x());
    }

    if (items_br.y() < scene_br.y()) {
        items_bounding_rect.setBottom(scene_br.y());
    }

    fitInView(items_bounding_rect, Qt::KeepAspectRatio);

    centerOn(items_bounding_rect.center());
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderNodeGraphicsObject               *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderNodeGraphicsObject::VisualShaderNodeGraphicsObject(VisualShader* vs, const int& n_id, QGraphicsItem* parent) : QGraphicsObject(parent), 
                                                                                                                           vs(vs),
                                                                                                                           n_id(n_id) {
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    setVisible(true);
	setOpacity(this->opacity);

	setZValue(0);

    TVector2 coordinate {vs->get_node_coordinate(n_id)};

    std::cout << "Node ID: " << n_id << " Coordinate: " << coordinate.x << ", " << coordinate.y << std::endl;

	setPos(coordinate.x, coordinate.y);
}

VisualShaderNodeGraphicsObject::~VisualShaderNodeGraphicsObject() {
    
}

QRectF VisualShaderNodeGraphicsObject::boundingRect() const {
	float w_margin {(float)size.width() * rect_margin_ratio};
	float h_margin {(float)size.height() * rect_margin_ratio};

	QMargins margins((int)w_margin, (int)h_margin, (int)w_margin, (int)h_margin);

	QRectF r({0.0f, 0.0f}, size);

    std::cout << "Bounding rect: " << r.x() << ", " << r.y() << ", " << r.width() << ", " << r.height() << std::endl;

	return r.marginsAdded(margins);
}

void VisualShaderNodeGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    std::cout << " ----------------- Paint method called for node ID: " << n_id << std::endl;
    
    const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

    if (!n) {
        return;
    }

    painter->setClipRect(option->exposedRect);

    {
        // Draw Node Rect
        QColor rect_color;
        if (isSelected()) {
            rect_color = this->normal_boundary_color;
        } else {
            rect_color = this->selected_boundary_color;
        }

        QPen p(rect_color, this->pen_width);
        painter->setPen(p);

        painter->setBrush(this->fill_color);

        QRectF boundary(0, 0, size.width(), size.height());

        painter->drawRoundedRect(boundary, this->corner_radius, this->corner_radius);
    }

    {
        // Draw Caption
        QString caption {QString::fromStdString(n->get_caption())};

        QFont f {painter->font()};
        f.setBold(true);
        QFontMetrics fm(f);
        QRect text_rect {fm.boundingRect(caption)};

        // Calculate the coordinate of the caption
        float x {0.5f * (float)(size.width() - text_rect.width())};
        float y {0.5f * this->port_spacing + (float)text_rect.height()};
        QPointF coordinate {QPointF(x, y)};

        painter->setFont(f);
        painter->setPen(this->font_color);
        painter->drawText(coordinate, caption);

        f.setBold(false);
        painter->setFont(f);
    }

    {
        // Draw Input Ports
        const int in_port_count {n->get_input_port_count()};
    }

    {
        // Draw Output Ports
    }
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****           VisualShaderConnectionGraphicsObject             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderConnectionGraphicsObject::VisualShaderConnectionGraphicsObject(QGraphicsItem* parent) : QGraphicsObject(parent) {
    
}

VisualShaderConnectionGraphicsObject::~VisualShaderConnectionGraphicsObject() {
    
}

QRectF VisualShaderConnectionGraphicsObject::boundingRect() const {
    return QRectF();
}

void VisualShaderConnectionGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               NodesCustomWidget                            *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

NodesCustomWidget::NodesCustomWidget(const std::shared_ptr<VisualShaderNode>& node, QWidget* parent) : QWidget(parent), 
                                                                                                       layout(nullptr) {
    // Create the main layout.
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    //////////////// End of Header ////////////////

    combo_boxes[0] = new QComboBox();
    combo_boxes[0]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    combo_boxes[0]->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    combo_boxes[0]->setMaximumSize(combo_boxes[0]->sizeHint());

    // Add items to the combo box
    combo_boxes[0]->addItem("Item 1");
    combo_boxes[0]->addItem("Item 2");
    combo_boxes[0]->addItem("Item 3");

    layout->addWidget(combo_boxes[0]);

    // Connect the combo box signal to the slot
    QObject::connect(combo_boxes[0], QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NodesCustomWidget::on_combo_box0_current_index_changed);


    if (std::dynamic_pointer_cast<VisualShaderNodeInput>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeColorConstant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeBooleanConstant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeFloatConstant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeIntConstant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeUIntConstant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeVec2Constant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeVec3Constant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeVec4Constant>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeFloatFunc>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeIntFunc>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeUIntFunc>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeDerivativeFunc>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeFloatOp>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeIntOp>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeUIntOp>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeValueNoise>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeCompare>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeIf>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeIs>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeSwitch>(node)) {

    } else if (std::dynamic_pointer_cast<VisualShaderNodeOutput>(node)) {

    } else {
        std::cout << "--- Unknown node type ---" << std::endl;
    }

    //////////////// Start of Footer ////////////////

    // TODO: Set the size of this widget based on the contents.

    this->setContentsMargins(0, 0, 0, 0); // Left, top, right, bottom
    // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->setLayout(layout);
}

NodesCustomWidget::~NodesCustomWidget() {
    if (layout) delete layout;
}

void NodesCustomWidget::on_combo_box0_current_index_changed(const int& index) {
    std::cout << "Combo box index changed: " << index << std::endl;
}
