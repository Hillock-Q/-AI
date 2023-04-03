#include "Delegate.h"
#include <QPainter>

CDelegate::CDelegate(QObject *parent) : QItemDelegate(parent)
{
    QColor colorDefault(255, 201, 14);
    m_kDefaultItemBackgroundcColor = QBrush(colorDefault);

    QColor colorHover(255, 201, 14);
    m_kHoverItemBackgroundcColor = QBrush(colorHover);

    QColor colorSelected(238, 107, 71);
    m_kSelectedItemBackgroundcColor = QBrush(colorSelected);
}

void CDelegate::onHoverIndexChanged(const QModelIndex &index)
{
    m_hoverRow = index.row();
}

void CDelegate::onSelectedIndexChanged(const QModelIndex &index)
{
    m_selectedRow = index.row();
}

void CDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.row() == m_selectedRow)
    {
        painter->fillRect(option.rect, m_kSelectedItemBackgroundcColor);
    }
    else if(index.row() == m_hoverRow)
    {
        painter->fillRect(option.rect, m_kHoverItemBackgroundcColor);
    }
    else
    {
        painter->fillRect(option.rect, option.backgroundBrush);
    }

    painter->drawText(option.rect, index.data().toString());
}
