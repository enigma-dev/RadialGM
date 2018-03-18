#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "resources/Background.pb.h"

#include <QWidget>

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
	void on_actionSave_triggered();

private:
	Ui::BackgroundEditor *ui;
};

#endif // BACKGROUNDEDITOR_H
