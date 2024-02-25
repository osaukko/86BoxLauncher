#ifndef MACHINE_H
#define MACHINE_H

#include <QSharedDataPointer>

class QIcon;
class MachineData;

class Machine
{
public:
    enum IconType { FromTheme, FromFile };

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
    void setSummary(const QString &description);

    [[nodiscard]] QString configFile() const;
    void setConfigFile(const QString &configFile);

    [[nodiscard]] QString startCommand() const;
    void setStartCommand(const QString &startCommand) const;

    [[nodiscard]] QString settingsCommand() const;
    void setSettingsCommand(const QString &settingsCommand) const;

    Machine &operator=(const Machine &other);
    Machine &operator=(Machine &&other) noexcept;

private:
    QSharedDataPointer<MachineData> data;
};

#endif // MACHINE_H
