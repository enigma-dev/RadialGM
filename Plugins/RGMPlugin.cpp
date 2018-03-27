#include "RGMPlugin.h"

RGMPlugin::RGMPlugin(const QApplication& app, MainWindow& mainWindow) : QObject(), app(app), mainWindow(mainWindow) {}
RGMPlugin::~RGMPlugin() {}
