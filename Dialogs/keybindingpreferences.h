#ifndef KEYBINDINGPREFERENCES_H
#define KEYBINDINGPREFERENCES_H

#include <QWidget>
#include <QAction>
#include <QString>
#include <QList>

#include <functional>

using KeybindingFactory = std::function<QWidget*()>;
using KeybindingContext = QPair<QString,KeybindingFactory>;

extern QList<KeybindingContext> keybindingContexts;

#endif // KEYBINDINGPREFERENCES_H
