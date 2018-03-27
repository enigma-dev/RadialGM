#include "RGMPlugin.h"

RGMPlugin::RGMPlugin(MainWindow& mainWindow) : QObject(&mainWindow), mainWindow(mainWindow) {}
RGMPlugin::~RGMPlugin() {}
