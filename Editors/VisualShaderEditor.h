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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QContextMenuEvent>

#include <string>
#include <vector>

#include "ResourceTransformations/VisualShader/visual_shader.h"
#include "BaseEditor.h"

class VisualShaderGraphicsScene;
class VisualShaderGraphicsView;
class VisualShaderNodeGraphicsObject;
class VisualShaderConnectionGraphicsObject;
class CreateNodeDialog;

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
  VisualShaderEditor(MessageModel* model, QWidget* parent = nullptr);
  ~VisualShaderEditor() override;

  void create_node(const QPointF& coordinate);

  void add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate);

  void show_create_node_dialog(const QPointF& coordinate);

  std::vector<std::string> pasre_node_category_path(const std::string& node_category_path);
  QTreeWidgetItem* find_or_create_category_item(QTreeWidgetItem* parent, const std::string& category, const std::string& category_path, QTreeWidget* create_node_dialog_nodes_tree, std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map);

 Q_SIGNALS:
  void on_create_node_dialog_requested(const QPointF& coordinate = {0, 0}); // {0, 0} is the top-left corner of the scene.

 private Q_SLOTS:
  void on_create_node_button_pressed();
  void on_create_node_action_triggered();

 private:
  VisualShader* visual_shader;

  QHBoxLayout* layout;

  QHBoxLayout* scene_layer_layout;
  QWidget* scene_layer; // Layer having the scene.
  VisualShaderGraphicsScene* scene;
  VisualShaderGraphicsView* view;

  QWidget* top_layer; // Layer having the menu bar.
  QHBoxLayout* menu_bar;

  QPushButton* create_node_button;
  QPushButton* preview_shader_button;
  QPushButton* zoom_in_button;
  QPushButton* reset_zoom_button;
  QPushButton* zoom_out_button;

  QAction* create_node_action;

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
                                  const std::string& description = std::string()) : name(name), 
                                                                                    category_path(category_path), 
                                                                                    type(type), 
                                                                                    description(description) {}

  };

  static const VisualShaderEditor::CreateNodeDialogNodesTreeItem create_node_dialog_nodes_tree_items[];

  CreateNodeDialog* create_node_dialog;
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
	VisualShaderGraphicsScene(VisualShader* vs, QObject *parent = nullptr);

	~VisualShaderGraphicsScene();

    bool add_node(const int& n_id);
    bool delete_node();

    bool add_connection();
    bool delete_connection();

    VisualShaderNodeGraphicsObject* get_node_graphics_object(const int& n_id) const;
    VisualShaderConnectionGraphicsObject* get_connection_graphics_object(const int& c_id) const;

    VisualShader* get_visual_shader() const { return vs; }

private:
    VisualShader* vs;

    std::unordered_map<int, VisualShaderNodeGraphicsObject*> node_graphics_objects;
    std::unordered_map<int, VisualShaderConnectionGraphicsObject*> connection_graphics_objects;

    VisualShaderConnectionGraphicsObject* one_free_end_connection;
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
	VisualShaderGraphicsView(VisualShaderGraphicsScene *scene, QWidget *parent = nullptr);

	~VisualShaderGraphicsView();

public Q_SLOTS:
    void zoom_in();
    void reset_zoom();
    void zoom_out();

private Q_SLOTS:
    void on_create_node_action_triggered();
    void on_delete_node_action_triggered();

Q_SIGNALS:
	void zoom_changed(const float& zoom);

private:
    // Style
    QColor background_color = QColor(53, 53, 53);
    QColor fine_grid_color = QColor(60, 60, 60);
    QColor coarse_grid_color = QColor(25, 25, 25);

    // Scene Rect
    float t_size = std::numeric_limits<short>::max(); // 32767
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
    QAction* create_node_action;

    QAction* delete_node_action;

    QAction* zoom_in_action;
    QAction* reset_zoom_action;
    QAction* zoom_out_action;

    QPointF last_click_coordinate;

    void drawBackground(QPainter *painter, const QRectF &r) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
	void showEvent(QShowEvent *event) override;

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
    VisualShaderNodeGraphicsObject(VisualShader* vs, const int& n_id, QGraphicsItem *parent = nullptr);
    ~VisualShaderNodeGraphicsObject();

private:
    VisualShader* vs;
    int n_id;

    // Style
    QColor normal_boundary_color = QColor(255, 255, 255);
    QColor selected_boundary_color = QColor(255, 165, 0);
    QColor gradient_color0 = QColor(80, 80, 80);
    QColor gradient_color1 = QColor(64, 64, 64);
    QColor gradient_color2 = QColor(58, 58, 58);
    QColor shadow_color = QColor(20, 20, 20);
    QColor font_color = QColor(255, 255, 255);
    QColor font_color_faded = QColor(169, 169, 169);
    QColor connection_point_color = QColor(169, 169, 169);
    QColor filled_connection_point_color = QColor(0, 255, 255);
    QColor error_color = QColor(255, 0, 0);
    QColor warning_color = QColor(128, 128, 0);
    QColor fill_color = QColor(0, 0, 0, 0);

    float pen_width = 1.0f;
    float hovered_pen_width = 1.5f;

    float opacity = 0.8f;
    float corner_radius = 3.0f;

    float port_spacing = 10.0f;

    float rect_margin_ratio = 0.2f;

    // Ports Style
    float connected_port_diameter = 8.0f;
    float unconnected_port_diameter = 6.0f;

    // Size
    QSizeF size = QSizeF(100.0f, 200.0f);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

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
    VisualShaderConnectionGraphicsObject(QGraphicsItem *parent = nullptr);
    ~VisualShaderConnectionGraphicsObject();

private:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               NodesCustomWidget                            *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class NodesCustomWidget : public QWidget
{
    Q_OBJECT

public:
    NodesCustomWidget(const std::shared_ptr<VisualShaderNode>& node, QWidget *parent = nullptr);
    ~NodesCustomWidget();

private Q_SLOTS:
    void on_combo_box0_current_index_changed(const int& index);

private:
    QVBoxLayout* layout;

    QComboBox* combo_boxes[2];
};

#endif // ENIGMA_VISUAL_SHADER_EDITOR_H
