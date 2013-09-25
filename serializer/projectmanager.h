/**
* @file  projectmanager.h
* @brief Header implementing a class for a project manager.
*
* @section License
*
* Copyright (C) 2013 Robert B. Colton
* This file is a part of the LateralGM IDE.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>
#include "gmk/src/include/gmk.h"

class ProjectManager
{
public:
    ProjectManager();
    Gmk::GmkFile* gmk;

    void LoadGMK(QString filename);
};

#endif // PROJECTMANAGER_H
