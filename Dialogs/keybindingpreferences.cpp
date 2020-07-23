#include "KeybindingPreferences.h"

#include "ui_MainWindow.h"
#include "ui_SpriteEditor.h"
#include "ui_SoundEditor.h"
#include "ui_BackgroundEditor.h"
#include "ui_PathEditor.h"
#include "ui_FontEditor.h"
#include "ui_SettingsEditor.h"
#include "ui_TimelineEditor.h"
#include "ui_ObjectEditor.h"
#include "ui_RoomEditor.h"

template <typename T, typename P = QWidget>
P *keybindingFactory() {
  P* context = new P();
  T *form = new T();
  form->setupUi(context);
  return context;
}

QList<KeybindingContext> keybindingContexts = {
  {QObject::tr("Global"),keybindingFactory<Ui::MainWindow,QMainWindow>},
  {QObject::tr("Sprite Editor"),keybindingFactory<Ui::SpriteEditor>},
  {QObject::tr("Sound Editor"),keybindingFactory<Ui::SoundEditor>},
  {QObject::tr("Background Editor"),keybindingFactory<Ui::BackgroundEditor>},
  {QObject::tr("Path Editor"),keybindingFactory<Ui::PathEditor>},
  //{QObject::tr("Script Editor"),keybindingFactory<Ui::ScriptEditor>},
  //{QObject::tr("Shader Editor"),keybindingFactory<Ui::ShaderEditor>},
  {QObject::tr("Font Editor"),keybindingFactory<Ui::FontEditor>},
  {QObject::tr("Timeline Editor"),keybindingFactory<Ui::TimelineEditor>},
  {QObject::tr("Object Editor"),keybindingFactory<Ui::ObjectEditor>},
  {QObject::tr("Room Editor"),keybindingFactory<Ui::RoomEditor>}
};
