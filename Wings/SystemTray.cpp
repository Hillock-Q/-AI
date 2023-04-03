#include "SystemTray.h"
#include <QApplication>
#include <QWidget>
#include <QAction>
#include <QMenu>

CSystemTray::CSystemTray(QWidget *parent)
    : m_Parent(parent), m_pSystemTray(new QSystemTrayIcon(m_Parent))
{
    auto miniAct = new QAction(tr("Minimized"), m_Parent);
    auto maxiAct = new QAction(tr("Maximized"), m_Parent);
    auto normAct = new QAction(tr("Normal"), m_Parent);
    auto quitAct = new QAction(tr("Exit"), m_Parent);
    auto pContextMenu = new QMenu(m_Parent);

    pContextMenu->addAction(miniAct);
    pContextMenu->addSeparator();
    pContextMenu->addAction(maxiAct);
    pContextMenu->addSeparator();
    pContextMenu->addAction(normAct);
    pContextMenu->addSeparator();
    pContextMenu->addAction(quitAct);

    m_pSystemTray->setContextMenu(pContextMenu);
    m_pSystemTray->setIcon(QIcon(":/images/logo.png"));
    m_pSystemTray->setToolTip(tr("Wings"));

    connect(miniAct, SIGNAL(triggered()), this, SLOT(on_miniAct()));
    connect(maxiAct, SIGNAL(triggered()), this, SLOT(on_maxiAct()));
    connect(normAct, SIGNAL(triggered()), this, SLOT(on_normAct()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(on_quitAct()));
    connect(m_pSystemTray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
}

QSystemTrayIcon* CSystemTray::GetQSystemTray() const
{
    return m_pSystemTray;
}

void CSystemTray::on_miniAct()
{
    m_Parent->showMinimized();
}

void CSystemTray::on_maxiAct()
{
    m_Parent->showMaximized();
    emit winChangeSignal(true);
}

void CSystemTray::on_normAct()
{
    m_Parent->showNormal();
    emit winChangeSignal(false);
}

void CSystemTray::on_quitAct()
{
    emit appQuitSignal();
    qApp->quit();
}

void CSystemTray::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        m_Parent->showMaximized();
        break;

    default:
        break;
    }
}
