// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  formatter.cpp
 * @brief Formatter class implementation
 */

#include "formatter.h"

// StringPart
//--------------------------------------------------------------------------------------------------

/**
 * @brief Formattable string part
 */
struct StringPart
{
    /**
     * @brief Type of the part
     * 
     * The type defines how the *string* is interpreted.
     */
    enum Type {
        Text,    /*!< @brief This part is literal text */
        Variable /*!< @brief This part is a variable that should be replaced when text is formatted */
    };
    Type type;      /*!< @brief Type for this part */
    QString string; /*!< @brief Text or variable name */
};

// FormatterData
//--------------------------------------------------------------------------------------------------

/**
 * @brief Implicitly shared data for Formatter objects
 */
class FormatterData : public QSharedData
{
public:
    QVector<StringPart> parts; /*!< @brief Parts of string to format */
};

// Formatter
//--------------------------------------------------------------------------------------------------

/**
 * @brief Construct a new empty Formatter
 */
Formatter::Formatter()
    : data(new FormatterData)
{}

/**
 * @brief Construct Formatter object with reference to *other*
 * @param[in] other   Use the same values as this object
 */
Formatter::Formatter(const Formatter &other) = default;

/**
 * @brief Construct Formatter object and move the reference from *other*. 
 * @param[in] other   Move reference from this object
 */
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

/**
 * @brief Set input string for formatting
 * 
 * The function splits the given string into parts, which can then be
 * used to form the formatted string.
 * 
 * @param[in] input   Format this string
 * @return `true` if input is valid, 
 *         `false` if something is wrong with the input.
 */
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

/**
 * @brief Make formatted string
 * 
 * Format the given input string and replace the variables in it with
 * the given *variables*.
 *
 * Variables are given as a hash map where *key* is the variable and
 * *value* is the text that should replace the variable. 
 * 
 * @param[in] variables   Use these variables for formatting the output
 * @return Formatted string
 */
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

/**
 * @brief Convenience function text formatting
 * 
 * This static function is a convenient one-command way to perform text formatting.
 * 
 * @param[in] input       Input string for formatting
 * @param[in] variables   Hash map of variables with their values
 * @param[out] ok         This optional flag allows checking if the formatting was successful
 * @return Formatted string
 */
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

/**
 * @brief Assignment operator
 * @param[in] other   Use the same values as this object 
 * @return Reference to this object
 */
Formatter &Formatter::operator=(const Formatter &other) = default;

/**
 * @brief Move assignment operator
 * @param[in] other   Move reference from this object
 * @return Reference to this object
 */
Formatter &Formatter::operator=(Formatter &&other) noexcept
{
    data = std::move(other.data);
    return *this;
}
