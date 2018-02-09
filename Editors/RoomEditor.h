#ifndef ROOMEDITOR_H
#define ROOMEDITOR_H

#include <QWidget>

namespace Ui {
class RoomEditor;
}

class RoomEditor : public QWidget
{
	Q_OBJECT

public:
	explicit RoomEditor(QWidget *parent = 0);
	~RoomEditor();

private:
	Ui::RoomEditor *ui;
};

#endif // ROOMEDITOR_H
