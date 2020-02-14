#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QFont>
#include <QPrinter>
#include <QWidget>

enum KeywordType { UNKNOWN = 0, FUNCTION = 1, GLOBAL = 2, TYPE_NAME = 3, MAX = 4 };

class CodeWidget : public QWidget {
  Q_OBJECT
  // this is a shim property over the real property
  // NOTE: don't forget the notify signal or the mapper won't work!
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged USER true)

 public:
  explicit CodeWidget(QWidget* parent = nullptr);
  ~CodeWidget();

  QString code() const;
  void setCode(QString);
  int lineCount();
  QPair<int, int> cursorPosition();

  static void prepareKeywordStore();
  static void addKeyword(const QString& keyword, KeywordType type);
  static void addCalltip(const QString& keyword, const QString& calltip, KeywordType type = KeywordType::FUNCTION);
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
  QFont _font;
  QWidget* _textWidget = nullptr;

 private:
  QStringList fileFilters() {
    return (QStringList() << "text/plain"
                          << "application/octet-stream");
  }
};

#endif  // CODEWIDGET_H
