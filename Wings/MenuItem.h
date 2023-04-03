#ifndef CMENUITEM_H
#define CMENUITEM_H

#include <QMenu>

class CMenuItem : public QMenu
{
    Q_OBJECT

public:
    explicit CMenuItem(QWidget *parent = nullptr);
    explicit CMenuItem(const QString &title, QWidget *parent = nullptr);
    ~CMenuItem() override;

    void EnableDeleteAction(bool enabled);
    void EnableAsInputAction(bool enabled);
    void EnableAsOutputAction(bool enabled);
    void EnableInputConfigAction(bool enabled);
    void EnableOutputConfigAction(bool enabled);

    void CheckedAsInputAction(bool checked);
    void CheckedAsOutputAction(bool checked);

signals:
    void asInputSelectedItemsSignal();
    void asOutputSelectedItemsSignal();
    void inputConfigSelectedItemsSignal();
    void outputConfigSelectedItemsSignal();
    void deleteSelectedItemsSignal();

private slots:
    void deleteSlot();
    void asInputSlot();
    void asOutputSlot();
    void inputConfigSlot();
    void outputConfigSlot();

private:
    void Init(QWidget *parent);

private:
    QAction*                        m_actDelete = {nullptr};
    QAction*                        m_actAsInput = {nullptr};
    QAction*                        m_actAsOutput = {nullptr};
    QAction*                        m_actInputConfig = {nullptr};
    QAction*                        m_actOutputConfig = {nullptr};
};

#endif // CMENUITEM_H
