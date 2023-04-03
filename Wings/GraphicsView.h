#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include <QGraphicsView>

class CGraphicsArrowItem;

class CGraphicsView: public QGraphicsView
{
    Q_OBJECT

public:
    CGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    CGraphicsView(QWidget *parent = nullptr);
    virtual ~CGraphicsView() override;

    qreal GetScaleFactor() const;

signals:
    void showInfoSignal(QString const &info);
    void signalEnable(bool enabled);
    void signalAddArrow(CGraphicsArrowItem *arrowItem, QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel, QGraphicsItem *endItem, QPointF const &endPos, int endChannel);
    void signalDeleteKey();
    void signalRestore();
    void signalResize();

public slots:
    void arrowSlot(QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel);


protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void addArrow(QPointF const &pos0, QPointF const &pos1);

private:
    bool                            m_isDraged = {false};
    QPoint                          m_pos0;
    QPointF                         m_center;

    bool                            m_isArrow = {false};
    int                             m_beginChannel;
    int                             m_endChannel;
    QPointF                         m_beginPos;
    QPointF                         m_endPos;
    QGraphicsItem*                  m_beginItem = {nullptr};
    QGraphicsItem*                  m_endItem = {nullptr};
    CGraphicsArrowItem*             m_arrowItem = {nullptr};
};

#endif // CGRAPHICSVIEW_H
