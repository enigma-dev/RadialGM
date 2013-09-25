/**
* @file  projectmanager.cpp
* @brief Source implementing a class for a project manager.
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

#include "projectmanager.h"
#include <string>
#include <QDebug>

ProjectManager::ProjectManager()
{
}

void ProjectManager::LoadGMK(QString filename) {
    gmk = new Gmk::GmkFile();
    std::string file = filename.toUtf8().constData();
    qDebug() << filename;
    gmk->Load(file);
    //gmk->Save(FILENAME_OUT);
    delete gmk;
}
