#include "ShaderEditor.h"
#include "ui_CodeEditor.h"

#include "Shader.pb.h"

#include <QComboBox>
#include <QStackedWidget>
#include <QLayout>

using namespace buffers::resources;

ShaderEditor::ShaderEditor(ProtoModelPtr model, QWidget *parent) :
    BaseEditor(model, parent), codeEditor(new CodeEditor(this)) {

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(codeEditor);
  layout->setMargin(0);
  setLayout(layout);
  resize(codeEditor->geometry().width(), codeEditor->geometry().height());

  Ui::CodeEditor* ui = codeEditor->ui;

  QLabel* shaderLabel = new QLabel(tr("Shader Type: "), ui->mainToolBar);
  QComboBox* shaderType = new QComboBox(ui->mainToolBar);
  shaderType->addItems({tr("Vertex"), tr("Fragment")});

  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addWidget(shaderLabel);
  ui->mainToolBar->addWidget(shaderType);

  CodeWidget* vertexWidget = codeEditor->AddCodeWidget();
  CodeWidget* fragWidget = codeEditor->AddCodeWidget();

  resMapper->addMapping(fragWidget, Shader::kFragmentCodeFieldNumber);
  resMapper->addMapping(vertexWidget, Shader::kVertexCodeFieldNumber);
  resMapper->toFirst();

  connect(shaderType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
    ui->stackedWidget->setCurrentIndex(index);
  });

  ui->stackedWidget->setCurrentIndex(0);
  codeEditor->updateCursorPositionLabel();
  codeEditor->updateLineCountLabel();
}

ShaderEditor::~ShaderEditor() {}
