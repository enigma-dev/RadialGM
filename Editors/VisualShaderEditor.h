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

#ifndef ENIGMA_VISUAL_SHADER_EDITOR_H
#define ENIGMA_VISUAL_SHADER_EDITOR_H

#include <QContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

#include <string>
#include <vector>

#include "ResourceTransformations/VisualShader/visual_shader.h"
#include "ResourceTransformations/VisualShader/visual_shader_nodes.h"
#include "ResourceTransformations/VisualShader/vs_noise_nodes.h"
#include "Editors/BaseEditor.h"

class VisualShaderGraphicsScene;
class VisualShaderGraphicsView;
class VisualShaderNodeGraphicsObject;
class VisualShaderConnectionGraphicsObject;
class CreateNodeDialog;
class VisualShaderInputPortGraphicsObject;
class VisualShaderOutputPortGraphicsObject;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderEditor                           *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

// Add const to any function that does not modify the object.

class VisualShaderEditor : public BaseEditor {
  Q_OBJECT

 public:
  VisualShaderEditor(QWidget* parent = nullptr);
  VisualShaderEditor(MessageModel* model, QWidget* parent = nullptr);
  ~VisualShaderEditor() override;

 Q_SIGNALS:
  /**
   * @brief Request the dialog that has all kinds of nodes we can
   *        create.
   * 
   * @note This signal is emitted from two sources: 
   *       @c VisualShaderEditor::on_create_node_button_pressed and
   *       @c VisualShaderGraphicsView::on_create_node_action_triggered slots 
   *       and it is connected to the @c VisualShaderEditor::show_create_node_dialog 
   *       function.
   * 
   * @param coordinate 
   */
  void on_create_node_dialog_requested(const QPointF& coordinate = {0,
                                                                    0});  // {0, 0} is the top-left corner of the scene.

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderEditor::create_node_button is pressed.
   * 
   * @note Connected in @c VisualShaderEditor::init function
   *       to @c QPushButton::pressed signal.
   * 
   * @note EMITS @c VisualShaderEditor::on_create_node_dialog_requested signal.
   * 
   */
  void on_create_node_button_pressed();
  void on_preview_shader_button_pressed();

  void on_menu_button_pressed();

 private:
  VisualShader* visual_shader;
  MessageModel* visual_shader_model;

  QHBoxLayout* layout;

  QWidget* side_widget;
  QVBoxLayout* side_outer_layout;
  QVBoxLayout* side_layout;
  QLineEdit* name_edit;
  QPushButton* save_button;

  QHBoxLayout* scene_layer_layout;
  QWidget* scene_layer;  // Layer having the scene.
  VisualShaderGraphicsScene* scene;
  VisualShaderGraphicsView* view;

  QWidget* top_layer;  // Layer having the menu bar.
  QHBoxLayout* menu_bar;

  QPushButton* menu_button;
  QPushButton* create_node_button;
  QPushButton* preview_shader_button;
  QPushButton* zoom_in_button;
  QPushButton* reset_zoom_button;
  QPushButton* zoom_out_button;

  QPushButton* load_image_button;
  QPushButton* match_image_button;

  QAction* create_node_action;

  ////////////////////////////////////
  // Code Previewer
  ////////////////////////////////////

  QDialog* code_previewer_dialog;
  QVBoxLayout* code_previewer_layout;
  QPlainTextEdit* code_previewer;

  ////////////////////////////////////
  // CreateNodeDialog Nodes Tree
  ////////////////////////////////////

  struct CreateNodeDialogNodesTreeItem {
    std::string name;
    std::string category_path;
    std::string type;
    std::string description;

    CreateNodeDialogNodesTreeItem(const std::string& name = std::string(),
                                  const std::string& category_path = std::string(),
                                  const std::string& type = std::string(),
                                  const std::string& description = std::string())
        : name(name), category_path(category_path), type(type), description(description) {}
  };

  static const VisualShaderEditor::CreateNodeDialogNodesTreeItem create_node_dialog_nodes_tree_items[];

  CreateNodeDialog* create_node_dialog;

  /**
   * @brief Initializes the UI
   * 
   * @note To be called from different constructors. This function shouldn't contain
   *       any code related to MessageModel class as this will break the tests.
   * 
   */
  void init();

  void init_graph();

  void create_node(const QPointF& coordinate);

  void add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate);

  void show_create_node_dialog(const QPointF& coordinate);

  std::vector<std::string> parse_node_category_path(const std::string& node_category_path);
  QTreeWidgetItem* find_or_create_category_item(QTreeWidgetItem* parent, const std::string& category,
                                                const std::string& category_path,
                                                QTreeWidget* create_node_dialog_nodes_tree,
                                                std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map);
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               CreateNodeDialog                             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class CreateNodeDialog : public QDialog {
  Q_OBJECT

 public:
  CreateNodeDialog(QWidget* parent = nullptr);
  ~CreateNodeDialog();

  QTreeWidget* get_nodes_tree() const { return create_node_dialog_nodes_tree; }

  QTreeWidgetItem* get_selected_item() const { return selected_item; }

 private Q_SLOTS:
  void on_create_node_button_pressed();
  void on_cancel_node_creation_button_pressed();

  void update_selected_item();

 private:
  QVBoxLayout* layout;

  QVBoxLayout* create_node_dialog_nodes_tree_layout;

  QTreeWidget* create_node_dialog_nodes_tree;
  QTextEdit* create_node_dialog_nodes_description;

  QHBoxLayout* buttons_layout;
  QPushButton* create_button;
  QPushButton* cancel_button;

  QTreeWidgetItem* selected_item;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsScene                    *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderGraphicsScene : public QGraphicsScene {
  Q_OBJECT

 public:
  VisualShaderGraphicsScene(VisualShader* vs, QObject* parent = nullptr);

  ~VisualShaderGraphicsScene();

  bool add_node(const std::string& type, const QPointF& coordinate);
  bool add_node(const int& n_id, const std::shared_ptr<VisualShaderNode>& n, const QPointF& coordinate, QWidget* embed_widget = nullptr);
  bool delete_node(const int& n_id);

  VisualShaderEditor* get_editor() const { return editor; }
  void set_editor(VisualShaderEditor* editor) const { this->editor = editor; }

  /**
     * @brief 
     * 
     * @note This function sets the @c temporary_connection_graphics_object if 
     *       we have a valid @c from_node_id and @c from_port_index only. Then it 
     *       resets it again if we have a valid @c to_node_id and @c to_port_index and 
     *       this is important because inside the drag and drop event, we need to know
     *       if we have a valid temporary connection or not.
     * 
     * @param from_node_id 
     * @param from_port_index 
     * @param to_node_id 
     * @param to_port_index 
     * @return true 
     * @return false 
     */
  bool add_connection(const int& from_node_id, const int& from_port_index,
                      const int& to_node_id = (int)VisualShader::NODE_ID_INVALID,
                      const int& to_port_index = (int)VisualShader::PORT_INDEX_INVALID);

  bool delete_connection(const int& from_node_id, const int& from_port_index,
                         const int& to_node_id = (int)VisualShader::NODE_ID_INVALID,
                         const int& to_port_index = (int)VisualShader::PORT_INDEX_INVALID);

  VisualShaderNodeGraphicsObject* get_node_graphics_object(const int& n_id) const;

 public Q_SLOTS:
  /**
   * @brief Called when an interaction with a port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function
   *       to @c VisualShaderInputPortGraphicsObject::port_* signals.
   * 
   * @param port 
   * @param coordinate 
   */
  void on_port_pressed(QGraphicsObject* port, const QPointF& coordinate);
  void on_port_dragged(QGraphicsObject* port, const QPointF& coordinate);
  void on_port_dropped(QGraphicsObject* port, const QPointF& coordinate);

 Q_SIGNALS:
  /**
   * @brief Notify the scene that a node has been moved.
   * 
   * @note EMITTED from @c VisualShaderNodeGraphicsObject::itemChange function.
   * 
   * @note Connected to @c VisualShaderGraphicsScene::on_node_moved slot in 
   *       @c VisualShaderGraphicsScene::VisualShaderGraphicsScene constructor.
   * 
   * @param n_id 
   * @param new_coordinate 
   */
  void node_moved(const int& n_id, const QPointF& new_coordinate);

 private Q_SLOTS:
  /**
   * @brief Called when a node is moved.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::VisualShaderGraphicsScene constructor
   *       to @c VisualShaderGraphicsScene::node_moved signal.
   * 
   * @param n_id 
   * @param new_coordinate 
   */
  void on_node_moved(const int& n_id, const QPointF& new_coordinate);

  /**
   * @brief Called when a delete node action is triggered.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::add_node function
   *       to @c VisualShaderNodeGraphicsObject::node_deleted signal.
   * 
   * @param n_id 
   */
  void on_node_deleted(const int& n_id);

 private:
  VisualShader* vs;
  mutable VisualShaderEditor* editor;

  std::unordered_map<int, VisualShaderNodeGraphicsObject*> node_graphics_objects;

  VisualShaderConnectionGraphicsObject* temporary_connection_graphics_object;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsView                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderGraphicsView : public QGraphicsView {
  Q_OBJECT

 public:
  VisualShaderGraphicsView(VisualShaderGraphicsScene* scene, QWidget* parent = nullptr);

  ~VisualShaderGraphicsView();

  float get_x() const { return rect_x; }
  float get_y() const { return rect_y; }
  float get_width() const { return rect_width; }
  float get_height() const { return rect_height; }

 public Q_SLOTS:
  /**
   * @todo If the button is pressed then zoom in from the center of the view.
   */
  void zoom_in();
  void reset_zoom();
  void zoom_out();

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderGraphicsView::create_node_action is triggered.
   * 
   * @note Connected in @c VisualShaderGraphicsView::VisualShaderGraphicsView constructor
   *       to @c QAction::triggered signal.
   * 
   * @note EMITS @c VisualShaderEditor::on_create_node_dialog_requested signal.
   * 
   */
  void on_create_node_action_triggered();

 Q_SIGNALS:
  void zoom_changed(const float& zoom);

 private:
  VisualShaderGraphicsScene* scene;

  // Style
  QColor background_color = QColor(40, 40, 40); // Dark Charcoal
  QColor fine_grid_color = QColor(50, 50, 50); // Soft Dark Gray
  QColor coarse_grid_color = QColor(30, 30, 30); // Muted Deep Gray

  // Scene Rect
  float t_size = std::numeric_limits<short>::max();  // 32767
  float rect_x = -1.0f * t_size * 0.5f;
  float rect_y = -1.0f * t_size * 0.5f;
  float rect_width = t_size;
  float rect_height = t_size;

  float fit_in_view_margin = 50.0f;

  // Zoom
  float zoom = 1.0f;
  float zoom_step = 1.2f;
  float zoom_min;
  float zoom_max;

  QMenu* context_menu;
  QPointF last_context_menu_coordinate = {0, 0};
  QAction* create_node_action;

  QAction* zoom_in_action;
  QAction* reset_zoom_action;
  QAction* zoom_out_action;

  QPointF last_click_coordinate;

  void drawBackground(QPainter* painter, const QRectF& r) override;
  void contextMenuEvent(QContextMenuEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void showEvent(QShowEvent* event) override;

  void move_view_to_fit_items();
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderNodeGraphicsObject               *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderNodeGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderNodeGraphicsObject(const int& n_id, 
                                 const QPointF& coordinate, 
                                 const std::shared_ptr<VisualShaderNode>& node,
                                 QGraphicsItem* parent = nullptr);
  ~VisualShaderNodeGraphicsObject();

  VisualShaderInputPortGraphicsObject* get_input_port_graphics_object(const int& p_index) const;
  VisualShaderOutputPortGraphicsObject* get_output_port_graphics_object(const int& p_index) const;

  void set_embed_widget(QWidget* embed_widget) { this->embed_widget = embed_widget; }

 Q_SIGNALS:
  /**
   * @brief Send a request to delete a node.
   * 
   * @note EMITTED from @c VisualShaderNodeGraphicsObject::on_delete_node_action_triggered slot.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::add_node function to 
   *       @c VisualShaderGraphicsScene::on_node_deleted slot.
   * 
   * @param n_id 
   */
  void node_deleted(const int& n_id);

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderNodeGraphicsObject::delete_node_action is triggered.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::VisualShaderNodeGraphicsObject constructor
   *       to @c QAction::triggered signal.
   * 
   * @note EMITS @c VisualShaderNodeGraphicsObject::node_deleted signal.
   * 
   */
  void on_delete_node_action_triggered();

 private:
  int n_id;
  QPointF coordinate;
  std::shared_ptr<VisualShaderNode> node;

  QMenu* context_menu;
  QAction* delete_node_action;

  std::unordered_map<int, VisualShaderInputPortGraphicsObject*> in_port_graphics_objects;
  std::unordered_map<int, VisualShaderOutputPortGraphicsObject*> out_port_graphics_objects;

  // Style
  QColor normal_boundary_color = QColor(220, 20, 60); // Crimson Red
  QColor selected_boundary_color = QColor(255, 69, 0); // Red-Orange
  QColor font_color = QColor(255, 255, 255); // Pure White
  QColor fill_color = QColor(40, 40, 40, 200); // Semi-transparent Dark Gray

  float pen_width = 1.0f;

  float opacity = 0.8f;
  float corner_radius = 3.0f;
  float port_diameter = 8.0f;

  float caption_h_padding = 10.0f;
  float caption_v_padding = 8.0f;

  mutable float rect_width;           // Calculated in boundingRect()
  mutable float caption_rect_height;  // Calculated in boundingRect()

  mutable float rect_height;  // Calculated in boundingRect()
  float body_rect_header_height = 30.0f;
  float body_rect_port_step = 40.0f;
  float body_rect_footer_height = 30.0f;

  mutable float rect_padding;  // Calculated in boundingRect()
  mutable float rect_margin;   // Calculated in boundingRect()

  // Ports Style
  float connected_port_diameter = 8.0f;
  float unconnected_port_diameter = 6.0f;

  // Caption
  float caption_font_size = 18.0f;
  float port_caption_font_size = 8.0f;

  QWidget* embed_widget;
  float embed_widget_h_padding = 10.0f;
  float embed_widget_v_padding = 5.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

class VisualShaderInputPortGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                      QGraphicsItem* parent = nullptr);
  ~VisualShaderInputPortGraphicsObject();

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }

  VisualShaderConnectionGraphicsObject* get_connection_graphics_object() const { return connection_graphics_object; }
  void connect(VisualShaderConnectionGraphicsObject* c_g_o) const { this->connection_graphics_object = c_g_o; }
  void detach_connection() const { this->connection_graphics_object = nullptr; }
  bool is_connected() const { return connection_graphics_object != nullptr; }

 Q_SIGNALS:
  /**
   * @brief Called when the an interaction with the port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function to
   *       @c VisualShaderGraphicsScene::on_port_* slots.
   * 
   * @param port 
   * @param pos 
   */
  void port_pressed(VisualShaderInputPortGraphicsObject* port, const QPointF& pos);
  void port_dragged(VisualShaderInputPortGraphicsObject* port, const QPointF& pos);
  void port_dropped(VisualShaderInputPortGraphicsObject* port, const QPointF& pos);

 private:
  int n_id;
  int p_index;
  QRectF rect;

  mutable VisualShaderConnectionGraphicsObject* connection_graphics_object;

  float padding = 0.5f;

  // Style
  QColor font_color = QColor(255, 255, 255);
  QColor connection_point_color = QColor(220, 20, 60); // Crimson

  float opacity = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class VisualShaderOutputPortGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderOutputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                       QGraphicsItem* parent = nullptr);
  ~VisualShaderOutputPortGraphicsObject();

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }

  VisualShaderConnectionGraphicsObject* get_connection_graphics_object() const { return connection_graphics_object; }
  void connect(VisualShaderConnectionGraphicsObject* c_o) const { this->connection_graphics_object = c_o; }
  void detach_connection() const { this->connection_graphics_object = nullptr; }
  bool is_connected() const { return connection_graphics_object != nullptr; }

 Q_SIGNALS:
  /**
   * @brief Called when the an interaction with the port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function to
   *       @c VisualShaderGraphicsScene::on_port_* slots.
   * 
   * @param port 
   * @param pos 
   */
  void port_pressed(VisualShaderOutputPortGraphicsObject* port, const QPointF& pos);
  void port_dragged(VisualShaderOutputPortGraphicsObject* port, const QPointF& pos);
  void port_dropped(VisualShaderOutputPortGraphicsObject* port, const QPointF& pos);

 private:
  int n_id;
  int p_index;
  QRectF rect;

  mutable VisualShaderConnectionGraphicsObject* connection_graphics_object;

  float padding = 0.5f;

  // Style
  QColor font_color = QColor(255, 255, 255);
  QColor connection_point_color = QColor(220, 20, 60); // Crimson

  float opacity = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****           VisualShaderConnectionGraphicsObject             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderConnectionGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderConnectionGraphicsObject(const int& from_n_id, const int& from_p_index, const QPointF& start_coordinate,
                                       QGraphicsItem* parent = nullptr);
  ~VisualShaderConnectionGraphicsObject();

  int get_from_node_id() const { return from_n_id; }
  int get_from_port_index() const { return from_p_index; }

  int get_to_node_id() const { return to_n_id; }
  int get_to_port_index() const { return to_p_index; }

  void detach_end() const {
    this->set_to_node_id((int)VisualShader::NODE_ID_INVALID);
    this->set_to_port_index((int)VisualShader::PORT_INDEX_INVALID);
  }

  void set_to_node_id(const int& to_n_id) const { this->to_n_id = to_n_id; }
  void set_to_port_index(const int& to_p_index) const { this->to_p_index = to_p_index; }

  void set_start_coordinate(const QPointF& start_coordinate) {
    this->start_coordinate = start_coordinate;
    update();
  }
  void set_end_coordinate(const QPointF& end_coordinate) {
    this->end_coordinate = end_coordinate;
    update();
  }

 private:
  int from_n_id;
  mutable int to_n_id;
  int from_p_index;
  mutable int to_p_index;

  QPointF start_coordinate;
  QPointF end_coordinate;

  // Style
  QColor construction_color = QColor(139, 0, 0); // Dark Red
  QColor normal_color = QColor(178, 34, 34); // Firebrick Red
  QColor selected_color = QColor(55, 55, 55); // Dark Gray
  QColor connection_point_color = QColor(211, 211, 211);

  float line_width = 3.0f;
  float construction_line_width = 2.0f;
  float point_diameter = 10.0f;

  mutable float rect_padding;  // Calculated in boundingRect()

  float h_abnormal_offset = 50.0f;
  float v_abnormal_offset = 40.0f;
  float abnormal_face_to_back_control_width_expansion_factor = 0.5f;
  float abnormal_face_to_back_control_height_expansion_factor = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  int detect_quadrant(const QPointF& reference, const QPointF& target) const;
  QRectF calculate_bounding_rect_from_coordinates(const QPointF& start_coordinate, const QPointF& end_coordinate) const;
  std::pair<QPointF, QPointF> calculate_control_points(const QPointF& start_coordinate,
                                                       const QPointF& end_coordinate) const;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****                 Embed Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderNodeInputEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeInputEmbedWidget(const std::shared_ptr<VisualShaderNodeInput>& node);
  ~VisualShaderNodeInputEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeInput> node;
};

/*************************************/
/* Float Op Node                     */
/*************************************/

class VisualShaderNodeFloatOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFloatOpEmbedWidget(const std::shared_ptr<VisualShaderNodeFloatOp>& node);
  ~VisualShaderNodeFloatOpEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeFloatOp> node;
};

/*************************************/
/* Int Op Node                       */
/*************************************/

class VisualShaderNodeIntOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeIntOpEmbedWidget(const std::shared_ptr<VisualShaderNodeIntOp>& node);
  ~VisualShaderNodeIntOpEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeIntOp> node;
};

/*************************************/
/* UInt Op Node                      */
/*************************************/

class VisualShaderNodeUIntOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeUIntOpEmbedWidget(const std::shared_ptr<VisualShaderNodeUIntOp>& node);
  ~VisualShaderNodeUIntOpEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeUIntOp> node;
};

/*************************************/
/* Vector Op Node                    */
/*************************************/

class VisualShaderNodeVectorOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeVectorOpEmbedWidget(const std::shared_ptr<VisualShaderNodeVectorOp>& node);
  ~VisualShaderNodeVectorOpEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeVectorOp> node;
};

/*************************************/
/* Float Funcs Node                  */
/*************************************/

class VisualShaderNodeFloatFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFloatFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeFloatFunc>& node);
  ~VisualShaderNodeFloatFuncEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeFloatFunc> node;
};

/*************************************/
/* Int Funcs Node                    */
/*************************************/

class VisualShaderNodeIntFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeIntFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeIntFunc>& node);
  ~VisualShaderNodeIntFuncEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeIntFunc> node;
};

/*************************************/
/* UInt Funcs Node                   */
/*************************************/

class VisualShaderNodeUIntFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeUIntFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeUIntFunc>& node);
  ~VisualShaderNodeUIntFuncEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeUIntFunc> node;
};

/*************************************/
/* Vector Funcs Node                 */
/*************************************/

class VisualShaderNodeVectorFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeVectorFuncEmbedWidget(const std::shared_ptr<VisualShaderNodeVectorFunc>& node);
  ~VisualShaderNodeVectorFuncEmbedWidget();

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  std::shared_ptr<VisualShaderNodeVectorFunc> node;
};

#endif  // ENIGMA_VISUAL_SHADER_EDITOR_H
