#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;
class Settings final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Settings)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings() final;

    [[nodiscard]] QByteArray mainWindowGeometry() const;

    [[nodiscard]] QString emulatorBinary() const;
    [[nodiscard]] QString startCommand() const;
    [[nodiscard]] QString settingsCommand() const;

    static QString configHome();

public slots:
    void resetDefaults();

    void setMainWindowGeometry(const QByteArray &);

    void setEmulatorBinary(const QString &);
    void setSettingsCommand(const QString &);
    void setStartCommand(const QString &);

private:
    QSettings *mSettings{};
};

#endif // SETTINGS_H
