#ifndef STACKEDCODEWIDGET_H
#define STACKEDCODEWIDGET_H

#include <QStackedWidget>
#include <QPrinter>

class StackedCodeWidget : public QStackedWidget {
  Q_OBJECT
public:
  StackedCodeWidget(QWidget* parent);

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
};

#endif // STACKEDCODEWIDGET_H
