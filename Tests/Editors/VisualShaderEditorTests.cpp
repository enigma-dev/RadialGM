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

#include "Editors/VisualShaderEditorTests.h"

#include <QSignalSpy>
#include <QtTest/QtTest>

void TestVisualShaderEditor::initTestCase() {
  editor = new VisualShaderEditor();

  // Get the scene
  VisualShaderGraphicsScene* scene{editor->get_scene()};
  QVERIFY(scene != nullptr);

  {
    // Add an input UV node
    bool result{scene->add_node("VisualShaderNodeInput", {-200, 0})};
    QVERIFY(result);
  }

  {
    // Add an input TIME node
    bool result{scene->add_node("VisualShaderNodeInput", {-200, 350})};
    QVERIFY(result);
  }

  {
    // Add a value noise node
    bool result{scene->add_node("VisualShaderNodeValueNoise", {-50, 0})};
    QVERIFY(result);
  }

  {
    // Add a float function node: sin
    bool result{scene->add_node("VisualShaderNodeFloatFunc", {-50, 350})};
    QVERIFY(result);
  }

  {
    // Add a divide operator node
    bool result{scene->add_node("VisualShaderNodeFloatOp", {150, 150})};
    QVERIFY(result);
  }
}
void TestVisualShaderEditor::init() {}

void TestVisualShaderEditor::cleanupTestCase() { delete editor; }
void TestVisualShaderEditor::cleanup() {}

void TestVisualShaderEditor::test_create_full_graph() {
  editor->show();

  // Wait for the editor to be shown
  QVERIFY(QTest::qWaitForWindowExposed(editor));

  // Get the scene
  VisualShaderGraphicsScene* scene{editor->get_scene()};
  QVERIFY(scene != nullptr);
  VisualShaderGraphicsView* view{editor->get_view()};
  QVERIFY(view != nullptr);

  VisualShaderOutputPortGraphicsObject* o_p_uv{nullptr};

  {
    VisualShaderNodeGraphicsObject* uv_node{scene->get_node_graphics_object(1)};  // 1 is the node id
    QVERIFY(uv_node != nullptr);
    o_p_uv = uv_node->get_output_port_graphics_object(0);  // The only output port
    QVERIFY(o_p_uv != nullptr);
    QWidget* embed_widget{uv_node->get_embed_widget()};
    QVERIFY(embed_widget != nullptr);
    VisualShaderNodeInputEmbedWidget* uv_embed_widget{dynamic_cast<VisualShaderNodeInputEmbedWidget*>(embed_widget)};
    QVERIFY(uv_embed_widget != nullptr);
    uv_embed_widget->set_current_index(1);  // 1 is UV
  }

  VisualShaderOutputPortGraphicsObject* o_p_time{nullptr};

  {
    VisualShaderNodeGraphicsObject* uv_node{scene->get_node_graphics_object(2)};  // 2 is the node id
    QVERIFY(uv_node != nullptr);
    o_p_time = uv_node->get_output_port_graphics_object(0);  // The only output port
    QVERIFY(o_p_time != nullptr);
    QWidget* embed_widget{uv_node->get_embed_widget()};
    QVERIFY(embed_widget != nullptr);
    VisualShaderNodeInputEmbedWidget* uv_embed_widget{dynamic_cast<VisualShaderNodeInputEmbedWidget*>(embed_widget)};
    QVERIFY(uv_embed_widget != nullptr);
    uv_embed_widget->set_current_index(2);  // 2 is TIME
  }

  VisualShaderInputPortGraphicsObject* i_p_noise{nullptr};
  VisualShaderOutputPortGraphicsObject* o_p_noise{nullptr};

  {
    VisualShaderNodeGraphicsObject* noise_node{scene->get_node_graphics_object(3)};  // 3 is the node id
    QVERIFY(noise_node != nullptr);
    i_p_noise = noise_node->get_input_port_graphics_object(0);  // The only input port
    QVERIFY(i_p_noise != nullptr);
    o_p_noise = noise_node->get_output_port_graphics_object(0);  // The only output port
    QVERIFY(o_p_noise != nullptr);
  }

  VisualShaderInputPortGraphicsObject* i_p_sin{nullptr};
  VisualShaderOutputPortGraphicsObject* o_p_sin{nullptr};

  {
    VisualShaderNodeGraphicsObject* sin_node{scene->get_node_graphics_object(4)};  // 4 is the node id
    QVERIFY(sin_node != nullptr);
    i_p_sin = sin_node->get_input_port_graphics_object(0);  // The only input port
    QVERIFY(i_p_sin != nullptr);
    o_p_sin = sin_node->get_output_port_graphics_object(0);  // The only output port
    QVERIFY(o_p_sin != nullptr);
    QWidget* embed_widget{sin_node->get_embed_widget()};
    QVERIFY(embed_widget != nullptr);
    VisualShaderNodeFloatFuncEmbedWidget* sin_embed_widget{
        dynamic_cast<VisualShaderNodeFloatFuncEmbedWidget*>(embed_widget)};
    QVERIFY(sin_embed_widget != nullptr);
    sin_embed_widget->set_current_index(0);  // 0 is sin
  }

  VisualShaderInputPortGraphicsObject* i1_p_divide{nullptr};
  VisualShaderInputPortGraphicsObject* i2_p_divide{nullptr};
  VisualShaderOutputPortGraphicsObject* o_p_divide{nullptr};

  {
    VisualShaderNodeGraphicsObject* divide_node{scene->get_node_graphics_object(5)};  // 5 is the node id
    QVERIFY(divide_node != nullptr);
    i1_p_divide = divide_node->get_input_port_graphics_object(0);  // The first input port
    QVERIFY(i1_p_divide != nullptr);
    i2_p_divide = divide_node->get_input_port_graphics_object(1);  // The second input port
    QVERIFY(i2_p_divide != nullptr);
    o_p_divide = divide_node->get_output_port_graphics_object(0);  // The only output port
    QVERIFY(o_p_divide != nullptr);
    QWidget* embed_widget{divide_node->get_embed_widget()};
    QVERIFY(embed_widget != nullptr);
    VisualShaderNodeFloatOpEmbedWidget* divide_embed_widget{
        dynamic_cast<VisualShaderNodeFloatOpEmbedWidget*>(embed_widget)};
    QVERIFY(divide_embed_widget != nullptr);
    divide_embed_widget->set_current_index(3);  // 3 is divide
  }

  {
    // Connect UV to noise
  }

  // Wait
  QTest::qWait(10000);
}
