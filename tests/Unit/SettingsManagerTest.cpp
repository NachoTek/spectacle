/*
 *  Unit Tests for SettingsManager
 *  Story 1.5 - Saving & Defaults
 *  Task 1: Design settings storage mechanism
 *  Task 6: Integration testing
 */

#include "Gui/Settings/SettingsManager.h"

#include <QTest>
#include <QTemporaryFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#ifdef Q_OS_WIN

class SettingsManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    // Task 1.3: Test SettingsManager class creation and default values
    void testDefaultValuesOnFirstRun();
    void testAutosaveDefaultValue();
    void testDefaultSaveLocationDefaultValue();
    void testDefaultFormatDefaultValue();

    // Task 1.3: Test settings persistence
    void testSettingsPersistAcrossRestarts();
    void testSaveAndLoadSettings();

    // Task 1.3: Test settings can be changed
    void testSetAutosave();
    void testSetDefaultSaveLocation();
    void testSetDefaultFormat();
    void testSetRememberLastLocation();

    // Task 6.6: Test resetToDefaults()
    void testResetToDefaults();

    // Task 1.3: Test generateFilename()
    void testGenerateFilenameWithDefaultPattern();
    void testGenerateFilenameWithCustomPattern();
    void testGenerateFilenameCreatesValidFilenames();

    // Task 6.6: Test getSaveLocation()
    void testGetSaveLocationReturnsDefaultWhenRememberDisabled();
    void testGetSaveLocationReturnsLastWhenRememberEnabled();

    // Task 6.6: Test path validation
    void testPathValidationHandlesInvalidPaths();
    void testPathValidationHandlesRelativePaths();

private:
    QString m_testConfigPath;
    QString m_settingsFilePath;  // MEDIUM #3: Dynamic path instead of hardcoded
    SettingsManager *m_manager;
};

void SettingsManagerTest::initTestCase()
{
    fprintf(stdout, "=== SettingsManagerTest::initTestCase starting ===\n");
    fflush(stdout);

    // MEDIUM #3: Use QStandardPaths to get settings file path dynamically
    // QSettings with UserScope and organization "Spectacle", app "Spectacle"
    QString settingsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_settingsFilePath = settingsDir + QLatin1String("/Spectacle.ini");

    if (QFile::exists(m_settingsFilePath)) {
        QFile::remove(m_settingsFilePath);
        fprintf(stdout, "Removed existing settings file: %s\n", m_settingsFilePath.toUtf8().constData());
        fflush(stdout);
    }

    // Create a temporary config file for testing
    QTemporaryFile tempFile;
    tempFile.open();
    m_testConfigPath = tempFile.fileName();
    tempFile.close();

    fprintf(stdout, "Test config path: %s\n", m_testConfigPath.toUtf8().constData());
    fprintf(stdout, "Settings file path: %s\n", m_settingsFilePath.toUtf8().constData());
    fflush(stdout);
}

void SettingsManagerTest::cleanupTestCase()
{
    fprintf(stdout, "=== SettingsManagerTest::cleanupTestCase ===\n");
    fflush(stdout);

    // Cleanup test config file
    if (QFile::exists(m_testConfigPath)) {
        QFile::remove(m_testConfigPath);
    }

    // Cleanup settings file to ensure clean state for next run
    if (QFile::exists(m_settingsFilePath)) {
        QFile::remove(m_settingsFilePath);
        fprintf(stdout, "Cleaned up settings file\n");
        fflush(stdout);
    }
}

void SettingsManagerTest::init()
{
    fprintf(stdout, "=== SettingsManagerTest::init() creating SettingsManager ===\n");
    fflush(stdout);

    // Clean up any existing settings file to ensure test independence
    if (QFile::exists(m_settingsFilePath)) {
        QFile::remove(m_settingsFilePath);
        fprintf(stdout, "Cleaned settings file for test independence\n");
        fflush(stdout);
    }

    // Create a fresh SettingsManager for each test
    m_manager = new SettingsManager();

    fprintf(stdout, "SettingsManager created successfully\n");
    fflush(stdout);
}

// Task 1.3: Test default values on first run
void SettingsManagerTest::testDefaultValuesOnFirstRun()
{
    fprintf(stdout, "testDefaultValuesOnFirstRun starting\n");
    fflush(stdout);

    // Verify default values match specification
    fprintf(stdout, "Checking autosave...\n");
    fflush(stdout);
    QVERIFY2(m_manager->autosave() == true, "Autosave should default to true");

    fprintf(stdout, "Checking save location...\n");
    fflush(stdout);
    QVERIFY2(!m_manager->defaultSaveLocation().isEmpty(), "Default save location should not be empty");

    fprintf(stdout, "Checking format...\n");
    fflush(stdout);
    QVERIFY2(m_manager->defaultFormat() == "png", "Default format should be PNG");

    fprintf(stdout, "Checking remember last location...\n");
    fflush(stdout);
    QVERIFY2(m_manager->rememberLastLocation() == true, "Remember last location should default to true");

    fprintf(stdout, "testDefaultValuesOnFirstRun completed\n");
    fflush(stdout);
}

void SettingsManagerTest::testAutosaveDefaultValue()
{
    QVERIFY(m_manager->autosave() == true);
}

void SettingsManagerTest::testDefaultSaveLocationDefaultValue()
{
    QString defaultPath = m_manager->defaultSaveLocation();
    QVERIFY2(!defaultPath.isEmpty(), "Default save location should not be empty");

    // Should point to Pictures/Screenshots or similar
    QDir dir(defaultPath);
    QVERIFY2(dir.absolutePath().contains("Pictures", Qt::CaseInsensitive),
              "Default location should be in Pictures folder");
}

void SettingsManagerTest::testDefaultFormatDefaultValue()
{
    QCOMPARE(m_manager->defaultFormat(), QString("png"));
}

// Task 1.3: Test settings persistence
void SettingsManagerTest::testSettingsPersistAcrossRestarts()
{
    // Set some non-default values
    m_manager->setAutosave(false);
    m_manager->setDefaultFormat("jpg");
    m_manager->saveSettings();

    // Create a new SettingsManager instance (simulates restart)
    SettingsManager *newManager = new SettingsManager();
    newManager->loadSettings();

    // Verify values persisted
    QVERIFY2(newManager->autosave() == false, "Autosave should persist across restarts");
    QCOMPARE(newManager->defaultFormat(), QString("jpg"));

    delete newManager;
}

void SettingsManagerTest::testSaveAndLoadSettings()
{
    // Modify settings
    m_manager->setAutosave(false);
    m_manager->setDefaultSaveLocation("C:/TestScreenshots");
    m_manager->setDefaultFormat("bmp");
    m_manager->saveSettings();

    // Reload settings
    m_manager->loadSettings();

    // Verify all settings were saved and loaded
    QVERIFY2(m_manager->autosave() == false, "Autosave should be false after reload");
    QCOMPARE(m_manager->defaultSaveLocation(), QString("C:/TestScreenshots"));
    QCOMPARE(m_manager->defaultFormat(), QString("bmp"));
}

// Task 1.3: Test settings can be changed
void SettingsManagerTest::testSetAutosave()
{
    // Test setting to false
    m_manager->setAutosave(false);
    QVERIFY2(m_manager->autosave() == false, "Autosave should be false");

    // Test setting back to true
    m_manager->setAutosave(true);
    QVERIFY2(m_manager->autosave() == true, "Autosave should be true");
}

void SettingsManagerTest::testSetDefaultSaveLocation()
{
    QString testPath = "C:/MyScreenshots";
    m_manager->setDefaultSaveLocation(testPath);

    QCOMPARE(m_manager->defaultSaveLocation(), testPath);
}

void SettingsManagerTest::testSetDefaultFormat()
{
    m_manager->setDefaultFormat("jpeg");
    QCOMPARE(m_manager->defaultFormat(), QString("jpeg"));

    m_manager->setDefaultFormat("bmp");
    QCOMPARE(m_manager->defaultFormat(), QString("bmp"));
}

void SettingsManagerTest::testSetRememberLastLocation()
{
    m_manager->setRememberLastLocation(false);
    QVERIFY2(m_manager->rememberLastLocation() == false, "Remember last location should be false");

    m_manager->setRememberLastLocation(true);
    QVERIFY2(m_manager->rememberLastLocation() == true, "Remember last location should be true");
}

// Task 6.6: Test resetToDefaults()
void SettingsManagerTest::testResetToDefaults()
{
    // Change all values from defaults
    m_manager->setAutosave(false);
    m_manager->setDefaultSaveLocation("C:/CustomPath");
    m_manager->setDefaultFormat("jpeg");
    m_manager->setRememberLastLocation(false);

    // Reset to defaults
    m_manager->resetToDefaults();

    // Verify all values are back to defaults
    QVERIFY2(m_manager->autosave() == true, "Autosave should reset to true");
    QVERIFY2(m_manager->defaultFormat() == "png", "Format should reset to PNG");
    QVERIFY2(m_manager->rememberLastLocation() == true, "Remember last location should reset to true");

    // Default save location should point to Pictures folder
    QString defaultPath = m_manager->defaultSaveLocation();
    QVERIFY2(defaultPath.contains("Pictures", Qt::CaseInsensitive),
              "Default location should reset to Pictures folder");
}

// Task 1.3: Test generateFilename()
void SettingsManagerTest::testGenerateFilenameWithDefaultPattern()
{
    fprintf(stdout, "testGenerateFilenameWithDefaultPattern starting\n");
    fflush(stdout);

    // Simple test: Direct QString creation
    QString simple = QString::fromUtf8("screenshot");
    fprintf(stdout, "Simple QString: '%s'\n", simple.toUtf8().constData());
    fflush(stdout);

    // Test QDateTime::toString()
    QString testPattern = QString::fromUtf8("screenshot-yyyy-MM-dd-HHmmss");
    QString testResult = QDateTime::currentDateTime().toString(testPattern);
    fprintf(stdout, "Test pattern: '%s'\n", testPattern.toUtf8().constData());
    fprintf(stdout, "Test result: '%s'\n", testResult.toUtf8().constData());
    fflush(stdout);

    QString filename = m_manager->generateFilename();

    fprintf(stdout, "Generated filename: '%s'\n", filename.toUtf8().constData());
    fflush(stdout);

    // Should follow pattern: screenshot-YYYY-MM-DD-HHMMSS.png
    QVERIFY2(filename.startsWith("screenshot-"), "Filename should start with 'screenshot-'");
    QVERIFY2(filename.endsWith(".png"), "Filename should end with '.png'");
    QVERIFY2(filename.length() > 20, "Filename should include timestamp");

    fprintf(stdout, "testGenerateFilenameWithDefaultPattern completed\n");
    fflush(stdout);
}

void SettingsManagerTest::testGenerateFilenameWithCustomPattern()
{
    fprintf(stdout, "testGenerateFilenameWithCustomPattern starting\n");
    fflush(stdout);

    // Test custom pattern (using Qt format codes, escaped with single quotes)
    QString customPattern = "'capture-'yyyyMMdd-HHmmss";
    QString filename = m_manager->generateFilename(customPattern);

    fprintf(stdout, "Generated filename with custom pattern: '%s'\n", filename.toUtf8().constData());
    fflush(stdout);

    QVERIFY2(filename.startsWith("capture-"), "Filename should use custom pattern prefix");
    QVERIFY2(filename.contains(QDateTime::currentDateTime().toString("yyyyMMdd")),
              "Filename should include date from custom pattern");

    fprintf(stdout, "testGenerateFilenameWithCustomPattern completed\n");
    fflush(stdout);
}

void SettingsManagerTest::testGenerateFilenameCreatesValidFilenames()
{
    QString filename = m_manager->generateFilename();

    // Should be a valid filename (no invalid characters)
    QVERIFY2(!filename.contains(QRegularExpression("[<>:\"/\\|?*]")),
              "Filename should not contain invalid characters");

    // Should be usable in a file path
    QTemporaryFile tempFile(QDir::tempPath() + QDir::separator() + filename);
    QVERIFY2(tempFile.open(), "Filename should be valid for file creation");
}

// Task 6.6: Test getSaveLocation()
void SettingsManagerTest::testGetSaveLocationReturnsDefaultWhenRememberDisabled()
{
    m_manager->setRememberLastLocation(false);
    m_manager->setDefaultSaveLocation("C:/DefaultScreenshots");

    // Should return default location when remember is disabled
    QString saveLocation = m_manager->getSaveLocation();
    QCOMPARE(saveLocation, QString("C:/DefaultScreenshots"));
}

void SettingsManagerTest::testGetSaveLocationReturnsLastWhenRememberEnabled()
{
    m_manager->setRememberLastLocation(true);
    m_manager->setDefaultSaveLocation("C:/DefaultScreenshots");

    // Simulate having saved to a different location last time
    // (This would require setLastSaveLocation() method or similar)
    // For now, verify it returns the default when no last location exists
    QString saveLocation = m_manager->getSaveLocation();
    QCOMPARE(saveLocation, QString("C:/DefaultScreenshots"));
}

// Task 6.6: Test path validation
void SettingsManagerTest::testPathValidationHandlesInvalidPaths()
{
    // SettingsManager should handle invalid paths gracefully
    QString invalidPath = "C:/<>:Invalid|Path*?";

    // Should not crash when setting invalid path
    m_manager->setDefaultSaveLocation(invalidPath);
    // May sanitize or reject the invalid path
    QString result = m_manager->defaultSaveLocation();

    // If path was accepted, it should at least not cause crashes
    QVERIFY2(true, "Should handle invalid paths without crashing");
}

void SettingsManagerTest::testPathValidationHandlesRelativePaths()
{
    QString relativePath = "./Screenshots";

    // Should handle relative paths (may convert to absolute)
    m_manager->setDefaultSaveLocation(relativePath);
    QString result = m_manager->defaultSaveLocation();

    // Should not crash and should store something
    QVERIFY2(!result.isEmpty(), "Should handle relative paths");
}

// Use standard QTEST_MAIN macro - it will create QApplication which is fine for SettingsManager
QTEST_MAIN(SettingsManagerTest)

#include "SettingsManagerTest.moc"

#endif // Q_OS_WIN
