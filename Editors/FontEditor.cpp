#include "FontEditor.h"
#include "Models/ProtoModel.h"

#include "ui_FontEditor.h"

FontEditor::FontEditor(EditorModel *model, QWidget *parent) :
    BaseEditor(model, parent), _ui(new Ui::FontEditor) {
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

}

void FontEditor::ValidateRangeChange(const QModelIndex &topLeft, const QModelIndex & /*bottomRight*/,
                                     const QVariant &oldValue) {

}

void FontEditor::RangeSelectionChanged(const QItemSelection &selected, const QItemSelection & /*deselected*/) {

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
}
