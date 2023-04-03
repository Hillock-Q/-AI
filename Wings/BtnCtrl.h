#ifndef BTNCTRL_H
#define BTNCTRL_H

#include <QToolButton>

class BtnCtrl : public QToolButton
{
    Q_OBJECT

public:
    BtnCtrl(QString const &netId, QColor const &color, QWidget *parent = nullptr);
    virtual ~BtnCtrl();

    void SetNetId(QString const &id);
    QString const& GetNetId() const;

    void SetColor(QColor const &color);
    QColor const& GetColor() const;

signals:
    void signalUseNet(QString const &netId);
    void signalAddNet(QString const &netId, QColor const &color);


protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual bool event(QEvent *event) override;

private:
    bool                            m_drag ={false};
    QString                         m_netId;
    QColor                          m_color;
};

#endif // BTNCTRL_H
