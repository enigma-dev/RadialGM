#ifndef UTILITY_H
#define UTILITY_H

#include <QFileDialog>

#include <cmath>

class ImageDialog : public QFileDialog {
 public:
  ImageDialog(QWidget* parent, QString xmlExtension, bool writer = false);
};

template <typename T>
T RoundNearest(int value, int multiple) {
  //if (compareDouble(multiple, 0)) return value;
  return static_cast<int>(std::round(static_cast<double>(value) / static_cast<double>(multiple)) *
                          static_cast<double>(multiple));
}

QPixmap CreateTransparentImage(const QPixmap& image);

#endif  // UTILITY_H
