#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include <QWidget>

namespace Ui {
class BackgroundEditor;
}

class BackgroundEditor : public QWidget
{
	Q_OBJECT

public:
	explicit BackgroundEditor(QWidget *parent = 0);
	~BackgroundEditor();

private:
	Ui::BackgroundEditor *ui;
};

#endif // BACKGROUNDEDITOR_H
