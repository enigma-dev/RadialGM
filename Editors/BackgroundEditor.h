#ifndef BACKGROUNDEDITOR_H
#define BACKGROUNDEDITOR_H

#include "resources/Background.pb.h"

#include "BackgroundRenderer.h"

#include <QItemDelegate>
#include <QDebug>
#include <QObject>
#include <QWidget>

class DataPusher : public QObject
{
	Q_OBJECT

public:
	DataPusher(QWidget *editor, QItemDelegate *delegate): QObject(editor),
		editor(editor), delegate(delegate) {}
public slots:
	void widgetChanged() {
		delegate->commitData(editor);
		delegate->closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
	}
private:
	QWidget* editor;
	QItemDelegate* delegate;
};

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
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
	void on_actionSave_triggered();

private:
	Ui::BackgroundEditor *ui;
    BackgroundRenderer* renderer;
};

#endif // BACKGROUNDEDITOR_H
