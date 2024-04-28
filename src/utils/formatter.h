// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  formatter.h
 * @brief Formatter class definition
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <QHash>
#include <QSharedDataPointer>

class FormatterData;

/**
 * @brief Simple text formatting
 * 
 * This class allows us to format simple text similarly to Python
 * f-string and Rust format. It can find and replace variables
 * surrounded with `{braces}`. However, that is all it can do,
 * and it has no advanced features.
 * 
 * It works by tokenizing a given string to a vector of StringPart
 * items with types of @ref StringPart::Type::Text "Text" and 
 * @ref StringPart::Type::Variable "Variable".
 * 
 * For example: `Hello {what}` results vector with:
 * 
 * - @ref StringPart::Type::Text "Text": "Hello "
 * - @ref StringPart::Type::Variable "Variable": "what"
 * 
 * Braces can be used in the text. To do this, type two braces in a row.
 * @verbatim Input of "{{Hello}}" results a single text part with "{Hello}" @endverbatim
 * 
 * @par Example
 * 
 * @code{.cpp}
 * #include <QHash>
 * #include <QString>
 * #include "utils/formatter.h"
 * 
 * void example()
 * {
 *     // Example with one variable
 *     const auto input = QString{"Hello, {what}"};
 *     const auto variables = QHash<QString, QString>{{"what", "world"}};
 *     const auto formatted = Formatter::format(input, variables);
 *     Q_ASSERT(formatted == "Hello, world");
 *     
 *     // Example with two variables
 *     const auto message = Formatter::format(
 *         "{name} says {message}",                   // Input
 *         {{"name", "John"}, {"message", "Hello!"}}  // Variables
 *     );
 *     Q_ASSERT(message == "John says Hello!");       // Expected output
 * }
 * @endcode
 */
class Formatter
{
public:
    Formatter();
    Formatter(const Formatter &other);
    Formatter(Formatter &&other) noexcept;
    ~Formatter();

    bool setInput(const QString &input);
    [[nodiscard]] QString format(const QHash<QString, QString> &variables) const;

    static QString format(const QString &input,
                          const QHash<QString, QString> &variables,
                          bool *ok = nullptr);

    Formatter &operator=(const Formatter &other);
    Formatter &operator=(Formatter &&other) noexcept;

private:
    QSharedDataPointer<FormatterData> data; /*!< @brief Pointer to the implicitly shared data */
};

#endif // FORMATTER_H
