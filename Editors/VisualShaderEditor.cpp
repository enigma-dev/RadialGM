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

#include "Editors/VisualShaderEditor.h"

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

    // Instantiate the node based on the type
    std::shared_ptr<VisualShaderNode> n;

    if (type == "VisualShaderNodeInput") {
        n = std::make_shared<VisualShaderNodeInput>();
    } else if (type == "VisualShaderNodeColorConstant") {
        n = std::make_shared<VisualShaderNodeColorConstant>();
    } else if (type == "VisualShaderNodeBooleanConstant") {
        n = std::make_shared<VisualShaderNodeBooleanConstant>();
    } else if (type == "VisualShaderNodeFloatConstant") {
        n = std::make_shared<VisualShaderNodeFloatConstant>();
    } else if (type == "VisualShaderNodeIntConstant") {
        n = std::make_shared<VisualShaderNodeIntConstant>();
    } else if (type == "VisualShaderNodeUIntConstant") {
        n = std::make_shared<VisualShaderNodeUIntConstant>();
    } else if (type == "VisualShaderNodeVec2Constant") {
        n = std::make_shared<VisualShaderNodeVec2Constant>();
    } else if (type == "VisualShaderNodeVec3Constant") {
        n = std::make_shared<VisualShaderNodeVec3Constant>();
    } else if (type == "VisualShaderNodeVec4Constant") {
        n = std::make_shared<VisualShaderNodeVec4Constant>();
    } else if (type == "VisualShaderNodeFloatFunc") {
        n = std::make_shared<VisualShaderNodeFloatFunc>();
    } else if (type == "VisualShaderNodeIntFunc") {
        n = std::make_shared<VisualShaderNodeIntFunc>();
    } else if (type == "VisualShaderNodeUIntFunc") {
        n = std::make_shared<VisualShaderNodeUIntFunc>();
    } else if (type == "VisualShaderNodeDerivativeFunc") {
        n = std::make_shared<VisualShaderNodeDerivativeFunc>();
    } else if (type == "VisualShaderNodeFloatOp") {
        n = std::make_shared<VisualShaderNodeFloatOp>();
    } else if (type == "VisualShaderNodeIntOp") {
        n = std::make_shared<VisualShaderNodeIntOp>();
    } else if (type == "VisualShaderNodeUIntOp") {
        n = std::make_shared<VisualShaderNodeUIntOp>();
    } else if (type == "VisualShaderNodeValueNoise") {
        n = std::make_shared<VisualShaderNodeValueNoise>();
    } else if (type == "VisualShaderNodeCompare") {
        n = std::make_shared<VisualShaderNodeCompare>();
    } else if (type == "VisualShaderNodeIf") {
        n = std::make_shared<VisualShaderNodeIf>();
    } else if (type == "VisualShaderNodeIs") {
        n = std::make_shared<VisualShaderNodeIs>();
    } else if (type == "VisualShaderNodeSwitch") {
        n = std::make_shared<VisualShaderNodeSwitch>();
    } else {
        std::cout << "Unknown node type: " << type << std::endl;
    }

    if (!n) {
        std::cout << "Failed to create node of type: " << type << std::endl;
        return;
    }

    int n_id {visual_shader->get_valid_node_id()};

    if (n_id == (int)VisualShader::NODE_ID_INVALID) {
        return;
    }

    scene->add_node(n_id, n, coordinate);
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

VisualShaderGraphicsScene::VisualShaderGraphicsScene(VisualShader* vs, QObject* parent) : QGraphicsScene(parent), 
                                                                                          vs(vs),
                                                                                          temporary_connection_graphics_object(nullptr) {
	setItemIndexMethod(QGraphicsScene::NoIndex); // https://doc.qt.io/qt-6/qgraphicsscene.html#ItemIndexMethod-enum

    QObject::connect(this, &VisualShaderGraphicsScene::node_moved, this, &VisualShaderGraphicsScene::on_node_moved);
}

VisualShaderGraphicsScene::~VisualShaderGraphicsScene() {}

bool VisualShaderGraphicsScene::add_node(const int& n_id, const std::shared_ptr<VisualShaderNode>& node, const QPointF& coordinate) {
    // Make sure the node doesn't already exist, we don't want to overwrite a node.
    if (node_graphics_objects.find(n_id) != node_graphics_objects.end()) {
        return false;
    }

    QList<QGraphicsView *> views {this->views()};
    if (views.isEmpty()) {
        std::cout << "No views available" << std::endl;
        return false;
    }

    // The output node cannot be removed or added by the user
    if (n_id >= VisualShader::NODE_ID_OUTPUT + 1) {
        bool result {vs->add_node(node, {(float)coordinate.x(), (float)coordinate.y()}, n_id)};

        if (!result) {
            return false;
        }
    }

    VisualShaderGraphicsView* view {dynamic_cast<VisualShaderGraphicsView *>(views.first())};

    if (vs->get_node_coordinate(n_id).x < view->get_x() || 
        vs->get_node_coordinate(n_id).x > view->get_x() + view->get_width() ||
        vs->get_node_coordinate(n_id).y < view->get_y() || 
        vs->get_node_coordinate(n_id).y > view->get_y() + view->get_height()) {
        std::cout << "Node is out of view bounds" << std::endl;
    }

    VisualShaderNodeGraphicsObject* n_o {new VisualShaderNodeGraphicsObject(vs, n_id)};

    node_graphics_objects[n_id] = n_o;

    addItem(n_o);

    return true;
}

bool VisualShaderGraphicsScene::delete_node(const int& n_id) {
    bool result {vs->remove_node(n_id)};

    if (!result) {
        return false;
    }

    const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

    if (!n) {
        return false;
    }

    VisualShaderNodeGraphicsObject* n_o {this->get_node_graphics_object(n_id)};

    if (!n_o) {
        return false;
    }

    // Remove all connections to the node
    for (int i{0}; i < n->get_input_port_count(); i++) {
        VisualShaderInputPortGraphicsObject* i_port {n_o->get_input_port_graphics_object(i)};

        if (!i_port || !i_port->is_connected()) {
            continue;
        }

        // Get the output port of the connection
        VisualShaderConnectionGraphicsObject* c_o {i_port->get_connection_graphics_object()};

        if (!c_o) {
            continue;
        }

        VisualShaderNodeGraphicsObject* from_n_o {this->get_node_graphics_object(c_o->get_from_node_id())};

        if (!from_n_o) {
            continue;
        }

        VisualShaderOutputPortGraphicsObject* o_port {from_n_o->get_output_port_graphics_object(c_o->get_from_port_index())};

        if (!o_port) {
            continue;
        }

        bool result {vs->disconnect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i)};

        if (!result) {
            std::cout << "Failed to disconnect nodes" << std::endl;
            continue;
        }

        result = this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i);

        if (!result) {
            std::cout << "Failed to delete connection" << std::endl;
            continue;
        }
    }

    for (int i{0}; i < n->get_output_port_count(); i++) {
        VisualShaderOutputPortGraphicsObject* o_port {n_o->get_output_port_graphics_object(i)};

        if (!o_port || !o_port->is_connected()) {
            continue;
        }

        // Get the input port of the connection
        VisualShaderConnectionGraphicsObject* c_o {o_port->get_connection_graphics_object()};

        if (!c_o) {
            continue;
        }

        VisualShaderNodeGraphicsObject* to_n_o {this->get_node_graphics_object(c_o->get_to_node_id())};

        if (!to_n_o) {
            continue;
        }

        VisualShaderInputPortGraphicsObject* i_port {to_n_o->get_input_port_graphics_object(c_o->get_to_port_index())};

        if (!i_port) {
            continue;
        }

        bool result {vs->disconnect_nodes(n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index())};

        if (!result) {
            std::cout << "Failed to disconnect nodes" << std::endl;
            continue;
        }

        result = this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i);

        if (!result) {
            std::cout << "Failed to delete connection" << std::endl;
            continue;
        }
    }

    // Remove the node from the scene
    removeItem(n_o);
    delete n_o; // Make sure to delete the node object to delete all child ports

    return true;
}

bool VisualShaderGraphicsScene::add_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id, const int& to_port_index) {
    QList<QGraphicsView *> views {this->views()};
    if (views.isEmpty()) {
        std::cout << "No views available" << std::endl;
        return false;
    }
    
    // Create the connection and set its start
    VisualShaderNodeGraphicsObject* from_n_o {this->get_node_graphics_object(from_node_id)};

    if (!from_n_o) {
        return false;
    }

    VisualShaderOutputPortGraphicsObject* from_o_port {from_n_o->get_output_port_graphics_object(from_port_index)};

    if (!from_o_port) {
        return false;
    }

    VisualShaderGraphicsView* view {dynamic_cast<VisualShaderGraphicsView *>(views.first())};

    if (from_o_port->get_global_coordinate().x() < view->get_x() || 
        from_o_port->get_global_coordinate().x() > view->get_x() + view->get_width()) {
        std::cout << "Start of connection is out of view bounds" << std::endl;
    }

    this->temporary_connection_graphics_object = new VisualShaderConnectionGraphicsObject(from_node_id, from_port_index, from_o_port->get_global_coordinate());
    from_o_port->connect(this->temporary_connection_graphics_object);
    addItem(this->temporary_connection_graphics_object);

    if (to_node_id != (int)VisualShader::NODE_ID_INVALID && to_port_index != (int)VisualShader::PORT_INDEX_INVALID) {
        // Set the end of the connection
        VisualShaderNodeGraphicsObject* to_n_o {this->get_node_graphics_object(to_node_id)};

        if (!to_n_o) {
            return false;
        }

        VisualShaderInputPortGraphicsObject* to_i_port {to_n_o->get_input_port_graphics_object(to_port_index)};

        if (!to_i_port) {
            return false;
        }

        if (to_i_port->get_global_coordinate().y() < view->get_y() || 
            to_i_port->get_global_coordinate().y() > view->get_y() + view->get_height()) {
            std::cout << "End of connection is out of view bounds" << std::endl;
        }

        this->temporary_connection_graphics_object->set_end_coordinate(to_i_port->get_global_coordinate());
        to_i_port->connect(this->temporary_connection_graphics_object);
        this->temporary_connection_graphics_object->set_to_node_id(to_node_id);
        this->temporary_connection_graphics_object->set_to_port_index(to_port_index);
        this->temporary_connection_graphics_object = nullptr; // Make sure to reset the temporary connection object

        // Connect the nodes in the VisualShader
        bool result {vs->can_connect_nodes(from_node_id, from_port_index, to_node_id, to_port_index)};
        if (!result) {
            std::cout << "Can't connect nodes" << std::endl;
            return false;
        }

        result = vs->connect_nodes(from_node_id, from_port_index, to_node_id, to_port_index);
        if (!result) {
            std::cout << "Failed to connect nodes" << std::endl;
            return false;
        }
    }

    return true;
}

bool VisualShaderGraphicsScene::delete_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id, const int& to_port_index) {
    VisualShaderNodeGraphicsObject* from_n_o {this->get_node_graphics_object(from_node_id)};

    if (!from_n_o) {
        return false;
    }

    VisualShaderOutputPortGraphicsObject* from_o_port {from_n_o->get_output_port_graphics_object(from_port_index)};

    if (!from_o_port) {
        return false;
    }

    VisualShaderConnectionGraphicsObject* c_o {from_o_port->get_connection_graphics_object()};

    if (!c_o) {
        return false;
    }

    if (to_node_id != (int)VisualShader::NODE_ID_INVALID && to_port_index != (int)VisualShader::PORT_INDEX_INVALID) {
        VisualShaderNodeGraphicsObject* to_n_o {this->get_node_graphics_object(to_node_id)};

        if (!to_n_o) {
            return false;
        }

        VisualShaderInputPortGraphicsObject* to_i_port {to_n_o->get_input_port_graphics_object(to_port_index)};

        if (!to_i_port) {
            return false;
        }

        bool result {vs->disconnect_nodes(from_node_id, from_port_index, to_node_id, to_port_index)};

        if (!result) {
            return false;
        }

        to_i_port->detach_connection();
    }

    from_o_port->detach_connection();
    removeItem(c_o);
    delete c_o;

    return true;
}

VisualShaderNodeGraphicsObject* VisualShaderGraphicsScene::get_node_graphics_object(const int& n_id) const {
    VisualShaderNodeGraphicsObject* n_o {nullptr};

    auto it {node_graphics_objects.find(n_id)};
    if (it != node_graphics_objects.end()) {
        n_o = it->second;
    }

    return n_o;
}

void VisualShaderGraphicsScene::on_node_moved(const int& n_id, const QPointF& new_coordinate) {
    const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

    if (!n) {
        return;
    }

    // Update the node's coordinate in the VisualShader
    vs->set_node_coordinate(n_id, {(float)new_coordinate.x(), (float)new_coordinate.y()});

    // Update coordinates of all connected connections
    VisualShaderNodeGraphicsObject* n_o {this->get_node_graphics_object(n_id)};

    for (int i{0}; i < n->get_input_port_count(); i++) {
        VisualShaderInputPortGraphicsObject* i_port {n_o->get_input_port_graphics_object(i)};

        if (!i_port || !i_port->is_connected()) {
            continue;
        }

        VisualShaderConnectionGraphicsObject* c_o {i_port->get_connection_graphics_object()};

        if (!c_o) {
            continue;
        }

        c_o->set_end_coordinate(i_port->get_global_coordinate());
    }
    
    for (int i{0}; i < n->get_output_port_count(); i++) {
        VisualShaderOutputPortGraphicsObject* o_port {n_o->get_output_port_graphics_object(i)};

        if (!o_port || !o_port->is_connected()) {
            continue;
        }

        VisualShaderConnectionGraphicsObject* c_o {o_port->get_connection_graphics_object()};

        if (!c_o) {
            continue;
        }

        c_o->set_start_coordinate(o_port->get_global_coordinate());
    }
}

void VisualShaderGraphicsScene::on_port_pressed(QGraphicsObject* port, const QPointF& coordinate) {}

void VisualShaderGraphicsScene::on_port_dragged(QGraphicsObject* port, const QPointF& coordinate) {
    VisualShaderConnectionGraphicsObject* c_o {nullptr};
    
    VisualShaderOutputPortGraphicsObject* o_port {dynamic_cast<VisualShaderOutputPortGraphicsObject *>(port)};

    if (!o_port) {
        VisualShaderInputPortGraphicsObject* i_port {dynamic_cast<VisualShaderInputPortGraphicsObject *>(port)};

        if (!i_port) {
            return;
        }

        if (i_port->is_connected() && !temporary_connection_graphics_object) {
            c_o = i_port->get_connection_graphics_object();
            temporary_connection_graphics_object = c_o; // Store the connection object for access in the next drag call
            bool result {vs->disconnect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(), c_o->get_to_port_index())};
            if (!result) {
                std::cout << "Failed to disconnect nodes" << std::endl;
            }
            i_port->detach_connection();
            c_o->detach_end();
        } else if (!i_port->is_connected() && temporary_connection_graphics_object) {
            c_o = temporary_connection_graphics_object;
        } else {
            return;
        }

        c_o->set_end_coordinate(coordinate);

        return;
    }

    if (!o_port->is_connected() && !temporary_connection_graphics_object) {
        bool result {this->add_connection(o_port->get_node_id(), o_port->get_port_index())};
        if (!result) {
            std::cout << "Failed to add connection" << std::endl;
            return;
        }
        c_o = temporary_connection_graphics_object;
    } else if (o_port->is_connected() && temporary_connection_graphics_object) {
        c_o = temporary_connection_graphics_object;
    } else if (o_port->is_connected() && !temporary_connection_graphics_object) {
        c_o = o_port->get_connection_graphics_object();
        temporary_connection_graphics_object = c_o; // Store the connection object for access in the next drag call

        // Detach the connection from the input port
        VisualShaderNodeGraphicsObject* n_o {this->get_node_graphics_object(c_o->get_to_node_id())};
        if (!n_o) {
            return;
        }
        VisualShaderInputPortGraphicsObject* i_port {n_o->get_input_port_graphics_object(c_o->get_to_port_index())};
        if (!i_port) {
            return;
        }
        bool result {vs->disconnect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(), c_o->get_to_port_index())};
        if (!result) {
            std::cout << "Failed to disconnect nodes" << std::endl;
        }
        i_port->detach_connection();
        c_o->detach_end();
    } else {
        return;
    }

    c_o->set_end_coordinate(coordinate);
}

void VisualShaderGraphicsScene::on_port_dropped(QGraphicsObject* port, const QPointF& coordinate) {
    VisualShaderConnectionGraphicsObject* c_o {temporary_connection_graphics_object};
    temporary_connection_graphics_object = nullptr; // Reset the temporary connection object

    if (!c_o) {
        return;
    }

    // Find all items under the coordinate
    QList<QGraphicsItem*> items_at_coordinate {this->items(coordinate)};

    // Iterate through the items and check if an input port is under the mouse
    VisualShaderInputPortGraphicsObject* in_p_o {nullptr};
    for (QGraphicsItem* item : items_at_coordinate) {
        // Check if the item is an input port
        in_p_o = dynamic_cast<VisualShaderInputPortGraphicsObject *>(item);

        if (in_p_o) {
            break;
        }
    }
    
    VisualShaderOutputPortGraphicsObject* o_port {dynamic_cast<VisualShaderOutputPortGraphicsObject *>(port)};

    if (!o_port) {
        VisualShaderInputPortGraphicsObject* i_port {dynamic_cast<VisualShaderInputPortGraphicsObject *>(port)};

        if (!i_port) {
            return;
        }

        if (!in_p_o) {
            bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

            if (!result) {
                std::cout << "Failed to delete connection" << std::endl;
            }

            return; // Return because we dragging an input port and dropped on nothing
        }

        bool result {vs->can_connect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index())};

        if (!result) {
            bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

            if (!result) {
                std::cout << "Failed to delete connection" << std::endl;
            }

            return; // Return because we dragging an input port and dropped on nothing
        }

        // Connect the nodes
        result = vs->connect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index());

        if (!result) {
            bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

            if (!result) {
                std::cout << "Failed to delete connection" << std::endl;
            }

            return; // Return because we dragging an input port and dropped on nothing
        }

        c_o->set_to_node_id(in_p_o->get_node_id());
        c_o->set_to_port_index(in_p_o->get_port_index());
        c_o->set_end_coordinate(in_p_o->get_global_coordinate());
        in_p_o->connect(c_o);
        return;
    }

    if (!in_p_o) {
        bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

        if (!result) {
            std::cout << "Failed to delete connection" << std::endl;
        }

        return; // Return because we dragging an output port and dropped on nothing
    }

    bool result {vs->can_connect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index())};

    if (!result) {
        bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

        if (!result) {
            std::cout << "Failed to delete connection" << std::endl;
        }

        return;
    }

    // Connect the nodes
    result = vs->connect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index());

    if (!result) {
        bool result {this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

        if (!result) {
            std::cout << "Failed to delete connection" << std::endl;
        }

        return;
    }

    c_o->set_to_node_id(in_p_o->get_node_id());
    c_o->set_to_port_index(in_p_o->get_port_index());
    c_o->set_end_coordinate(in_p_o->get_global_coordinate());
    in_p_o->connect(c_o);
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

    VisualShader* vs {scene->get_visual_shader()};

    // Load the nodes and connections from the VisualShader
    std::vector<int> ns {vs->get_nodes()};
    for (const int& n_id : ns) {
        const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

        if (!n) {
            continue;
        }

        TVector2 c {vs->get_node_coordinate(n_id)};

        scene->add_node(n_id, n, {c.x, c.y});
    }

    std::vector<VisualShader::Connection> cs {vs->get_connections()};
    for (const VisualShader::Connection& c : cs) {
        scene->add_connection(c.from_node, c.from_port, c.to_node, c.to_port);
    }
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

	move_view_to_fit_items();
}

void VisualShaderGraphicsView::move_view_to_fit_items() {
    if (!scene()) {
        return;
    }

    if (scene()->items().isEmpty()) {
        return;
    }

    std::cout << "Changing view port to fit items..." << std::endl;

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

    if ((float)transform().m11() > zoom_max) {
        std::cout << "Current zoom level is greater than the maximum zoom level." << std::endl;
        std::cout << "Maybe due to having a very large distance between the nodes." << std::endl;
    }

    if ((float)transform().m11() < zoom_min) {
        std::cout << "Current zoom level is less than the minimum zoom level." << std::endl;
        std::cout << "Maybe due to having all the nodes outside the scene bounds." << std::endl;
    }
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
                                                                                                                           n_id(n_id),
                                                                                                                           rect_width(0.0f),
                                                                                                                           caption_rect_height(0.0f),
                                                                                                                           rect_height(0.0f),
                                                                                                                           rect_margin(0.0f),
                                                                                                                           rect_padding(0.0f) {
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

	setPos(coordinate.x, coordinate.y);
}

VisualShaderNodeGraphicsObject::~VisualShaderNodeGraphicsObject() {
    
}

VisualShaderInputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_input_port_graphics_object(const int& p_index) const {
    if (in_port_graphics_objects.find(p_index) != in_port_graphics_objects.end()) {
        return in_port_graphics_objects.at(p_index);
    }

    return nullptr;
}

VisualShaderOutputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_output_port_graphics_object(const int& p_index) const {
    if (out_port_graphics_objects.find(p_index) != out_port_graphics_objects.end()) {
        return out_port_graphics_objects.at(p_index);
    }

    return nullptr;
}

QRectF VisualShaderNodeGraphicsObject::boundingRect() const {
    const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

    if (!n) {
        return QRectF();
    }

    QFont f("Arial", caption_font_size);
    f.setBold(true);
    QFontMetrics fm(f);

    QString caption {QString::fromStdString(n->get_caption())};

    rect_width = (float)(fm.horizontalAdvance(caption, caption.length()) + 20.0f);
    caption_rect_height = (float)((fm.height()) + 30.0f);

    int max_num_ports {qMax(n->get_input_port_count(), n->get_output_port_count())};

    // Calculate the height of the node
    float t_rect_h {caption_rect_height};

    t_rect_h += body_rect_header_height; // Header
    if (max_num_ports >= 0) {
        t_rect_h += (float)(max_num_ports - 1) * body_rect_port_step; // Ports
    }
    t_rect_h += body_rect_footer_height; // Footer

    rect_height = t_rect_h;

	QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

    // Calculate the margin
    this->rect_margin = rect_width * 0.1f;

    // Calculate the rect padding
    // We add a safe area around the rect to make it easier to get an accurate coordinate of the size
    this->rect_padding = rect_width * 0.15f;

    r.adjust(-rect_margin - rect_padding, -rect_margin - rect_padding,
             rect_margin + rect_padding, rect_margin + rect_padding);

	return r;
}

void VisualShaderNodeGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    const std::shared_ptr<VisualShaderNode> n {vs->get_node(n_id)};

    if (!n) {
        return;
    }

    int n_id {vs->find_node_id(n)};

    if (n_id == (int)VisualShader::NODE_ID_INVALID) {
        return;
    }

    painter->setClipRect(option->exposedRect);

    // Get the rect without the padding
    QRectF r {this->boundingRect()};
    r.adjust(rect_padding, rect_padding, -rect_padding, -rect_padding);

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

        painter->drawRoundedRect(r, this->corner_radius, this->corner_radius);
    }

    // Add the margin to the rect
    r.adjust(rect_margin, rect_margin, -rect_margin, -rect_margin);

    float rect_x {(float)r.topLeft().x()};
    float rect_y {(float)r.topLeft().y()};

    float rect_w {(float)r.width()};
    float rect_h {(float)r.height()};

    float min_side {qMin(rect_w, rect_h)};

    QRectF caption_rect(rect_x, rect_y, rect_w, caption_rect_height);

    {
        // Draw Caption
        QString caption {QString::fromStdString(n->get_caption())};

        QFont t_f {painter->font()};

        QFont f("Arial", caption_font_size);
        f.setBold(true);
        QFontMetrics fm(f);
        painter->setFont(f);

        // Calculate the coordinates of the caption
        float x {(float)(caption_rect.center().x() - (float)fm.horizontalAdvance(caption) * 0.5f)};

        // Instead of subtracting, add the ascent to properly align text within the rect
        float y {(float)(caption_rect.center().y() + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent()))};

        QPointF coordinate {x, y};

        painter->setPen(this->font_color);
        painter->drawText(coordinate, caption);

        painter->setFont(t_f); // Reset the font
    }

    QPointF caption_rect_bl {caption_rect.bottomLeft()};
    QPointF first_in_port_coordinate {caption_rect_bl.x(), caption_rect_bl.y() + body_rect_header_height};

    // Correct X coordinate: Remove the margin
    first_in_port_coordinate.setX((float)first_in_port_coordinate.x() - this->rect_margin);

    {
        // Draw Input Ports
        int in_port_count {n->get_input_port_count()};

        for (unsigned i {0}; i < in_port_count; ++i) {
            QPointF port_coordinate {first_in_port_coordinate.x(), first_in_port_coordinate.y() + body_rect_port_step * i};

            QRectF port_rect(port_coordinate.x(), port_coordinate.y(), min_side * 0.1f, min_side * 0.1f);

            // Adjust the port rect to be centered
            port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f, -port_rect.height() * 0.5f);

            // Draw caption
            QString p_n {QString::fromStdString(n->get_input_port_name(i))};

            if (!p_n.isEmpty()) {
                QFont t_f {painter->font()};

                QFont f("Arial", port_caption_font_size);
                QFontMetrics fm(f);
                painter->setFont(f);

                float x {rect_x + 5.0f};

                float y {(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

                QPointF coordinate {x, y};

                painter->setPen(this->font_color);
                painter->drawText(coordinate, p_n);

                painter->setFont(t_f); // Reset the font
            }

            if (in_port_graphics_objects.find(i) != in_port_graphics_objects.end()) 
                continue;

            // Draw the port
            VisualShaderInputPortGraphicsObject* p_o {new VisualShaderInputPortGraphicsObject(port_rect, n_id, i, this)};
            in_port_graphics_objects[i] = p_o;

            // Connect the signals
            // QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_pressed, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_pressed);
            QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dragged, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dragged);
            QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dropped, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dropped);
        }
    }

    QPointF caption_rect_br {caption_rect.bottomRight()};
    QPointF first_out_port_coordinate {caption_rect_br.x(), caption_rect_br.y() + body_rect_header_height};

    // Correct X coordinate: Remove the margin
    first_out_port_coordinate.setX((float)first_out_port_coordinate.x() + this->rect_margin);

    {
        // Draw Output Ports
        int out_port_count {n->get_output_port_count()};

        for (unsigned i {0}; i < out_port_count; ++i) {
            QPointF port_coordinate {first_out_port_coordinate.x(), first_out_port_coordinate.y() + body_rect_port_step * i};

            QRectF port_rect(port_coordinate.x(), port_coordinate.y(), min_side * 0.1f, min_side * 0.1f);

            // Adjust the port rect to be centered
            port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f, -port_rect.height() * 0.5f);

            // Draw caption
            QString p_n {QString::fromStdString(n->get_output_port_name(i))};

            if (!p_n.isEmpty()) {
                QFont t_f {painter->font()};

                QFont f("Arial", port_caption_font_size);
                QFontMetrics fm(f);
                painter->setFont(f);

                float x {rect_x + rect_w - (float)fm.horizontalAdvance(p_n) - 5.0f};

                float y {(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

                QPointF coordinate {x, y};

                painter->setPen(this->font_color);
                painter->drawText(coordinate, p_n);

                painter->setFont(t_f); // Reset the font
            }

            if (out_port_graphics_objects.find(i) != out_port_graphics_objects.end()) 
                continue;

            // Draw the port
            VisualShaderOutputPortGraphicsObject* p_o {new VisualShaderOutputPortGraphicsObject(port_rect, n_id, i, this)};
            out_port_graphics_objects[i] = p_o;

            // Connect the signals
            // QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_pressed, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_pressed);
            QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dragged, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dragged);
            QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dropped, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dropped);
        }
    }    
}


QVariant VisualShaderNodeGraphicsObject::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (scene() && change == ItemScenePositionHasChanged) {
        Q_EMIT dynamic_cast<VisualShaderGraphicsScene*>(scene())->node_moved(n_id, pos());
	}

	return QGraphicsObject::itemChange(change, value);
}

VisualShaderInputPortGraphicsObject::VisualShaderInputPortGraphicsObject(const QRectF& rect,
                                                                         const int& n_id,
                                                                         const int& p_index, 
                                                                         QGraphicsItem* parent) : QGraphicsObject(parent), 
                                                                                                  rect(rect), 
                                                                                                  n_id(n_id),
                                                                                                  p_index(p_index),
                                                                                                  connection_graphics_object(nullptr) {
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

    setCursor(Qt::PointingHandCursor);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    setVisible(true);
	setOpacity(this->opacity);

	setZValue(0);
}

VisualShaderInputPortGraphicsObject::~VisualShaderInputPortGraphicsObject() {
    
}

QRectF VisualShaderInputPortGraphicsObject::boundingRect() const {
    // rect.adjust(-padding, -padding, padding, padding);

    return rect;
}

void VisualShaderInputPortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setClipRect(option->exposedRect);

    // rect.adjust(padding, padding, -padding, -padding);
    
    painter->setBrush(this->connection_point_color);

    painter->drawEllipse(rect);
}

void VisualShaderInputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit port_pressed(this, event->scenePos());
    QGraphicsObject::mousePressEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    emit port_dragged(this, event->scenePos());
    QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    emit port_dropped(this, event->scenePos());
    QGraphicsObject::mouseReleaseEvent(event);
}

VisualShaderOutputPortGraphicsObject::VisualShaderOutputPortGraphicsObject(const QRectF& rect,
                                                                           const int& n_id,
                                                                           const int& p_index, 
                                                                           QGraphicsItem* parent) : QGraphicsObject(parent), 
                                                                                                    rect(rect),
                                                                                                    n_id(n_id),
                                                                                                    p_index(p_index),
                                                                                                    connection_graphics_object(nullptr) {
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

    setCursor(Qt::PointingHandCursor);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    setVisible(true);
	setOpacity(this->opacity);

	setZValue(0);
}

VisualShaderOutputPortGraphicsObject::~VisualShaderOutputPortGraphicsObject() {
    
}

QRectF VisualShaderOutputPortGraphicsObject::boundingRect() const {
    // rect.adjust(-padding, -padding, padding, padding);

    return rect;
}

void VisualShaderOutputPortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setClipRect(option->exposedRect);

    // rect.adjust(padding, padding, -padding, -padding);
    
    painter->setBrush(this->connection_point_color);

    painter->drawEllipse(rect);
}

void VisualShaderOutputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit port_pressed(this, event->scenePos());
    QGraphicsObject::mousePressEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    emit port_dragged(this, event->scenePos());
    QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    emit port_dropped(this, event->scenePos());
    QGraphicsObject::mouseReleaseEvent(event);
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

VisualShaderConnectionGraphicsObject::VisualShaderConnectionGraphicsObject(const int& from_n_id, 
                                                                           const int& from_p_index, 
                                                                           const QPointF& start_coordinate, 
                                                                           QGraphicsItem* parent) : QGraphicsObject(parent),
                                                                                                    from_n_id(from_n_id),
                                                                                                    from_p_index(from_p_index),
                                                                                                    to_n_id((int)VisualShader::NODE_ID_INVALID),
                                                                                                    to_p_index((int)VisualShader::PORT_INDEX_INVALID),
                                                                                                    start_coordinate(start_coordinate),
                                                                                                    end_coordinate(start_coordinate),
                                                                                                    rect_padding(0.0f) {
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

	setZValue(-1.0f);
}

VisualShaderConnectionGraphicsObject::~VisualShaderConnectionGraphicsObject() {
    
}

QRectF VisualShaderConnectionGraphicsObject::boundingRect() const {
    QRectF r {calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

    // Calculate the rect padding
    // We add a safe area around the rect to make it easier to get an accurate coordinate of the size
    this->rect_padding = 10.0f;

    r.adjust(-rect_padding, -rect_padding, rect_padding, rect_padding);

	return r;
}

void VisualShaderConnectionGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setClipRect(option->exposedRect);

    {
        QPen pen;
        pen.setWidth(this->line_width);
        pen.setColor(this->construction_color);
        pen.setStyle(Qt::DashLine);

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        std::pair<QPointF, QPointF> control_points {calculate_control_points(start_coordinate, end_coordinate)}; 

        QPainterPath cubic(start_coordinate);
        cubic.cubicTo(control_points.first, control_points.second, end_coordinate);

        // cubic spline
        painter->drawPath(cubic);
    }

    {
        // draw normal line
        QPen p;
        p.setWidth(this->line_width);

        const bool selected {this->isSelected()};

        std::pair<QPointF, QPointF> control_points {calculate_control_points(start_coordinate, end_coordinate)}; 

        QPainterPath cubic(start_coordinate);
        cubic.cubicTo(control_points.first, control_points.second, end_coordinate);

        p.setColor(this->normal_color);

        if (selected) {
            p.setColor(this->selected_color);
        }

        painter->setPen(p);
        painter->setBrush(Qt::NoBrush);

        painter->drawPath(cubic);
    }

    painter->setBrush(this->connection_point_color);
    
    {
        // Draw start point
        QRectF start_rect(start_coordinate.x(), start_coordinate.y(), this->point_diameter, this->point_diameter);

        // Adjust the port rect to be centered
        start_rect.adjust(-start_rect.width() * 0.5f, -start_rect.height() * 0.5f, -start_rect.width() * 0.5f, -start_rect.height() * 0.5f);

        painter->drawEllipse(start_rect);
    }

    {
        // Draw end point
        QRectF end_rect(end_coordinate.x(), end_coordinate.y(), this->point_diameter, this->point_diameter);

        // Adjust the port rect to be centered
        end_rect.adjust(-end_rect.width() * 0.5f, -end_rect.height() * 0.5f, -end_rect.width() * 0.5f, -end_rect.height() * 0.5f);

        painter->drawEllipse(end_rect);
    }
}

int VisualShaderConnectionGraphicsObject::detect_quadrant(const QPointF& reference, const QPointF& target) const {
    float relative_x {(float)(target.x() - reference.x())};
    float relative_y {(float)(target.y() - reference.y())};

    // Note that the default coordinate system in Qt is as follows:
    // - X-axis: Positive to the right, negative to the left
    // - Y-axis: Positive downwards, negative upwards

    // Check if the point is on an axis or the origin
    if (relative_x == 0 && relative_y == 0) {
        return 0; // Stack on the reference
    } else if (relative_y == 0) {
        return (relative_x > 0) ? 5 : 6; // On X-axis: 5 is the +ve part while 6 is the -ve one.
    } else if (relative_x == 0) {
        return (relative_y < 0) ? 7 : 8; // On Y-axis: 7 is the +ve part while 8 is the -ve one.
    }

    // Determine the quadrant based on the relative coordinates
    if (relative_x > 0 && relative_y < 0) {
        return 1; // Quadrant I
    } else if (relative_x < 0 && relative_y < 0) {
        return 2; // Quadrant II
    } else if (relative_x < 0 && relative_y > 0) {
        return 3; // Quadrant III
    } else if (relative_x > 0 && relative_y > 0) {
        return 4; // Quadrant IV
    }

    // Default case (should not reach here)
    return -1;
}

QRectF VisualShaderConnectionGraphicsObject::calculate_bounding_rect_from_coordinates(const QPointF& start_coordinate, const QPointF& end_coordinate) const {
    float x1 {(float)start_coordinate.x()};
    float y1 {(float)start_coordinate.y()};
    float x2 {(float)end_coordinate.x()};
    float y2 {(float)end_coordinate.y()};
    
    // Calculate the expanded rect
    float min_x {qMin(x1, x2)};
    float min_y {qMin(y1, y2)};
    float max_x {qMax(x1, x2)};
    float max_y {qMax(y1, y2)};

    QRectF r({min_x, min_y}, QSizeF(max_x - min_x, max_y - min_y));

    bool in_abnormal_region {x2 < (x1 + min_h_distance)};

    float a_width_expansion {((x1 + min_h_distance) - x2) * abnormal_face_to_back_control_width_expansion_factor};

    if (in_abnormal_region) {
        // The connection is not going from left to right normally
        // Our control points will be outside the end_coordinate and start_coordinate bounding rect
        // We will expand the bounding rect horizontally to make it easier to get an accurate coordinate of the size
        r.adjust(-a_width_expansion, 0.0f, a_width_expansion, 0.0f);
    }

    return r;
}

std::pair<QPointF, QPointF> VisualShaderConnectionGraphicsObject::calculate_control_points(const QPointF& start_coordinate, const QPointF& end_coordinated) const {
    QPointF cp1;
    QPointF cp2;
    
    float x1 {(float)start_coordinate.x()};
    float y1 {(float)start_coordinate.y()};
    float x2 {(float)end_coordinate.x()};
    float y2 {(float)end_coordinate.y()};
    
    QRectF r {calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

    bool in_abnormal_region {x2 < (x1 + min_h_distance)};

    int quadrant {detect_quadrant({x1, y1}, {x2, y2})};

    float face_to_face_control_width_expansion_factor {0.8f};
    float face_to_face_control_height_expansion_factor {0.25f};

    float width_expansion {(x2 - x1) * face_to_face_control_width_expansion_factor};

    float a_width_expansion {((x1 + min_h_distance) - x2) * abnormal_face_to_back_control_width_expansion_factor};
    float a_height_expansion {a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

    if (in_abnormal_region) {
        r.adjust(-a_width_expansion, 0.0f, a_width_expansion, 0.0f);
    }

    switch(quadrant) {
        case 1: // Quadrant I: Normal face to back
            // Find out if the connection is going from left to right normally
            if (in_abnormal_region) {
                // The connection is not going from left to right normally
                // Our control points will be outside the end_coordinate and start_coordinate bounding rect
                // We will expand the bounding rect horizontally to make it easier to get an accurate coordinate of the size

                // Here we cover cases of nodes not facing each other.
                // This means we can't just send the path straight to the node.

                // Treated as inside Quadrant II
                cp1 = {x1 + a_width_expansion, y1};
                cp2 = {x2 - a_width_expansion, y2};
                
            } else {
                // Treated as inside Quadrant I
                cp1 = {x1 + width_expansion, y1};
                cp2 = {x2 - width_expansion, y2};
            }
            break;
        case 2: // Quadrant II: Abnormal face to back
            cp1 = {x1 + a_width_expansion, y1};
            cp2 = {x2 - a_width_expansion, y2};
            break;
        case 3: // Quadrant III: Abnormal face to back
            cp1 = {x1 + a_width_expansion, y1 + a_height_expansion};
            cp2 = {x2 - a_width_expansion, y2 - a_height_expansion};
            break;
        case 4: // Quadrant IV: Normal face to back
            if (in_abnormal_region) {
                // Treated as inside Quadrant III
                cp1 = {x1 + a_width_expansion, y1};
                cp2 = {x2 - a_width_expansion, y2};
            } else {
                // Treated as inside Quadrant IV
                cp1 = {x1 + width_expansion, y1};
                cp2 = {x2 - width_expansion, y2};
            }
            break;
        case 5: // On +ve X-axis: Normal face to back
            // Straight line
            cp1 = {x1, y1};
            cp2 = {x2, y2};
            break;
        case 6: // On -ve X-axis: Abnormal face to back
            r.adjust(0.0f, -a_height_expansion, 0.0f, a_height_expansion);
            cp1 = {x1 + a_width_expansion, y1};
            cp2 = {x2 - a_width_expansion, y2};
            break;
        case 7: // On +ve Y-axis: Abnormal face to back
            r.adjust(0.0f, -a_height_expansion, 0.0f, a_height_expansion);
            cp1 = {x1 + a_width_expansion, y1};
            cp2 = {x2 - a_width_expansion, y2};
            break;
        case 8: // On -ve Y-axis: Abnormal face to back
            r.adjust(0.0f, -a_height_expansion, 0.0f, a_height_expansion);
            cp1 = {x1 + a_width_expansion, y1};
            cp2 = {x2 - a_width_expansion, y2};
            break;
        default:
            return std::make_pair(start_coordinate, end_coordinate);
    }

    return std::make_pair(cp1, cp2);
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
