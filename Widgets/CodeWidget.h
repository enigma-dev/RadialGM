#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QFont>
#include <QWidget>

class CodeWidget : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString code READ code WRITE setCode USER true)

 public:
  explicit CodeWidget(QWidget* parent);
  ~CodeWidget();

  QString code() const;
  void setCode(QString);

 public slots:
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();

 protected:
  QFont font;
  QWidget* textWidget = nullptr;
};

#endif  // CODEWIDGET_H
