#ifndef CSYSTEMTRAY_H
#define CSYSTEMTRAY_H

#include <QObject>
#include <QSystemTrayIcon>

class QWidget;

class CSystemTray: public QObject
{
    Q_OBJECT

public:
    CSystemTray(QWidget *parent);
    QSystemTrayIcon* GetQSystemTray() const;

signals:
    void winChangeSignal(bool isMaximized);
    void appQuitSignal();

private slots:
    void on_miniAct();
    void on_maxiAct();
    void on_normAct();
    void on_quitAct();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private:
    QWidget*                            m_Parent = {nullptr};
    QSystemTrayIcon*                    m_pSystemTray = {nullptr};
};

#endif // CSYSTEMTRAY_H
