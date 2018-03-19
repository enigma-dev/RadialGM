#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "resources/Background.pb.h"

#include <QWidget>
#include <QGraphicsScene>

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
    void on_model_modified();
	void on_actionSave_triggered();

private:
    void draw();
	Ui::BackgroundEditor *ui;
    QGraphicsScene* scene;
    QPixmap image;
};

#endif // BACKGROUNDEDITOR_H
