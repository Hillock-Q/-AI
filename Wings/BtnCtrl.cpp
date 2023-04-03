#include "BtnCtrl.h"
#include <QEvent>
#include <QMouseEvent>

BtnCtrl::BtnCtrl(QString const &netId, QColor const &color, QWidget *parent)
    : QToolButton(parent)
{
    SetNetId(netId);
    SetColor(color);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setFont(QFont("Microsoft YaHei UI", 9));
    setIcon(QIcon(":/images/btn_rattle.png"));
    //setMouseTracking(true);
}

BtnCtrl::~BtnCtrl()
{

}

void BtnCtrl::SetNetId(QString const &id)
{
    m_netId = id;
    setObjectName(m_netId);
    setText(m_netId);
}

QString const& BtnCtrl::GetNetId() const
{
    return m_netId;
}

void BtnCtrl::SetColor(QColor const &color)
{
    m_color = color;

    setStyleSheet("color: rgb(35, 62, 95); background-color: rgb(221, 233, 246); border:2px solid rgb(" + QString::number(color.red()) + ", " + QString::number(color.green()) + ", " + QString::number(color.blue()) + "); border-radius:5px;");
}

QColor const& BtnCtrl::GetColor() const
{
    return m_color;
}

void BtnCtrl::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        setCursor(Qt::DragMoveCursor);

    emit signalUseNet(m_netId);

    QToolButton::mousePressEvent(e);
}

void BtnCtrl::mouseReleaseEvent(QMouseEvent *e)
{
    auto pos0 = mapToGlobal(rect().topLeft());
    auto pos1 = mapToGlobal(rect().bottomRight());
    auto pos = cursor().pos();
    if (pos.x() >= pos0.x() && pos.x() <= pos1.x() && pos.y() >= pos0.y() && pos.y() <= pos1.y())
    {
        setCursor(Qt::ArrowCursor);
    }

    QToolButton::mouseReleaseEvent(e);
}

bool BtnCtrl::event(QEvent *event)
{
    if (event->type() == QEvent::HoverLeave && Qt::DragMoveCursor == cursor().shape())
    {
        m_drag = true;
    }
    else if(m_drag && event->type() == QEvent::Paint && Qt::DragMoveCursor == cursor().shape())
    {
        m_drag = false;
        setCursor(Qt::ArrowCursor);
        emit signalAddNet(m_netId, m_color);
    }

    return QToolButton::event(event);
}
