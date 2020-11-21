#ifndef EGMMANAGER_H
#define EGMMANAGER_H

#include "Models/ResourceChangesModel.h"
#include "Models/GitHistoryModel.h"

#include "event_reader/event_parser.h"
#include "egm.h"

#include <QString>
#include <QStringList>
#include <QObject>

#include <git2.h>

class EGMManager : public QObject {
  Q_OBJECT
public:
  EGMManager();
  ~EGMManager();
  buffers::Project* NewProject();
  buffers::Project* LoadProject(const QString& fPath);
  buffers::Game *GetGame();
  ResourceChangesModel& GetResourceChangesModel();
  GitHistoryModel& GetGitHistoryModel();
  void ResourceChanged(Resource &res, ResChange change, const QString &oldName);
  bool LoadEventData(const QString& fPath);
  EventData* GetEventData();
  bool InitRepo();
  git_commit* GitLookUp(const git_oid* id);
  bool CreateBranch(const QString& branchName);
  bool Checkout(const QString& ref);
  //bool CheckoutFile(const QString& ref, const QString& file);
  bool AddFile(const QString& file);
  bool RemoveFile(const QString& file);
  bool RemoveDir(const QString& dir);
  bool CommitChanges(const QString& message);
  /*bool AddRemote(const QString& url);
  bool ChangeRemote(unsigned index, const QString& url);
  bool RemoveRemote(unsigned index);
  const QStringList& GetRemotes() const;
  bool PushChanges();*/

signals:
  QStringList FilesEditedExternally();
  QStringList ConflictedFilesDetected();

public slots:
  void GitError();
  bool Save(const QString& fPath);

protected:
  std::unique_ptr<buffers::Project> _project;
  ResourceChangesModel _resChangeModel;
  GitHistoryModel _gitHistoryModel;
  QStringList _remoteURLs;
  std::unique_ptr<EventData> _event_data;
  QString _rootPath;
  git_repository* _repo;
  git_signature* _git_sig;
  git_index* _git_index;
  git_oid _git_tree_id, _git_commit_id;
  git_tree* _git_tree;
};

#endif // EGMMANAGER_H
