/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2024 Saif Kandil (k0T0z)                                       */
/*                                                                               */
/*  This file is a part of the ENIGMA Development Environment.                   */
/*                                                                               */
/*                                                                               */
/*  ENIGMA is free software: you can redistribute it and/or modify it under the  */
/*  terms of the GNU General Public License as published by the Free Software    */
/*  Foundation, version 3 of the license or any later version.                   */
/*                                                                               */
/*  This application and its source code is distributed AS-IS, WITHOUT ANY       */
/*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/*  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        */
/*  details.                                                                     */
/*                                                                               */
/*  You should have recieved a copy of the GNU General Public License along      */
/*  with this code. If not, see <http://www.gnu.org/licenses/>                   */
/*                                                                               */
/*  ENIGMA is an environment designed to create games and other programs with a  */
/*  high-level, fully compilable language. Developers of ENIGMA or anything      */
/*  associated with ENIGMA are in no way responsible for its users or            */
/*  applications created by its users, or damages caused by the environment      */
/*  or programs made in the environment.                                         */
/*                                                                               */
/*********************************************************************************/

#include "Editors/VisualShaderEditor.h"

#include <unordered_map>

#include "EVisualShader.pb.h"

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderEditor                           *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderEditor::VisualShaderEditor(QWidget* parent)
  : BaseEditor(parent),
      visual_shader(nullptr),
      layout(nullptr),
      side_widget(nullptr),
      side_outer_layout(nullptr),
      side_layout(nullptr),
      name_edit(nullptr),
      save_button(nullptr),
      scene_layer_layout(nullptr),
      scene_layer(nullptr),
      scene(nullptr),
      view(nullptr),
      top_layer(nullptr),
      menu_bar(nullptr),
      menu_button(nullptr),
      create_node_button(nullptr),
      preview_shader_button(nullptr),
      create_node_action(nullptr),
      zoom_in_button(nullptr),
      reset_zoom_button(nullptr),
      zoom_out_button(nullptr),
      load_image_button(nullptr),
      match_image_button(nullptr),
      create_node_dialog(nullptr),
      code_previewer_dialog(nullptr),
      code_previewer_layout(nullptr),
      code_previewer(nullptr) {
  VisualShaderEditor::init();
}

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent),
      visual_shader(nullptr),
      layout(nullptr),
      side_widget(nullptr),
      side_outer_layout(nullptr),
      side_layout(nullptr),
      name_edit(nullptr),
      save_button(nullptr),
      scene_layer_layout(nullptr),
      scene_layer(nullptr),
      scene(nullptr),
      view(nullptr),
      top_layer(nullptr),
      menu_bar(nullptr),
      menu_button(nullptr),
      create_node_button(nullptr),
      preview_shader_button(nullptr),
      create_node_action(nullptr),
      zoom_in_button(nullptr),
      reset_zoom_button(nullptr),
      zoom_out_button(nullptr),
      load_image_button(nullptr),
      match_image_button(nullptr),
      create_node_dialog(nullptr),
      code_previewer_dialog(nullptr),
      code_previewer_layout(nullptr),
      code_previewer(nullptr) {
  VisualShaderEditor::init();

  _nodeMapper->addMapping(name_edit, TreeNode::kNameFieldNumber);
  QObject::connect(save_button, &QAbstractButton::pressed, this, &BaseEditor::OnSave);
  // visual_shader_model = _model->GetSubModel<MessageModel*>(TreeNode::kVisualShaderFieldNumber);
}

VisualShaderEditor::~VisualShaderEditor() {
  if (visual_shader) delete visual_shader;
}

void VisualShaderEditor::init() {
  visual_shader = new VisualShader();

  // Create the main layout.
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the side widget
  side_widget = new QWidget();
  side_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  side_widget->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  side_widget->setVisible(false);

  // Create the side outer layout
  side_outer_layout = new QVBoxLayout(side_widget);
  side_outer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  side_outer_layout->setSpacing(5);
  side_outer_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  side_outer_layout->setSizeConstraint(QLayout::SetNoConstraint);

  // Add the side inner layout
  side_layout = new QVBoxLayout();
  side_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  side_layout->setSpacing(5);
  side_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  side_layout->setSizeConstraint(QLayout::SetNoConstraint);

  // Fill in the left layout
  QHBoxLayout* name_layout = new QHBoxLayout();
  name_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  name_layout->setSpacing(5);
  name_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  name_layout->setSizeConstraint(QLayout::SetNoConstraint);

  QLabel* name_label = new QLabel("Name");
  name_layout->addWidget(name_label, 1);

  name_edit = new QLineEdit();
  name_layout->addWidget(name_edit, 4);

  side_layout->addLayout(name_layout);

  side_outer_layout->addLayout(side_layout);

  save_button = new QPushButton("Save");
  save_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  save_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  save_button->setToolTip("Save editor changes including the graph");
  save_button->setIcon(QIcon(":/actions/accept.png"));
  side_outer_layout->addWidget(save_button);

  side_widget->setLayout(side_outer_layout);

  // Create the scene layer.
  scene_layer = new QWidget();
  scene_layer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  scene_layer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  // Create the scene layer layout.
  scene_layer_layout = new QHBoxLayout(scene_layer);
  scene_layer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  scene_layer_layout->setSpacing(0);
  scene_layer_layout->setSizeConstraint(QLayout::SetNoConstraint);
  scene_layer_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  scene = new VisualShaderGraphicsScene(visual_shader);
  scene->set_editor(this);

  view = new VisualShaderGraphicsView(scene, scene_layer);
  view->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  VisualShaderEditor::init_graph(); // Must be called after a scene and a view are created.

  scene_layer_layout->addWidget(view);

  // Set the scene layer layout.
  scene_layer->setLayout(scene_layer_layout);

  // Create the menu bar layer on top of the scene layer.
  top_layer = new QWidget(view);
  top_layer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  top_layer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Create the menu bar layout.
  menu_bar = new QHBoxLayout(top_layer);
  menu_bar->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  menu_bar->setSpacing(5);                       // Adjust spacing as needed
  menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  menu_bar->setSizeConstraint(QLayout::SetMinimumSize);

  // Create the menu button
  menu_button = new QPushButton("Show Menu", top_layer);
  menu_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  menu_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  menu_button->setToolTip("Toggle Menu");
  menu_bar->addWidget(menu_button);
  QObject::connect(menu_button, &QPushButton::pressed, this, &VisualShaderEditor::on_menu_button_pressed);

  // Create the create node button.
  create_node_button = new QPushButton("Create Node", top_layer);
  create_node_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_button->setToolTip("Create a new node");
  menu_bar->addWidget(create_node_button);
  QObject::connect(create_node_button, &QPushButton::pressed, this, &VisualShaderEditor::on_create_node_button_pressed);

  this->connect(this, &VisualShaderEditor::on_create_node_dialog_requested, this,
                &VisualShaderEditor::show_create_node_dialog);

  // Create the preview shader button.
  preview_shader_button = new QPushButton("Preview Shader", top_layer);
  preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  preview_shader_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  preview_shader_button->setToolTip("Preview the expected generated shader code");
  menu_bar->addWidget(preview_shader_button);
  QObject::connect(preview_shader_button, &QPushButton::pressed, this, &VisualShaderEditor::on_preview_shader_button_pressed);

  zoom_in_button = new QPushButton("Zoom In", scene_layer);
  zoom_in_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  zoom_in_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  zoom_in_button->setToolTip("Zoom In");
  menu_bar->addWidget(zoom_in_button);
  QObject::connect(zoom_in_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_in);

  reset_zoom_button = new QPushButton("Reset Zoom", scene_layer);
  reset_zoom_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  reset_zoom_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  reset_zoom_button->setToolTip("Reset Zoom");
  menu_bar->addWidget(reset_zoom_button);
  QObject::connect(reset_zoom_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::reset_zoom);

  zoom_out_button = new QPushButton("Zoom Out", scene_layer);
  zoom_out_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  zoom_out_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  zoom_out_button->setToolTip("Zoom Out");
  menu_bar->addWidget(zoom_out_button);
  QObject::connect(zoom_out_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_out);

  load_image_button = new QPushButton("Load Image", scene_layer);
  load_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  load_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  load_image_button->setToolTip("Load an image to match");
  menu_bar->addWidget(load_image_button);
  QObject::connect(load_image_button, &QPushButton::pressed, this, &VisualShaderEditor::on_load_image_button_pressed);

  match_image_button = new QPushButton("Match Image", scene_layer);
  match_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  match_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  match_image_button->setToolTip("Match the shader to the loaded image");
  menu_bar->addWidget(match_image_button);

  // Set the top layer layout.
  top_layer->setLayout(menu_bar);

  // Add the left layout
  layout->addWidget(side_widget, 1);

  // Add the scene layer to the main layout.
  layout->addWidget(scene_layer, 4);

  ////////////////////////////////////
  // Code Previewer
  ////////////////////////////////////

  code_previewer_dialog = new QDialog(this);
  code_previewer_dialog->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer_dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  code_previewer_layout = new QVBoxLayout(code_previewer_dialog);
  code_previewer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer_layout->setSpacing(0);                    // Adjust spacing as needed
  code_previewer_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  code_previewer_layout->setSizeConstraint(QLayout::SetMinimumSize);

  code_previewer = new QPlainTextEdit(code_previewer_dialog);
  code_previewer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  code_previewer->setReadOnly(true);
  code_previewer->setLineWrapMode(QPlainTextEdit::NoWrap);
  code_previewer->setWordWrapMode(QTextOption::NoWrap);
  code_previewer->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  code_previewer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  code_previewer->setTabChangesFocus(true);
  code_previewer->setMinimumSize(800, 600);

  code_previewer_layout->addWidget(code_previewer);

  code_previewer_dialog->setLayout(code_previewer_layout);

  ////////////////////////////////////
  // CreateNodeDialog Nodes Tree
  ////////////////////////////////////

  // Create the create node dialog under the main layout.
  create_node_dialog = new CreateNodeDialog(this);
  create_node_dialog->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  //////////////////////////////////////////
  // CreateNodeDialog Nodes Tree Children
  //////////////////////////////////////////

  const VisualShaderEditor::CreateNodeDialogNodesTreeItem* items{
      VisualShaderEditor::create_node_dialog_nodes_tree_items};

  // Map to store category items
  std::unordered_map<std::string, QTreeWidgetItem*> category_path_map;

  int i{0};

  while (!items[i].type.empty()) {
    const CreateNodeDialogNodesTreeItem& item{items[i]};

    // Parse the category string into a vector of strings
    std::vector<std::string> categories{parse_node_category_path(item.category_path)};
    QTreeWidgetItem* parent{nullptr};  // Start from the root

    std::string current_category_path;
    // Create/find each level of categories
    for (const std::string& category : categories) {
      if (!current_category_path.empty()) {
        current_category_path += "/";
      }

      current_category_path += category;

      parent = find_or_create_category_item(parent, category, current_category_path,
                                            create_node_dialog->get_nodes_tree(), category_path_map);
    }

    // Now add the item to its corresponding parent category
    QTreeWidgetItem* node_item = new QTreeWidgetItem(parent);
    node_item->setText(0, QString::fromStdString(item.name));
    node_item->setData(0, Qt::UserRole, QString::fromStdString(item.type));
    node_item->setData(0, Qt::UserRole + 1, QString::fromStdString(item.description));

    i++;
  }

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title and icon.
  this->setWindowTitle("Visual Shader Editor");
  // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
  this->setLayout(layout);
}

void VisualShaderEditor::init_graph() {
  // Load the nodes and connections from the VisualShader
  std::vector<int> ns{visual_shader->get_nodes()};
  for (const int& n_id : ns) {
    const std::shared_ptr<VisualShaderNode> n{visual_shader->get_node(n_id)};

    if (!n) {
      continue;
    }

    TVector2 c{visual_shader->get_node_coordinate(n_id)};

    scene->add_node(n_id, n, {c.x, c.y});
  }

  std::vector<VisualShader::Connection> cs{visual_shader->get_connections()};
  for (const VisualShader::Connection& c : cs) {
    scene->add_connection(c.from_node, c.from_port, c.to_node, c.to_port);
  }
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

    {"ValueNoise", "Procedural/Noise", "VisualShaderNodeValueNoise",
     "Generates a simple, or Value, noise based on input 'UV'. The scale of the generated noise is controlled by input "
     "'Scale'."},

    // Utility

    {"Compare", "Utility/Logic", "VisualShaderNodeCompare",
     "Returns the boolean result of the comparison between two parameters."},
    {"If", "Utility/Logic", "VisualShaderNodeIf",
     "Returns the value of the 'True' or 'False' input based on the value of the 'Condition' input."},
    {"Switch", "Utility/Logic", "VisualShaderNodeSwitch",
     "Returns an associated scalar if the provided boolean value is true or false."},
    {"Is", "Utility/Logic", "VisualShaderNodeIs",
     "Returns the boolean result of the comparison between INF (or NaN) and a scalar parameter."},

    {"", "", "", ""},
};

void VisualShaderEditor::create_node(const QPointF& coordinate) {
  QTreeWidgetItem* selected_item{create_node_dialog->get_selected_item()};

  if (!selected_item) {
    return;
  }

  VisualShaderEditor::add_node(selected_item, coordinate);
}

void VisualShaderEditor::add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate) {
  std::string type{selected_item->data(0, Qt::UserRole).toString().toStdString()};

  if (type.empty()) {
    return;
  }

  scene->add_node(type, coordinate);
}

void VisualShaderEditor::show_create_node_dialog(const QPointF& coordinate) {
  int status{create_node_dialog->exec()};
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

void VisualShaderEditor::on_create_node_button_pressed() { Q_EMIT on_create_node_dialog_requested(); }

void VisualShaderEditor::on_preview_shader_button_pressed() {
  bool result{visual_shader->generate_shader()};
  if (!result) {
    std::cout << "Failed to generate shader" << std::endl;
    return;
  }
  code_previewer->setPlainText(QString::fromStdString(visual_shader->get_code()));
  code_previewer_dialog->exec();
}

void VisualShaderEditor::on_menu_button_pressed() {
  bool is_visible{side_widget->isVisible()};
  side_widget->setVisible(!is_visible);
  menu_button->setText(!is_visible ? "Hide Menu" : "Show Menu");
}

void VisualShaderEditor::on_load_image_button_pressed() {
  // TODO: Decide on how to load an image
  // For example, use QFileDialog to open an image file or
  // load an existing sprite or background from the project.
  // Then, send the image to OriginalMatchingImageWidget widget to display it.
}

std::vector<std::string> VisualShaderEditor::parse_node_category_path(const std::string& node_category_path) {
  std::vector<std::string> tokens;
  std::stringstream ss(node_category_path);
  std::string token;
  while (std::getline(ss, token, '/')) {
    tokens.push_back(token);
  }
  return tokens;
}

QTreeWidgetItem* VisualShaderEditor::find_or_create_category_item(
    QTreeWidgetItem* parent, const std::string& category, const std::string& category_path,
    QTreeWidget* create_node_dialog_nodes_tree, std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map) {
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

CreateNodeDialog::CreateNodeDialog(QWidget* parent)
    : QDialog(parent),
      layout(nullptr),
      create_node_dialog_nodes_tree_layout(nullptr),
      create_node_dialog_nodes_tree(nullptr),
      create_node_dialog_nodes_description(nullptr),
      buttons_layout(nullptr),
      create_button(nullptr),
      cancel_button(nullptr),
      selected_item(nullptr) {
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the nodes tree layout.
  create_node_dialog_nodes_tree_layout = new QVBoxLayout();
  create_node_dialog_nodes_tree_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_tree_layout->setSpacing(0);                   // Adjust spacing as needed
  create_node_dialog_nodes_tree_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  create_node_dialog_nodes_tree_layout->setSizeConstraint(QLayout::SetMinimumSize);

  // Add the nodes tree layout to the main layout.
  layout->addLayout(create_node_dialog_nodes_tree_layout);

  // Create the nodes tree.
  create_node_dialog_nodes_tree = new QTreeWidget();
  create_node_dialog_nodes_tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_dialog_nodes_tree->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_tree->setColumnCount(1);
  create_node_dialog_nodes_tree->setHeaderHidden(true);
  this->connect(create_node_dialog_nodes_tree, &QTreeWidget::itemSelectionChanged, this,
                &CreateNodeDialog::update_selected_item);

  // Add the nodes tree to the nodes tree layout.
  create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_tree,
                                                  2);  // 2x the size of the nodes description.

  // Create the nodes description.
  create_node_dialog_nodes_description = new QTextEdit();
  create_node_dialog_nodes_description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_dialog_nodes_description->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_description->setReadOnly(true);
  create_node_dialog_nodes_description->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Add the nodes description to the nodes tree layout.
  create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_description, 1);

  // Create the buttons layout.
  buttons_layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  create_button = new QPushButton("Create");
  QObject::connect(create_button, &QPushButton::pressed, this, &CreateNodeDialog::on_create_node_button_pressed);
  create_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_button->setToolTip("Create the selected node.");

  cancel_button = new QPushButton("Cancel");
  QObject::connect(cancel_button, &QPushButton::pressed, this,
                   &CreateNodeDialog::on_cancel_node_creation_button_pressed);
  cancel_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  cancel_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  cancel_button->setToolTip("Cancel the node creation.");

  // Add the buttons to the buttons layout.
  buttons_layout->addWidget(create_button);
  buttons_layout->addWidget(cancel_button);

  // Add the buttons layout to the main layout.
  layout->addLayout(buttons_layout);

  //////////////// Start of Footer ////////////////

  this->setWindowTitle("Create Shader Node");

  this->setLayout(layout);
}

CreateNodeDialog::~CreateNodeDialog() {}

void CreateNodeDialog::on_create_node_button_pressed() { this->accept(); }

void CreateNodeDialog::on_cancel_node_creation_button_pressed() { this->reject(); }

void CreateNodeDialog::update_selected_item() {
  QTreeWidgetItem* item{create_node_dialog_nodes_tree->currentItem()};
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

VisualShaderGraphicsScene::VisualShaderGraphicsScene(VisualShader* vs, QObject* parent)
    : QGraphicsScene(parent), vs(vs), temporary_connection_graphics_object(nullptr) {
  setItemIndexMethod(QGraphicsScene::NoIndex);  // https://doc.qt.io/qt-6/qgraphicsscene.html#ItemIndexMethod-enum

  QObject::connect(this, &VisualShaderGraphicsScene::node_moved, this, &VisualShaderGraphicsScene::on_node_moved);
}

VisualShaderGraphicsScene::~VisualShaderGraphicsScene() {}

bool VisualShaderGraphicsScene::add_node(const std::string& type, const QPointF& coordinate) {
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
    return false;
  }

  int n_id{vs->get_valid_node_id()};

  if (n_id == (int)VisualShader::NODE_ID_INVALID) {
    return false;
  }

  return VisualShaderGraphicsScene::add_node(n_id, n, coordinate);
}

bool VisualShaderGraphicsScene::add_node(const int& n_id, const std::shared_ptr<VisualShaderNode>& n, const QPointF& coordinate) {
  // Make sure the node doesn't already exist, we don't want to overwrite a node.
  if (node_graphics_objects.find(n_id) != node_graphics_objects.end()) {
    return false;
  }

  QList<QGraphicsView*> views{this->views()};
  if (views.isEmpty()) {
    std::cout << "No views available" << std::endl;
    return false;
  }

  // The output node cannot be removed or added by the user
  if (n_id >= VisualShader::NODE_ID_OUTPUT + 1) {
    bool result{vs->add_node(n, {(float)coordinate.x(), (float)coordinate.y()}, n_id)};

    if (!result) {
      return false;
    }
  }

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (vs->get_node_coordinate(n_id).x < view->get_x() ||
      vs->get_node_coordinate(n_id).x > view->get_x() + view->get_width() ||
      vs->get_node_coordinate(n_id).y < view->get_y() ||
      vs->get_node_coordinate(n_id).y > view->get_y() + view->get_height()) {
    std::cout << "Node is out of view bounds" << std::endl;
  } 

  VisualShaderNodeGraphicsObject* n_o{new VisualShaderNodeGraphicsObject(n_id, coordinate, n)};

  if (n_id != (int)VisualShader::NODE_ID_OUTPUT) {
    VisualShaderNodeEmbedWidget* embed_widget {new VisualShaderNodeEmbedWidget(n)};
    QGraphicsProxyWidget* embed_widget_proxy{new QGraphicsProxyWidget(n_o)};
    embed_widget_proxy->setWidget(embed_widget);
    n_o->set_embed_widget(embed_widget);

    // Send the shader previewer widget
    embed_widget->set_shader_previewer_widget(n_o->get_shader_previewer_widget());
  }

  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::node_deleted, this, &VisualShaderGraphicsScene::on_node_deleted);

  node_graphics_objects[n_id] = n_o;
  
  addItem(n_o);

  return true;
}

bool VisualShaderGraphicsScene::delete_node(const int& n_id) {
  const std::shared_ptr<VisualShaderNode> n{vs->get_node(n_id)};

  if (!n) {
    return false;
  }

  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};

  if (!n_o) {
    return false;
  }

  // Remove all connections to the node
  for (int i{0}; i < n->get_input_port_count(); i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};

    if (!i_port || !i_port->is_connected()) {
      continue;
    }

    // Get the output port of the connection
    VisualShaderConnectionGraphicsObject* c_o{i_port->get_connection_graphics_object()};

    if (!c_o) {
      continue;
    }

    bool result{this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i)};

    if (!result) {
      std::cout << "Failed to delete connection" << std::endl;
      continue;
    }
  }

  for (int i{0}; i < n->get_output_port_count(); i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};

    if (!o_port || !o_port->is_connected()) {
      continue;
    }

    // Get the input port of the connection
    VisualShaderConnectionGraphicsObject* c_o{o_port->get_connection_graphics_object()};

    if (!c_o) {
      continue;
    }

    bool result{this->delete_connection(n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index())};

    if (!result) {
      std::cout << "Failed to delete connection" << std::endl;
      continue;
    }
  }

  bool result{vs->remove_node(n_id)};

  if (!result) {
    return false;
  }

  // Remove the node from the scene
  removeItem(n_o);
  this->node_graphics_objects.erase(n_id);

  delete n_o;  // Make sure to delete the node object to delete all child ports

  return true;
}

bool VisualShaderGraphicsScene::add_connection(const int& from_node_id, const int& from_port_index,
                                               const int& to_node_id, const int& to_port_index) {
  QList<QGraphicsView*> views{this->views()};
  if (views.isEmpty()) {
    std::cout << "No views available" << std::endl;
    return false;
  }

  // Create the connection and set its start
  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};

  if (!from_n_o) {
    return false;
  }

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};

  if (!from_o_port) {
    return false;
  }

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (from_o_port->get_global_coordinate().x() < view->get_x() ||
      from_o_port->get_global_coordinate().x() > view->get_x() + view->get_width()) {
    std::cout << "Start of connection is out of view bounds" << std::endl;
  }

  if (!this->temporary_connection_graphics_object) {
    this->temporary_connection_graphics_object =
        new VisualShaderConnectionGraphicsObject(from_node_id, from_port_index, from_o_port->get_global_coordinate());
    from_o_port->connect(this->temporary_connection_graphics_object);
    addItem(this->temporary_connection_graphics_object);
  }

  if (to_node_id != (int)VisualShader::NODE_ID_INVALID && to_port_index != (int)VisualShader::PORT_INDEX_INVALID) {
    // Set the end of the connection
    VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};

    if (!to_n_o) {
      return false;
    }

    VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};

    if (!to_i_port) {
      return false;
    }

    if (to_i_port->get_global_coordinate().y() < view->get_y() ||
        to_i_port->get_global_coordinate().y() > view->get_y() + view->get_height()) {
      std::cout << "End of connection is out of view bounds" << std::endl;
    }

    // Connect the nodes in the VisualShader
    bool result{vs->can_connect_nodes(from_node_id, from_port_index, to_node_id, to_port_index)};
    if (!result) {
      std::cout << "Can't connect nodes" << std::endl;
      return false;
    }

    result = vs->connect_nodes(from_node_id, from_port_index, to_node_id, to_port_index);
    if (!result) {
      std::cout << "Failed to connect nodes" << std::endl;
      return false;
    }

    this->temporary_connection_graphics_object->set_end_coordinate(to_i_port->get_global_coordinate());
    to_i_port->connect(this->temporary_connection_graphics_object);
    this->temporary_connection_graphics_object->set_to_node_id(to_node_id);
    this->temporary_connection_graphics_object->set_to_port_index(to_port_index);
    this->temporary_connection_graphics_object = nullptr;  // Make sure to reset the temporary connection object
  }

  return true;
}

bool VisualShaderGraphicsScene::delete_connection(const int& from_node_id, const int& from_port_index,
                                                  const int& to_node_id, const int& to_port_index) {
  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};

  if (!from_n_o) {
    return false;
  }

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};

  if (!from_o_port) {
    return false;
  }
  
  // If we have a complete connection, then we can disconnect the nodes
  if (to_node_id != (int)VisualShader::NODE_ID_INVALID && to_port_index != (int)VisualShader::PORT_INDEX_INVALID && !this->temporary_connection_graphics_object) {
    VisualShaderConnectionGraphicsObject* c_o{from_o_port->get_connection_graphics_object()};

    if (!c_o) {
      return false;
    }
    
    VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};

    if (!to_n_o) {
      return false;
    }

    VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};

    if (!to_i_port) {
      return false;
    }

    bool result{vs->disconnect_nodes(from_node_id, from_port_index, to_node_id, to_port_index)};

    if (!result) {
      return false;
    }

    to_i_port->detach_connection();
    from_o_port->detach_connection();
    removeItem(c_o);
    delete c_o;
    return true;
  } else if (to_node_id != (int)VisualShader::NODE_ID_INVALID && to_port_index != (int)VisualShader::PORT_INDEX_INVALID) {
    from_o_port->detach_connection();
    removeItem(this->temporary_connection_graphics_object);
    delete this->temporary_connection_graphics_object;
    this->temporary_connection_graphics_object = nullptr;
    return true;
  }

  return false;
}

VisualShaderNodeGraphicsObject* VisualShaderGraphicsScene::get_node_graphics_object(const int& n_id) const {
  VisualShaderNodeGraphicsObject* n_o{nullptr};

  auto it{node_graphics_objects.find(n_id)};
  if (it != node_graphics_objects.end()) {
    n_o = it->second;
  }

  return n_o;
}

void VisualShaderGraphicsScene::on_node_moved(const int& n_id, const QPointF& new_coordinate) {
  const std::shared_ptr<VisualShaderNode> n{vs->get_node(n_id)};

  if (!n) {
    return;
  }

  // Update the node's coordinate in the VisualShader
  vs->set_node_coordinate(n_id, {(float)new_coordinate.x(), (float)new_coordinate.y()});

  // Update coordinates of all connected connections
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};

  for (int i{0}; i < n->get_input_port_count(); i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};

    if (!i_port || !i_port->is_connected()) {
      continue;
    }

    VisualShaderConnectionGraphicsObject* c_o{i_port->get_connection_graphics_object()};

    if (!c_o) {
      continue;
    }

    c_o->set_end_coordinate(i_port->get_global_coordinate());
  }

  for (int i{0}; i < n->get_output_port_count(); i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};

    if (!o_port || !o_port->is_connected()) {
      continue;
    }

    VisualShaderConnectionGraphicsObject* c_o{o_port->get_connection_graphics_object()};

    if (!c_o) {
      continue;
    }

    c_o->set_start_coordinate(o_port->get_global_coordinate());
  }
}

void VisualShaderGraphicsScene::on_node_deleted(const int& n_id) {
  bool result{this->delete_node(n_id)};

  if (!result) {
    std::cout << "Failed to delete node" << std::endl;
  }
}

void VisualShaderGraphicsScene::on_port_pressed(QGraphicsObject* port, const QPointF& coordinate) {
  this->temporary_connection_graphics_object = nullptr;  // Reset the temporary connection object
}

void VisualShaderGraphicsScene::on_port_dragged(QGraphicsObject* port, const QPointF& coordinate) {
  VisualShaderConnectionGraphicsObject* c_o{nullptr};

  VisualShaderOutputPortGraphicsObject* o_port{dynamic_cast<VisualShaderOutputPortGraphicsObject*>(port)};

  if (!o_port) {
    VisualShaderInputPortGraphicsObject* i_port{dynamic_cast<VisualShaderInputPortGraphicsObject*>(port)};

    if (!i_port) {
      return;
    }

    if (i_port->is_connected() && !temporary_connection_graphics_object) {
      c_o = i_port->get_connection_graphics_object();
      temporary_connection_graphics_object = c_o;  // Store the connection object for access in the next drag call
      bool result{vs->disconnect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                                       c_o->get_to_port_index())};
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
    bool result{this->add_connection(o_port->get_node_id(), o_port->get_port_index())};
    if (!result) {
      std::cout << "Failed to add connection" << std::endl;
      return;
    }
    c_o = temporary_connection_graphics_object;
  } else if (o_port->is_connected() && temporary_connection_graphics_object) {
    c_o = temporary_connection_graphics_object;
  } else if (o_port->is_connected() && !temporary_connection_graphics_object) {
    c_o = o_port->get_connection_graphics_object();
    temporary_connection_graphics_object = c_o;  // Store the connection object for access in the next drag call

    // Detach the connection from the input port
    VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(c_o->get_to_node_id())};
    if (!n_o) {
      return;
    }
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(c_o->get_to_port_index())};
    if (!i_port) {
      return;
    }
    bool result{vs->disconnect_nodes(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                                     c_o->get_to_port_index())};
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
  // Find all items under the coordinate
  QList<QGraphicsItem*> items_at_coordinate{this->items(coordinate)};

  // Iterate through the items and check if an input port is under the mouse
  VisualShaderInputPortGraphicsObject* in_p_o{nullptr};
  for (QGraphicsItem* item : items_at_coordinate) {
    // Check if the item is an input port
    in_p_o = dynamic_cast<VisualShaderInputPortGraphicsObject*>(item);

    if (in_p_o) {
      break;
    }
  }

  VisualShaderOutputPortGraphicsObject* o_port{dynamic_cast<VisualShaderOutputPortGraphicsObject*>(port)};

  if (!o_port) {
    VisualShaderInputPortGraphicsObject* i_port{dynamic_cast<VisualShaderInputPortGraphicsObject*>(port)};

    if (!i_port) {
      return;
    }

    if (!in_p_o) {
      bool result{this->delete_connection(temporary_connection_graphics_object->get_from_node_id(), 
                                          temporary_connection_graphics_object->get_from_port_index())};

      if (!result) {
        std::cout << "Failed to delete connection" << std::endl;
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    bool result {add_connection(temporary_connection_graphics_object->get_from_node_id(), 
                                temporary_connection_graphics_object->get_from_port_index(), 
                                in_p_o->get_node_id(), 
                                in_p_o->get_port_index())};

    if (!result) {
      bool result{this->delete_connection(temporary_connection_graphics_object->get_from_node_id(), 
                                          temporary_connection_graphics_object->get_from_port_index())};

      if (!result) {
        std::cout << "Failed to delete connection" << std::endl;
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    return;
  }

  if (!in_p_o) {
    bool result{this->delete_connection(temporary_connection_graphics_object->get_from_node_id(), 
                                        temporary_connection_graphics_object->get_from_port_index())};

    if (!result) {
      std::cout << "Failed to delete connection" << std::endl;
    }

    return;  // Return because we dragging an output port and dropped on nothing
  }
  
  bool result{add_connection(o_port->get_node_id(), o_port->get_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index())};

  if (!result) {
    bool result{this->delete_connection(temporary_connection_graphics_object->get_from_node_id(), 
                                        temporary_connection_graphics_object->get_from_port_index())};

    if (!result) {
      std::cout << "Failed to delete connection" << std::endl;
    }

    return;
  }
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

VisualShaderGraphicsView::VisualShaderGraphicsView(VisualShaderGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent),
      scene(scene),
      context_menu(nullptr),
      create_node_action(nullptr),
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
  QObject::connect(create_node_action, &QAction::triggered, this,
                   &VisualShaderGraphicsView::on_create_node_action_triggered);
  context_menu->addAction(create_node_action);

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

VisualShaderGraphicsView::~VisualShaderGraphicsView() {}

//////////////////////////////
// Private slots
//////////////////////////////

void VisualShaderGraphicsView::on_create_node_action_triggered() {
  VisualShaderEditor* editor {scene->get_editor()};

  Q_EMIT editor->on_create_node_dialog_requested(this->last_context_menu_coordinate);
}

void VisualShaderGraphicsView::zoom_in() {
  const float factor{std::pow(zoom_step, zoom)};

  QTransform t{transform()};
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

  resetTransform();  // Reset the zoom level to 1.0f
  Q_EMIT zoom_changed(transform().m11());
}

void VisualShaderGraphicsView::zoom_out() {
  const float factor{std::pow(zoom_step, -1.0f * zoom)};

  QTransform t{transform()};
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

void VisualShaderGraphicsView::drawBackground(QPainter* painter, const QRectF& r) {
  QGraphicsView::drawBackground(painter, r);

  std::function<void(float)> draw_grid = [&](float grid_step) {
    QRect window_rect{this->rect()};

    QPointF tl{mapToScene(window_rect.topLeft())};
    QPointF br{mapToScene(window_rect.bottomRight())};

    float left{std::floor((float)tl.x() / grid_step)};
    float right{std::ceil((float)br.x() / grid_step)};
    float bottom{std::floor((float)tl.y() / grid_step)};
    float top{std::ceil((float)br.y() / grid_step)};

    // Vertical lines
    for (int xi{(int)left}; xi <= (int)right; ++xi) {
      QLineF line(xi * grid_step, bottom * grid_step, xi * grid_step, top * grid_step);
      painter->drawLine(line);
    }

    // Horizontal lines
    for (int yi{(int)bottom}; yi <= (int)top; ++yi) {
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

void VisualShaderGraphicsView::contextMenuEvent(QContextMenuEvent* event) {
  QGraphicsView::contextMenuEvent(event);

  // TODO: Make sure to not show the context menu if an item is under the mouse
  if (itemAt(event->pos())) {
    return;
  }

  this->last_context_menu_coordinate = (QPointF)event->globalPos();

  context_menu->exec({(int)last_context_menu_coordinate.x(), (int)last_context_menu_coordinate.y()});
}

void VisualShaderGraphicsView::wheelEvent(QWheelEvent* event) {
  float t_zoom{(float)transform().m11()};

  const QPoint delta{event->angleDelta()};

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

void VisualShaderGraphicsView::mousePressEvent(QMouseEvent* event) {
  QGraphicsView::mousePressEvent(event);

  switch (event->button()) {
    case Qt::LeftButton:
      last_click_coordinate = mapToScene(event->pos());
      break;
    default:
      break;
  }
}

void VisualShaderGraphicsView::mouseMoveEvent(QMouseEvent* event) {
  QGraphicsView::mouseMoveEvent(event);

  switch (event->buttons()) {
    case Qt::LeftButton: {
      QPointF current_coordinate{mapToScene(event->pos())};
      QPointF difference{last_click_coordinate - current_coordinate};
      setSceneRect(sceneRect().translated(difference.x(), difference.y()));
      last_click_coordinate = current_coordinate;
    } break;
    default:
      break;
  }
}

void VisualShaderGraphicsView::mouseReleaseEvent(QMouseEvent* event) { QGraphicsView::mouseReleaseEvent(event); }

void VisualShaderGraphicsView::showEvent(QShowEvent* event) {
  QGraphicsView::showEvent(event);

  move_view_to_fit_items();
}

void VisualShaderGraphicsView::move_view_to_fit_items() {
  if (!scene) {
    return;
  }

  if (scene->items().isEmpty()) {
    return;
  }

  std::cout << "Changing view port to fit items..." << std::endl;

  QRectF items_bounding_rect{scene->itemsBoundingRect()};
  items_bounding_rect.adjust(-fit_in_view_margin, -fit_in_view_margin, fit_in_view_margin, fit_in_view_margin);

  QPointF scene_tl{scene->sceneRect().topLeft()};
  QPointF scene_br{scene->sceneRect().bottomRight()};
  QPointF items_tl{items_bounding_rect.topLeft()};
  QPointF items_br{items_bounding_rect.bottomRight()};

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

VisualShaderNodeGraphicsObject::VisualShaderNodeGraphicsObject(const int& n_id, 
                                                               const QPointF& coordinate,
                                                               const std::shared_ptr<VisualShaderNode>& node,
                                                               QGraphicsItem* parent) : QGraphicsObject(parent),
                                                                                        n_id(n_id),
                                                                                        coordinate(coordinate),
                                                                                        node(node),
                                                                                        context_menu(nullptr),
                                                                                        delete_node_action(nullptr),
                                                                                        rect_width(0.0f),
                                                                                        caption_rect_height(0.0f),
                                                                                        rect_height(0.0f),
                                                                                        rect_margin(0.0f),
                                                                                        rect_padding(0.0f),
                                                                                        embed_widget(nullptr),
                                                                                        matching_image_widget(nullptr),
                                                                                        shader_previewer_widget(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);

  setPos(coordinate.x(), coordinate.y());

  // Output node should have a matching image widget
  if (n_id == (int)VisualShader::NODE_ID_OUTPUT) {
    QGraphicsProxyWidget* matching_image_widget_proxy{new QGraphicsProxyWidget(this)};
    matching_image_widget = new OriginalMatchingImageWidget();
    matching_image_widget_proxy->setWidget(matching_image_widget);
  } else {
    // Create the shader previewer widget
    QGraphicsProxyWidget* shader_previewer_widget_proxy{new QGraphicsProxyWidget(this)};
    shader_previewer_widget = new ShaderPreviewerWidget();
    shader_previewer_widget->setVisible(false);
    shader_previewer_widget_proxy->setWidget(shader_previewer_widget);
  }

  // Set the context menu
  context_menu = new QMenu();
  delete_node_action = new QAction(QStringLiteral("Delete Node"), context_menu);
  delete_node_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
  delete_node_action->setShortcut(QKeySequence(QKeySequence::Delete));
  QObject::connect(delete_node_action, &QAction::triggered, this,
                  &VisualShaderNodeGraphicsObject::on_delete_node_action_triggered);
  context_menu->addAction(delete_node_action);
}

VisualShaderNodeGraphicsObject::~VisualShaderNodeGraphicsObject() {
  if (context_menu) delete context_menu;
}

void VisualShaderNodeGraphicsObject::on_delete_node_action_triggered() {
  Q_EMIT node_deleted(n_id);
}

VisualShaderInputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_input_port_graphics_object(
    const int& p_index) const {
  if (in_port_graphics_objects.find(p_index) != in_port_graphics_objects.end()) {
    return in_port_graphics_objects.at(p_index);
  }

  return nullptr;
}

VisualShaderOutputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_output_port_graphics_object(
    const int& p_index) const {
  if (out_port_graphics_objects.find(p_index) != out_port_graphics_objects.end()) {
    return out_port_graphics_objects.at(p_index);
  }

  return nullptr;
}

QRectF VisualShaderNodeGraphicsObject::boundingRect() const {
  QFont f("Arial", caption_font_size);
  f.setBold(true);
  QFontMetrics fm(f);

  QString caption{QString::fromStdString(node->get_caption())};

  rect_width = (float)(fm.horizontalAdvance(caption, caption.length()) + caption_h_padding * 2.0f);

  caption_rect_height = (float)((fm.height()) + caption_v_padding * 2.0f);

  int max_num_ports{qMax(node->get_input_port_count(), node->get_output_port_count())};

  // Calculate the height of the node
  float t_rect_h{caption_rect_height};

  t_rect_h += body_rect_header_height;  // Header
  if (max_num_ports >= 0) {
    t_rect_h += (float)(max_num_ports - 1) * body_rect_port_step;  // Ports
  }
  t_rect_h += body_rect_footer_height;  // Footer

  rect_height = t_rect_h;

  // Correct node rect if it has an embed widget (if needed).
  if (embed_widget) {
    float embed_widget_width{(float)embed_widget->width()};

    // Find biggest horizontal length of input port names
    int in_p_count{node->get_input_port_count()};
    float max_in_p_width{0.0f};
    for (unsigned i{0}; i < in_p_count; ++i) {
      QString p_n{QString::fromStdString(node->get_input_port_name(i))};

      if (!p_n.isEmpty()) {
        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);

        float w{(float)fm.horizontalAdvance(p_n)};

        if (w > max_in_p_width) {
          max_in_p_width = w;
        }
      }
    }

    // Find biggest horizontal length of output port names
    int out_p_count{node->get_output_port_count()};
    float max_out_p_width{0.0f};
    for (unsigned i{0}; i < out_p_count; ++i) {
      QString p_n{QString::fromStdString(node->get_output_port_name(i))};

      if (!p_n.isEmpty()) {
        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);

        float w{(float)fm.horizontalAdvance(p_n)};

        if (w > max_out_p_width) {
          max_out_p_width = w;
        }
      }
    }

    float calculated_rect {max_in_p_width + embed_widget_width + max_out_p_width + embed_widget_h_padding * 2.0f};

    if (calculated_rect > rect_width) {
      rect_width = calculated_rect;
    }

    // Check the height
    float calculated_height{caption_rect_height + 
                            body_rect_header_height + 
                            embed_widget->height() + 
                            body_rect_footer_height + 
                            embed_widget_v_padding * 2.0f};

    if (calculated_height > rect_height) {
      rect_height = calculated_height;
    }
  }

  QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

  // Calculate the margin
  this->rect_margin = port_diameter * 0.5f;

  // Calculate the rect padding
  // We add a safe area around the rect to prevent the ports from being cut off
  this->rect_padding = port_diameter * 0.5f;

  r.adjust(-rect_margin - rect_padding, -rect_margin - rect_padding, rect_margin + rect_padding,
           rect_margin + rect_padding);

  return r;
}

void VisualShaderNodeGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  // Get the rect without the padding
  QRectF r{this->boundingRect()};

  // {
  //   // Draw Node Rect
  //   painter->setPen(Qt::red);
  //   painter->setBrush(Qt::NoBrush);
  //   painter->drawRect(r);
  // }

  // Add the padding to the rect
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

  // Draw Matching Image Widget
  if (n_id == (int)VisualShader::NODE_ID_OUTPUT) {
    float matching_image_widget_x{(float)r.x() + (float)r.width() + spacing_between_output_node_and_matching_image};
    float matching_image_widget_y{(float)r.y()};

    matching_image_widget->setGeometry(matching_image_widget_x, matching_image_widget_y, matching_image_widget->width(), matching_image_widget->height());
  } else {
    // Draw Shader Previewer Widget
    float shader_previewer_widget_x{(float)r.x()};
    float shader_previewer_widget_y{(float)r.y() + (float)r.height() + spacing_between_current_node_and_shader_previewer};
    shader_previewer_widget->setGeometry(shader_previewer_widget_x, shader_previewer_widget_y, shader_previewer_widget->width(), shader_previewer_widget->height());
  }

  // Add the margin to the rect
  r.adjust(rect_margin, rect_margin, -rect_margin, -rect_margin);

  // {
  //   // Draw Node Rect
  //   painter->setPen(Qt::red);
  //   painter->setBrush(Qt::NoBrush);
  //   painter->drawRect(r);
  // }

  float rect_x{(float)r.topLeft().x()};
  float rect_y{(float)r.topLeft().y()};

  float rect_w{(float)r.width()};
  float rect_h{(float)r.height()};

  QRectF caption_rect(rect_x, rect_y, rect_w, caption_rect_height);

  {
    // Draw Caption
    QString caption{QString::fromStdString(node->get_caption())};

    QFont t_f{painter->font()};

    QFont f("Arial", caption_font_size);
    f.setBold(true);
    QFontMetrics fm(f);
    painter->setFont(f);

    // Calculate the coordinates of the caption
    float x{(float)(caption_rect.center().x() - (float)fm.horizontalAdvance(caption) * 0.5f)};

    // Instead of subtracting, add the ascent to properly align text within the rect
    float y{(float)(caption_rect.center().y() + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent()))};

    QPointF coordinate{x, y};

    painter->setPen(this->font_color);
    painter->drawText(coordinate, caption);

    painter->setFont(t_f);  // Reset the font
  }

  QPointF caption_rect_bl{caption_rect.bottomLeft()};
  QPointF first_in_port_coordinate{caption_rect_bl.x(), caption_rect_bl.y() + body_rect_header_height};

  // Correct X coordinate: Remove the margin
  first_in_port_coordinate.setX((float)first_in_port_coordinate.x() - this->rect_margin);

  {
    // Draw Input Ports
    int in_port_count{node->get_input_port_count()};

    for (unsigned i{0}; i < in_port_count; ++i) {
      QPointF port_coordinate{first_in_port_coordinate.x(), first_in_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      // Draw caption
      QString p_n{QString::fromStdString(node->get_input_port_name(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);
        painter->setFont(f);

        float x{rect_x + port_caption_spacing};

        float y{(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

        QPointF coordinate{x, y};

        painter->setPen(this->font_color);
        painter->drawText(coordinate, p_n);

        painter->setFont(t_f);  // Reset the font
      }

      if (in_port_graphics_objects.find(i) != in_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderInputPortGraphicsObject* p_o{new VisualShaderInputPortGraphicsObject(port_rect, n_id, i, this)};
      in_port_graphics_objects[i] = p_o;

      // Connect the signals
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_pressed, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_pressed);
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dragged,
                       dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dragged);
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dropped,
                       dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dropped);
    }
  }

  QPointF caption_rect_br{caption_rect.bottomRight()};
  QPointF first_out_port_coordinate{caption_rect_br.x(), caption_rect_br.y() + body_rect_header_height};

  // Correct X coordinate: Remove the margin
  first_out_port_coordinate.setX((float)first_out_port_coordinate.x() + this->rect_margin);

  {
    // Draw Output Ports
    int out_port_count{node->get_output_port_count()};

    for (unsigned i{0}; i < out_port_count; ++i) {
      QPointF port_coordinate{first_out_port_coordinate.x(), first_out_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      // Draw caption
      QString p_n{QString::fromStdString(node->get_output_port_name(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);
        painter->setFont(f);

        float x{rect_x + rect_w - (float)fm.horizontalAdvance(p_n) - port_caption_spacing};

        float y{(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

        QPointF coordinate{x, y};

        painter->setPen(this->font_color);
        painter->drawText(coordinate, p_n);

        painter->setFont(t_f);  // Reset the font
      }

      if (out_port_graphics_objects.find(i) != out_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderOutputPortGraphicsObject* p_o{new VisualShaderOutputPortGraphicsObject(port_rect, n_id, i, this)};
      out_port_graphics_objects[i] = p_o;

      // Connect the signals
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_pressed, dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_pressed);
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dragged,
                       dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dragged);
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dropped,
                       dynamic_cast<VisualShaderGraphicsScene*>(scene()), &VisualShaderGraphicsScene::on_port_dropped);
    }
  }

  {
    // Correct the position of the embed widget
    if (embed_widget) {
      float embed_widget_x{rect_x + rect_w * 0.5f - embed_widget->width() * 0.5f};
      float embed_widget_y{rect_y + caption_rect_height + body_rect_header_height + embed_widget_v_padding};

      embed_widget->setGeometry(embed_widget_x, embed_widget_y, embed_widget->width(), embed_widget->height());
    }
  }
}

QVariant VisualShaderNodeGraphicsObject::itemChange(GraphicsItemChange change, const QVariant& value) {
  if (scene() && change == ItemScenePositionHasChanged) {
    Q_EMIT dynamic_cast<VisualShaderGraphicsScene*>(scene())->node_moved(n_id, pos());
  }

  return QGraphicsObject::itemChange(change, value);
}

void VisualShaderNodeGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
  QGraphicsObject::contextMenuEvent(event);

  context_menu->exec(event->screenPos());
}

VisualShaderInputPortGraphicsObject::VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id,
                                                                         const int& p_index, QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index), connection_graphics_object(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);
}

VisualShaderInputPortGraphicsObject::~VisualShaderInputPortGraphicsObject() {}

QRectF VisualShaderInputPortGraphicsObject::boundingRect() const {
  QRectF t_rect{rect};
  t_rect.adjust(-padding, -padding, padding, padding);
  return t_rect;
}

void VisualShaderInputPortGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  painter->setBrush(this->connection_point_color);

  painter->drawEllipse(rect);
}

void VisualShaderInputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  emit port_pressed(this, event->scenePos());
  QGraphicsObject::mousePressEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  emit port_dragged(this, event->scenePos());
  QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  emit port_dropped(this, event->scenePos());
  QGraphicsObject::mouseReleaseEvent(event);
}

VisualShaderOutputPortGraphicsObject::VisualShaderOutputPortGraphicsObject(const QRectF& rect, const int& n_id,
                                                                           const int& p_index, QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index), connection_graphics_object(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);
}

VisualShaderOutputPortGraphicsObject::~VisualShaderOutputPortGraphicsObject() {}

QRectF VisualShaderOutputPortGraphicsObject::boundingRect() const {
  QRectF t_rect{rect};
  t_rect.adjust(-padding, -padding, padding, padding);
  return t_rect;
}

void VisualShaderOutputPortGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                 QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  painter->setBrush(this->connection_point_color);

  painter->drawEllipse(rect);
}

void VisualShaderOutputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  emit port_pressed(this, event->scenePos());
  QGraphicsObject::mousePressEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  emit port_dragged(this, event->scenePos());
  QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
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
                                                                           QGraphicsItem* parent)
    : QGraphicsObject(parent),
      from_n_id(from_n_id),
      from_p_index(from_p_index),
      to_n_id((int)VisualShader::NODE_ID_INVALID),
      to_p_index((int)VisualShader::PORT_INDEX_INVALID),
      start_coordinate(start_coordinate),
      end_coordinate(start_coordinate),
      rect_padding(0.0f) {
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setAcceptedMouseButtons(Qt::NoButton);

  setZValue(-1.0f);
}

VisualShaderConnectionGraphicsObject::~VisualShaderConnectionGraphicsObject() {}

QRectF VisualShaderConnectionGraphicsObject::boundingRect() const {
  QRectF r{calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

  // Calculate the rect padding
  // We add a safe area around the rect to prevent the ports from being cut off
  // Due to inaccuracy in the calculation of the bounding rect we use the point diameter not the radius
  this->rect_padding = this->point_diameter;

  r.adjust(-rect_padding, -rect_padding, rect_padding, rect_padding);

  return r;
}

void VisualShaderConnectionGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                 QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  {
    // Draw the connection
    QPen p;
    p.setWidth(this->line_width);

    const bool selected{this->isSelected()};

    std::pair<QPointF, QPointF> control_points{calculate_control_points(start_coordinate, end_coordinate)};

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
    start_rect.adjust(-start_rect.width() * 0.5f, -start_rect.height() * 0.5f, -start_rect.width() * 0.5f,
                      -start_rect.height() * 0.5f);

    painter->drawEllipse(start_rect);
  }

  {
    // Draw end point
    QRectF end_rect(end_coordinate.x(), end_coordinate.y(), this->point_diameter, this->point_diameter);

    // Adjust the port rect to be centered
    end_rect.adjust(-end_rect.width() * 0.5f, -end_rect.height() * 0.5f, -end_rect.width() * 0.5f,
                    -end_rect.height() * 0.5f);

    painter->drawEllipse(end_rect);
  }
}

int VisualShaderConnectionGraphicsObject::detect_quadrant(const QPointF& reference, const QPointF& target) const {
  float relative_x{(float)(target.x() - reference.x())};
  float relative_y{(float)(target.y() - reference.y())};

  // Note that the default coordinate system in Qt is as follows:
  // - X-axis: Positive to the right, negative to the left
  // - Y-axis: Positive downwards, negative upwards

  // Check if the point is on an axis or the origin
  if (relative_x == 0 && relative_y == 0) {
    return 0;  // Stack on the reference
  } else if (relative_y == 0) {
    return (relative_x > 0) ? 5 : 6;  // On X-axis: 5 is the +ve part while 6 is the -ve one.
  } else if (relative_x == 0) {
    return (relative_y < 0) ? 7 : 8;  // On Y-axis: 7 is the +ve part while 8 is the -ve one.
  }

  // Determine the quadrant based on the relative coordinates
  if (relative_x > 0 && relative_y < 0) {
    return 1;  // Quadrant I
  } else if (relative_x < 0 && relative_y < 0) {
    return 2;  // Quadrant II
  } else if (relative_x < 0 && relative_y > 0) {
    return 3;  // Quadrant III
  } else if (relative_x > 0 && relative_y > 0) {
    return 4;  // Quadrant IV
  }

  // Default case (should not reach here)
  return -1;
}

QRectF VisualShaderConnectionGraphicsObject::calculate_bounding_rect_from_coordinates(
    const QPointF& start_coordinate, const QPointF& end_coordinate) const {
  const float x1{(float)start_coordinate.x()};
  const float y1{(float)start_coordinate.y()};
  const float x2{(float)end_coordinate.x()};
  const float y2{(float)end_coordinate.y()};

  // Calculate the expanded rect
  const float min_x{qMin(x1, x2)};
  const float min_y{qMin(y1, y2)};
  const float max_x{qMax(x1, x2)};
  const float max_y{qMax(y1, y2)};

  QRectF r({min_x, min_y}, QSizeF(max_x - min_x, max_y - min_y));

  const bool in_h_abnormal_region{x2 < (x1 + h_abnormal_offset)};
  const bool in_v_abnormal_region{std::abs(y2 - y1) < v_abnormal_offset};

  const int quadrant{detect_quadrant({x1, y1}, {x2, y2})};

  // We will expand the bounding rect horizontally so that our connection don't get cut off
  const float a_width_expansion{((x1 + h_abnormal_offset) - x2) * abnormal_face_to_back_control_width_expansion_factor};
  const float a_height_expansion{a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

  if (in_h_abnormal_region) {
    r.adjust(-a_width_expansion, 0.0f, a_width_expansion, 0.0f);
  }

  switch (quadrant) {
    case 2:  // Quadrant II: Abnormal face to back
    case 3:  // Quadrant III: Abnormal face to back
    case 6:  // On -ve X-axis: Abnormal face to back
      // Elipse like curve
      if (in_v_abnormal_region) {
        r.adjust(0.0f, -a_height_expansion, 0.0f, a_height_expansion);
      }
      break;
    default:
      break;
  }

  return r;
}

std::pair<QPointF, QPointF> VisualShaderConnectionGraphicsObject::calculate_control_points(
    const QPointF& start_coordinate, const QPointF& end_coordinated) const {
  QPointF cp1;
  QPointF cp2;

  const float x1{(float)start_coordinate.x()};
  const float y1{(float)start_coordinate.y()};
  const float x2{(float)end_coordinate.x()};
  const float y2{(float)end_coordinate.y()};

  QRectF r{calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

  const bool in_h_abnormal_region{x2 < (x1 + h_abnormal_offset)};
  const bool in_v_abnormal_region{std::abs(y2 - y1) < v_abnormal_offset};

  const int quadrant{detect_quadrant({x1, y1}, {x2, y2})};

  // We will expand the bounding rect horizontally so that our connection don't get cut off
  const float a_width_expansion{((x1 + h_abnormal_offset) - x2) * abnormal_face_to_back_control_width_expansion_factor};
  const float a_height_expansion{a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

  const float cp_x_delta_factor{0.8f};
  const float cp_y_delta_factor{0.25f};

  // Normal region control points deltas
  const float cp_x_delta{(float)r.width() * cp_x_delta_factor};
  const float cp_y_delta{(float)r.height() * cp_y_delta_factor};

  // Abnormal region control points deltas
  const float a_cp_x_delta{((float)r.width() - a_width_expansion) * cp_x_delta_factor};
  const float a_cp_y_delta{((float)r.height() - a_height_expansion) * cp_y_delta_factor};

  switch (quadrant) {
    case 1:  // Quadrant I: Normal face to back
      // Find out if the connection is going from left to right normally
      if (in_h_abnormal_region) {
        // The connection is not going from left to right normally
        // Our control points will be outside the end_coordinate and start_coordinate bounding rect
        // We will expand the bounding rect horizontally to make it easier to get an accurate coordinate of the size

        // Here we cover cases of nodes not facing each other.
        // This means we can't just send the path straight to the node.

        // Treated as inside Quadrant II
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      } else {
        // Treated as inside Quadrant I
        cp1 = {x1 + cp_x_delta, y1 - cp_y_delta};
        cp2 = {x2 - cp_x_delta, y2 + cp_y_delta};
      }
      break;
    case 2:  // Quadrant II: Abnormal face to back
      if (in_v_abnormal_region) {
        cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
        cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      } else {
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      }
      break;
    case 3:  // Quadrant III: Abnormal face to back
      if (in_v_abnormal_region) {
        cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
        cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      } else {
        cp1 = {x1 + a_width_expansion, y1};
        cp2 = {x2 - a_width_expansion, y2};
      }
      break;
    case 4:  // Quadrant IV: Normal face to back
      if (in_h_abnormal_region) {
        // Treated as inside Quadrant III
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      } else {
        // Treated as inside Quadrant IV
        cp1 = {x1 + cp_x_delta, y1 + cp_y_delta};
        cp2 = {x2 - cp_x_delta, y2 - cp_y_delta};
      }
      break;
    case 5:  // On +ve X-axis: Normal face to back
      // Straight line
      cp1 = {x1, y1};
      cp2 = {x2, y2};
      break;
    case 6:  // On -ve X-axis: Abnormal face to back
      // Elipse like curve
      cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
      cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      break;
    case 7:  // On +ve Y-axis: Abnormal face to back
    case 8:  // On -ve Y-axis: Abnormal face to back
      cp1 = {x1 + a_cp_x_delta, y1};
      cp2 = {x2 - a_cp_x_delta, y2};
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
/*****                 Embed Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderNodeEmbedWidget::VisualShaderNodeEmbedWidget(const std::shared_ptr<VisualShaderNode>& node, 
                                                         QWidget* parent) : QWidget(parent),
                                                                            layout(nullptr), 
                                                                            preview_shader_button(nullptr),
                                                                            shader_previewer_widget(nullptr) {
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(2);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  if (auto p {std::dynamic_pointer_cast<VisualShaderNodeInput>(node)}) {
    VisualShaderNodeInputEmbedWidget* embed_widget = new VisualShaderNodeInputEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeFloatFunc>(node)}) {
    VisualShaderNodeFloatFuncEmbedWidget* embed_widget = new VisualShaderNodeFloatFuncEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeIntFunc>(node)}) {
    VisualShaderNodeIntFuncEmbedWidget* embed_widget = new VisualShaderNodeIntFuncEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeUIntFunc>(node)}) {
    VisualShaderNodeUIntFuncEmbedWidget* embed_widget = new VisualShaderNodeUIntFuncEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeFloatOp>(node)}) {
    VisualShaderNodeFloatOpEmbedWidget* embed_widget = new VisualShaderNodeFloatOpEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeIntOp>(node)}) {
    VisualShaderNodeIntOpEmbedWidget* embed_widget = new VisualShaderNodeIntOpEmbedWidget(p);
    layout->addWidget(embed_widget);
  } else if (auto p {std::dynamic_pointer_cast<VisualShaderNodeUIntOp>(node)}) {
    VisualShaderNodeUIntOpEmbedWidget* embed_widget = new VisualShaderNodeUIntOpEmbedWidget(p);
    layout->addWidget(embed_widget);
  }

  // Create the button that will show/hide the shader previewer
  preview_shader_button = new QPushButton("Show Preview", this);
  preview_shader_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  preview_shader_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  preview_shader_button->setToolTip("Create a new node");
  layout->addWidget(preview_shader_button);
  QObject::connect(preview_shader_button, &QPushButton::pressed, this, &VisualShaderNodeEmbedWidget::on_preview_shader_button_pressed);

  this->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  setLayout(layout);
}

VisualShaderNodeEmbedWidget::~VisualShaderNodeEmbedWidget() {}

/*************************************/
/* Input Node                        */
/*************************************/

VisualShaderNodeInputEmbedWidget::VisualShaderNodeInputEmbedWidget(const std::shared_ptr<VisualShaderNodeInput>& node) : QComboBox(), 
                                                                                                                         node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  // Add the default item
  addItem(QString::fromStdString(node->get_input_name()), "");

  const VisualShaderNodeInput::Port* ps {VisualShaderNodeInput::get_ports()};

  int i{0};

  while (ps[i].type != VisualShaderNode::PortType::PORT_TYPE_ENUM_SIZE) {
    addItem(QString::fromStdString(ps[i].name));
    i++;
  }

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeInputEmbedWidget::on_current_index_changed);
}

VisualShaderNodeInputEmbedWidget::~VisualShaderNodeInputEmbedWidget() {}

void VisualShaderNodeInputEmbedWidget::on_current_index_changed(const int& index) {
  node->set_input_name(itemText(index).toStdString());
}

/*************************************/
/* Float Op Node                     */
/*************************************/

VisualShaderNodeFloatOpEmbedWidget::VisualShaderNodeFloatOpEmbedWidget(const std::shared_ptr<VisualShaderNodeFloatOp>& node) : QComboBox(), 
                                                                                                                               node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Add", (int)VisualShaderNodeFloatOp::OP_ADD);
  addItem("Subtract", (int)VisualShaderNodeFloatOp::OP_SUB);
  addItem("Multiply", (int)VisualShaderNodeFloatOp::OP_MUL);
  addItem("Divide", (int)VisualShaderNodeFloatOp::OP_DIV);
  addItem("Modulus", (int)VisualShaderNodeFloatOp::OP_MOD);
  addItem("Power", (int)VisualShaderNodeFloatOp::OP_POW);
  addItem("Maximum", (int)VisualShaderNodeFloatOp::OP_MAX);
  addItem("Minimum", (int)VisualShaderNodeFloatOp::OP_MIN);
  addItem("Arc Tangent 2", (int)VisualShaderNodeFloatOp::OP_ATAN2);
  addItem("Step", (int)VisualShaderNodeFloatOp::OP_STEP);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeFloatOpEmbedWidget::on_current_index_changed);
}

VisualShaderNodeFloatOpEmbedWidget::~VisualShaderNodeFloatOpEmbedWidget() {}

void VisualShaderNodeFloatOpEmbedWidget::on_current_index_changed(const int& index) {
  node->set_operator((VisualShaderNodeFloatOp::Operator)itemData(index).toInt());
}

/*************************************/
/* Int Op Node                       */
/*************************************/

VisualShaderNodeIntOpEmbedWidget::VisualShaderNodeIntOpEmbedWidget(const std::shared_ptr<VisualShaderNodeIntOp>& node) : QComboBox(), 
                                                                                                                         node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Add", (int)VisualShaderNodeIntOp::OP_ADD);
  addItem("Subtract", (int)VisualShaderNodeIntOp::OP_SUB);
  addItem("Multiply", (int)VisualShaderNodeIntOp::OP_MUL);
  addItem("Divide", (int)VisualShaderNodeIntOp::OP_DIV);
  addItem("Modulus", (int)VisualShaderNodeIntOp::OP_MOD);
  addItem("Maximum", (int)VisualShaderNodeIntOp::OP_MAX);
  addItem("Minimum", (int)VisualShaderNodeIntOp::OP_MIN);
  addItem("Bitwise AND", (int)VisualShaderNodeIntOp::OP_BITWISE_AND);
  addItem("Bitwise OR", (int)VisualShaderNodeIntOp::OP_BITWISE_OR);
  addItem("Bitwise XOR", (int)VisualShaderNodeIntOp::OP_BITWISE_XOR);
  addItem("Bitwise Left Shift", (int)VisualShaderNodeIntOp::OP_BITWISE_LEFT_SHIFT);
  addItem("Bitwise Right Shift", (int)VisualShaderNodeIntOp::OP_BITWISE_RIGHT_SHIFT);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeIntOpEmbedWidget::on_current_index_changed);
}

VisualShaderNodeIntOpEmbedWidget::~VisualShaderNodeIntOpEmbedWidget() {}

void VisualShaderNodeIntOpEmbedWidget::on_current_index_changed(const int& index) {
  node->set_operator((VisualShaderNodeIntOp::Operator)itemData(index).toInt());
}

/*************************************/
/* UInt Op Node                      */
/*************************************/

VisualShaderNodeUIntOpEmbedWidget::VisualShaderNodeUIntOpEmbedWidget(const std::shared_ptr<VisualShaderNodeUIntOp>& node) : QComboBox(), 
                                                                                                                            node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Add", (int)VisualShaderNodeUIntOp::OP_ADD);
  addItem("Subtract", (int)VisualShaderNodeUIntOp::OP_SUB);
  addItem("Multiply", (int)VisualShaderNodeUIntOp::OP_MUL);
  addItem("Divide", (int)VisualShaderNodeUIntOp::OP_DIV);
  addItem("Modulus", (int)VisualShaderNodeUIntOp::OP_MOD);
  addItem("Maximum", (int)VisualShaderNodeUIntOp::OP_MAX);
  addItem("Minimum", (int)VisualShaderNodeUIntOp::OP_MIN);
  addItem("Bitwise AND", (int)VisualShaderNodeUIntOp::OP_BITWISE_AND);
  addItem("Bitwise OR", (int)VisualShaderNodeUIntOp::OP_BITWISE_OR);
  addItem("Bitwise XOR", (int)VisualShaderNodeUIntOp::OP_BITWISE_XOR);
  addItem("Bitwise Left Shift", (int)VisualShaderNodeUIntOp::OP_BITWISE_LEFT_SHIFT);
  addItem("Bitwise Right Shift", (int)VisualShaderNodeUIntOp::OP_BITWISE_RIGHT_SHIFT);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeUIntOpEmbedWidget::on_current_index_changed);
}

VisualShaderNodeUIntOpEmbedWidget::~VisualShaderNodeUIntOpEmbedWidget() {}

void VisualShaderNodeUIntOpEmbedWidget::on_current_index_changed(const int& index) {
  node->set_operator((VisualShaderNodeUIntOp::Operator)itemData(index).toInt());
}

/*************************************/
/* Vector Op Node                    */
/*************************************/

VisualShaderNodeVectorOpEmbedWidget::VisualShaderNodeVectorOpEmbedWidget(const std::shared_ptr<VisualShaderNodeVectorOp>& node) : QComboBox(), 
                                                                                                                               node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Add", (int)VisualShaderNodeVectorOp::OP_ADD);
  addItem("Subtract", (int)VisualShaderNodeVectorOp::OP_SUB);
  addItem("Multiply", (int)VisualShaderNodeVectorOp::OP_MUL);
  addItem("Divide", (int)VisualShaderNodeVectorOp::OP_DIV);
  addItem("Modulus", (int)VisualShaderNodeVectorOp::OP_MOD);
  addItem("Power", (int)VisualShaderNodeVectorOp::OP_POW);
  addItem("Maximum", (int)VisualShaderNodeVectorOp::OP_MAX);
  addItem("Minimum", (int)VisualShaderNodeVectorOp::OP_MIN);
  addItem("Cross Product", (int)VisualShaderNodeVectorOp::OP_CROSS);
  addItem("Arc Tangent 2", (int)VisualShaderNodeVectorOp::OP_ATAN2);
  addItem("Reflect", (int)VisualShaderNodeVectorOp::OP_REFLECT);
  addItem("Step", (int)VisualShaderNodeVectorOp::OP_STEP);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeVectorOpEmbedWidget::on_current_index_changed);
}

VisualShaderNodeVectorOpEmbedWidget::~VisualShaderNodeVectorOpEmbedWidget() {}

void VisualShaderNodeVectorOpEmbedWidget::on_current_index_changed(const int& index) {
  node->set_operator((VisualShaderNodeVectorOp::Operator)itemData(index).toInt());
}

/*************************************/
/* Float Funcs Node                  */
/*************************************/

VisualShaderNodeFloatFuncEmbedWidget::VisualShaderNodeFloatFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeFloatFunc>& node) : QComboBox(), 
                                                                                                                                   node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Sin", (int)VisualShaderNodeFloatFunc::FUNC_SIN);
  addItem("Cos", (int)VisualShaderNodeFloatFunc::FUNC_COS);
  addItem("Tan", (int)VisualShaderNodeFloatFunc::FUNC_TAN);
  addItem("Arc Sine", (int)VisualShaderNodeFloatFunc::FUNC_ASIN);
  addItem("Arc Cosine", (int)VisualShaderNodeFloatFunc::FUNC_ACOS);
  addItem("Arc Tangent", (int)VisualShaderNodeFloatFunc::FUNC_ATAN);
  addItem("Sine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_SINH);
  addItem("Cosine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_COSH);
  addItem("Tangent Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_TANH);
  addItem("Logarithm", (int)VisualShaderNodeFloatFunc::FUNC_LOG);
  addItem("Exponential", (int)VisualShaderNodeFloatFunc::FUNC_EXP);
  addItem("Square Root", (int)VisualShaderNodeFloatFunc::FUNC_SQRT);
  addItem("Absolute", (int)VisualShaderNodeFloatFunc::FUNC_ABS);
  addItem("Sign", (int)VisualShaderNodeFloatFunc::FUNC_SIGN);
  addItem("Floor", (int)VisualShaderNodeFloatFunc::FUNC_FLOOR);
  addItem("Round", (int)VisualShaderNodeFloatFunc::FUNC_ROUND);
  addItem("Ceil", (int)VisualShaderNodeFloatFunc::FUNC_CEIL);
  addItem("Fraction", (int)VisualShaderNodeFloatFunc::FUNC_FRACT);
  addItem("Saturate", (int)VisualShaderNodeFloatFunc::FUNC_SATURATE);
  addItem("Negate", (int)VisualShaderNodeFloatFunc::FUNC_NEGATE);
  addItem("Arc Cosine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ACOSH);
  addItem("Arc Sine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ASINH);
  addItem("Arc Tangent Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ATANH);
  addItem("Degrees", (int)VisualShaderNodeFloatFunc::FUNC_DEGREES);
  addItem("Exponential 2", (int)VisualShaderNodeFloatFunc::FUNC_EXP2);
  addItem("Inverse Square Root", (int)VisualShaderNodeFloatFunc::FUNC_INVERSE_SQRT);
  addItem("Logarithm 2", (int)VisualShaderNodeFloatFunc::FUNC_LOG2);
  addItem("Radians", (int)VisualShaderNodeFloatFunc::FUNC_RADIANS);
  addItem("Reciprocal", (int)VisualShaderNodeFloatFunc::FUNC_RECIPROCAL);
  addItem("Round Even", (int)VisualShaderNodeFloatFunc::FUNC_ROUNDEVEN);
  addItem("Truncate", (int)VisualShaderNodeFloatFunc::FUNC_TRUNC);
  addItem("One Minus", (int)VisualShaderNodeFloatFunc::FUNC_ONEMINUS);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeFloatFuncEmbedWidget::on_current_index_changed);
}

VisualShaderNodeFloatFuncEmbedWidget::~VisualShaderNodeFloatFuncEmbedWidget() {}

void VisualShaderNodeFloatFuncEmbedWidget::on_current_index_changed(const int& index) {
  node->set_function((VisualShaderNodeFloatFunc::Function)itemData(index).toInt());
}

/*************************************/
/* Int Funcs Node                    */
/*************************************/

VisualShaderNodeIntFuncEmbedWidget::VisualShaderNodeIntFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeIntFunc>& node) : QComboBox(), 
                                                                                                                         node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Abs", (int)VisualShaderNodeIntFunc::FUNC_ABS);
  addItem("Negate", (int)VisualShaderNodeIntFunc::FUNC_NEGATE);
  addItem("Sign", (int)VisualShaderNodeIntFunc::FUNC_SIGN);
  addItem("Bitwise NOT", (int)VisualShaderNodeIntFunc::FUNC_BITWISE_NOT);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeIntFuncEmbedWidget::on_current_index_changed);
}

VisualShaderNodeIntFuncEmbedWidget::~VisualShaderNodeIntFuncEmbedWidget() {}

void VisualShaderNodeIntFuncEmbedWidget::on_current_index_changed(const int& index) {
  node->set_function((VisualShaderNodeIntFunc::Function)itemData(index).toInt());
}

/*************************************/
/* UInt Funcs Node                   */
/*************************************/

VisualShaderNodeUIntFuncEmbedWidget::VisualShaderNodeUIntFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeUIntFunc>& node) : QComboBox(), 
                                                                                                                            node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Negate", (int)VisualShaderNodeUIntFunc::FUNC_NEGATE);
  addItem("Bitwise NOT", (int)VisualShaderNodeUIntFunc::FUNC_BITWISE_NOT);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeUIntFuncEmbedWidget::on_current_index_changed);
}

VisualShaderNodeUIntFuncEmbedWidget::~VisualShaderNodeUIntFuncEmbedWidget() {}

void VisualShaderNodeUIntFuncEmbedWidget::on_current_index_changed(const int& index) {
  node->set_function((VisualShaderNodeUIntFunc::Function)itemData(index).toInt());
}

/*************************************/
/* Vector Funcs Node                 */
/*************************************/

VisualShaderNodeVectorFuncEmbedWidget::VisualShaderNodeVectorFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeVectorFunc>& node) : QComboBox(), 
                                                                                                                                   node(node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  addItem("Normalize", (int)VisualShaderNodeVectorFunc::FUNC_NORMALIZE);
  addItem("Saturate", (int)VisualShaderNodeVectorFunc::FUNC_SATURATE);
  addItem("Negate", (int)VisualShaderNodeVectorFunc::FUNC_NEGATE);
  addItem("Reciprocal", (int)VisualShaderNodeVectorFunc::FUNC_RECIPROCAL);
  addItem("Abs", (int)VisualShaderNodeVectorFunc::FUNC_ABS);
  addItem("Arc Cosine", (int)VisualShaderNodeVectorFunc::FUNC_ACOS);
  addItem("Arc Cosine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ACOSH);
  addItem("Arc Sine", (int)VisualShaderNodeVectorFunc::FUNC_ASIN);
  addItem("Arc Sine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ASINH);
  addItem("Arc Tangent", (int)VisualShaderNodeVectorFunc::FUNC_ATAN);
  addItem("Arc Tangent Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ATANH);
  addItem("Ceil", (int)VisualShaderNodeVectorFunc::FUNC_CEIL);
  addItem("Cos", (int)VisualShaderNodeVectorFunc::FUNC_COS);
  addItem("Cosine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_COSH);
  addItem("Degrees", (int)VisualShaderNodeVectorFunc::FUNC_DEGREES);
  addItem("Exp", (int)VisualShaderNodeVectorFunc::FUNC_EXP);
  addItem("Exp2", (int)VisualShaderNodeVectorFunc::FUNC_EXP2);
  addItem("Floor", (int)VisualShaderNodeVectorFunc::FUNC_FLOOR);
  addItem("Fraction", (int)VisualShaderNodeVectorFunc::FUNC_FRACT);
  addItem("Inverse Square Root", (int)VisualShaderNodeVectorFunc::FUNC_INVERSE_SQRT);
  addItem("Log", (int)VisualShaderNodeVectorFunc::FUNC_LOG);
  addItem("Log2", (int)VisualShaderNodeVectorFunc::FUNC_LOG2);
  addItem("Radians", (int)VisualShaderNodeVectorFunc::FUNC_RADIANS);
  addItem("Round", (int)VisualShaderNodeVectorFunc::FUNC_ROUND);
  addItem("Round Even", (int)VisualShaderNodeVectorFunc::FUNC_ROUNDEVEN);
  addItem("Sign", (int)VisualShaderNodeVectorFunc::FUNC_SIGN);
  addItem("Sin", (int)VisualShaderNodeVectorFunc::FUNC_SIN);
  addItem("Sine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_SINH);
  addItem("Sqrt", (int)VisualShaderNodeVectorFunc::FUNC_SQRT);
  addItem("Tan", (int)VisualShaderNodeVectorFunc::FUNC_TAN);
  addItem("Tangent Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_TANH);
  addItem("Truncate", (int)VisualShaderNodeVectorFunc::FUNC_TRUNC);
  addItem("One Minus", (int)VisualShaderNodeVectorFunc::FUNC_ONEMINUS);

  QObject::connect(this, 
                   QOverload<int>::of(&QComboBox::currentIndexChanged), 
                   this,
                   &VisualShaderNodeVectorFuncEmbedWidget::on_current_index_changed);
}

VisualShaderNodeVectorFuncEmbedWidget::~VisualShaderNodeVectorFuncEmbedWidget() {}

void VisualShaderNodeVectorFuncEmbedWidget::on_current_index_changed(const int& index) {
  node->set_function((VisualShaderNodeVectorFunc::Function)itemData(index).toInt());
}
