#ifndef CGRAPHICSRECTITEM_H
#define CGRAPHICSRECTITEM_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QJsonObject>
#include "ItemInfo.h"

class CGraphicsRectItem: public QObject, public QGraphicsRectItem, public CItemInfo
{
    Q_OBJECT

public:
    enum {Type = UserType + 1};
    CGraphicsRectItem(bool edit, int sn, qreal x, qreal y, qreal width, qreal height, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent = nullptr);
    CGraphicsRectItem(bool edit, int sn, const QRectF &rect, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent = nullptr);
    CGraphicsRectItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent = nullptr);

    virtual ~CGraphicsRectItem() override;
    virtual int type() const override;

    void SetNetId(QString const &netId);
    QString const& GetNetId() const;

    void SetNetName(QString const &netName);
    QString const& GetNetName() const;

    void SetColor(QColor const &color);
    QColor const& GetColor() const;

    void SetAiParams(QJsonObject const &aiParams);
    QJsonObject GetAiParams() const;

    QPointF GetInputArrowBeginPos() const;
    QPointF GetInputArrowEndPos() const;

    QPointF GetOutputArrowBeginPos() const;
    QPointF GetOutputArrowEndPos() const;

    vector<int> GetInputInChannels() const;
    ChannelPairs_t GetInputChannelPairs();
    ChannelPairs_t GetOutputChannelPairs();

    bool inArrowArea(QPointF const &pos) const;
    int nextInChannel() const;
    int nextOutChannel() const;
    int inChannels() const;

signals:
    void signalMoved(bool edit, QGraphicsItem *self);
    void signalArrow(QGraphicsItem *self, QPointF const &pos, int channel);

    void signalContextMenu(QPoint const &pos);
    void signalEditNetName(QString const &netName, QPoint const &pos);

    void signalShowNet(int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams);
    void signalExtendNet(int sn, QString netId, QString netName, QJsonObject aiParams);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    bool judgeIn(QPointF const &pos) const;
    void adjustWidth();
    int indent(int cnt) const;

private:
    bool                        m_edit;
    QString                     m_netId;
    QString                     m_netName;
    QColor                      m_color;
    QJsonObject                 m_aiParams;
};

#endif // CGRAPHICSRECTITEM_H
