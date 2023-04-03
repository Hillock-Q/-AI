#ifndef CMENUPROJECTLIST_H
#define CMENUPROJECTLIST_H

#include <QMenu>

class CMenuProjectList : public QMenu
{
    Q_OBJECT

public:
    explicit CMenuProjectList(QWidget *parent = nullptr);
    explicit CMenuProjectList(const QString &title, QWidget *parent = nullptr);
    ~CMenuProjectList() override;

    void SetProject(QString const &project);

    void EnableDelProjectAction(bool enabled);

signals:
    void delProjectSignal();


private slots:
    void delProjectSlot();

private:
    void Init(QWidget *parent);

private:
    QAction*                        m_actDelProject = {nullptr};
};

#endif // CMENUPROJECTLIST_H
