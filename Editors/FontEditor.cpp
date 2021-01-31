#include "FontEditor.h"
#include "Models/MessageModel.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(MessageModel *model, QWidget *parent) : BaseEditor(model, parent), _ui(new Ui::FontEditor) {
  _ui->setupUi(this);
  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _font = _ui->fontComboBox->currentFont();

  _ui->fontPreviewText->setFont(_font);

  _ui->fontPreviewText->setText(
      "abcdefghijklmnopqrstuvwxyz\n\
ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\
0123456789\n\
~!@#$%^&*()_+{}|:\"<>?`-=[];\',./\n\
The quick brown fox jumps over the lazy dog.");

  _nodeMapper->addMapping(_ui->nameEdit, TreeNode::kNameFieldNumber);
  _resMapper->addMapping(_ui->fontComboBox, Font::kFontNameFieldNumber);
  _resMapper->addMapping(_ui->sizeSpinBox, Font::kSizeFieldNumber);
  _resMapper->addMapping(_ui->boldCheckBox, Font::kBoldFieldNumber);
  _resMapper->addMapping(_ui->italicCheckBox, Font::kItalicFieldNumber);

  RebindSubModels();
}

FontEditor::~FontEditor() { delete _ui; }

void FontEditor::RebindSubModels() {
  MessageModel *fontModel = _model->GetSubModel<MessageModel *>(TreeNode::kFontFieldNumber);
  _rangesModel = fontModel->GetSubModel<RepeatedMessageModel *>(Font::kRangesFieldNumber);
  _ui->rangeTableView->setModel(_rangesModel);
  _ui->rangeTableView->hideColumn(0);

  _ui->deleteRangeButton->setDisabled(true);

  connect(_rangesModel, &RepeatedMessageModel::DataChanged, this, &FontEditor::ValidateRangeChange);
  connect(_ui->rangeTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &FontEditor::RangeSelectionChanged);

  if (fontModel->Data(FieldPath::Of<Font>(Font::kSizeFieldNumber)).toInt() <= 0) {
    _font.setPointSize(12);
    fontModel->SetData(FieldPath::Of<Font>(Font::kSizeFieldNumber), 12);
  }

  if (!fontModel->Data(FieldPath::Of<Font>(Font::kFontNameFieldNumber)).isValid()) {
    QFontDatabase db;
    fontModel->SetData(FieldPath::Of<Font>(Font::kFontNameFieldNumber), db.families().first());
  }

  BaseEditor::RebindSubModels();
}

void FontEditor::ValidateRangeChange(const QModelIndex &topLeft, const QModelIndex & /*bottomRight*/,
                                     const QVariant &oldValue) {
  int min =
      _rangesModel
          ->Data(FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMinFieldNumber, topLeft.row())))
          .toInt();
  int max =
      _rangesModel
          ->Data(FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMaxFieldNumber, topLeft.row())))
          .toInt();
  if (min > max) {
    _rangesModel->setData(topLeft, oldValue);
    return;
  } else if (min < 0 || min > 255) {
    _rangesModel->SetData(
        FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMinFieldNumber, topLeft.row())),
        std::min(std::max(0, min), 255));
  } else if (max < 0 || max > 255) {
    _rangesModel->SetData(
        FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMaxFieldNumber, topLeft.row())),
        std::min(std::max(0, max), 255));
  }

  QModelIndexList selection = _ui->rangeTableView->selectionModel()->selectedRows();
  if (!selection.empty()) UpdateRangeText(min, max);
}

void FontEditor::RangeSelectionChanged(const QItemSelection &selected, const QItemSelection & /*deselected*/) {
  _ui->deleteRangeButton->setDisabled(selected.empty());
  if (!selected.empty()) {
    auto index = selected.indexes().first();
    int min =
        _rangesModel
            ->Data(FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMinFieldNumber, index.row())))
            .toInt();
    int max =
        _rangesModel
            ->Data(FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMaxFieldNumber, index.row())))
            .toInt();
    UpdateRangeText(min, max);
  }
}

void FontEditor::on_sizeSpinBox_valueChanged(int arg1) {
  _font.setPointSize(arg1);
  _ui->fontPreviewText->setFont(_font);
  _ui->rangePreviewText->setFont(_font);
}

void FontEditor::on_fontComboBox_currentIndexChanged(const QString &arg1) {
  _font.setFamily(arg1);
  _ui->fontPreviewText->setFont(_font);
  _ui->rangePreviewText->setFont(_font);
}

void FontEditor::on_boldCheckBox_toggled(bool checked) {
  _font.setBold(checked);
  _ui->fontPreviewText->setFont(_font);
  _ui->rangePreviewText->setFont(_font);
}

void FontEditor::on_italicCheckBox_clicked(bool checked) {
  _font.setItalic(checked);
  _ui->fontPreviewText->setFont(_font);
  _ui->rangePreviewText->setFont(_font);
}

void FontEditor::on_addRangeButtom_pressed() {
  _rangesModel->insertRow(_rangesModel->rowCount());
  _rangesModel->SetData(
      FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMaxFieldNumber, _rangesModel->rowCount() - 1)),
      _ui->rangeEndBox->value());
  _rangesModel->SetData(
      FieldPath::Of<Font::Range>(FieldPath::RepeatedOffset(Font::Range::kMinFieldNumber, _rangesModel->rowCount() - 1)),
      _ui->rangeBeginBox->value());
  _ui->rangeTableView->selectionModel()->setCurrentIndex(_rangesModel->index(_rangesModel->rowCount() - 1, 0),
                                                         QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}

void FontEditor::on_rangeBeginBox_valueChanged(int arg1) {
  _ui->addRangeButtom->setDisabled((arg1 > _ui->rangeEndBox->value()));
}

void FontEditor::on_rangeEndBox_valueChanged(int arg1) {
  _ui->addRangeButtom->setDisabled((arg1 < _ui->rangeBeginBox->value()));
}

void FontEditor::UpdateRangeText(int min, int max) {
  QString str;
  for (int i = min; i < max; ++i) str += QChar(i);
  _ui->rangePreviewText->setText(str);
}

void FontEditor::on_deleteRangeButton_pressed() {
  {  // so our remover gets deleted
    RepeatedMessageModel::RowRemovalOperation remover(_rangesModel);
    remover.RemoveRow(_ui->rangeTableView->selectionModel()->selectedRows().first().row());
  }

  if (_rangesModel->rowCount() == 0) {
    _ui->deleteRangeButton->setDisabled(true);
    _ui->rangePreviewText->clear();
  } else
    _ui->rangeTableView->selectionModel()->setCurrentIndex(_rangesModel->index(_rangesModel->rowCount() - 1, 0),
                                                           QItemSelectionModel::QItemSelectionModel::ClearAndSelect);
}
