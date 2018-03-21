#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include <QWidget>

namespace Ui {
class SpriteEditor;
}

class SpriteEditor : public QWidget
{
	Q_OBJECT

public:
	explicit SpriteEditor(QWidget *parent = 0);
	~SpriteEditor();

private:
	Ui::SpriteEditor *ui;
};

#endif // SPRITEEDITOR_H
