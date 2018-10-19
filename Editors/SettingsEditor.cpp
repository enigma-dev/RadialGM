#include "SettingsEditor.h"
#include "MainWindow.h"
#include "ui_SettingsEditor.h"

#include "codegen/Settings.pb.h"

#include <QDebug>
#include <QPushButton>

using namespace buffers::resources;

Q_DECLARE_METATYPE(buffers::SystemInfo);

static std::string get_combo_system_id(const QComboBox* combo) {
  auto data = combo->currentData();
  auto subsystem = data.value<buffers::SystemInfo>();
  return subsystem.id();
}

SettingsEditor::SettingsEditor(ProtoModel* model, QWidget* parent)
    : BaseEditor(model, parent), ui(new Ui::SettingsEditor) {
  ui->setupUi(this);

  QPushButton* saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
  saveButton->setIcon(QIcon(":/actions/accept.png"));
  connect(saveButton, &QPushButton::clicked, [=]() {
    qDebug() << "hey";
    Settings settings;
    auto* api = settings.mutable_api();
    api->set_target_audio(get_combo_system_id(ui->audioCombo));
    api->set_target_collision(get_combo_system_id(ui->collisionCombo));
    api->set_target_compiler(get_combo_system_id(ui->compilerCombo));
    api->set_target_graphics(get_combo_system_id(ui->graphicsCombo));
    api->set_target_network(get_combo_system_id(ui->networkCombo));
    api->set_target_platform(get_combo_system_id(ui->platformCombo));
    api->set_target_widgets(get_combo_system_id(ui->widgetsCombo));
    api->add_extensions("Paths");

    qDebug() << "hi" << ui->audioCombo->currentData().toString();

    emit MainWindow::setCurrentConfig(settings);
  });
  QPushButton* discardButton = ui->buttonBox->button(QDialogButtonBox::Discard);
  discardButton->setIcon(QIcon(":/actions/cancel.png"));

  pageMap = {{"api", ui->apiPage},
             {"extensions", ui->extensionsPage},
             {"compiler", ui->compilerPage},
             {"graphics", ui->graphicsPage}};

  const QMap<QString, QWidget*> systemUIMap = {
      {QString("Audio"), ui->audioCombo},         {QString("Platform"), ui->platformCombo},
      {QString("Graphics"), ui->graphicsCombo},   {QString("Widget"), ui->widgetsCombo},
      {QString("Collision"), ui->collisionCombo}, {QString("Compilers"), ui->compilerCombo},
      {QString("Network"), ui->networkCombo},     {QString("Extensions"), ui->extensionsList},
  };
  foreach (auto system, MainWindow::systemCache) {
    const QString systemName = QString::fromStdString(system.name());
    auto it = systemUIMap.find(systemName);
    if (it == systemUIMap.end()) continue;
    auto widget = it.value();
    const QString className = widget->metaObject()->className();
    QListWidget* listWidget = nullptr;
    QComboBox* combo = nullptr;
    if (className == "QListWidget") {
      listWidget = static_cast<QListWidget*>(widget);
    } else if (className == "QComboBox") {
      combo = static_cast<QComboBox*>(widget);
      connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]() {
        auto data = combo->currentData();
        auto subsystem = data.value<buffers::SystemInfo>();
        const QString subsystemDesc = QString::fromStdString(subsystem.description());
        const QString subsystemAuthor = QString::fromStdString(subsystem.author());
        ui->authorName->setText(subsystemAuthor);
        ui->systemDesc->setPlainText(subsystemDesc);
      });
    }
    foreach (auto subsystem, system.subsystems()) {
      const QString subsystemName = QString::fromStdString(subsystem.name());
      const QString subsystemId = QString::fromStdString(subsystem.id());
      const QString subsystemDesc = QString::fromStdString(subsystem.description());
      if (combo) {
        QVariant data;
        data.setValue(subsystem);
        combo->addItem(subsystemName, data);
      } else if (listWidget) {
        auto item = new QListWidgetItem(subsystemName, listWidget);
        item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setToolTip(subsystemDesc);
        item->setData(Qt::UserRole, subsystemId);
      }
    }
  }
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
