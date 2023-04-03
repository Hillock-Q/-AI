#include "MenuItem.h"

CMenuItem::CMenuItem(QWidget *parent)
    : QMenu(parent)
{
    Init(parent);
}

CMenuItem::CMenuItem(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    Init(parent);
}

CMenuItem::~CMenuItem()
{
    disconnect();

    m_actAsInput->disconnect();
    delete m_actAsInput;

    m_actAsOutput->disconnect();
    delete m_actAsOutput;

    m_actInputConfig->disconnect();
    delete m_actInputConfig;

    m_actOutputConfig->disconnect();
    delete m_actOutputConfig;

    m_actDelete->disconnect();
    delete m_actDelete;
}

void CMenuItem::Init(QWidget *parent)
{
    m_actAsInput = new QAction(tr("AsInput"), parent);
    m_actAsOutput = new QAction(tr("AsOutput"), parent);

    m_actInputConfig = new QAction(tr("    InputConfig..."), parent);
    m_actOutputConfig = new QAction(tr("    OutputConfig..."), parent);

    m_actDelete = new QAction(tr("    Delete"), parent);

    m_actAsInput->setCheckable(true);
    m_actAsInput->setChecked(false);

    m_actAsOutput->setCheckable(true);
    m_actAsOutput->setChecked(false);

    this->addAction(m_actAsInput);
    this->addAction(m_actAsOutput);
    this->addSeparator();
    this->addAction(m_actInputConfig);
    this->addAction(m_actOutputConfig);
    this->addSeparator();
    this->addAction(m_actDelete);

    connect(m_actAsInput, SIGNAL(triggered(bool)), this, SLOT(asInputSlot()));
    connect(m_actAsOutput, SIGNAL(triggered(bool)), this, SLOT(asOutputSlot()));
    connect(m_actInputConfig, SIGNAL(triggered(bool)), this, SLOT(inputConfigSlot()));
    connect(m_actOutputConfig, SIGNAL(triggered(bool)), this, SLOT(outputConfigSlot()));
    connect(m_actDelete, SIGNAL(triggered(bool)), this, SLOT(deleteSlot()));
}

void CMenuItem::EnableDeleteAction(bool enabled)
{
    m_actDelete->setEnabled(enabled);
}

void CMenuItem::EnableAsInputAction(bool enabled)
{
    m_actAsInput->setEnabled(enabled);
}

void CMenuItem::EnableAsOutputAction(bool enabled)
{
    m_actAsOutput->setEnabled(enabled);
}

void CMenuItem::EnableInputConfigAction(bool enabled)
{
    m_actInputConfig->setEnabled(enabled);
}

void CMenuItem::EnableOutputConfigAction(bool enabled)
{
    m_actOutputConfig->setEnabled(enabled);
}

void CMenuItem::CheckedAsInputAction(bool checked)
{
    m_actAsInput->setChecked(checked);
}

void CMenuItem::CheckedAsOutputAction(bool checked)
{
    m_actAsOutput->setChecked(checked);
}

void CMenuItem::deleteSlot()
{
    emit deleteSelectedItemsSignal();
}

void CMenuItem::asInputSlot()
{
    emit asInputSelectedItemsSignal();
}

void CMenuItem::asOutputSlot()
{
    emit asOutputSelectedItemsSignal();
}

void CMenuItem::inputConfigSlot()
{
    emit inputConfigSelectedItemsSignal();
}

void CMenuItem::outputConfigSlot()
{
    emit outputConfigSelectedItemsSignal();
}
