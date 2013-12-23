/**
* @file  iconmanager.h
* @brief Header implementing a class to manage icons.
*
* @section License
*
* Copyright (C) 2013 Josh Ventura <JoshV10@gmail.com>
* This file is a part of the RadialGM IDE.
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

#ifndef _NGM_ICONMANAGER__H
#define _NGM_ICONMANAGER__H

#include <map>
#include <vector>
#include <memory>

#include <QObject>
#include <QString>
#include <QIcon>

class IconManager: public QObject
{
  Q_OBJECT
  private:
    std::vector<QString> searchPaths; ///< List of paths to search for theme icons
    std::map<QString, int> extensionPriorities; ///< Mapping of known and preferred (or unpreferred) extensions
    std::map<QString, std::pair<QString, int> > pathCache; ///< Map intended to cache fully qualified paths and their respective priorities by unqualified icon names.
    std::map<QString, std::shared_ptr <QIcon> > iconCache; ///< Map intended to prevent loading the same icon twice.
    QString currentTheme; ///< The name of the current icon theme.

    void cachePathnames(); ///< Called to crawl the filesystem for icons, caching the most favorable name.
    void dumpCaches(); ///< Drop the caches; clear them and move their data to be collected.

  public:
    // Methods the main initializer and plugin initializers will use

    /// Default constructor.
    IconManager();

    /// Add a path from which to check for icons. The most obvious is "icons/".
    void addSearchPath(const QString& path);

    /// Set the priority of a given extension. A smaller number indicates a higher priority;
    /// the default priority is zero. Thus, a file having an extension with a priority of
    /// zero is chosen over a file of the same name with an extension having a priority of
    /// one, but is swept aside in favor of such a file with a priority of -1.
    /// @param extension  The extension whose priority is to be set.
    /// @param priority   The priority; -10 is a higher priority than 0, which is higher than 10.
    void setExtensionPriority(const QString &extension, int priority);

    /// Set the current icon theme, by name.
    void setIconTheme(const QString& iconTheme);

    /// Grab a list of available icon theme names.
    const std::vector<QString> getAvailableIconThemes();

    // Methods everyone will use

    /// Retrieve a QIcon for the current theme from its most basic name.
    /// @param iconName The unqualified name of this icon, eg, "LoadFile".
    std::shared_ptr<QIcon> getIcon(const QString &iconName);


  signals:
    /// Signaled when the current theme changes
    void iconThemeChanged(IconManager *manager);

};

#endif // _NGM_ICONMANAGER__H
