#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>


class CDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CDelegate(QObject *parent = nullptr);
    void onHoverIndexChanged(const QModelIndex &index);
    void onSelectedIndexChanged(const QModelIndex &index);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int                     m_hoverRow;
    int                     m_selectedRow;
    QBrush                  m_kDefaultItemBackgroundcColor;
    QBrush                  m_kHoverItemBackgroundcColor;
    QBrush                  m_kSelectedItemBackgroundcColor;
};

#endif // DELEGATE_H
