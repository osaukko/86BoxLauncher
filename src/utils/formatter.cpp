#include "formatter.h"

// StringPart
//--------------------------------------------------------------------------------------------------

struct StringPart
{
    enum Type { Text, Variable };
    Type type;
    QString string;
};

// FormatterData
//--------------------------------------------------------------------------------------------------

class FormatterData : public QSharedData
{
public:
    QVector<StringPart> parts;
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

namespace {

// Processing next character for Text segment
bool textProcessing(QString::ConstIterator &it,
                    QVector<StringPart> &parts,
                    StringPart::Type &type,
                    QString &string)
{
    if (*it == '{') {
        it = std::next(it);
        if (*it == '{') {
            // double { is one { in the text segment
            string += *it;
        } else {
            // Switching to Variable mode, save text segment if have any content
            if (!string.isEmpty()) {
                parts.push_back({type, string});
            }
            type = StringPart::Variable;
            string.clear();

            // We are ahead one characater, reverse a little bit
            it = std::prev(it);
        }
    } else if (*it == '}') {
        it = std::next(it);
        if (*it == '}') {
            // double } is one } in the text segment
            string += *it;
        } else {
            // single } in text segment is error
            return false;
        }
    } else {
        string += *it;
    }

    return true;
}

// Processing next character for Variable segment
bool variableProcessing(QString::ConstIterator &it,
                        QVector<StringPart> &parts,
                        StringPart::Type &type,
                        QString &string)
{
    // We do not allow { in variable names
    if (*it == '{') {
        return false;
    }

    if (*it == '}') {
        // We do not allow empty variable names
        if (string.isEmpty()) {
            return false;
        }

        parts.push_back({type, string});
        type = StringPart::Text;
        string.clear();
    } else {
        string += *it;
    }

    return true;
}

} // namespace

bool Formatter::setInput(const QString &input)
{
    data->parts.clear();
    auto parts = QVector<StringPart>();

    auto type = StringPart::Text;
    QString string;

    const auto *it = input.begin();
    while (it != input.end()) {
        switch (type) {
        case StringPart::Text:
            if (!textProcessing(it, parts, type, string)) {
                return false;
            }
            break;

        case StringPart::Variable:
            if (!variableProcessing(it, parts, type, string)) {
                return false;
            }
            break;
        }

        it = std::next(it);
    }

    // If we end up to Variable type, then input is missing a }
    if (type == StringPart::Variable) {
        return false;
    }

    // Append last text part if we have content for it
    if (!string.isEmpty()) {
        parts.push_back({type, string});
    }

    // Input was valid
    data->parts = parts;
    return true;
}

QString Formatter::format(const QHash<QString, QString> &variables) const
{
    QString string;
    for (const auto &part : data->parts) {
        switch (part.type) {
        case StringPart::Text:
            string.append(part.string);
            break;
        case StringPart::Variable:
            string.append(variables.value(part.string));
            break;
        }
    }
    return string;
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
