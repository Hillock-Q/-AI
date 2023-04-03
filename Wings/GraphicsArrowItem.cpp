#include "GraphicsArrowItem.h"
#include <QGraphicsSceneMouseEvent>
#include <corecrt_math_defines.h>
#include <QGraphicsScene>
#include <QPainter>
#include <QPen>

constexpr int PEN_WIDTH = 2;
constexpr int MARGIN_WIDTH = 10;
constexpr qreal ARROW_SIZE = 20;

CGraphicsArrowItem::CGraphicsArrowItem(bool edit, int sn, qreal x1, qreal y1, qreal x2, qreal y2, QColor const &color, QGraphicsItem *parent)
    : QGraphicsLineItem(x1, y1, x2, y2, parent), CItemInfo(sn), m_edit(edit), m_color(color), m_begin(x1, y1), m_end(x2, y2)
{
    if (edit)
    {
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable /*| QGraphicsItem::ItemIsMovable*/ | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    }

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);

    calcChannelPos();
    setLine(m_begin.x(), m_begin.y(), m_end.x(), m_end.y());
}

CGraphicsArrowItem::CGraphicsArrowItem(bool edit, int sn, QLineF const &line, QColor const &color, QGraphicsItem *parent)
    : QGraphicsLineItem(line, parent), CItemInfo(sn), m_edit(edit), m_color(color), m_begin(line.x1(), line.y1()), m_end(line.x2(), line.y2())
{
    if (edit)
    {
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable /*| QGraphicsItem::ItemIsMovable*/ | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    }

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);

    calcChannelPos();
    setLine(m_begin.x(), m_begin.y(), m_end.x(), m_end.y());
}

CGraphicsArrowItem::CGraphicsArrowItem(bool edit, int sn, QColor const &color, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), CItemInfo(sn), m_edit(edit), m_color(color)
{
    if (edit)
    {
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable /*| QGraphicsItem::ItemIsMovable*/ | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    }

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);
}

CGraphicsArrowItem::~CGraphicsArrowItem()
{
    disconnect();
}

int CGraphicsArrowItem::type() const
{
    return Type;
}

QPainterPath CGraphicsArrowItem::shape() const
{
    auto path = QGraphicsLineItem::shape();
    QPainterPathStroker stroker;
    stroker.setWidth(MARGIN_WIDTH);
    return stroker.createStroke(path);
}

void CGraphicsArrowItem::SetColor(QColor const &color)
{
    m_color = color;

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);
}

QColor const& CGraphicsArrowItem::GetColor() const
{
    return m_color;
}

void CGraphicsArrowItem::SetBeginPoint(QPointF const &begin)
{
    m_begin = begin;
    calcChannelPos();
    setLine(m_begin.x(), m_begin.y(), m_end.x(), m_end.y());
    scene()->update();
}

void CGraphicsArrowItem::SetEndPoint(QPointF const &end)
{
    m_end = end;
    calcChannelPos();
    setLine(m_begin.x(), m_begin.y(), m_end.x(), m_end.y());
    scene()->update();
}

void CGraphicsArrowItem::calcChannelPos()
{
    auto dx = m_end.x() - m_begin.x();
    auto dy = m_end.y() - m_begin.y();

    m_outPos.setX(m_begin.x() + 0.2 * dx);
    m_outPos.setY(m_begin.y() + 0.2 * dy);

    m_inPos.setX(m_end.x() - 0.2 * dx);
    m_inPos.setY(m_end.y() - 0.2 * dy);
}

void CGraphicsArrowItem::SetChannel(ROLE role, int newChannel)
{
    for (auto &point : m_points)
        if (point.role == role)
            point.channel = newChannel;
}

qreal CGraphicsArrowItem::distance(QPointF const &pos0, QPointF const &pos1) const
{
    auto dx = pos0.x() - pos1.x();
    auto dy = pos0.y() - pos1.y();
    return std::sqrt(dx * dx + dy * dy);
}

QString CGraphicsArrowItem::inChannelsTips() const
{
    QString tips;

    if (m_points.size())
    {
        auto const &point = m_points[0];
        for (auto i = 0u; i < point.channels.size(); i++)
        {
            if (i == 3)
            {
                if (point.channels.size() == 4)
                    tips += "," + QString::number(point.channels[i].second);
                else
                    tips += "..." + QString::number(point.channels[point.channels.size() - 1].second);

                break;
            }
            else
            {
                if (i > 0)
                    tips += ",";

                tips += QString::number(point.channels[i].second);
            }
        }
    }

    return tips;
}

QString CGraphicsArrowItem::outChannelsTips() const
{
    QString tips;

    if (m_points.size())
    {
        auto const &point = m_points[0];
        for (auto i = 0u; i < point.channels.size(); i++)
        {
            if (i == 3)
            {
                if (point.channels.size() == 4)
                    tips += "," + QString::number(point.channels[i].first);
                else
                    tips += "..." + QString::number(point.channels[point.channels.size() - 1].first);

                break;
            }
            else
            {
                if (i > 0)
                    tips += ",";

                tips += QString::number(point.channels[i].first);
            }
        }
    }

    return tips;
}

void CGraphicsArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto pen = QPen(isSelected() ? Qt::black : m_color);
    pen.setWidth(PEN_WIDTH);
    painter->setPen(pen);
    painter->setBrush(QBrush(isSelected() ? Qt::white : m_color));

    QLineF line(m_end, m_begin);
    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * ARROW_SIZE, cos(angle + M_PI / 3) * ARROW_SIZE);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * ARROW_SIZE, cos(angle + M_PI - M_PI / 3) * ARROW_SIZE);
    QPolygonF arrowHead;
    arrowHead << line.p1() << arrowP1 << arrowP2;
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(line);
    painter->drawPolygon(arrowHead);

    pen.setColor(Qt::red);
    pen.setWidth(4);
    painter->setPen(pen);

    if (m_points.size() > 1)
    {
        painter->drawPoint(m_outPos);
        painter->drawPoint(m_inPos);

        pen.setColor(Qt::black);
        painter->setPen(pen);

        painter->drawText(m_outPos, QString::number(m_points[0].channel));
        painter->drawText(m_inPos, QString::number(m_points[1].channel));
    }
    else if (m_points.size() > 0)
    {
        painter->drawPoint(m_inPos);
        painter->drawPoint(m_outPos);

        pen.setColor(Qt::black);
        painter->setPen(pen);

        painter->drawText(m_inPos, inChannelsTips());
        painter->drawText(m_outPos, outChannelsTips());
    }
}

void CGraphicsArrowItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);
}

void CGraphicsArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseMoveEvent(event);
}

void CGraphicsArrowItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseReleaseEvent(event);
}

void CGraphicsArrowItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_edit && m_points.size() == 2)
    {
        auto pos = event->scenePos();
        auto dsOut = distance(pos, m_outPos);
        auto dsIn = distance(pos, m_inPos);

        if (dsOut < MARGIN_WIDTH && dsIn < MARGIN_WIDTH)
        {
            if (dsOut < dsIn)
            {
                if (m_points[0].role == e_src)
                    signalEditChannel(m_points[0].owner, m_points[0].arrow, false, m_points[0].channel, event->screenPos());
            }
            else
            {
                if (m_points[1].role == e_dst)
                    signalEditChannel(m_points[1].owner, m_points[1].arrow, true, m_points[1].channel, event->screenPos());
            }
        }
        else if (dsOut < MARGIN_WIDTH)
        {
            if (m_points[0].role == e_src)
                signalEditChannel(m_points[0].owner, m_points[0].arrow, false, m_points[0].channel, event->screenPos());
        }
        else if (dsIn < MARGIN_WIDTH)
        {
            if (m_points[1].role == e_dst)
                signalEditChannel(m_points[1].owner, m_points[1].arrow, true, m_points[1].channel, event->screenPos());
        }
    }

    //QGraphicsLineItem::mouseDoubleClickEvent(event);
}

void CGraphicsArrowItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (isSelected())
        emit signalContextMenu(event->screenPos());

    //QGraphicsLineItem::contextMenuEvent(event);
}
