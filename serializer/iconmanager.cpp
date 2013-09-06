/**
* @file  iconmanager.cpp
* @brief Header implementing a class to manage icons.
*
* @section License
*
* Copyright (C) 2013 Josh Ventura <JoshV10@gmail.com>
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

#include "iconmanager.h"
#include <QDirIterator>
#include <set>

using std::map;
using std::set;
using std::pair;
using std::vector;
using std::shared_ptr;

IconManager::IconManager() {}

void IconManager::addSearchPath(const QString& path) {
    QString nixPath = path;
    nixPath.replace('\\', '/');
    if (!nixPath.endsWith('/'))
        nixPath.append('/');
    searchPaths.push_back(nixPath);
}

void IconManager::setExtensionPriority(const QString &extension, int priority) {
    extensionPriorities[extension] = priority;
}

void IconManager::setIconTheme(const QString& iconTheme) {
    currentTheme = iconTheme;
    cachePathnames();
}

const vector<QString> IconManager::getAvailableIconThemes() {
    set<QString> foundThemes;
    vector<QString> result;
    for (auto path = searchPaths.begin(); path != searchPaths.end(); ++path) {
        QDir dir(*path);
        if (dir.exists()) {
            QDirIterator dirit(dir);
            while (dirit.hasNext()) {
                QString theme = dirit.next();
                if (foundThemes.find(theme) == foundThemes.end()) {
                    foundThemes.insert(theme);
                    result.push_back(theme);
                }
            }
        }
    }
    return result;
}

std::shared_ptr<QIcon> IconManager::getIcon(const QString &iconName) {
    auto iconIt = iconCache.find(iconName);
    if (iconIt != iconCache.end())
        return iconIt->second;
    auto iconPathIt = pathCache.find(iconName);
    QIcon *allocIcon = (iconPathIt == pathCache.end()) ? new QIcon() : new QIcon(iconPathIt->first);
    std::shared_ptr<QIcon> result(allocIcon);
    iconCache[iconName] = result;
    return result;
}

void IconManager::cachePathnames() {
    pathCache.clear();
    iconCache.clear();
    for (auto path = searchPaths.begin(); path != searchPaths.end(); ++path) {
        QDir dir(*path);
        if (dir.exists()) {
            QDirIterator dirit(dir);
            while (dirit.hasNext()) {
                QString fn = dirit.next();
                QString nixname = fn.replace('\\', '/');
                QString filename;
                int fileNameStart = fn.lastIndexOf('/');
                int extensionStart = fn.lastIndexOf('.', fileNameStart);
                int priority = 0;
                if (extensionStart != -1) {
                    auto extit = extensionPriorities.find(nixname.mid(extensionStart + 1));
                    if (extit != extensionPriorities.end())
                        priority = extit->second;
                    filename = nixname.mid(fileNameStart + 1, extensionStart - fileNameStart - 1);
                }
                else filename = nixname.mid(fileNameStart + 1);

                auto mapFile = pathCache.find(filename);
                if (mapFile == pathCache.end() || priority < mapFile->second.second)
                    pathCache[filename] = pair<QString, int>(fn, priority);
            }
        }
    }

}
