#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;
class Settings final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Settings)
    Q_PROPERTY(QString emulatorBinary READ emulatorBinary WRITE setEmulatorBinary NOTIFY
                   emulatorBinaryChanged FINAL)
    Q_PROPERTY(QString startCommand READ startCommand WRITE setStartCommand NOTIFY
                   startCommandChanged FINAL)
    Q_PROPERTY(QString settingsCommand READ settingsCommand WRITE setSettingsCommand NOTIFY
                   settingsCommandChanged FINAL)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings() final;

    [[nodiscard]] QString emulatorBinary() const;
    [[nodiscard]] QString startCommand() const;
    [[nodiscard]] QString settingsCommand() const;

    static QString configHome();

public slots:
    void resetDefaults();
    void setEmulatorBinary(const QString &);
    void setStartCommand(const QString &);
    void setSettingsCommand(const QString &);

signals:
    void emulatorBinaryChanged(const QString &);
    void startCommandChanged(const QString &);
    void settingsCommandChanged(const QString &);

private:
    QSettings *mSettings{};
};

#endif // SETTINGS_H
