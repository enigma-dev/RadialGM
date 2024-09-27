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

#ifndef VISUAL_SHADER_EDITOR_TESTS_H
#define VISUAL_SHADER_EDITOR_TESTS_H

#include "Editors/VisualShaderEditor.h"

class TestVisualShaderEditor : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();  // Will be called before the first test function is executed.
  void init();        // Will be called before each test function is executed.

  void cleanupTestCase();  // Will be called after the last test function was executed.
  void cleanup();        // Will be called after every test function.

  void test_create_full_graph();

 private:
  VisualShaderEditor* editor = nullptr;
};

#endif  // VISUAL_SHADER_EDITOR_TESTS_H
