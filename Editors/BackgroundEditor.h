#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "resources/Background.pb.h"

#include "BackgroundRenderer.h"

#include <QWidget>
#include <QPainter>

namespace Ui {
class BackgroundEditor;
}

class BackgroundEditor : public QWidget
{
	Q_OBJECT

public:
    explicit BackgroundEditor(QWidget *parent, buffers::resources::Background *bkg);
	~BackgroundEditor();

private slots:
    void on_model_modified(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
	void on_actionSave_triggered();

private:
	Ui::BackgroundEditor *ui;
    BackgroundRenderer* renderer;
};

#endif // BACKGROUNDEDITOR_H
