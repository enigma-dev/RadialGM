#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QFileDialog>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QTextStream>
#include <QWidget>

class CodeWidget : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString code READ code WRITE setCode USER true)

 public:
  explicit CodeWidget(QWidget* parent);
  ~CodeWidget();

  QString code() const;
  void setCode(QString);
  int lineCount();

 public slots:
  void newSource() {
    QMessageBox::StandardButton reply;
    reply =
        QMessageBox::question(this, tr("New Source"), tr("Are you sure you want to clear the source and start over?"),
                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      this->setCode("");
    }
  }

  void loadSource() {
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

  void saveSource() {
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

  void printSource();

  void gotoLineDialog() {
    bool ok;
    int lineNumber = QInputDialog::getInt(this, tr("Go to Line"), tr("Line:"), 1, 1, lineCount(), 1, &ok);
    if (!ok) return;
    gotoLine(lineNumber);
  }

  void print(QPrinter* printer);
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void gotoLine(int line);

 protected:
  QFont font;
  QWidget* textWidget = nullptr;

 private:
  QStringList fileFilters() {
    return (QStringList() << "text/plain"
                          << "application/octet-stream");
  }
};

#endif  // CODEWIDGET_H
