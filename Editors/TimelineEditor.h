#ifndef TIMELINEEDITOR_H
#define TIMELINEEDITOR_H

#include <QWidget>

namespace Ui {
class TimelineEditor;
}

class TimelineEditor : public QWidget
{
	Q_OBJECT

public:
	explicit TimelineEditor(QWidget *parent = 0);
	~TimelineEditor();

private:
	Ui::TimelineEditor *ui;
};

#endif // TIMELINEEDITOR_H
