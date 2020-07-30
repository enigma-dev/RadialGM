#include "ShaderEditor.h"
#include "ui_CodeEditor.h"

#include "Shader.pb.h"

#include <QComboBox>
#include <QLayout>
#include <QStackedWidget>

using namespace buffers::resources;

ShaderEditor::ShaderEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), _codeEditor(new CodeEditor(this)) {
  this->setWindowIcon(QIcon(":/resources/shader.png"));
  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(_codeEditor);
  layout->setMargin(0);
  setLayout(layout);
  resize(_codeEditor->geometry().width(), _codeEditor->geometry().height());

  Ui::CodeEditor* ui = _codeEditor->_ui;
  connect(ui->actionSave, &QAction::triggered, this, &BaseEditor::OnSave);

  QLabel* shaderLabel = new QLabel(tr("Shader Type: "), ui->mainToolBar);
  QComboBox* shaderType = new QComboBox(ui->mainToolBar);
  shaderType->addItems({tr("Vertex"), tr("Fragment")});

  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addWidget(shaderLabel);
  ui->mainToolBar->addWidget(shaderType);

  CodeWidget* vertexWidget = _codeEditor->AddCodeWidget();
  CodeWidget* fragWidget = _codeEditor->AddCodeWidget();

  _resMapper->addMapping(fragWidget, Shader::kFragmentCodeFieldNumber);
  _resMapper->addMapping(vertexWidget, Shader::kVertexCodeFieldNumber);
  _resMapper->toFirst();

  connect(shaderType, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index) { ui->stackedWidget->setCurrentIndex(index); });

  ui->stackedWidget->setCurrentIndex(0);
  _codeEditor->updateCursorPositionLabel();
  _codeEditor->updateLineCountLabel();
}
