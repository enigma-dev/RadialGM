#include "MainWindow.h"
#include "EGMManager.h"
#include "gmk.h"
#include "gmx.h"
#include "yyp.h"
#include "filesystem.h"

#include <QDebug>
#include <QErrorMessage>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

EGMManager::EGMManager() : _repo(nullptr) {
  LoadEventData(MainWindow::EnigmaRoot.absolutePath() + "/events.ey");
  qDebug() << "Intializing git library";
  git_libgit2_init();
  connect(&_gitHistoryModel, &GitHistoryModel::GitError, this, &EGMManager::GitError);
}

EGMManager::~EGMManager() {
  git_tree_free(_git_tree);
  git_signature_free(_git_sig);
  git_repository_free(_repo);
  git_libgit2_shutdown();
}

buffers::Project* EGMManager::NewProject() {
  _resChangeModel.ClearChanges();

  _project = std::make_unique<buffers::Project>();
  QTemporaryDir dir;
  dir.setAutoRemove(false);
  _rootPath = dir.path();
  InitRepo();
  return _project.get();
}

buffers::Project* EGMManager::LoadProject(const QString& fPath) {
  _resChangeModel.ClearChanges();

  QFileInfo fileInfo(fPath);
  const QString suffix = fileInfo.suffix();

  _project = egm::LoadProject(fPath.toStdString());

  return _project.get();
}

buffers::Game* EGMManager::GetGame() { return _project->mutable_game(); }

ResourceChangesModel& EGMManager::GetResourceChangesModel() {
  return _resChangeModel;
}

GitHistoryModel& EGMManager::GetGitHistoryModel() {
  return _gitHistoryModel;
}

void EGMManager::ResourceChanged(Resource& res, ResChange change, const QString& oldName) {
  _resChangeModel.ResourceChanged(res, change, oldName);

  QString dir =_rootPath;// + "get the res path somehow???";

  // Clear the existing folder I guess
  //if (FolderExists(dir.toStdString())) RemoveDir(dir);

  if (change != ResChange::Removed) {
    egm::WriteResource(res.buffer, dir.toStdString());
    //AddFile(res.name + ".ext"); ??
  }
}

bool EGMManager::LoadEventData(const QString& fPath) {
  QFile f(fPath);
  if (f.exists()) {
    _event_data = std::make_unique<EventData>(ParseEventFile(fPath.toStdString()));
  } else {
    qDebug() << "Error: Failed to load events file. Loading internal events.ey.";
    QFile internal_events(":/events.ey");
    internal_events.open(QIODevice::ReadOnly | QFile::Text);
    std::stringstream ss;
    ss << internal_events.readAll().toStdString();
    _event_data = std::make_unique<EventData>(ParseEventFile(ss));
  }

  egm::LibEGMInit(_event_data.get());

  return _event_data->events().size() > 0;
}

EventData* EGMManager::GetEventData() { return _event_data.get(); }

git_commit* EGMManager::GitLookUp(const git_oid* id) {
  git_commit* commit = nullptr;
  int error = git_commit_lookup(&commit, _repo, id);
  if (error != 0) GitError();
  return commit;
}

bool EGMManager::InitRepo() {
  qDebug() << "Intializing git repository at: " << _rootPath;
  git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
  opts.description = "ENIGMA Game";
  int error = git_repository_init_ext(&_repo, _rootPath.toStdString().c_str(), &opts);
  if (error != 0) {
    GitError();
    return false;
  }

  error = git_signature_default(&_git_sig, _repo);
  if (error != 0) {
    GitError();
    return false;
  }

  error = git_repository_index(&_git_index, _repo);
  if (error != 0) {
    GitError();
    return false;
  }
  error = git_index_write_tree(&_git_tree_id, _git_index);
  if (error != 0) {
    GitError();
    return false;
  }
  error = git_tree_lookup(&_git_tree, _repo, &_git_tree_id);
  if (error != 0) {
    GitError();
    return false;
  }

  error = git_commit_create_v(&_git_commit_id, _repo, "HEAD", _git_sig, _git_sig, NULL, "Initial commit", _git_tree, 0);
  if (error != 0) {
    GitError();
    return false;
  }

  _gitHistoryModel.LoadRepo(_repo);

  return CreateBranch("RadialGM") && Checkout("refs/heads/RadialGM");
}

bool EGMManager::CreateBranch(const QString& branchName) {
  qDebug() << "Creating git branch: " << branchName;
  git_commit* c = GitLookUp(&_git_commit_id);
  git_reference* ref = nullptr;
  int error = git_branch_create(&ref, _repo, branchName.toStdString().c_str(), c, 0);
  git_reference_free(ref);
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

bool EGMManager::Checkout(const QString& ref) {
  qDebug() << "Checking out git reference: " << ref;
  int error = git_repository_set_head(_repo, ref.toStdString().c_str());
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

/*bool EGMManager::CheckoutFile(const QString& ref, const QString& file) {
}*/

bool EGMManager::AddFile(const QString& file) {
  qDebug() << "Adding file to EGM: " << file;
  int error = git_index_add_bypath(_git_index, file.toStdString().c_str());
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

bool EGMManager::RemoveFile(const QString& file) {
  qDebug() << "Removing file from EGM: " << file;
  int error = git_index_remove_bypath(_git_index, file.toStdString().c_str());
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

bool EGMManager::RemoveDir(const QString& dir) {
  qDebug() << "Removing directory from EGM: " << dir;
  DeleteFolder(dir.toStdString());
  int error = git_index_remove_directory(_git_index, dir.toStdString().c_str(), 0);
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

bool EGMManager::CommitChanges(const QString& message) {
  qDebug() << "Commiting changes";
  int error = git_commit_create_v(&_git_commit_id, _repo, "HEAD", _git_sig, _git_sig, NULL,
                                  message.toStdString().c_str(), _git_tree, 0);
  if (error != 0) {
    GitError();
    return false;
  }
  return true;
}

/*bool EGMManager::AddRemote(const QString& url) {

}

bool EGMManager::ChangeRemote(unsigned index, const QString& url) {

}

bool EGMManager::RemoveRemote(unsigned index) {

}

const QStringList& EGMManager::GetRemotes() const {

}

bool EGMManager::PushChanges() {

}*/

bool EGMManager::Save(const QString& fPath) {
  return false;
}

void EGMManager::GitError() {
  const git_error* e = git_error_last();
  qDebug() << QString("Git Error: ") + e->message;
}
