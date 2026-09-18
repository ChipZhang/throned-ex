#include "include/sys/UrlScheme.hpp"

#include <QApplication>
#include <QDir>
#include <QSettings>

#include <shlobj.h>

// In QSettings NativeFormat the value name "Default" is a key's unnamed (Default) value, and '/' separates subkeys.

static const QString kClasses = "HKEY_CURRENT_USER\\Software\\Classes";
static const QString kProgId = "Throned.Config";
static const QString kLegacyProgId = "Throne.Config";
// The key is deliberately stable. The actual executable filename is allowed to
// change for portable builds (for example, Throned-1.3.7-test.exe), but Windows
// should still see one logical application in every "Open with" dialog.
static const QString kApplicationKey = "Throned.exe";
static const QString kLegacyApplicationKey = "Throne.exe";

static const QStringList kConfigExtensions = {".json", ".conf", ".yaml", ".yml"};
// Claimed before 1.3; never registered again, only taken back.
static const QStringList kRetiredExtensions = {".ini", ".txt"};

static QString openCommand() {
    return "\"" + QDir::toNativeSeparators(QApplication::applicationFilePath()) + "\" \"%1\"";
}

// None of these is keyed by install path, so two portable copies write the same three keys and the last launched one wins.
static QStringList commandKeys(Association a) {
    if (a == Association::Links) return {kClasses + "\\throne"};
    return {kClasses + "\\" + kProgId, kClasses + "\\Applications\\" + kApplicationKey};
}

static bool isLegacyApplicationKey(const QString &key) {
    return key.compare(kLegacyApplicationKey, Qt::CaseInsensitive) == 0 ||
           ((key.startsWith("Throned-", Qt::CaseInsensitive) || key.startsWith("Throne-", Qt::CaseInsensitive)) &&
            key.endsWith(".exe", Qt::CaseInsensitive));
}

static bool isOurLegacyApplication(const QString &key, QSettings &app) {
    const QString friendlyName = app.value("FriendlyAppName").toString();
    if (friendlyName.compare("Throned", Qt::CaseInsensitive) == 0) return true;
    return isLegacyApplicationKey(key) && friendlyName.compare("Throne", Qt::CaseInsensitive) == 0;
}

static void removeLegacyRegistrations() {
    // Before the application key was made stable, every renamed portable exe
    // created another Applications\\<exe> entry. Remove only entries written by
    // our previous registration code; unrelated applications are left intact.
    QSettings applications(kClasses + "\\Applications", QSettings::NativeFormat);
    for (const QString &key: applications.childGroups()) {
        if (key.compare(kApplicationKey, Qt::CaseInsensitive) == 0 || !isLegacyApplicationKey(key)) continue;

        QSettings app(kClasses + "\\Applications\\" + key, QSettings::NativeFormat);
        if (isOurLegacyApplication(key, app)) {
            app.remove("");
            app.sync();
        }
    }

    // Also remove the old product name left by builds from before the rename.
    QSettings legacyProgId(kClasses + "\\" + kLegacyProgId, QSettings::NativeFormat);
    legacyProgId.remove("");
    legacyProgId.sync();

    for (const QString &ext: kConfigExtensions + kRetiredExtensions) {
        QSettings assoc(kClasses + "\\" + ext + "\\OpenWithProgids", QSettings::NativeFormat);
        assoc.remove(kLegacyProgId);
        assoc.sync();
    }
}

// The installer records its folder under HKCU or HKLM, depending on the install mode; a zip copy has no entry pointing at itself.
bool UrlScheme_AutoRegisterByDefault() {
    const QString appDir = QDir(QApplication::applicationDirPath()).canonicalPath();
    for (const QString &root: {QStringLiteral("HKEY_CURRENT_USER"), QStringLiteral("HKEY_LOCAL_MACHINE")}) {
        const QString installPath = QSettings(root + "\\Software\\Throne", QSettings::NativeFormat).value("InstallPath").toString();
        if (!installPath.isEmpty() && QDir(installPath).canonicalPath().compare(appDir, Qt::CaseInsensitive) == 0) return true;
    }
    return false;
}

QString UrlScheme_DesiredState(Association a) {
    return (a == Association::Links ? "v2|" : "v1|") + openCommand();
}

bool UrlScheme_IsCurrent(Association a) {
    const QString command = openCommand();
    for (const QString &key: commandKeys(a)) {
        QSettings s(key, QSettings::NativeFormat);
        if (s.value("shell/open/command/Default").toString() != command) return false;
    }
    return true;
}

static void applyLinks(const QString &command) {
    QSettings scheme(kClasses + "\\throne", QSettings::NativeFormat);
    scheme.setValue("Default", "URL:Throned Protocol");
    scheme.setValue("URL Protocol", "");
    scheme.setValue("shell/open/command/Default", command);
}

static void applyConfigFiles(const QString &command) {
    const QString exe = QDir::toNativeSeparators(QApplication::applicationFilePath());
    QSettings progId(kClasses + "\\" + kProgId, QSettings::NativeFormat);
    progId.setValue("Default", "Throned profile");
    progId.setValue("DefaultIcon/Default", exe + ",0");
    progId.setValue("shell/open/command/Default", command);

    // OpenWithProgids is the additive half of an association: the extension's own default is left alone.
    for (const QString &ext: kConfigExtensions) {
        QSettings(kClasses + "\\" + ext + "\\OpenWithProgids", QSettings::NativeFormat).setValue(kProgId, "");
    }
    for (const QString &ext: kRetiredExtensions) {
        QSettings(kClasses + "\\" + ext + "\\OpenWithProgids", QSettings::NativeFormat).remove(kProgId);
    }

    // Applications\Throned.exe is what "Open with > Choose another app" reads
    // for a file whose extension has no explicit association. Keep this key
    // stable even when the portable executable itself has a test-specific name.
    QSettings app(kClasses + "\\Applications\\" + kApplicationKey, QSettings::NativeFormat);
    app.setValue("FriendlyAppName", "Throned");
    app.setValue("DefaultIcon/Default", exe + ",0");
    app.setValue("shell/open/command/Default", command);
    app.remove("SupportedTypes");
    for (const QString &ext: kConfigExtensions) {
        app.setValue("SupportedTypes/" + ext, "");
    }
}

void UrlScheme_Apply(Association a) {
    removeLegacyRegistrations();
    if (a == Association::Links)
        applyLinks(openCommand());
    else
        applyConfigFiles(openCommand());

    // QSettings flushes on destruction, so the writers must have returned before the shell is told to reload.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}

void UrlScheme_Remove(Association a) {
    // Removing from the parent key drops the whole subtree; QSettings::remove("") would only empty it and leave the node behind.
    QSettings classes(kClasses, QSettings::NativeFormat);
    if (a == Association::Links) {
        classes.remove("throne");
    } else {
        classes.remove(kProgId);
        classes.remove("Applications/" + kApplicationKey);
        // Only our own progid goes; the extension's default was never ours to touch.
        for (const QString &ext: kConfigExtensions + kRetiredExtensions) {
            QSettings(kClasses + "\\" + ext + "\\OpenWithProgids", QSettings::NativeFormat).remove(kProgId);
        }
    }
    classes.sync();

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
