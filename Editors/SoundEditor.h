#ifndef SOUNDEDITOR_H
#define SOUNDEDITOR_H

#include "BaseEditor.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class SoundEditor;
}

class SoundEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit SoundEditor(ProtoModelPtr model, QWidget* parent);
  ~SoundEditor() override;

 public slots:
  void RebindSubModels() override;

 private slots:
  void on_playButton_clicked();
  void on_loopButton_clicked();
  void on_playbackSlider_sliderPressed();
  void on_playbackSlider_sliderReleased();
  void on_volumeSlider_sliderMoved(int position);
  void on_volumeSpinBox_valueChanged(double arg1);
  void on_saveAsButton_clicked();
  void on_loadButton_clicked();
  void on_editButton_clicked();
  void on_stopButton_clicked();

 private:
  Ui::SoundEditor* ui;
  QMediaPlayer* mediaPlayer;
  QMediaPlaylist* playlist;  //it's only one song but Qt puts looping stuff here
  bool userPaused;
  ProtoModelPtr soundModel;
};

#endif  // SOUNDEDITOR_H
