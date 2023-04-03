#include "ListViewMgr.h"
#include <QListWidget>
#include <QJsonObject>
#include <QTimerEvent>
#include "WgCtrlList.h"
#include "DirFileMgr.h"
#include "MenuCustomList.h"
#include "MenuProjectList.h"

constexpr int       ROW_HEIGHT  = 36;

CListViewMgr& CListViewMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CListViewMgr());
	return *kp.get();
}

CListViewMgr::CListViewMgr()
{

}

CListViewMgr::~CListViewMgr()
{
    killTimer(m_nRunTimerID);

    delete m_pMenuCustomList;
    delete m_pMenuProjectList;
}

void CListViewMgr::Init(QListWidget *standard, QListWidget *custom, QListWidget *projects, QString const &menuQss)
{
    m_listwgStandard = standard;

    m_menuQss = menuQss;

    m_pMenuCustomList = new CMenuCustomList();
    m_pMenuCustomList->setStyleSheet(m_menuQss);

    connect(m_pMenuCustomList, SIGNAL(delNetSignal()), this, SLOT(delNetSlot()));
    connect(m_pMenuCustomList, SIGNAL(delCatalogSignal()), this, SLOT(delCatalogSlot()));
    connect(m_pMenuCustomList, SIGNAL(aboutToHide()), this, SLOT(aboutToHideSlot()));

    m_listwgCustom = custom;
    m_listwgCustom->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listwgCustom, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showCustomListWidgetMenuSlot(const QPoint &)));


    m_pMenuProjectList = new CMenuProjectList();
    m_pMenuProjectList->setStyleSheet(m_menuQss);

    connect(m_pMenuProjectList, SIGNAL(delProjectSignal()), this, SLOT(delProjectSlot()));
    //connect(m_pMenuProjectList, SIGNAL(aboutToHide()), this, SLOT(aboutToHideSlot()));

    m_listwgProjects = projects;
    m_listwgProjects->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listwgProjects, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showProjectsListWidgetMenuSlot(const QPoint &)));
    connect(m_listwgProjects, SIGNAL(currentRowChanged(int)), this, SLOT(listProjectsCurRowChangedSlot(int)));
}

void CListViewMgr::LoadListView()
{
    loadStandardListView();
    loadCustomListView();
    loadProjectsListView();
}

void CListViewMgr::SetMode(bool bDesign)
{
    m_bDesign = bDesign;

    if (m_bDesign)
    {
        if (m_projectListWidgetItem)
        {
            delete m_listwgStandard->takeItem((m_listwgStandard->row(m_projectListWidgetItem)));
            m_projectListWidgetItem = nullptr;
        }
    }
    else
    {
        loadStandardCatalog("rununit");
    }
}

void CListViewMgr::loadStandardCatalog(QString const &catalogName)
{
    auto wgCtrlList = new WgCtrlList(catalogName, m_listwgStandard);

    auto netIds = CDirFileMgr::ins().GetStandardNetIds(catalogName);
    for (auto const &netId : netIds)
    {
        QJsonObject netJsonObj;
        CDirFileMgr::ins().LoadStandardNet(catalogName, netId, netJsonObj);
        wgCtrlList->AddNet(netId, getColor(netJsonObj));
    }

    wgCtrlList->show();

    connect(wgCtrlList, SIGNAL(signalUpdate(QString const&)), this, SLOT(updateStandardListSlot(QString const&)));
    connect(wgCtrlList, SIGNAL(signalUseNet(QString const&, QString const&)), this, SLOT(useStandardNetSlot(QString const&, QString const&)));
    connect(wgCtrlList, SIGNAL(signalAddNet(QString const&, QString const&, QColor const&)), this, SLOT(addStandardNetSlot(QString const&, QString const&, QColor const&)));


    auto item = new QListWidgetItem(m_listwgStandard);
    item->setSizeHint(QSize(m_listwgStandard->width(), wgCtrlList->height()));
    if (catalogName == "rununit")
    {
        m_projectListWidgetItem = item;
    }

    m_listwgStandard->addItem(item);
    m_listwgStandard->setItemWidget(item, wgCtrlList);
}

void CListViewMgr::loadStandardListView()
{
    auto catalogs = CDirFileMgr::ins().GetStandardCatalogs();
    for (auto const &catalog : catalogs)
    {
        if (m_bDesign && catalog == "rununit")
            continue;

        loadStandardCatalog(catalog);
    }
}

void CListViewMgr::loadCustomListView()
{
    auto catalogs = CDirFileMgr::ins().GetCustomCatalogs();
    for (auto const &catalog : catalogs)
    {
        auto wgCtrlList = new WgCtrlList(catalog, m_listwgCustom);

        auto netIds = CDirFileMgr::ins().GetCustomNetIds(catalog);
        for (auto const &netId : netIds)
        {
            QJsonObject netJsonObj;
            CDirFileMgr::ins().LoadCustomNet(catalog, netId, netJsonObj);
            wgCtrlList->AddNet(netId, getColor(netJsonObj));
        }

        wgCtrlList->show();

        connect(wgCtrlList, SIGNAL(signalUpdate(QString const&)), this, SLOT(updateCustomListSlot(QString const&)));
        connect(wgCtrlList, SIGNAL(signalUseNet(QString const&, QString const&)), this, SLOT(useCustomNetSlot(QString const&, QString const&)));
        connect(wgCtrlList, SIGNAL(signalAddNet(QString const&, QString const&, QColor const&)), this, SLOT(addCustomNetSlot(QString const&, QString const&, QColor const&)));

        auto item = new QListWidgetItem(m_listwgCustom);
        item->setSizeHint(QSize(m_listwgCustom->width(), wgCtrlList->height()));

        m_listwgCustom->addItem(item);
        m_listwgCustom->setItemWidget(item, wgCtrlList);
    }
}

QColor CListViewMgr::getColor(QJsonObject const &netJsonObj)
{
    if (!netJsonObj.contains("02_net") || !netJsonObj.value("02_net").isObject())
        return Qt::black;
    auto net = netJsonObj.value("02_net").toObject();

    if (!net.contains("01_express") || !net.value("01_express").isObject())
        return Qt::black;
    auto express = net.value("01_express").toObject();

    if (!express.contains("06_r") || !express.value("06_r").isDouble())
        return Qt::black;
    auto r = express.value("06_r").toInt();

    if (!express.contains("07_g") || !express.value("07_g").isDouble())
        return Qt::black;
    auto g = express.value("07_g").toInt();

    if (!express.contains("08_b") || !express.value("08_b").isDouble())
        return Qt::black;
    auto b = express.value("08_b").toInt();

    return QColor(r, g, b);
}

void CListViewMgr::AddCustomNet(QString const &catalogName, QString const &netId, QColor const &color)
{
    for (auto i = 0; i < m_listwgCustom->count(); i++)
    {
        auto wgCtrlList = dynamic_cast<WgCtrlList*>(m_listwgCustom->itemWidget(m_listwgCustom->item(i)));
        if (wgCtrlList == nullptr || wgCtrlList->GetCatalogName() != catalogName)
            continue;

        wgCtrlList->AddNet(netId, color);
        wgCtrlList->Update();

        return;
    }

    auto wgCtrlList = new WgCtrlList(catalogName, m_listwgCustom);

    wgCtrlList->AddNet(netId, color);
    wgCtrlList->show();

    connect(wgCtrlList, SIGNAL(signalUpdate(QString const&)), this, SLOT(updateCustomListSlot(QString const&)));
    connect(wgCtrlList, SIGNAL(signalUseNet(QString const&, QString const&)), this, SLOT(useCustomNetSlot(QString const&, QString const&)));
    connect(wgCtrlList, SIGNAL(signalAddNet(QString const&, QString const&, QColor const&)), this, SLOT(addCustomNetSlot(QString const&, QString const&, QColor const&)));

    auto item = new QListWidgetItem(m_listwgCustom);
    item->setSizeHint(QSize(m_listwgCustom->width(), wgCtrlList->height()));

    m_listwgCustom->addItem(item);
    m_listwgCustom->setItemWidget(item, wgCtrlList);
}

void CListViewMgr::loadProjectsListView()
{
    auto files = CDirFileMgr::ins().GetProjectsFilePath();
    for (auto const &file : files)
    {
        auto fileName = file.mid(file.lastIndexOf(PROJECT_DIR_NAME + "/") + (PROJECT_DIR_NAME + "/").size());
        auto item = new QListWidgetItem(fileName, m_listwgProjects);
        item->setData(Qt::ToolTipRole, file);
        item->setFont(m_listwgProjects->font());
        item->setSizeHint(QSize(m_listwgProjects->width(), ROW_HEIGHT));

        m_listwgProjects->addItem(item);
    }

    if (m_listwgProjects->count() > 0)
        m_listwgProjects->setCurrentRow(0);
}

void CListViewMgr::AddProject(QString const &filePath)
{
    auto fileName = filePath.mid(filePath.lastIndexOf(PROJECT_DIR_NAME + "/") + (PROJECT_DIR_NAME + "/").size());
    auto item = new QListWidgetItem(fileName, m_listwgProjects);
    item->setData(Qt::ToolTipRole, filePath);
    item->setFont(m_listwgProjects->font());
    item->setSizeHint(QSize(m_listwgProjects->width(), ROW_HEIGHT));

    m_listwgProjects->addItem(item);
    m_listwgProjects->setCurrentItem(item);
}

QString CListViewMgr::GetSelectedProjectFilePath() const
{
    if (m_listwgProjects->currentRow() != -1)
        return m_listwgProjects->currentItem()->data(Qt::ToolTipRole).toString();

    return QString();
}

QString CListViewMgr::GetSelectedProjectDir(QString const &filePath) const
{
    return filePath.mid(0, filePath.lastIndexOf(PROJECT_DIR_NAME + "/") + (PROJECT_DIR_NAME + "/").size());
}

QString CListViewMgr::GetSelectedProjectName(QString const &filePath, bool suffix) const
{
    auto fileName = filePath.mid(filePath.lastIndexOf(PROJECT_DIR_NAME + "/") + (PROJECT_DIR_NAME + "/").size());

    if (!suffix)
        fileName = fileName.mid(0, fileName.lastIndexOf("."));

    return fileName;
}

void CListViewMgr::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nRunTimerID)
    {
        killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;

        m_useCustomNetId.clear();
        m_useCustomCatalog.clear();
    }
}


void CListViewMgr::updateStandardListSlot(QString const &)
{
    m_useCustomNetId.clear();
    m_useCustomCatalog.clear();

    for (auto i = 0; i < m_listwgStandard->count(); i++)
    {
        auto item = m_listwgStandard->item(i);
        auto wgCtrlList = m_listwgStandard->itemWidget(item);
        //qDebug() << i << ": " << item->sizeHint().height() << ", " << wgCtrlList->height();
        //auto w = m_listwgStandard->verticalScrollBar()->width();
        item->setSizeHint(QSize(m_listwgStandard->width(), wgCtrlList->height()));
    }
}

void CListViewMgr::useStandardNetSlot(QString const &catalogName, QString const &netId)
{
    emit signalUseNet(true, catalogName, netId);
}

void CListViewMgr::addStandardNetSlot(QString const &catalogName, QString const &netId, QColor const &color)
{
    emit signalAddNet(true, catalogName, netId, color);
}

void CListViewMgr::updateCustomListSlot(QString const &catalogName)
{
    m_useCustomNetId.clear();
    m_useCustomCatalog = catalogName;

    for (auto i = 0; i < m_listwgCustom->count(); i++)
    {
        auto item = m_listwgCustom->item(i);
        auto wgCtrlList = m_listwgCustom->itemWidget(item);
        //qDebug() << i << ": " << item->sizeHint().height() << ", " << wgCtrlList->height();
        //auto w = m_listwgCustom->verticalScrollBar()->width();
        item->setSizeHint(QSize(m_listwgCustom->width() - 20, wgCtrlList->height()));
    }
}

void CListViewMgr::useCustomNetSlot(QString const &catalogName, QString const &netId)
{
    m_useCustomNetId = netId;
    m_useCustomCatalog = catalogName;

    emit signalUseNet(false, catalogName, netId);
}

void CListViewMgr::addCustomNetSlot(QString const &catalogName, QString const &netId, QColor const &color)
{
    emit signalAddNet(false, catalogName, netId, color);
}

void CListViewMgr::showCustomListWidgetMenuSlot(QPoint const &)
{
    if (m_useCustomNetId.isEmpty() && (m_useCustomCatalog.isEmpty() || m_useCustomCatalog == BASEBLOCK_SUBDIR_NAME || m_useCustomCatalog == BACKBONE_SUBDIR_NAME || m_useCustomCatalog == NET_SUBDIR_NAME))
        return;

    m_pMenuCustomList->SetNetId(m_useCustomNetId);
    m_pMenuCustomList->SetCatalog(m_useCustomCatalog);

    m_pMenuCustomList->EnableDelNetAction(!m_useCustomNetId.isEmpty());
    m_pMenuCustomList->EnableDelCatalogAction(!m_useCustomCatalog.isEmpty() && m_useCustomCatalog != BASEBLOCK_SUBDIR_NAME && m_useCustomCatalog != BACKBONE_SUBDIR_NAME && m_useCustomCatalog != NET_SUBDIR_NAME);

    auto newPos = QCursor::pos();
    newPos.setX(newPos.x() - 20);
    newPos.setY(newPos.y() - 20);
    m_pMenuCustomList->exec(newPos);
}

void CListViewMgr::delNetSlot()
{
    auto delNetId = m_useCustomNetId;
    auto delCatalog = m_useCustomCatalog;

    QStringList delNetIds;
    delNetIds << delNetId;
    emit signalDelNets(delNetIds);

    for (auto i = 0; i < m_listwgCustom->count(); i++)
    {
        auto wgCtrlList = dynamic_cast<WgCtrlList*>(m_listwgCustom->itemWidget(m_listwgCustom->item(i)));
        if (wgCtrlList == nullptr || wgCtrlList->GetCatalogName() != delCatalog)
            continue;

        wgCtrlList->DelNet(delNetId);
        wgCtrlList->Update();
        break;
    }

    if (CDirFileMgr::ins().DelCustomNet(delCatalog, delNetId))
    {
        emit showInfoSignal(tr("Del net ") + delNetId + "@" + delCatalog + tr(" ok."));
    }
    else
    {
        emit showInfoSignal(tr("Del net ") + delNetId + "@" + delCatalog + tr(" failed."));
    }

    m_useCustomNetId.clear();
    m_useCustomCatalog.clear();
}

void CListViewMgr::delCatalogSlot()
{
    auto delCatalog = m_useCustomCatalog;
    auto delNetIds = CDirFileMgr::ins().GetCustomNetIds(delCatalog);
    emit signalDelNets(delNetIds);

    for (auto i = 0; i < m_listwgCustom->count(); i++)
    {
        auto wgCtrlList = dynamic_cast<WgCtrlList*>(m_listwgCustom->itemWidget(m_listwgCustom->item(i)));
        if (wgCtrlList == nullptr || wgCtrlList->GetCatalogName() != delCatalog)
            continue;

        m_listwgCustom->removeItemWidget(m_listwgCustom->item(i));
        delete m_listwgCustom->takeItem(i);
        break;
    }

    if (CDirFileMgr::ins().DelCustomCatalog(delCatalog))
    {
        for (auto const &delNetId : delNetIds)
        {
            emit showInfoSignal(tr("Del net ") + delNetId + "@" + delCatalog + tr(" ok."));
        }

        emit showInfoSignal(tr("Del catalog ") + delCatalog + tr(" ok."));
    }
    else
    {
        emit showInfoSignal(tr("Del catalog ") + delCatalog + tr(" failed."));
    }

    m_useCustomNetId.clear();
    m_useCustomCatalog.clear();
}

void CListViewMgr::aboutToHideSlot()
{
    m_nRunTimerID = startTimer(100);
}

void CListViewMgr::showProjectsListWidgetMenuSlot(QPoint const &)
{
    auto filePath = GetSelectedProjectFilePath();
    if (filePath.isEmpty())
        return;

    auto fileName = GetSelectedProjectName(filePath);
    if (fileName == DEFAULT_DEMO_PROJECT_FILE_NAME)
        return;

    m_pMenuProjectList->SetProject(fileName);
    m_pMenuProjectList->EnableDelProjectAction(!fileName.isEmpty());

    auto newPos = QCursor::pos();
    newPos.setX(newPos.x() - 20);
    newPos.setY(newPos.y() - 20);
    m_pMenuProjectList->exec(newPos);
}

void CListViewMgr::delProjectSlot()
{
    auto filePath = GetSelectedProjectFilePath();
    if (filePath.isEmpty())
        return;

    auto fileName = GetSelectedProjectName(filePath);
    if (fileName == DEFAULT_DEMO_PROJECT_FILE_NAME)
        return;

    auto fileDir = CListViewMgr::ins().GetSelectedProjectDir(filePath);
    auto projectName = CListViewMgr::ins().GetSelectedProjectName(filePath, false);
    if (CDirFileMgr::ins().DelProject(projectName, fileDir))
        delete m_listwgProjects->takeItem(m_listwgProjects->currentRow());
}

void CListViewMgr::listProjectsCurRowChangedSlot(int currentRow)
{
    if (currentRow != -1)
    {
        auto filePath = m_listwgProjects->currentItem()->data(Qt::ToolTipRole).toString();
        emit signalSelectedProject(filePath);
    }
}
