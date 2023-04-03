#ifndef CMENUCUSTOMLIST_H
#define CMENUCUSTOMLIST_H

#include <QMenu>

class CMenuCustomList : public QMenu
{
    Q_OBJECT

public:
    explicit CMenuCustomList(QWidget *parent = nullptr);
    explicit CMenuCustomList(const QString &title, QWidget *parent = nullptr);
    ~CMenuCustomList() override;

    void SetNetId(QString const &netId);
    void SetCatalog(QString const &catalog);

    void EnableDelNetAction(bool enabled);
    void EnableDelCatalogAction(bool enabled);

signals:
    void delNetSignal();
    void delCatalogSignal();

private slots:
    void delNetSlot();
    void delCatalogSlot();

private:
    void Init(QWidget *parent);

private:
    QAction*                        m_actDelNet = {nullptr};
    QAction*                        m_actDelCatalog = {nullptr};
};

#endif // CMENUCUSTOMLIST_H
