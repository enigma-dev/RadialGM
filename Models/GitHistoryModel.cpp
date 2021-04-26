#include "GitHistoryModel.h"
#include "Components/GitTreeItem.h"

#include <QDebug>

GitHistoryModel::GitHistoryModel(QObject* parent) : QAbstractTableModel(parent) {}

GitHistoryModel::~GitHistoryModel() {
  for (git_commit* c : _commits)
    git_commit_free(c);
}

void GitHistoryModel::LoadRepo(git_repository *repo) {
  beginResetModel();

  for (git_commit* c : _commits)
    git_commit_free(c);

  _commits.clear();

  git_revwalk *walker;

  int error = git_revwalk_new(&walker, repo);
  if (error != 0) emit GitError();

  error = git_revwalk_push_head(walker);
  if (error != 0) emit GitError();

  error = git_revwalk_push_ref(walker, "HEAD");
  if (error != 0) emit GitError();

  git_revwalk_sorting(walker, GIT_SORT_NONE);

  git_oid oid;
  while (!git_revwalk_next(&oid, walker)) {
    git_commit* commit = nullptr;
    git_commit_lookup(&commit, repo, &oid);
    _commits.append(commit);
  }

  git_revwalk_free(walker);

  endResetModel();
}

int GitHistoryModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return _commits.count();
}

int GitHistoryModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return 5;
}

QVariant GitHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || orientation != Qt::Orientation::Horizontal) return QVariant();

  switch(section) {
    case 0: return tr("Tree");
    case 1: return tr("ID");
    case 2: return tr("Description");
    case 3: return tr("Author");
    case 4: return tr("Email");
  }

  return QVariant();
}

QVariant GitHistoryModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  git_commit* commit = _commits[index.row()];

  switch(index.column()) {
    case 0: return QVariant::fromValue(GitTreeItem());
    case 1: return git_oid_tostr_s(git_commit_id(commit));
    case 2: return git_commit_summary(commit);
    case 3: return git_commit_author(commit)->name;
    case 4: return git_commit_author(commit)->email;
  }

  return QVariant();
}
