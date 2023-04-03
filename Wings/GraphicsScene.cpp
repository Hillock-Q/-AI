#include "GraphicsScene.h"
#include <QPainter>

CGraphicsScene::CGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height, parent)
{

}

CGraphicsScene::CGraphicsScene(const QRectF &sceneRect, QObject *parent)
    : QGraphicsScene(sceneRect, parent)
{

}

CGraphicsScene::CGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{

}

CGraphicsScene::~CGraphicsScene()
{

}

void CGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    qreal left = rect.left();
    qreal right = rect.right();
    qreal top = rect.top();
    qreal bottom = rect.bottom();

    left = (left / 10) * 10;
    right = (right / 10) * 10;
    top = (top / 10) * 10;
    bottom = (bottom / 10) * 10;


    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0, 0, 0));
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    painter->drawLine(left, 0, right, 0);
    painter->drawLine(0, top, 0, bottom);

    pen.setWidth(3);
    pen.setColor(Qt::red);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    painter->drawPoint(0, 0);


    pen.setWidth(0);
    pen.setColor(QColor(0, 0, 0));
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    for(int y = 100; y >= top; y -= 100)
    {
        painter->drawLine(left, y, right, y);
    }

    for(int y = 100; y <= bottom; y += 100)
    {
        painter->drawLine(left, y, right, y);
    }

    for(int x = 100; x <= right; x += 100)
    {
        painter->drawLine(x, top, x, bottom);
    }

    for(int x = 100; x >= left; x -= 100)
    {
        painter->drawLine(x, top, x, bottom);
    }


    pen.setWidth(0);
    pen.setColor(QColor(60, 60, 60));
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    for(int y = 10; y >= top; y -= 10)
    {
        if (y % 100)
            painter->drawLine(left, y, right, y);
    }

    for(int y = 10; y <= bottom; y += 10)
    {
        if (y % 100)
            painter->drawLine(left, y, right, y);
    }

    for(int x = 10; x <= right; x += 10)
    {
        if (x % 100)
            painter->drawLine(x, top, x, bottom);
    }

    for(int x = 10; x >= left; x -= 10)
    {
        if (x % 100)
            painter->drawLine(x, top, x, bottom);
    }
}
