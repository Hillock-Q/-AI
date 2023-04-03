#include "GraphicsRectItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QPen>

constexpr int PEN_WIDTH = 0;
constexpr int MARGIN_WIDTH = 20;
constexpr int ITEM_WIDTH = 100;
constexpr int ITEM_HEIGHT = 62;
constexpr int MAX_STRLENGTH = 10;
constexpr int ADJUSR_WIDTH = 5;

CGraphicsRectItem::CGraphicsRectItem(bool edit, int sn, qreal x, qreal y, qreal width, qreal height, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
{
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);

    adjustWidth();
}

CGraphicsRectItem::CGraphicsRectItem(bool edit, int sn, const QRectF &rect, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
{
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);

    adjustWidth();
}

CGraphicsRectItem::CGraphicsRectItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
{
    // 可选择、可移动
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | ItemIsFocusable);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);

    adjustWidth();
}

CGraphicsRectItem::~CGraphicsRectItem()
{
    disconnect();
}

int CGraphicsRectItem::type() const
{
    return Type;
}

void CGraphicsRectItem::SetNetId(QString const &netId)
{
    m_netId = netId;
}

QString const& CGraphicsRectItem::GetNetId() const
{
    return m_netId;
}

void CGraphicsRectItem::SetNetName(QString const &netName)
{
    m_netName = netName;
    adjustWidth();
}

QString const& CGraphicsRectItem::GetNetName() const
{
    return m_netName;
}

void CGraphicsRectItem::SetColor(QColor const &color)
{
    m_color = color;

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);
}

QColor const& CGraphicsRectItem::GetColor() const
{
    return m_color;
}

void CGraphicsRectItem::SetAiParams(QJsonObject const &aiParams)
{
    m_aiParams = aiParams;
}

QJsonObject CGraphicsRectItem::GetAiParams() const
{
    return m_aiParams;
}

QPointF CGraphicsRectItem::GetInputArrowBeginPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), -rect().height()));
}

QPointF CGraphicsRectItem::GetInputArrowEndPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), 0));
}

QPointF CGraphicsRectItem::GetOutputArrowBeginPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), rect().height()));
}

QPointF CGraphicsRectItem::GetOutputArrowEndPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), rect().height() * 2));
}

vector<int> CGraphicsRectItem::GetInputInChannels() const
{
    vector<int> inChannels;
    for (auto channel = 0; channel < m_in; ++channel)
    {
        bool inuse = false;

        for (auto const &point : m_points)
            if (point.role == e_dst && point.channel == channel)
            {
                inuse = true;
                break;
            }

        if (inuse)
            continue;

        inChannels.push_back(channel);
    }

    return inChannels;
}

ChannelPairs_t CGraphicsRectItem::GetInputChannelPairs()
{
    for (auto const &point : m_points)
        if (point.role == e_input)
            return point.channels;

    return ChannelPairs_t();
}

ChannelPairs_t CGraphicsRectItem::GetOutputChannelPairs()
{
    for (auto const &point : m_points)
        if (point.role == e_output)
            return point.channels;

    return ChannelPairs_t();
}

bool CGraphicsRectItem::inArrowArea(QPointF const &pos) const
{
    return judgeIn(mapFromScene(pos));
}

bool CGraphicsRectItem::judgeIn(QPointF const &pos) const
{
    if (pos.x() <= (rect().x() + MARGIN_WIDTH) || pos.x() >= (rect().x() + rect().width() - MARGIN_WIDTH) ||
            pos.y() <= (rect().y() + MARGIN_WIDTH) || pos.y() >= (rect().y() + rect().height() - MARGIN_WIDTH))
    {
        return true;
    }

    return false;
}

void CGraphicsRectItem::adjustWidth()
{
    if ((int)m_netName.size() > MAX_STRLENGTH)
    {
        auto dw = ADJUSR_WIDTH * ((int)m_netName.size() - MAX_STRLENGTH);
        auto rc = QRectF(0, 0, ITEM_WIDTH, ITEM_HEIGHT);
        rc.adjust(-dw, 0, +dw, 0);
        setRect(rc);
    }
}

int CGraphicsRectItem::indent(int cnt) const
{
    int loop = 0;
    while(cnt /= 10)
        loop++;

    return loop + 1;
}

int CGraphicsRectItem::nextInChannel() const
{
    for (auto channel = 0; channel < m_in; ++channel)
    {
        bool inuse = false;

        for (auto const &point : m_points)
            if (point.role == e_input)
            {
                for (auto const &channelPair : point.channels)
                    if (channelPair.second == channel)
                    {
                        inuse = true;
                        break;
                    }

                if (inuse)
                    break;
            }
            else if (point.role == e_dst && point.channel == channel)
            {
                inuse = true;
                break;
            }

        if (inuse)
            continue;

        return channel;
    }

    return -1;
}

int CGraphicsRectItem::nextOutChannel() const
{
    for (auto channel = 0; channel < m_out; ++channel)
    {
        bool inuse = false;
        for (auto const &point : m_points)
            if (point.role == e_src && point.channel == channel)
            {
                inuse = true;
                break;
            }
        if (inuse)
            continue;

        return channel;
    }

    return m_out - 1;
}

int CGraphicsRectItem::inChannels() const
{
    int cnt = 0;
    for (auto const &point : m_points)
        if (point.role == e_input || point.role == e_dst)
            cnt++;

    return cnt;
}

void CGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto pen = QPen(isSelected() ? Qt::black : m_color);
    pen.setWidth(PEN_WIDTH);
    painter->setPen(pen);
    painter->setBrush(QBrush(isSelected() ? Qt::black : m_color));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRect(rect());

    if (isSelected())
    {
        auto rc = rect();
        rc.adjust(MARGIN_WIDTH, MARGIN_WIDTH, -MARGIN_WIDTH, -MARGIN_WIDTH);

        pen.setColor(Qt::black);
        pen.setWidth(PEN_WIDTH);

        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(rc);
    }
    else
    {
        pen.setColor(Qt::white);
    }
    painter->setPen(pen);

    auto point = rect().center();
    point.setX(point.x() - 4.5 * m_netName.size());
    point.setY(point.y() + 6);
    painter->drawText(point, m_netName);


    pen.setColor(Qt::white);
    painter->setPen(pen);

    point.setX(rect().center().x() - ADJUSR_WIDTH * indent(m_in));
    point.setY(rect().y() + 16);
    painter->drawText(point, QString::number(m_in));

    point.setX(rect().center().x() - ADJUSR_WIDTH * indent(m_out));
    point.setY(rect().y() + rect().height() - ADJUSR_WIDTH);
    painter->drawText(point, QString::number(m_out));
}

void CGraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_edit)
    {
        auto pos = event->pos();
        if (event->button() == Qt::LeftButton && judgeIn(pos))
        {
            setEnabled(false);
            emit signalArrow(this, mapToScene(pos), nextOutChannel());
        }
    }
    else
    {
        emit signalShowNet(m_sn, m_netId, m_netName, m_aiParams);
    }

    QGraphicsRectItem::mousePressEvent(event);
}

void CGraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalMoved(m_edit, this);
    QGraphicsRectItem::mouseMoveEvent(event);
}

void CGraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void CGraphicsRectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_edit)
    {
        auto pos = event->pos();
        if (event->button() == Qt::LeftButton && !judgeIn(pos))
            emit signalEditNetName(m_netName, event->screenPos());
    }
    else
    {
        emit signalExtendNet(m_sn, m_netId, m_netName, m_aiParams);
    }

    //QGraphicsRectItem::mouseDoubleClickEvent(event);
}

void CGraphicsRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (m_edit && isSelected())
        emit signalContextMenu(event->screenPos());

    //QGraphicsRectItem::contextMenuEvent(event);
}
