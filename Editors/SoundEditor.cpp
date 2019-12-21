#include "SoundEditor.h"
#include "ui_SoundEditor.h"

#include "gmx.h"

#include "Components/ArtManager.h"
#include "Components/Utility.h"

#include <QDebug>
#include <QDesktopServices>
#include <QSlider>
#include <QTime>

SoundEditor::SoundEditor(ProtoModelPtr model, QWidget* parent)
    : BaseEditor(model, parent),
      ui(new Ui::SoundEditor),
      mediaPlayer(new QMediaPlayer(this)),
      playlist(new QMediaPlaylist(mediaPlayer)),
      userPaused(false) {
  ui->setupUi(this);

  nodeMapper->addMapping(ui->nameEdit, TreeNode::kNameFieldNumber);
  resMapper->addMapping(ui->volumeSpinBox, Sound::kVolumeFieldNumber);

  ui->volumeSlider->setValue(static_cast<int>(ui->volumeSpinBox->value() * 100));

  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
  mediaPlayer->setPlaylist(playlist);
  // Update the signals every 50ms instead of Qt's default 1000ms to keep slider up to date
  mediaPlayer->setNotifyInterval(50);

  connect(mediaPlayer, &QMediaPlayer::positionChanged, [=]() {
    if (mediaPlayer->duration() > 0) {
      int percent = static_cast<int>(100 * (static_cast<float>(mediaPlayer->position()) / mediaPlayer->duration()));
      ui->playbackSlider->setSliderPosition(percent);
    } else
      ui->playbackSlider->setSliderPosition(0);

    QTime timestamp(0, 0);
    ui->playbackPositionLabel->setText(timestamp.addMSecs(static_cast<int>(mediaPlayer->position())).toString());
  });

  connect(mediaPlayer, &QMediaPlayer::mediaChanged, [=]() {
    playlist->clear();
    playlist->addMedia(QUrl::fromLocalFile(soundModel->data(Sound::kDataFieldNumber).toString()));
  });

  connect(mediaPlayer, &QMediaPlayer::stateChanged, [=]() {
    if (mediaPlayer->state() == QMediaPlayer::PausedState || mediaPlayer->state() == QMediaPlayer::StoppedState)
      ui->playButton->setIcon(ArtManager::GetIcon(":/actions/play.png"));
    else
      ui->playButton->setIcon(ArtManager::GetIcon(":/actions/pause.png"));
  });

  mediaPlayer->setPlaylist(playlist);

  RebindSubModels();
}

SoundEditor::~SoundEditor() { delete ui; }

void SoundEditor::RebindSubModels() {
  playlist->clear();
  soundModel = _model->GetSubModel(TreeNode::kSoundFieldNumber);
  playlist->addMedia(QUrl::fromLocalFile(soundModel->data(Sound::kDataFieldNumber).toString()));
  BaseEditor::RebindSubModels();
}

void SoundEditor::on_playButton_clicked() {
  if (mediaPlayer->state() == QMediaPlayer::PausedState || mediaPlayer->state() == QMediaPlayer::StoppedState) {
    mediaPlayer->play();
    userPaused = false;
  } else {
    mediaPlayer->pause();
    userPaused = true;
  }
}

void SoundEditor::on_loopButton_clicked() {
  if (playlist->playbackMode() == QMediaPlaylist::CurrentItemOnce)
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
  else
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
}

void SoundEditor::on_playbackSlider_sliderPressed() {
  if (mediaPlayer->state() == QMediaPlayer::PlayingState) mediaPlayer->pause();
}

void SoundEditor::on_playbackSlider_sliderReleased() {
  mediaPlayer->setPosition(static_cast<int>((ui->playbackSlider->value() / 100.f) * mediaPlayer->duration()));
  if (mediaPlayer->state() == QMediaPlayer::PausedState && !userPaused) mediaPlayer->play();
}

void SoundEditor::on_volumeSlider_sliderMoved(int position) {
  ui->volumeSpinBox->setValue(position / 100.0);
  mediaPlayer->setVolume(position);
}

void SoundEditor::on_volumeSpinBox_valueChanged(double arg1) {
  ui->volumeSlider->setValue(static_cast<int>(arg1 * 100));
  mediaPlayer->setVolume(static_cast<int>(arg1 * 100));
}

void SoundEditor::on_saveAsButton_clicked() {
  //  TODO: implement this when egm is done
}

void SoundEditor::on_loadButton_clicked() {
  FileDialog* dialog = new FileDialog(this, FileDialog_t::SoundLoad, false);

  if (dialog->exec() && dialog->selectedFiles().size() > 0) {
    QString fName = dialog->selectedFiles()[0];
    if (fName.endsWith("Sound.gmx")) {
      Sound* snd = gmx::LoadSound(fName.toStdString());
      if (snd != nullptr) {
        // QString lastData = GetModelData(Sound::kDataFieldNumber).toString();
        ReplaceBuffer(snd);
        // QString newData = GetModelData(Sound::kDataFieldNumber).toString();
        // TODO: Copy data into our egm and reset the path
        // SetModelData(Sound::kDataFieldNumber, lastData);
      } else
        qDebug() << "Failed to load gmx sound";
    } else {
      // TODO: Copy data into our egm
      SetModelData(Sound::kDataFieldNumber, fName);
      emit mediaPlayer->mediaChanged(mediaPlayer->media());
    }
  }
}

void SoundEditor::on_editButton_clicked() {
  QString fName = GetModelData(Sound::kDataFieldNumber).toString();
  QDesktopServices::openUrl(QUrl::fromLocalFile(fName));
  // TODO: file watcher reload
  // TODO: editor settings
}

void SoundEditor::on_stopButton_clicked() { mediaPlayer->stop(); }
