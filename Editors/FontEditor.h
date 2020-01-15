#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include "BaseEditor.h"
#include "Models/RepeatedMessageModel.h"

#include <QFont>
#include <QItemSelection>

namespace Ui {
class FontEditor;
}

class FontEditor : public BaseEditor {
  Q_OBJECT

 public:
  explicit FontEditor(ProtoModelPtr model, QWidget* parent);
  ~FontEditor() override;

 public slots:
  void RebindSubModels() override;

 private slots:
  void ValidateRangeChange(const QModelIndex& topLeft, const QModelIndex& /*bottomRight*/, const QVariant& oldValue);
  void RangeSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/);
  void on_sizeSpinBox_valueChanged(int arg1);
  void on_fontComboBox_currentIndexChanged(const QString& arg1);
  void on_boldCheckBox_toggled(bool checked);
  void on_italicCheckBox_clicked(bool checked);
  void on_addRangeButtom_pressed();
  void on_rangeBeginBox_valueChanged(int arg1);
  void on_rangeEndBox_valueChanged(int arg1);
  void on_deleteRangeButton_pressed();

 private:
  void UpdateRangeText(int min, int max);
  Ui::FontEditor* ui;
  QFont font;
  RepeatedMessageModel* rangesModel;
};

#endif  // FONTEDITOR_H
