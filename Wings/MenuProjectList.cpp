#include "MenuProjectList.h"

CMenuProjectList::CMenuProjectList(QWidget *parent)
    : QMenu(parent)
{
    Init(parent);
}

CMenuProjectList::CMenuProjectList(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    Init(parent);
}

CMenuProjectList::~CMenuProjectList()
{
    disconnect();

    m_actDelProject->disconnect();
    delete m_actDelProject;
}

void CMenuProjectList::Init(QWidget *parent)
{
    m_actDelProject = new QAction(tr("Del Project"), parent);

    this->addAction(m_actDelProject);

    connect(m_actDelProject, SIGNAL(triggered(bool)), this, SLOT(delProjectSlot()));
}

void CMenuProjectList::SetProject(QString const &project)
{
    m_actDelProject->setText(tr("Del Project: ") + project);
}

void CMenuProjectList::EnableDelProjectAction(bool enabled)
{
    if (!enabled)
        m_actDelProject->setText(tr("Del Project"));

    m_actDelProject->setEnabled(enabled);
}

void CMenuProjectList::delProjectSlot()
{
    emit delProjectSignal();
}
