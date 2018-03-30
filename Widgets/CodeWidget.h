#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QFont>
#include <QWidget>

class CodeWidget : public QWidget {
 public:
  explicit CodeWidget(QWidget* parent);
  ~CodeWidget();

 protected:
  QFont font;
};

#endif  // CODEWIDGET_H
