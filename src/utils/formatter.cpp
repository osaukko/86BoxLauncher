#include "formatter.h"

// FormatterData
//--------------------------------------------------------------------------------------------------

class FormatterData : public QSharedData
{
public:
};

// Formatter
//--------------------------------------------------------------------------------------------------

Formatter::Formatter()
    : data(new FormatterData)
{}

Formatter::Formatter(const Formatter &rhs) = default;

Formatter::Formatter(Formatter &&other) noexcept
    : data(std::move(other.data))
{}

bool Formatter::setInput(const QString &input)
{
    return true;
}

QString Formatter::format(const QHash<QString, QString> &variables)
{
    return {};
}

QString Formatter::format(const QString &input, const QHash<QString, QString> &variables, bool *ok)
{
    Formatter formatter;
    auto is_ok = formatter.setInput(input);
    if (ok != nullptr) {
        *ok = is_ok;
    }
    return formatter.format(variables);
}

Formatter::~Formatter() = default;

Formatter &Formatter::operator=(const Formatter &other) = default;

Formatter &Formatter::operator=(Formatter &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
