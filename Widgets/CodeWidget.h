#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QFont>
#include <QPrinter>
#include <QWidget>

class CodeWidget : public QWidget {
  Q_OBJECT
  // this is a shim property over the real property
  // NOTE: don't forget the notify signal or the mapper won't work!
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged USER true)

 public:
  explicit CodeWidget(QWidget* parent);
  ~CodeWidget();

  QString code() const;
  void setCode(QString);
  int lineCount();
  QPair<int, int> cursorPosition();

  static void prepareKeywordStore();
  static void addKeyword(const QString& keyword);
  static void finalizeKeywords();

 public slots:
  void newSource();
  void loadSource();
  void saveSource();
  void printSource();
  void gotoLineDialog();

  void print(QPrinter* printer);
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void gotoLine(int line);

 signals:
  void cursorPositionChanged(int line, int index);
  void lineCountChanged(int lines);
  void codeChanged();

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
