// Copyright (C) 2024 Ossi Saukko <osaukko@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file  machinedelegate.cpp
 * @brief MachineDelegate class implementation
 */

#include "machinedelegate.h"
#include "machinelistmodel.h"

#include <QApplication>
#include <QPainter>

/**
 * @brief Construct machine delegate
 * @param[in] parent   Pointer to the parent object
 */
MachineDelegate::MachineDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

MachineDelegate::~MachineDelegate() = default;

/**
 * @brief Painting Machine item
 * 
 * This function starts by initializing the style options and checking
 * whether the desktop theme is light or dark.
 *
 * Next, the function calculates the positions of the items to be drawn
 * using the @ref calculateLayout function.
 * 
 * The function saves the drawing settings before drawing and returns
 * them at the end of the drawing.
 * 
 * @par Painting
 * 
 * <img src="MachineDelegate-Layout.svg" alt="Machine item layout">
 * 
 * The first step is to draw a rounded background rectangle. This is
 * drawn if the target is selected or the mouse pointer is over it.
 * The rectangle's color is derived from the style highlight color and
 * modified based on the desktop color theme, selection status, and
 * whether the mouse is over the item. The outline for the rectangle is
 * drawn if the mouse pointer is over the item.
 * 
 * The machine icon is drawn in the middle of the reserved area.
 *
 * The name is drawn using a 1 pt larger version of the style font in
 * the space reserved.
 *
 * The summary is drawn using a 1 pt smaller version of the style font
 * in the space reserved. Text is colored using the placeholder color
 * from the style.
 * 
 * If the name or summary does not fit in the reserved area, they are
 * cut to fit, and `...` is added to the end of the text.
 * 
 * @param[in] painter   Pointer to painter object used for drawing
 * @param[in] option    Style options for the item
 * @param[in] index     Index for reading Machine item data
 */
void MachineDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    // Init style option
    auto styleOption = option;
    initStyleOption(&styleOption, index);
    const bool darkTheme = styleOption.palette.color(QPalette::Window).value()
                           < styleOption.palette.color(QPalette::WindowText).value();

    // Calculate layout
    QRect iconArea;
    QRect nameArea;
    QRect summaryArea;
    calculateLayout(styleOption, index, &iconArea, &nameArea, &summaryArea);

    // Save painter state
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Avoid painting outside of the dedicated area
    painter->setClipRect(styleOption.rect);

    // Draw background?
    const auto selected = styleOption.state.testFlag(QStyle::State_Selected);
    const auto mouseOver = styleOption.state.testFlag(QStyle::State_MouseOver);
    if (selected || mouseOver) {
        auto backgroundColor = styleOption.palette.color(QPalette::Highlight);
        backgroundColor = darkTheme ? backgroundColor.darker() : backgroundColor.lighter();
        if (!selected) {
            backgroundColor = darkTheme ? backgroundColor.darker() : backgroundColor.lighter();
        }
        painter->setBrush(backgroundColor);
        if (mouseOver) {
            painter->setPen(styleOption.palette.color(QPalette::Highlight));
        } else {
            painter->setPen(Qt::NoPen);
        }
        painter->drawRoundedRect(styleOption.rect.adjusted(1, 1, -1, -1), 3, 3);
    }

    // Draw decoration icon center of icon area
    if (!styleOption.icon.isNull()) {
        QRect drawArea = iconArea;
        drawArea.setSize(styleOption.decorationSize);
        drawArea.moveLeft(drawArea.left() + (iconArea.width() - drawArea.width()) / 2);
        drawArea.moveTop(drawArea.top() + (iconArea.height() - drawArea.height()) / 2);
        styleOption.icon.paint(painter, drawArea);
    }

    // Draw name label
    auto nameFont = styleOption.font;
    nameFont.setPointSize(nameFont.pointSize() + 1);
    auto nameFontMetrics = QFontMetrics(nameFont);
    auto elidedText = nameFontMetrics.elidedText(index.data(Qt::DisplayRole).toString(),
                                                 Qt::ElideRight,
                                                 nameArea.width());
    painter->setFont(nameFont);
    painter->setPen(styleOption.palette.color(QPalette::WindowText));
    painter->drawText(nameArea, Qt::TextSingleLine, elidedText);

    // Draw summary label
    auto summaryFont = styleOption.font;
    summaryFont.setPointSize(summaryFont.pointSize() - 1);
    auto summaryFontMetrics = QFontMetrics(summaryFont);
    elidedText = summaryFontMetrics.elidedText(index.data(MachineListModel::SummaryRole).toString(),
                                               Qt::ElideRight,
                                               summaryArea.width());
    painter->setFont(summaryFont);
    painter->setPen(styleOption.palette.placeholderText().color());
    painter->drawText(summaryArea, Qt::TextSingleLine, elidedText);

    // Restore painter to previous state
    painter->restore();
}

/**
 * @brief Calculates the appropriate size to fit all the data
 * 
 * This function initializes the style options and passes it and *index*
 * for the @ref calculateLayout to the math.
 * 
 * @param[in] option    Style options for the item
 * @param[in] index     Index for reading Machine item data
 * @return Optimal size for the item
 */
QSize MachineDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto styleOption = option;
    initStyleOption(&styleOption, index);
    return calculateLayout(styleOption, index);
}

/**
 * @brief Search for a style object
 *
 * A helper function that returns the style object of the parent widget
 * or application default style as a fallback.
 * 
 * @return Pointer to style object
 */
QStyle *MachineDelegate::getStyle() const
{
    // Use parent widget style when available and use application style as a fallback.
    auto *widget = qobject_cast<QWidget *>(parent());
    return widget != nullptr ? widget->style() : QApplication::style();
}

/**
 * @brief Calculate optimal size and item positions
 * 
 * The method first retrieves the style sheet using the @ref getStyle
 * method. Style object is used to retrieve information about margins
 * and spacing.
 * 
 * The method calculates the size of the areas needed to draw each
 * item. Using this information, the optimal size can be calculated to
 * fit all the information.
 * 
 * Pointers to rectangle objects are optional. If they are given, then
 * the positions and sizes of the items are calculated for them. These
 * calculations take into account the total available drawing area.
 * 
 * <img src="MachineDelegate-Layout.svg" alt="Machine item layout">
 * 
 * @param[in] option        Style options for the item
 * @param[in] index         Index for reading Machine item data
 * @param[out] iconArea     Calculate the icon area into this rectangle (optional)
 * @param[out] nameArea     Calculate the name label area into this rectangle (optional)
 * @param[out] summaryArea  Calculate the summary label area into this rectangle (optional)
 * @return Optimal size for the item
 */
QSize MachineDelegate::calculateLayout(const QStyleOptionViewItem &option,
                                       const QModelIndex &index,
                                       QRect *iconArea,
                                       QRect *nameArea,
                                       QRect *summaryArea) const
{
    // Collect metrics
    const auto *style = getStyle();
    const auto leftMargin = style->pixelMetric(QStyle::PM_LayoutLeftMargin, &option);
    const auto topMargin = style->pixelMetric(QStyle::PM_LayoutTopMargin, &option);
    const auto rightMargin = style->pixelMetric(QStyle::PM_LayoutRightMargin, &option);
    const auto bottomMargin = style->pixelMetric(QStyle::PM_LayoutBottomMargin, &option);
    const auto horizontalSpacing = style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing, &option);

    // Size for the name label
    auto nameFont = option.font;
    nameFont.setPointSize(nameFont.pointSize() + 1);
    auto nameFontMetrics = QFontMetrics(nameFont);
    auto nameSize = nameFontMetrics.size(Qt::TextSingleLine, index.data(Qt::DisplayRole).toString());

    // Size for the summary label
    auto summaryFont = option.font;
    summaryFont.setPointSize(summaryFont.pointSize() - 1);
    auto summaryFontMetrics = QFontMetrics(summaryFont);
    auto summarySize = summaryFontMetrics.size(Qt::TextSingleLine,
                                               index.data(MachineListModel::SummaryRole).toString());

    // Size for dectoration (also content height)
    const auto decorationSize = std::max(option.decorationSize.height(),
                                         nameSize.height() + summarySize.height());

    // Calculate width and height that fits everything
    auto width = leftMargin + decorationSize + horizontalSpacing
                 + std::max(nameSize.width(), summarySize.width()) + rightMargin;
    auto height = topMargin + decorationSize + bottomMargin;

    // Prepare areas if they are given
    if (iconArea != nullptr) {
        iconArea->setRect(option.rect.left() + leftMargin,
                          option.rect.top() + topMargin,
                          decorationSize,
                          decorationSize);
    }
    if (nameArea != nullptr) {
        nameArea->setTop(option.rect.top() + topMargin);
        nameArea->setHeight(nameSize.height());
        nameArea->setLeft(option.rect.left() + leftMargin + decorationSize + horizontalSpacing);
        nameArea->setRight(option.rect.right() - rightMargin);
    }
    if (summaryArea != nullptr) {
        summaryArea->setHeight(summarySize.height());
        summaryArea->moveBottom(option.rect.bottom() - bottomMargin);
        summaryArea->setLeft(option.rect.left() + leftMargin + decorationSize + horizontalSpacing);
        summaryArea->setRight(option.rect.right() - rightMargin);
    }

    return {width, height};
}
