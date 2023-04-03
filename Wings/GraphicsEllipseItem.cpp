#include "GraphicsEllipseItem.h"
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

CGraphicsEllipseItem::CGraphicsEllipseItem(bool edit, int sn, qreal x, qreal y, qreal width, qreal height, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x, y, width, height, parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
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

CGraphicsEllipseItem::CGraphicsEllipseItem(bool edit, int sn, const QRectF &rect, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsEllipseItem(rect, parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
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

CGraphicsEllipseItem::CGraphicsEllipseItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), CItemInfo(sn), m_edit(edit), m_netId(netId), m_netName(netName), m_color(color)
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

CGraphicsEllipseItem::~CGraphicsEllipseItem()
{
    disconnect();
}

int CGraphicsEllipseItem::type() const
{
    return Type;
}

void CGraphicsEllipseItem::SetNetId(QString const &netId)
{
    m_netId = netId;
}

QString const& CGraphicsEllipseItem::GetNetId() const
{
    return m_netId;
}

void CGraphicsEllipseItem::SetNetName(QString const &netName)
{
    m_netName = netName;
    adjustWidth();
}

QString const& CGraphicsEllipseItem::GetNetName() const
{
    return m_netName;
}

void CGraphicsEllipseItem::SetColor(QColor const &color)
{
    m_color = color;

    auto pen = QPen(m_color);
    pen.setWidth(PEN_WIDTH);
    setPen(pen);
}

QColor const& CGraphicsEllipseItem::GetColor() const
{
    return m_color;
}

void CGraphicsEllipseItem::SetAiParams(QJsonObject const &aiParams)
{
    m_aiParams = aiParams;
}

QJsonObject CGraphicsEllipseItem::GetAiParams() const
{
    return m_aiParams;
}

QPointF CGraphicsEllipseItem::GetInputArrowBeginPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), -rect().height()));
}

QPointF CGraphicsEllipseItem::GetInputArrowEndPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), 0));
}

QPointF CGraphicsEllipseItem::GetOutputArrowBeginPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), rect().height()));
}

QPointF CGraphicsEllipseItem::GetOutputArrowEndPos() const
{
    auto point = rect().center();
    return mapToScene(QPointF(point.x(), rect().height() * 2));
}

vector<int> CGraphicsEllipseItem::GetInputInChannels() const
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

ChannelPairs_t CGraphicsEllipseItem::GetInputChannelPairs()
{
    for (auto const &point : m_points)
        if (point.role == e_input)
            return point.channels;

    return ChannelPairs_t();
}

ChannelPairs_t CGraphicsEllipseItem::GetOutputChannelPairs()
{
    for (auto const &point : m_points)
        if (point.role == e_output)
            return point.channels;

    return ChannelPairs_t();
}

bool CGraphicsEllipseItem::inArrowArea(QPointF const &pos) const
{
    return judgeIn(mapFromScene(pos));
}

bool CGraphicsEllipseItem::judgeIn(QPointF const &pos) const
{
    auto center = this->rect().center();
    auto r = this->rect().width() / 2;
    auto dx = center.x() - pos.x();
    auto dy = center.y() - pos.y();

    if (dx * dx + dy * dy >= (r - MARGIN_WIDTH) * (r - MARGIN_WIDTH))
    {
        return true;
    }

    return false;
}

void CGraphicsEllipseItem::adjustWidth()
{
    return;
    if ((int)m_netName.size() > MAX_STRLENGTH)
    {
        auto dw = ADJUSR_WIDTH * ((int)m_netName.size() - MAX_STRLENGTH);
        auto rc = QRectF(0, 0, ITEM_WIDTH, ITEM_HEIGHT);
        rc.adjust(-dw, 0, +dw, 0);
        setRect(rc);
    }
}

int CGraphicsEllipseItem::indent(int cnt) const
{
    int loop = 0;
    while(cnt /= 10)
        loop++;

    return loop + 1;
}

int CGraphicsEllipseItem::nextInChannel() const
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

int CGraphicsEllipseItem::nextOutChannel() const
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

int CGraphicsEllipseItem::inChannels() const
{
    int cnt = 0;
    for (auto const &point : m_points)
        if (point.role == e_input || point.role == e_dst)
            cnt++;

    return cnt;
}

void CGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto pen = QPen(isSelected() ? Qt::black : m_color);
    pen.setWidth(PEN_WIDTH);
    painter->setPen(pen);
    painter->setBrush(QBrush(isSelected() ? Qt::black : m_color));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(rect());

    if (isSelected())
    {
        auto rc = rect();
        rc.adjust(MARGIN_WIDTH, MARGIN_WIDTH, -MARGIN_WIDTH, -MARGIN_WIDTH);

        pen.setColor(Qt::black);
        pen.setWidth(PEN_WIDTH);

        painter->setBrush(QBrush(Qt::white));
        painter->drawEllipse(rc);
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

void CGraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

    QGraphicsEllipseItem::mousePressEvent(event);
}

void CGraphicsEllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalMoved(m_edit, this);
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void CGraphicsEllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void CGraphicsEllipseItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
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

    //QGraphicsEllipseItem::mouseDoubleClickEvent(event);
}

void CGraphicsEllipseItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (m_edit && isSelected())
        emit signalContextMenu(event->screenPos());

    //QGraphicsEllipseItem::contextMenuEvent(event);
}
