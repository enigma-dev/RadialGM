#ifndef IMEDIATEDATAWIDGETWRAPPER_H
#define IMEDIATEDATAWIDGETWRAPPER_H

#include <QDataWidgetMapper>
#include <QList>

class ImmediateDataWidgetMapper : public QDataWidgetMapper {
  Q_OBJECT

 public:
  explicit ImmediateDataWidgetMapper(QObject *parent);

  void addMapping(QWidget *widget, int section, QByteArray propertyName = "");
  void removeMapping(QWidget *widget);
  void clearMapping();
  QModelIndex indexAt(int section);

 public slots:
  void setCurrentIndex(int index) override;  // toFirst(), toLast(), etc.

 private slots:
  void widgetChanged();

 private:
  QList<QWidget *> widgetList;
};

#endif  // IMEDIATEDATAWIDGETWRAPPER_H
