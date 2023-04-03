#ifndef CGRAPHICSSCENE_H
#define CGRAPHICSSCENE_H

#include <QGraphicsScene>

class CGraphicsScene: public QGraphicsScene
{
public:
    CGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);
    CGraphicsScene(const QRectF &sceneRect, QObject *parent = nullptr);
    CGraphicsScene(QObject *parent = nullptr);

    virtual ~CGraphicsScene() override;

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // CGRAPHICSSCENE_H
