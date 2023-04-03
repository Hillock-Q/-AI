#ifndef WGBTNCTRL_H
#define WGBTNCTRL_H

#include <QWidget>

namespace Ui {
class WgBtnCtrl;
}

class WgBtnCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit WgBtnCtrl(QString const &netId, QColor const &color, QWidget *parent = nullptr);
    ~WgBtnCtrl();

    void SetNetId(QString const &id);
    QString const& GetNetId() const;

    void SetColor(QColor const &color);
    QColor const& GetColor() const;

signals:
    void signalUseNet(QString const &netId);
    void signalAddNet(QString const &netId, QColor const &color);


private:
    Ui::WgBtnCtrl *ui;
};

#endif // WGBTNCTRL_H
