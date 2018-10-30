#ifndef UTILITY_H
#define UTILITY_H

#endif  // UTILITY_H

#include <QFileDialog>

class ImageDialog : public QFileDialog {
 public:
  ImageDialog(QWidget* parent, QString xmlExtension, bool writer = false);
};
