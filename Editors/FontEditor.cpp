#include "FontEditor.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(ProtoModelPtr model, QWidget *parent) : BaseEditor(model, parent), ui(new Ui::FontEditor) {
  ui->setupUi(this);
  connect(ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  font = ui->fontComboBox->currentFont();

  ui->fontPreviewText->setFont(font);

  ui->fontPreviewText->setText(
      "abcdefghijklmnopqrstuvwxyz\n\
ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\
0123456789\n\
~!@#$%^&*()_+{}|:\"<>?`-=[];\',./\n\
The quick brown fox jumps over the lazy dog.");

  nodeMapper->addMapping(ui->nameEdit, TreeNode::kNameFieldNumber);
  resMapper->addMapping(ui->fontComboBox, Font::kFontNameFieldNumber);
  resMapper->addMapping(ui->sizeSpinBox, Font::kSizeFieldNumber);
  resMapper->addMapping(ui->boldCheckBox, Font::kBoldFieldNumber);
  resMapper->addMapping(ui->italicCheckBox, Font::kItalicFieldNumber);

  RebindSubModels();
}

FontEditor::~FontEditor() { delete ui; }

void FontEditor::RebindSubModels() {
  ProtoModelPtr fontModel = _model->GetSubModel(TreeNode::kFontFieldNumber);
  rangesModel = fontModel->GetRepeatedSubModel(Font::kRangesFieldNumber);
  ui->rangeTableView->setModel(rangesModel);
  ui->rangeTableView->hideColumn(0);

  ui->deleteRangeButton->setDisabled(true);

  connect(rangesModel, &RepeatedProtoModel::dataChanged, this, &FontEditor::ValidateRangeChange);
  connect(ui->rangeTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &FontEditor::RangeSelectionChanged);

  if (fontModel->data(Font::kSizeFieldNumber).toInt() <= 0) {
    font.setPointSize(12);
    fontModel->setData(Font::kSizeFieldNumber, 0, 12);
  }

  if (!fontModel->data(Font::kFontNameFieldNumber).isValid()) {
    QFontDatabase db;
    fontModel->setData(Font::kFontNameFieldNumber, 0, db.families().first());
  }

  BaseEditor::RebindSubModels();
}

void FontEditor::ValidateRangeChange(const QModelIndex &topLeft, const QModelIndex & /*bottomRight*/,
                                     const QVariant &oldValue) {
  int min = rangesModel->data(topLeft.row(), Font::Range::kMinFieldNumber).toInt();
  int max = rangesModel->data(topLeft.row(), Font::Range::kMaxFieldNumber).toInt();
  if (min > max) {
    rangesModel->setData(topLeft.row(), topLeft.column(), oldValue);
    return;
  } else if (min < 0 || min > 255) {
    rangesModel->setData(topLeft.row(), Font::Range::kMinFieldNumber, std::min(std::max(0, min), 255));
  } else if (max < 0 || max > 255) {
    rangesModel->setData(topLeft.row(), Font::Range::kMaxFieldNumber, std::min(std::max(0, max), 255));
  }

  QModelIndexList selection = ui->rangeTableView->selectionModel()->selectedRows();
  if (!selection.empty()) UpdateRangeText(min, max);
}

void FontEditor::RangeSelectionChanged(const QItemSelection &selected, const QItemSelection & /*deselected*/) {
  ui->deleteRangeButton->setDisabled(selected.empty());
  if (!selected.empty()) {
    auto index = selected.indexes().first();
    int min = rangesModel->data(index.row(), Font::Range::kMinFieldNumber).toInt();
    int max = rangesModel->data(index.row(), Font::Range::kMaxFieldNumber).toInt();
    UpdateRangeText(min, max);
  }
}

void FontEditor::on_sizeSpinBox_valueChanged(int arg1) {
  font.setPointSize(arg1);
  ui->fontPreviewText->setFont(font);
  ui->rangePreviewText->setFont(font);
}

void FontEditor::on_fontComboBox_currentIndexChanged(const QString &arg1) {
  font.setFamily(arg1);
  ui->fontPreviewText->setFont(font);
  ui->rangePreviewText->setFont(font);
}

void FontEditor::on_boldCheckBox_toggled(bool checked) {
  font.setBold(checked);
  ui->fontPreviewText->setFont(font);
  ui->rangePreviewText->setFont(font);
}

void FontEditor::on_italicCheckBox_clicked(bool checked) {
  font.setItalic(checked);
  ui->fontPreviewText->setFont(font);
  ui->rangePreviewText->setFont(font);
}

void FontEditor::on_addRangeButtom_pressed() {
  rangesModel->insertRow(rangesModel->rowCount());
  rangesModel->setData(rangesModel->rowCount() - 1, Font::Range::kMaxFieldNumber, ui->rangeEndBox->value());
  rangesModel->setData(rangesModel->rowCount() - 1, Font::Range::kMinFieldNumber, ui->rangeBeginBox->value());
  ui->rangeTableView->selectionModel()->setCurrentIndex(rangesModel->index(rangesModel->rowCount() - 1, 0),
                                                        QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}

void FontEditor::on_rangeBeginBox_valueChanged(int arg1) {
  ui->addRangeButtom->setDisabled((arg1 > ui->rangeEndBox->value()));
}

void FontEditor::on_rangeEndBox_valueChanged(int arg1) {
  ui->addRangeButtom->setDisabled((arg1 < ui->rangeBeginBox->value()));
}

void FontEditor::UpdateRangeText(int min, int max) {
  QString str;
  for (int i = min; i < max; ++i) str += QChar(i);
  ui->rangePreviewText->setText(str);
}

void FontEditor::on_deleteRangeButton_pressed() {
  {  // so our remover gets deleted
    RepeatedProtoModel::RowRemovalOperation remover(rangesModel);
    remover.RemoveRow(ui->rangeTableView->selectionModel()->selectedRows().first().row());
  }

  if (rangesModel->rowCount() == 0) {
    ui->deleteRangeButton->setDisabled(true);
    ui->rangePreviewText->clear();
  } else
    ui->rangeTableView->selectionModel()->setCurrentIndex(rangesModel->index(rangesModel->rowCount() - 1, 0),
                                                          QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}
