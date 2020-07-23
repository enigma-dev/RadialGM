#ifndef PREFERENCESKEYS_H
#define PREFERENCESKEYS_H

#include <QString>

// these are settings key helpers to prevent typos and promote
// consistency, or at least turn such mistakes into compile-time
// errors and not difficult-to-diagnose issues at runtime
// NOTE: some QSettings implementations are case sensitive
// (e.g, macOS) while others are case insensitive (e.g, Windows)
inline QString preferencesKey() { return QStringLiteral("Preferences"); }

inline QString generalKey() { return QStringLiteral("General"); }
inline QString documentationURLKey() { return QStringLiteral("documentationURL"); }
inline QString websiteURLKey() { return QStringLiteral("websiteURL"); }
inline QString communityURLKey() { return QStringLiteral("communityURL"); }
inline QString submitIssueURLKey() { return QStringLiteral("submitIssueURL"); }

inline QString appearanceKey() { return QStringLiteral("Appearance"); }
inline QString styleNameKey() { return QStringLiteral("styleName"); }

inline QString keybindingKey() { return QStringLiteral("Keybinding"); }

#include <QSettings>

// settings value helpers to prevent duplication of the defaults

inline QString documentationURL() {
  QSettings settings;
  QString path = preferencesKey() + "/" + generalKey() + "/" + documentationURLKey();
  return settings.value(path, "https://enigma-dev.org/docs/Wiki/Main_Page").toString();
}

inline QString websiteURL() {
  QSettings settings;
  QString path = preferencesKey() + "/" + generalKey() + "/" + websiteURLKey();
  return settings.value(path, "https://enigma-dev.org").toString();
}

inline QString communityURL() {
  QSettings settings;
  QString path = preferencesKey() + "/" + generalKey() + "/" + communityURLKey();
  return settings.value(path, "https://enigma-dev.org/forums/").toString();
}

inline QString submitIssueURL() {
  QSettings settings;
  QString path = preferencesKey() + "/" + generalKey() + "/" + submitIssueURLKey();
  return settings.value(path, "https://github.com/enigma-dev/RadialGM/issues").toString();
}

#endif  // PREFERENCESKEYS_H
