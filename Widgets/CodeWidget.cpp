#include "CodeWidget.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextStream>

void CodeWidget::newSource() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("New Source"), tr("Are you sure you want to clear the source and start over?"),
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    this->setCode("");
  }
}

void CodeWidget::loadSource() {
  QFileDialog fileDialog(this);
  fileDialog.setWindowModality(Qt::WindowModal);
  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setFileMode(QFileDialog::ExistingFile);
  fileDialog.setMimeTypeFilters(this->fileFilters());
  if (fileDialog.exec() != QDialog::Accepted) return;
  const QString fn = fileDialog.selectedFiles().first();
  if (fn.isEmpty()) return;
  QFile file(fn);

  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream in(&file);
  this->setCode(in.readAll());
}

void CodeWidget::saveSource() {
  QFileDialog fileDialog(this);
  fileDialog.setWindowModality(Qt::WindowModal);
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);
  fileDialog.setMimeTypeFilters(this->fileFilters());
  if (fileDialog.exec() != QDialog::Accepted) return;

  const QString fn = fileDialog.selectedFiles().first();
  if (fn.isEmpty()) return;
  QFile file(fn);

  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);
  out << this->code();
}

void CodeWidget::gotoLineDialog() {
  bool ok;
  int lineNumber = QInputDialog::getInt(this, tr("Go to Line"), tr("Line:"), 1, 1, lineCount(), 1, &ok);
  if (!ok) return;
  gotoLine(lineNumber);
}
