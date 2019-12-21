#include "StackedCodeWidget.h"
#include "CodeWidget.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextStream>

StackedCodeWidget::StackedCodeWidget(QWidget* parent) :  QStackedWidget(parent) {}

void StackedCodeWidget::newSource() {
  static_cast<CodeWidget*>(currentWidget())->newSource();
}

void StackedCodeWidget::loadSource() {
  static_cast<CodeWidget*>(currentWidget())->loadSource();
}

void StackedCodeWidget::saveSource() {
  static_cast<CodeWidget*>(currentWidget())->saveSource();
}

void StackedCodeWidget::printSource() {
  static_cast<CodeWidget*>(currentWidget())->printSource();
}

void StackedCodeWidget::gotoLineDialog() {
  static_cast<CodeWidget*>(currentWidget())->gotoLineDialog();
}

void StackedCodeWidget::print(QPrinter* printer) {
  static_cast<CodeWidget*>(currentWidget())->print(printer);
}

void StackedCodeWidget::undo() {
  static_cast<CodeWidget*>(currentWidget())->undo();
}

void StackedCodeWidget::redo() {
  static_cast<CodeWidget*>(currentWidget())->redo();
}

void StackedCodeWidget::cut() {
  static_cast<CodeWidget*>(currentWidget())->cut();
}
void StackedCodeWidget::copy() {
  static_cast<CodeWidget*>(currentWidget())->copy();
}

void StackedCodeWidget::paste() {
  static_cast<CodeWidget*>(currentWidget())->paste();
}

void StackedCodeWidget::gotoLine(int line) {
  static_cast<CodeWidget*>(currentWidget())->gotoLine(line);
}
