#ifndef CGRAPHICSARROWITEM_H
#define CGRAPHICSARROWITEM_H

#include <QObject>
#include <QGraphicsLineItem>
#include "ItemInfo.h"

class CGraphicsArrowItem: public QObject, public QGraphicsLineItem, public CItemInfo
{
    Q_OBJECT

public:
    enum {Type = UserType + 3};
    CGraphicsArrowItem(bool edit, int sn, qreal x1, qreal y1, qreal x2, qreal y2, QColor const &color, QGraphicsItem *parent = nullptr);
    CGraphicsArrowItem(bool edit, int sn, QLineF const &line, QColor const &color, QGraphicsItem *parent = nullptr);
    CGraphicsArrowItem(bool edit, int sn, QColor const &color, QGraphicsItem *parent = nullptr);

    virtual ~CGraphicsArrowItem() override;
    virtual int type() const override;
    virtual QPainterPath shape() const override;

    void SetColor(QColor const &color);
    QColor const& GetColor() const;

    void SetBeginPoint(QPointF const &begin);
    void SetEndPoint(QPointF const &end);

    void SetChannel(ROLE role, int newChannel);

signals:
    void signalContextMenu(QPoint const &pos);
    void signalEditChannel(int owner, int arrow, bool isIn, int channel, QPoint const &pos);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override; 
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;


private:
    void calcChannelPos();
    qreal distance(QPointF const &pos0, QPointF const &pos1) const;

    QString inChannelsTips() const;
    QString outChannelsTips() const;

private:
    bool                        m_edit;
    QColor                      m_color;

    QPointF                     m_begin;
    QPointF                     m_end;

    QPointF                     m_outPos;
    QPointF                     m_inPos;
};

#endif // CGRAPHICSARROWITEM_H
