#include "SettingsEditor.h"
#include "Models/SystemsModel.h"
#include "MainWindow.h"
#include "ui_SettingsEditor.h"

#include "Settings.pb.h"

#include <QPushButton>

using namespace buffers::resources;

static QMap<QString, SystemsModel*> systemsModels;

static void initModelCache() {
  if (systemsModels.empty()) {
    for (const auto& sys : qAsConst(MainWindow::systemCache)) {
      systemsModels.insert(QString::fromStdString(sys.name()), new SystemsModel(sys, MainWindow::instance));
    }
  }
}

SettingsEditor::SettingsEditor(MessageModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::SettingsEditor) {
  ui->setupUi(this);

  pageMap = {{"api", ui->apiPage},           {"extensions", ui->extensionsPage}, {"compiler", ui->compilerPage},
             {"controls", ui->controlsPage}, {"graphics", ui->graphicsPage},     {"project info", ui->projectInfoPage},
             {"version", ui->versionPage}};

  initModelCache();

  ui->audioCombo->setModel(systemsModels["Audio"]);
  ui->platformCombo->setModel(systemsModels["Platform"]);
  ui->compilerCombo->setModel(systemsModels["Compilers"]);
  ui->graphicsCombo->setModel(systemsModels["Graphics"]);
  ui->widgetsCombo->setModel(systemsModels["Widget"]);
  ui->collisionCombo->setModel(systemsModels["Collision"]);
  ui->networkCombo->setModel(systemsModels["Network"]);

  SettingsEditor::RebindSubModels();
}

void SettingsEditor::RebindSubModels() {
  MessageModel* sm = _model->GetSubModel<MessageModel*>(TreeNode::kSettingsFieldNumber);

  ModelMapper* apiMapper = new ModelMapper(sm->GetSubModel<MessageModel*>(Settings::kApiFieldNumber));
  apiMapper->addMapping(ui->audioCombo, API::kTargetAudioFieldNumber);
  apiMapper->addMapping(ui->platformCombo, API::kTargetPlatformFieldNumber);
  apiMapper->addMapping(ui->compilerCombo, API::kTargetCompilerFieldNumber);
  apiMapper->addMapping(ui->graphicsCombo, API::kTargetGraphicsFieldNumber);
  apiMapper->addMapping(ui->widgetsCombo, API::kTargetWidgetsFieldNumber);
  apiMapper->addMapping(ui->networkCombo, API::kTargetNetworkFieldNumber);

  BaseEditor::RebindSubModels();
}

SettingsEditor::~SettingsEditor() { delete ui; }

void SettingsEditor::on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
  QWidget* widget = ui->emptyPage;
  auto item = current;
  if (item) {
    const QString path = item->text().toLower();
    auto it = pageMap.find(path);
    if (it != pageMap.end()) {
      widget = it.value();
    }
  }

  ui->stackedWidget->setCurrentWidget(widget);
}
