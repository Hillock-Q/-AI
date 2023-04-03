#ifndef LISTVIEWMGR_H
#define LISTVIEWMGR_H

#include <QObject>

class QListWidget;
class QListWidgetItem;
class CMenuCustomList;
class CMenuProjectList;
class QListWidgetItem;

class CListViewMgr: public QObject
{
    Q_OBJECT

public:
    static CListViewMgr& ins();

    void Init(QListWidget *standard, QListWidget *custom, QListWidget *projects, QString const &menuQss);

    void LoadListView();

    void SetMode(bool bDesign);

    void AddCustomNet(QString const &catalogName, QString const &netId, QColor const &color);

    void AddProject(QString const &filePath);

    QString GetSelectedProjectFilePath() const;
    QString GetSelectedProjectDir(QString const &filePath) const;
    QString GetSelectedProjectName(QString const &filePath, bool suffix = true) const;

signals:
    void showInfoSignal(QString const &info);
    void signalUseNet(bool std, QString const &catalogName, QString const &netId);
    void signalAddNet(bool std, QString const &catalogName, QString const &netId, QColor const &color);
    void signalDelNets(QStringList const &delNetIds);
    void signalSelectedProject(QString const &filePath);

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void updateStandardListSlot(QString const &catalogName);
    void useStandardNetSlot(QString const &catalogName, QString const &netId);
    void addStandardNetSlot(QString const &catalogName, QString const &netId, QColor const &color);

    void updateCustomListSlot(QString const &catalogName);
    void useCustomNetSlot(QString const &catalogName, QString const &netId);
    void addCustomNetSlot(QString const &catalogName, QString const &netId, QColor const &color);

    void showCustomListWidgetMenuSlot(QPoint const &pos);
    void delNetSlot();
    void delCatalogSlot();
    void aboutToHideSlot();

    void showProjectsListWidgetMenuSlot(QPoint const &pos);
    void delProjectSlot();

    void listProjectsCurRowChangedSlot(int currentRow);

private:
    CListViewMgr();
    ~CListViewMgr();

    CListViewMgr(CListViewMgr &&rhs) = delete;
    CListViewMgr(CListViewMgr const &rhs) = delete;

    CListViewMgr& operator=(CListViewMgr &&rhs) = delete;
    CListViewMgr& operator=(CListViewMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CListViewMgr *pt = 0) : p(pt){}
        CListViewMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CListViewMgr *p;
    };

    void loadStandardCatalog(QString const &catalogName);
    void loadStandardListView();
    void loadCustomListView();
    void loadProjectsListView();

    QColor getColor(QJsonObject const &netJsonObj);


private:
    QString                             m_menuQss;
    CMenuCustomList*                    m_pMenuCustomList = {nullptr};
    CMenuProjectList*                   m_pMenuProjectList = {nullptr};

    QListWidget*                        m_listwgStandard = {nullptr};
    QListWidget*                        m_listwgCustom = {nullptr};
    QListWidget*                        m_listwgProjects = {nullptr};

    int                                 m_nRunTimerID = {0};
    QString                             m_useCustomNetId;
    QString                             m_useCustomCatalog;

    bool                                m_bDesign = {true};
    QListWidgetItem*                    m_projectListWidgetItem = {nullptr};
};

#endif // LISTVIEWMGR_H
