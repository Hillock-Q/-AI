#include "GraphicsView.h"
#include <QMouseEvent>
#include "GraphicsRectItem.h"
#include "GraphicsEllipseItem.h"
#include "GraphicsArrowItem.h"
#include <QDebug>

CGraphicsView::CGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setDragMode(QGraphicsView::RubberBandDrag);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

CGraphicsView::CGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::RubberBandDrag);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

CGraphicsView::~CGraphicsView()
{
    delete m_arrowItem;
}

qreal CGraphicsView::GetScaleFactor() const
{
    return this->matrix().m11();
}

void CGraphicsView::arrowSlot(QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel)
{
    m_isArrow = true;
    m_beginItem = beginItem;
    m_beginPos = beginPos;
    m_beginChannel = beginChannel;
}

void CGraphicsView::addArrow(QPointF const &pos0, QPointF const &pos1)
{
    if (m_arrowItem == nullptr)
    {
        m_arrowItem = new CGraphicsArrowItem(true, -1, pos0.x(), pos0.y(), pos1.x(), pos1.y(), QColor(18, 157, 221));
        scene()->addItem(m_arrowItem);
    }
    else
    {
        m_arrowItem->SetBeginPoint(pos0);
        m_arrowItem->SetEndPoint(pos1);
    }
}

void CGraphicsView::wheelEvent(QWheelEvent *event)
{
    static qreal scaleFactor = 1.0;

    if(event->delta() > 0 && scaleFactor >= 1000)
    {
        return;
    }
    else if(event->delta() < 0 && scaleFactor <= 0.01)
    {
        return;
    }
    else
    {
        scaleFactor = this->matrix().m11();
        auto wheelDeltaValue = event->delta();

        /*
        auto m11 = this->matrix().m11();
        auto m12 = this->matrix().m12();
        auto m21 = this->matrix().m21();
        auto m22 = this->matrix().m22();
        auto dx = this->matrix().dx();
        auto dy = this->matrix().dy();
        this->setMatrix(QMatrix(m11, m12, m21, m22, dx, dy));
        */

        // wheel up, enlarge;
        if (wheelDeltaValue > 0)
        {
            this->scale(1.1, 1.1);
        }
        // wheel down, narrow;
        else
        {
            this->scale(1.0 / 1.1, 1.0 / 1.1);
        }

        update();
    }
}

void CGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
        emit signalDeleteKey();
}

void CGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (scene() == nullptr)
        return;

    m_isArrow = false;

    if (event->modifiers() == Qt::SHIFT)
    {
        emit signalEnable(false);
        setDragMode(QGraphicsView::ScrollHandDrag);

        m_isDraged = true;
        m_pos0 = event->pos();
        m_center = mapToScene(m_pos0) - m_pos0 + QPointF(width() / 2, height() / 2);
    }
    else
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }

    QGraphicsView::mousePressEvent(event);
}

void CGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::SHIFT && m_isDraged)
    {
        auto dpos = event->pos() - m_pos0;
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        centerOn(m_center - dpos);
    }
    else if (m_isArrow)
    {
        addArrow(m_beginPos, mapToScene(event->pos()));
        update();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isDraged)
    {
        m_isDraged = false;
        emit signalEnable(true);
    }
    else if (m_isArrow)
    {
        m_isArrow = false;
        m_endChannel = 0;

        bool delArrow = true;
        auto endPos = mapToScene(event->pos());
        auto items = scene()->items(endPos);
        for (auto item : items)
        {
            if (item == m_arrowItem || item == m_beginItem || item == nullptr)
                continue;

            if (item->type() == CGraphicsRectItem::Type)
            {
                auto endRect = dynamic_cast<CGraphicsRectItem*>(item);
                if (endRect && endRect->inArrowArea(endPos) && (m_endChannel = endRect->nextInChannel()) >= 0)
                {
                    m_endItem = endRect;
                    m_endPos = endPos;
                    addArrow(m_beginPos, m_endPos);
                    delArrow = false;
                }
            }
            else if (item->type() == CGraphicsEllipseItem::Type)
            {
                auto endEllipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                if (endEllipse && endEllipse->inArrowArea(endPos) && (m_endChannel = endEllipse->nextInChannel()) >= 0)
                {
                    m_endItem = endEllipse;
                    m_endPos = endPos;
                    addArrow(m_beginPos, m_endPos);
                    delArrow = false;
                }
            }
        }

        if (delArrow)
        {
            if (m_endChannel < 0)
                emit showInfoSignal(tr("Arrow lead to exceed in-degree!"));
            else
                emit showInfoSignal(tr("Cursor is not in arrow area!"));

            scene()->removeItem(m_arrowItem);
            delete m_arrowItem;
            update();
        }
        else
        {
            emit signalAddArrow(m_arrowItem, m_beginItem, m_beginPos, m_beginChannel, m_endItem, m_endPos, m_endChannel);
        }
        m_arrowItem = nullptr;

        m_beginItem->setEnabled(true);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void CGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    auto items = scene()->selectedItems();
    if (items.size() == 0)
    {
        QMatrix q;
        q.setMatrix(1, matrix().m12(), matrix().m21(), 1, matrix().dx(), matrix().dy());
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        setMatrix(q, false);

        emit signalRestore();
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}

void CGraphicsView::resizeEvent(QResizeEvent *event)
{
    emit signalResize();
    QGraphicsView::resizeEvent(event);
}
