#ifndef MACHINE_H
#define MACHINE_H

#include <QSharedDataPointer>

class MachineData;
class QIcon;
class QString;

class Machine
{
public:
    enum IconType { NoIcon, IconFromTheme, IconFromFile };

    Machine();
    Machine(const Machine &other);
    Machine(Machine &&other) noexcept;
    ~Machine();

    [[nodiscard]] IconType iconType() const;
    [[nodiscard]] QString iconName() const;
    [[nodiscard]] QIcon icon() const;
    void setIcon(IconType type, const QString &name);

    [[nodiscard]] QString name() const;
    void setName(const QString &name);

    [[nodiscard]] QString summary() const;
    void setSummary(const QString &summary);

    [[nodiscard]] QString configFile() const;
    void setConfigFile(const QString &configFile);

    [[nodiscard]] QString startCommand() const;
    void setStartCommand(const QString &startCommand);

    [[nodiscard]] QString settingsCommand() const;
    void setSettingsCommand(const QString &settingsCommand);

    Machine &operator=(const Machine &other);
    Machine &operator=(Machine &&other) noexcept;

private:
    QSharedDataPointer<MachineData> data;
};

#endif // MACHINE_H
