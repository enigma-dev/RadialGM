#ifndef UTILITY_H
#define UTILITY_H

#endif  // UTILITY_H

#include <QFileDialog>

enum FileDialog_t { BackgroundLoad, BackgroundSave, SpriteLoad, SpriteSave, SoundSave, SoundLoad };

class FileDialog : public QFileDialog {
 public:
  FileDialog(QWidget* parent, FileDialog_t type, bool writer);
};
