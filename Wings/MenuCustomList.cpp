#include "MenuCustomList.h"

CMenuCustomList::CMenuCustomList(QWidget *parent)
    : QMenu(parent)
{
    Init(parent);
}

CMenuCustomList::CMenuCustomList(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    Init(parent);
}

CMenuCustomList::~CMenuCustomList()
{
    disconnect();

    m_actDelNet->disconnect();
    delete m_actDelNet;

    m_actDelCatalog->disconnect();
    delete m_actDelCatalog;
}

void CMenuCustomList::Init(QWidget *parent)
{
    m_actDelNet = new QAction(tr("Del Net"), parent);
    m_actDelCatalog = new QAction(tr("Del Catalog"), parent);

    this->addAction(m_actDelNet);
    this->addAction(m_actDelCatalog);

    connect(m_actDelNet, SIGNAL(triggered(bool)), this, SLOT(delNetSlot()));
    connect(m_actDelCatalog, SIGNAL(triggered(bool)), this, SLOT(delCatalogSlot()));
}

void CMenuCustomList::SetNetId(QString const &netId)
{
    m_actDelNet->setText(tr("Del Net: ") + netId);
}

void CMenuCustomList::SetCatalog(QString const &catalog)
{
    m_actDelCatalog->setText(tr("Del Catalog: ") + catalog);
}

void CMenuCustomList::EnableDelNetAction(bool enabled)
{
    if (!enabled)
        m_actDelNet->setText(tr("Del Net"));

    m_actDelNet->setEnabled(enabled);
}

void CMenuCustomList::EnableDelCatalogAction(bool enabled)
{
    if (!enabled)
        m_actDelCatalog->setText(tr("Del Catalog"));

    m_actDelCatalog->setEnabled(enabled);
}

void CMenuCustomList::delNetSlot()
{
    emit delNetSignal();
}

void CMenuCustomList::delCatalogSlot()
{
    emit delCatalogSignal();
}
