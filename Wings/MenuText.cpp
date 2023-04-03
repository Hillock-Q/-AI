#include "MenuText.h"

CMenuText::CMenuText(QWidget *parent)
    : QMenu(parent)
{
    Init(parent);
}

CMenuText::CMenuText(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    Init(parent);
}

CMenuText::~CMenuText()
{
    disconnect();

    m_actCopy->disconnect();
    delete m_actCopy;

    m_actClear->disconnect();
    delete m_actClear;

    m_actSelectAll->disconnect();
    delete m_actSelectAll;
}

void CMenuText::Init(QWidget *parent)
{
    m_actCopy = new QAction(tr("Copy"), parent);
    m_actClear = new QAction(tr("Clear"), parent);
    m_actSelectAll = new QAction(tr("Select All"), parent);

    this->addAction(m_actCopy);
    this->addAction(m_actClear);
    this->addAction(m_actSelectAll);

    connect(m_actCopy, SIGNAL(triggered(bool)), this, SLOT(copySlot()));
    connect(m_actClear, SIGNAL(triggered(bool)), this, SLOT(clearSlot()));
    connect(m_actSelectAll, SIGNAL(triggered(bool)), this, SLOT(selectAllSlot()));
}

void CMenuText::EnableCopyAction(bool enabled)
{
    m_actCopy->setEnabled(enabled);
}

void CMenuText::EnableClearAction(bool enabled)
{
    m_actClear->setEnabled(enabled);
}

void CMenuText::EnableSelectAllAction(bool enabled)
{
    m_actSelectAll->setEnabled(enabled);
}

void CMenuText::copySlot()
{
    emit copySignal();
}

void CMenuText::clearSlot()
{
    emit clearSignal();
}

void CMenuText::selectAllSlot()
{
    emit selectAllSignal();
}
