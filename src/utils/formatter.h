#ifndef FORMATTER_H
#define FORMATTER_H

#include <QHash>
#include <QSharedDataPointer>

class FormatterData;

class Formatter
{
public:
    Formatter();
    Formatter(const Formatter &);
    Formatter(Formatter &&) noexcept;
    ~Formatter();

    bool setInput(const QString &input);
    [[nodiscard]] QString format(const QHash<QString, QString> &variables) const;

    static QString format(const QString &input,
                          const QHash<QString, QString> &variables,
                          bool *ok = nullptr);

    Formatter &operator=(const Formatter &other);
    Formatter &operator=(Formatter &&other) noexcept;

private:
    QSharedDataPointer<FormatterData> data;
};

#endif // FORMATTER_H
