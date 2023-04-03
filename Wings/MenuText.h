#ifndef CMENUTEXT_H
#define CMENUTEXT_H

#include <QMenu>

class CMenuText : public QMenu
{
    Q_OBJECT

public:
    explicit CMenuText(QWidget *parent = nullptr);
    explicit CMenuText(const QString &title, QWidget *parent = nullptr);
    ~CMenuText() override;

    void EnableCopyAction(bool enabled);
    void EnableClearAction(bool enabled);
    void EnableSelectAllAction(bool enabled);

signals:
    void copySignal();
    void clearSignal();
    void selectAllSignal();

private slots:
    void copySlot();
    void clearSlot();
    void selectAllSlot();

private:
    void Init(QWidget *parent);

private:
    QAction*                        m_actCopy = {nullptr};
    QAction*                        m_actClear = {nullptr};
    QAction*                        m_actSelectAll = {nullptr};
};

#endif // CMENUTEXT_H
