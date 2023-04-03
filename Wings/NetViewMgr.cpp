#include "NetViewMgr.h"
#include <stack>
#include <deque>
#include <QJsonArray>
#include <corecrt_math_defines.h>
#include <QDebug>
#include "GraphicsView.h"
#include "GraphicsRectItem.h"
#include "GraphicsEllipseItem.h"
#include "GraphicsArrowItem.h"
#include "MenuItem.h"
#include "DirFileMgr.h"

using std::stack;
using std::deque;

constexpr int ITEM_WIDTH = 100;
constexpr int ITEM_HEIGHT = 62;

CNetViewMgr& CNetViewMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CNetViewMgr());
	return *kp.get();
}

CNetViewMgr::CNetViewMgr()
{
    initGraph();
    initNetInfoMap();
}

CNetViewMgr::~CNetViewMgr()
{
    delete m_pMenuItem;
}

void CNetViewMgr::Init(CGraphicsView *editView, CGraphicsView *showView, QString const &menuQss, QString const &version)
{
    m_verson = version;
    m_menuQss = menuQss;

    m_pMenuItem = new CMenuItem();
    m_pMenuItem->setStyleSheet(m_menuQss);

    connect(m_pMenuItem, SIGNAL(asInputSelectedItemsSignal()), this, SLOT(asInputSelectedItemSlot()));
    connect(m_pMenuItem, SIGNAL(asOutputSelectedItemsSignal()), this, SLOT(asOutputSelectedItemSlot()));
    connect(m_pMenuItem, SIGNAL(inputConfigSelectedItemsSignal()), this, SLOT(inputConfigSelectedItemsSlot()));
    connect(m_pMenuItem, SIGNAL(outputConfigSelectedItemsSignal()), this, SLOT(outputConfigSelectedItemsSlot()));
    connect(m_pMenuItem, SIGNAL(deleteSelectedItemsSignal()), this, SLOT(deleteSelectedItemsSlot()));

    m_editView = editView;
    m_editView->setRenderHint(QPainter::Antialiasing);
    connect(m_editView, SIGNAL(signalEnable(bool)), this, SLOT(enableSlot(bool)));
    connect(m_editView, SIGNAL(signalAddArrow(CGraphicsArrowItem*, QGraphicsItem*, QPointF const&, int, QGraphicsItem*, QPointF const&, int)), this, SLOT(addArrowSlot(CGraphicsArrowItem*, QGraphicsItem*, QPointF const&, int, QGraphicsItem*, QPointF const&, int)));
    connect(m_editView, SIGNAL(signalDeleteKey()), this, SLOT(deleteKeySlot()));
    connect(m_editView, SIGNAL(signalRestore()), this, SLOT(restoreSlot()));

    connect(m_editView->scene(), SIGNAL(selectionChanged()), this, SLOT(editSceneSelectionChangedSlot()));
    connect(m_editView->scene(), SIGNAL(focusItemChanged(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)), this, SLOT(editSceneFocusItemChangedSlot(QGraphicsItem*, QGraphicsItem*, Qt::FocusReason)));

    m_showView = showView;
    m_showView->setRenderHint(QPainter::Antialiasing);
    connect(m_showView->scene(), SIGNAL(selectionChanged()), this, SLOT(showSceneSelectionChangedSlot()));
}

void CNetViewMgr::AddNet(QString const &netId, QColor const &color, QPointF const &pos)
{
    m_pNetInfo = getNetInfo(netId);
    if (m_pNetInfo == nullptr)
        return;

    auto item = genItem(netId, color);
    addItem(true, item, pos);

    emit netChangedSignal();
}

QJsonObject CNetViewMgr::GetNet()
{
    NetInfo info;

    info.version = m_verson;

    info.net = "temp net";
    info.outline = info.net;
    info.specify = info.net;

    info.std = false;
    info.atomic = false;

    info.shape = "rect";
    info.r = 0;
    info.g = 0;
    info.b = 0;

    info.in = m_outChannelCnt;
    info.out = m_inChannelCnt;

    QJsonObject jsonExpressParams;
    jsonExpressParams["01_cycle"] = 1;
    jsonExpressParams["02_reqGrad"] = 1;
    info.expressParams = jsonExpressParams;

    return genJsonObj(info);
}

void CNetViewMgr::SetNet(QJsonObject const &netJsonObj)
{
    showToView(netJsonObj);
}

bool CNetViewMgr::inUse(QString const &name) const
{
    auto itor = m_netMap.find(name);
    return itor != m_netMap.end();
}

int CNetViewMgr::GetInputChannelCnt() const
{
    return m_outChannelCnt;
}

int CNetViewMgr::GetOutputChannelCnt() const
{
    return m_inChannelCnt;
}

set<QString> CNetViewMgr::GetMissingNetIds() const
{
    set<QString> missingNetIds;

    for (auto const &net : m_netInuse)
    {
        auto itor = m_netMap.find(net);
        if (itor == m_netMap.end())
            missingNetIds.insert(net);
    }

    return missingNetIds;
}

vector<QString> CNetViewMgr::GetMatchNetNames (QString const &netId) const
{
    vector<QString> matchNetNames;

    for (auto &item : m_items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr || rect->GetNetId() != netId)
                continue;

            matchNetNames.push_back(rect->GetNetName());
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr || ellipse->GetNetId() != netId)
                continue;

            matchNetNames.push_back(ellipse->GetNetName());
        }
    }

    return matchNetNames;
}

vector<QString> CNetViewMgr::GetNullInputItems() const
{
    vector<QString> items;

    for (auto &item : m_items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr || rect->GetIn() == 0 || rect->inChannels() != 0)
                continue;

            items.push_back(rect->GetNetName());
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr || ellipse->GetIn() == 0 || ellipse->inChannels() != 0)
                continue;

            items.push_back(ellipse->GetNetName());
        }
    }

    return items;
}

vector<QString> CNetViewMgr::GetNeedFullInputItems() const
{
    vector<QString> items;

    for (auto &item : m_items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr || rect->inChannels() == rect->GetIn() || rect->GetNetId() == "cat" || rect->GetNetId() == "train2d")
                continue;

            items.push_back(rect->GetNetName());
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr || ellipse->inChannels() == ellipse->GetIn() || ellipse->GetNetId() == "cat" || ellipse->GetNetId() == "train2d")
                continue;

            items.push_back(ellipse->GetNetName());
        }
    }

    return items;
}

bool CNetViewMgr::loss(int owner, Points_t const &points) const
{
    bool hasUnet = false;
    bool hasDataloader = false;

    for (auto const &point : points)
    {
        if (point.owner == owner && point.role == e_dst)
        {
            auto item = m_itemPointsMap[point.peer].item;
            if (item->type() == CGraphicsRectItem::Type)
            {
                auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                if (rect == nullptr)
                    continue;

                if (rect->GetNetId() == "dataloader2d")
                {
                    hasDataloader = true;
                }
                else
                {
                    // custom net
                    auto pInfo = getNetInfo(rect->GetNetId());
                    if (pInfo && !pInfo->atomic)
                        hasUnet = true;
                }
            }
            else if (item->type() == CGraphicsEllipseItem::Type)
            {
                auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                if (ellipse == nullptr)
                    continue;

                if (ellipse->GetNetId() == "dataloader2d")
                {
                    hasDataloader = true;
                }
                else
                {
                    // custom net
                    auto pInfo = getNetInfo(ellipse->GetNetId());
                    if (pInfo && !pInfo->atomic)
                        hasUnet = true;
                }
            }
        }
    }

    return hasUnet && hasDataloader;
}

bool CNetViewMgr::inflow(int owner, Points_t const &points, QString const &dstNetId) const
{
    for (auto const &point : points)
    {
        if (point.owner == owner && point.role == e_src)
        {
            auto item = m_itemPointsMap[point.peer].item;
            if (item->type() == CGraphicsRectItem::Type)
            {
                auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                if (rect == nullptr)
                    continue;

                if (rect->GetNetId() == dstNetId)
                    return true;

                // custom net
                if (dstNetId.isEmpty())
                {
                    auto pInfo = getNetInfo(rect->GetNetId());
                    if (pInfo && !pInfo->atomic)
                        return true;
                }
            }
            else if (item->type() == CGraphicsEllipseItem::Type)
            {
                auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                if (ellipse == nullptr)
                    continue;

                if (ellipse->GetNetId() == dstNetId)
                    return true;

                // custom net
                if (dstNetId.isEmpty())
                {
                    auto pInfo = getNetInfo(ellipse->GetNetId());
                    if (pInfo && !pInfo->atomic)
                        return true;
                }
            }
        }
    }

    return false;
}

void CNetViewMgr::updateProjectInfo(int sn, QString const &netId, QString const &netName, Points_t const &points, ProjectInfo &info) const
{
    if (netId == "dataloader2d")
    {
        info.dataloaderCnt++;

        // custom net
        if (inflow(sn, points, ""))
            info.dataloader2unet++;
    }
    else if (netId == "loss")
    {
        info.lossCnt++;

        if (!loss(sn, points))
            info.lossInputErrs.push_back(netName);

        if (!inflow(sn, points, "train2d"))
            info.lossOutputErrs.push_back(netName);
    }
    else if (netId == "optadadelta" || netId == "optadagrad" || netId == "optadam" || netId == "optrmsprop" || netId == "optsgd")
    {
        info.optCnt++;

        if (!inflow(sn, points, "train2d"))
            info.optOutputErrs.push_back(netName);
    }
    else if (netId == "train2d")
    {
        info.trainCnt++;
    }
    else if (netId == "test2d")
    {
        info.testCnt++;
    }
    else
    {
        auto pInfo = getNetInfo(netId);
        if (pInfo && !pInfo->atomic)
            info.unetCnt++;
    }
}

ProjectInfo CNetViewMgr::GetProjectInfo() const
{
    ProjectInfo info;

    for (auto &item : m_items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr)
                continue;

            updateProjectInfo(rect->GetSn(), rect->GetNetId(), rect->GetNetName(), rect->GetPoints(), info);
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr)
                continue;

            updateProjectInfo(ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName(), ellipse->GetPoints(), info);
        }
    }

    return info;
}

QJsonObject CNetViewMgr::GenNetCode(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString const &shape, QColor const &color, QString &paramCode, QString &netCode)
{
    auto net = genNet(catalog, netId, outline, specify, shape, color);

    GenCode(catalog, netId, outline, specify, paramCode, netCode);

    return net;
}

QString const& CNetViewMgr::getItemNetId(QGraphicsItem const *item) const
{
    static QString null;

    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem const*>(item);
        if (rect != nullptr)
            return rect->GetNetId();
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem const*>(item);
        if (ellipse != nullptr)
            return ellipse->GetNetId();
    }

    return null;
}

CItemInfo const* CNetViewMgr::getItemInfo(QGraphicsItem const *item) const
{
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem const*>(item);
        if (rect != nullptr)
            return (CItemInfo const*)rect;
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem const*>(item);
        if (ellipse != nullptr)
            return (CItemInfo const*)ellipse;
    }

    return nullptr;
}

CItemInfo const* CNetViewMgr::getItemInfo(QGraphicsItem const *item, int owner, int peer, int arrow, int &channel) const
{
    auto pItemInfo = getItemInfo(item);
    if (pItemInfo != nullptr && pItemInfo->GetSn() == owner)
    {
        auto const &points = pItemInfo->GetPoints();
        for (auto const &point : points)
        {
            if (point.role == e_src && point.owner == owner && point.peer == peer && point.arrow == arrow)
            {
                channel = point.channel;
                return pItemInfo;
            }
        }
    }

    return nullptr;
}

CItemInfo const* CNetViewMgr::getItemInfo(vector<QGraphicsItem*> items, int owner, int peer, int arrow, int &channel) const
{
    for (auto item : items)
    {
        auto pItemInfo = getItemInfo(item, owner, peer, arrow, channel);
        if (pItemInfo != nullptr)
            return pItemInfo;
    }

    return nullptr;
}

vector<QGraphicsItem*> CNetViewMgr::getPreposeItems(int sn) const
{
    vector<QGraphicsItem*> items;

    auto toIndex = getIndex(m_itemIndexs, sn);
    for (auto index = 0; index < m_graph.vertexNum; index++)
    {
        if (index == toIndex)
            continue;

        if (m_graph.arc[index][toIndex] > 0)
        {
            auto preposeSn = m_itemIndexs[index];
            auto itor = m_itemPointsMap.find(preposeSn);
            if (itor != m_itemPointsMap.end())
            {
                items.push_back(itor->second.item);
            }
        }
    }

    return items;
}

ForwardInfo CNetViewMgr::genForwardInfo(int index, QString const &netId, CItemInfo const &itemInfo, vector<QGraphicsItem*> const &preposeItems) const
{
    ForwardInfo info;

    info.netId = netId;
    info.sn = itemInfo.GetSn();
    info.index = index;
    info.inCnt = itemInfo.GetIn();
    info.inputCnt = m_outChannelCnt;

    auto const &points = itemInfo.GetPoints();
    for (auto const &point : points)
    {
        if (point.role == e_dst)
        {
            info.inChannels.push_back(point.channel);

            int channel = -1;
            auto pItemInfo = getItemInfo(preposeItems, point.peer, point.owner, point.arrow, channel);
            if (pItemInfo != nullptr)
            {
                info.outCnts.push_back(pItemInfo->GetOut());
                info.outChannels.push_back(channel);
                info.outIndexs.push_back(tuoPuOrder(getIndex(m_itemIndexs, pItemInfo->GetSn())));
            }
        }
        else if (point.role == e_input)
        {
            info.inputs = point.channels;
        }
    }

    return info;
}

FinishInfo CNetViewMgr::genFinishInfo(int outputCnt, vector<QGraphicsItem*> const &items) const
{
    FinishInfo info;

    info.outputCnt = outputCnt;

    for (auto item : items)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto const &points = pItemInfo->GetPoints();
        for (auto const &point : points)
        {
            if (point.role == e_output)
            {
                for (auto &outIn : point.channels)
                {
                    info.outputs.push_back(outIn);
                    info.outCnts.push_back(pItemInfo->GetOut());
                    info.outIndexs.push_back(tuoPuOrder(getIndex(m_itemIndexs, pItemInfo->GetSn())));
                }
            }
        }
    }

    return info;
}

void CNetViewMgr::getNetDirSlot(QString const &itemNetId, QString &itemDir)
{
    auto netInfo = getNetInfo(itemNetId);
    if (netInfo != nullptr)
    {
        if (netInfo->std)
        {
            itemDir = STANDARD_DIR_NAME + "." + netInfo->catalog;
        }
        else
        {
            itemDir = CUSTOM_DIR_NAME + "." + netInfo->catalog;
        }
    }
    else
    {
        emit showInfoSignal(tr("Error: net ") + itemNetId + tr(" is missing!"));
    }
}

void CNetViewMgr::getLossInfo(Points_t const &points, LossInfo &info, set<std::pair<QString, int>> &unets) const
{
    for (auto const &point : points)
    {
        if (point.role == e_dst)
        {
            int channel = -1;
            auto item = m_itemPointsMap[point.peer].item;
            auto pItemInfo = getItemInfo(item, point.peer, point.owner, point.arrow, channel);
            if (pItemInfo != nullptr)
            {
                if (item->type() == CGraphicsRectItem::Type)
                {
                    auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                    if (rect == nullptr)
                        continue;

                    if (rect->GetNetId() == "dataloader2d")
                    {
                        info.dataInputs.push_back(channel);
                    }
                    // custom net
                    else
                    {
                        auto pInfo = getNetInfo(rect->GetNetId());
                        if (pInfo && !pInfo->atomic)
                        {
                            info.netInputs.push_back(channel);
                            unets.insert(std::make_pair(rect->GetNetId(), rect->GetSn()));
                        }
                    }
                }
                else if (item->type() == CGraphicsEllipseItem::Type)
                {
                    auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                    if (ellipse == nullptr)
                        continue;

                    if (ellipse->GetNetId() == "dataloader2d")
                    {
                        info.dataInputs.push_back(channel);
                    }
                    // custom net
                    else
                    {
                        auto pInfo = getNetInfo(ellipse->GetNetId());
                        if (pInfo && !pInfo->atomic)
                        {
                            info.netInputs.push_back(channel);
                            unets.insert(std::make_pair(ellipse->GetNetId(), ellipse->GetSn()));
                        }
                    }
                }
            }
        } // e_dst
    } // for
}

QString CNetViewMgr::GenProjectCode(QString const &projectName, bool isTest)
{
    QString code;
    vector<QGraphicsItem*> dataloaderItems, unetItems, lossItems, optItems, trainItems;
    set<std::pair<QString, int>> dataloaderItemsInfo, unetItemsInfo, optItemsInfo, trainItemsInfo;
    emit showInfoSignal(tr("Start compiling..."));

    CCompileMgr::ins().GenProjectCodeInit(projectName, isTest);

    tuoPuOrder(m_graph, m_tuoPuOrders);
    for (auto index = 0; index < (int)m_tuoPuOrders.size(); index++)
    {
        if (m_tuoPuOrders[index] < 0 || m_tuoPuOrders[index] >= (int)m_items.size())
            continue;

        auto item = m_items[m_tuoPuOrders[index]];
        if (item == nullptr)
            continue;

        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        QString itemDir;
        getNetDirSlot(itemNetId, itemDir);

        CCompileMgr::ins().GenProjectImportCode(itemDir, itemNetId, false);
        CCompileMgr::ins().GenProjectRunInitParamCode(itemNetId, pItemInfo->GetSn(), getNetAiParams(item));

        if (itemNetId == "dataloader2d")
        {
            dataloaderItems.push_back(item);
            dataloaderItemsInfo.insert(std::make_pair(itemNetId, pItemInfo->GetSn()));
        }
        else if (itemNetId == "loss")
        {
            lossItems.push_back(item);
        }
        else if (itemNetId == "optadadelta" || itemNetId == "optadagrad" || itemNetId == "optadam" || itemNetId == "optrmsprop" || itemNetId == "optsgd")
        {
            optItems.push_back(item);
            optItemsInfo.insert(std::make_pair(itemNetId, pItemInfo->GetSn()));
        }
        else if (itemNetId == "train2d")
        {
            trainItems.push_back(item);
            trainItemsInfo.insert(std::make_pair(itemNetId, pItemInfo->GetSn()));
        }
        else if (itemNetId == "test2d")
        {

        }
        else
        {
            auto pInfo = getNetInfo(itemNetId);
            if (pInfo && !pInfo->atomic)
            {
                unetItems.push_back(item);
            }
        }
    }

    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#init dataloader net"));
    for (auto item : dataloaderItems)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        CCompileMgr::ins().GenProjectRunInitNetCode(itemNetId, pItemInfo->GetSn());
        CCompileMgr::ins().GenProjectRunCallDataloaderCode(itemNetId, pItemInfo->GetSn());
    }

    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#init self-define net"));
    for (auto item : unetItems)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        CCompileMgr::ins().GenProjectRunInitNetCode(itemNetId, pItemInfo->GetSn());
    }

    LossInfo info;
    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#init loss function nets, and combined into loss function set"));
    CCompileMgr::ins().GenProjectRunDefineLossFunctionsCode();
    for (auto item : lossItems)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto const &points = pItemInfo->GetPoints();
        getLossInfo(points, info, unetItemsInfo);

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        CCompileMgr::ins().GenProjectRunInitNetCode(itemNetId, pItemInfo->GetSn());
        CCompileMgr::ins().GenProjectRunAddLossFunctionCode(itemNetId, pItemInfo->GetSn());
    }
    CCompileMgr::ins().GenProjectRunInitLossInputCode(info);

    if (unetItemsInfo.size() != 1u)
    {
        emit showInfoSignal(tr("Error:  only 1 self-define net can inflow to loss!"));
    }

    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#init optimizer net"));
    for (auto item : optItems)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        CCompileMgr::ins().GenProjectRunInitNetCode(itemNetId, pItemInfo->GetSn());
    }

    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#init train net"));
    for (auto item : trainItems)
    {
        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        CCompileMgr::ins().GenProjectRunInitNetCode(itemNetId, pItemInfo->GetSn());
    }

    CCompileMgr::ins().AddProjectRunCode("");
    CCompileMgr::ins().AddProjectRunCode(tr("#run train function"));
    if (dataloaderItemsInfo.size() == 1u && unetItemsInfo.size() == 1u && optItemsInfo.size() == 1u && trainItemsInfo.size() == 1u)
    {
        CCompileMgr::ins().GenProjectRunTrainFunctionCode(*dataloaderItemsInfo.begin(), *unetItemsInfo.begin(), *optItemsInfo.begin(), *trainItemsInfo.begin());
    }

    CCompileMgr::ins().GenProjectCodeFinish();
    CCompileMgr::ins().GetProjectCode(code);

    emit showInfoSignal(tr("Finish compiling."));

    return code;
}

void CNetViewMgr::GenCode(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString &paramCode, QString &netCode)
{
    emit showInfoSignal(tr("Start compiling..."));

    CCompileMgr::ins().GenCodeInit(catalog, netId, outline, specify);

    tuoPuOrder(m_graph, m_tuoPuOrders);
    for (auto index = 0; index < (int)m_tuoPuOrders.size(); index++)
    {
        if (m_tuoPuOrders[index] < 0 || m_tuoPuOrders[index] >= (int)m_items.size())
            continue;

        auto item = m_items[m_tuoPuOrders[index]];
        if (item == nullptr)
            continue;

        auto pItemInfo = getItemInfo(item);
        if (pItemInfo == nullptr)
            continue;

        auto itemNetId = getItemNetId(item);
        emit showInfoSignal(tr("Compile ") + itemNetId + "_" + QString::number(pItemInfo->GetSn()));

        QString itemDir;
        getNetDirSlot(itemNetId, itemDir);

        CCompileMgr::ins().GenParamImportCode(itemDir, itemNetId);
        CCompileMgr::ins().GenParamInitCode(index, itemNetId, pItemInfo->GetSn());

        CCompileMgr::ins().GenNetImportCode(itemDir, itemNetId);
        CCompileMgr::ins().GenNetInitCode(index, netId, itemNetId, pItemInfo->GetSn());

        auto preposeItems = getPreposeItems(pItemInfo->GetSn());
        auto info = genForwardInfo(index, itemNetId, *pItemInfo, preposeItems);
        CCompileMgr::ins().GenNetForwardCode(info);
    }

    CCompileMgr::ins().GenCodeFinish(genFinishInfo(m_inChannelCnt, m_outputItems));
    CCompileMgr::ins().GetCode(paramCode, netCode);

    emit showInfoSignal(tr("Finish compiling."));
}

QJsonObject CNetViewMgr::genNet(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString const &shape, QColor const &color)
{
    NetInfo info;

    info.version = m_verson;

    info.net = netId;
    info.catalog = catalog;
    info.outline = outline;
    info.specify = specify;

    info.std = false;
    info.atomic = false;

    info.shape = shape;
    info.r = color.red();
    info.g = color.green();
    info.b = color.blue();

    info.in = m_outChannelCnt;
    info.out = m_inChannelCnt;

    QJsonObject jsonExpressParams;
    jsonExpressParams["01_cycle"] = 1;
    jsonExpressParams["02_reqGrad"] = 1;
    info.expressParams = jsonExpressParams;

    QJsonArray jsonLogicParams;

    tuoPuOrder(m_graph, m_tuoPuOrders);
    for (auto index = 0; index < (int)m_tuoPuOrders.size(); index++)
    {
        if (m_tuoPuOrders[index] < 0 || m_tuoPuOrders[index] >= (int)m_items.size())
            continue;

        auto item = m_items[m_tuoPuOrders[index]];
        if (item == nullptr)
            continue;

        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr)
                continue;

            jsonLogicParams.append(rect->GetAiParams());
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr)
                continue;

            jsonLogicParams.append(ellipse->GetAiParams());
        }
    }
    info.logicParams["01_items"] = jsonLogicParams;

    m_netMap[netId] = info;

    return genJsonObj(info);
}

bool CNetViewMgr::RewindNet()
{
    m_curShowNetMark.Reset();

    auto extendNet = m_curEditNetMark;
    m_extendNetMarkStack.pop_back();
    if (!m_extendNetMarkStack.empty())
        extendNet = m_curShowNetMark = m_extendNetMarkStack.back();

    auto netInfo = getNetInfo(extendNet.net);
    if (netInfo == nullptr || netInfo->std)
        return false;

    auto rootAiParams = getNetAiParams(m_editFocusItem);
    if (rootAiParams.isEmpty())
        return false;

    auto aiParams = getNetAiParams(extendNet, rootAiParams, m_extendNetMarkStack);
    if (aiParams.isEmpty())
        return false;

    emit markNetSignal(extendNet.sn, extendNet.net, extendNet.name, m_extendNetMarkStack);
    showNetAiParams(false, extendNet.sn, extendNet.net, extendNet.name, aiParams);

    QJsonObject netJsonObj;
    CDirFileMgr::ins().LoadCustomNet(netInfo->catalog, extendNet.net, netJsonObj);
    showView(netJsonObj, aiParams);
    emit clearAttrSignal();

    return !m_extendNetMarkStack.empty() || extendNet != m_curEditNetMark;
}

void CNetViewMgr::addInputChannelSlot(int outChannel, int inChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                // first channel
                if (!rect->AsInput())
                {
                    m_pMenuItem->CheckedAsInputAction(true);
                    rect->AsInput(true);

                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, rect->GetInputArrowBeginPos().x(), rect->GetInputArrowBeginPos().y(), rect->GetInputArrowEndPos().x(), rect->GetInputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = rect->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_input;
                    info.pos.setX(0);
                    info.pos.setY(0);
                    info.channels.push_back(std::make_pair(outChannel, inChannel));

                    addInputItem(rect);

                    rect->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    PointInfo info;
                    if (rect->GetInputPoint(info))
                    {
                        info.channels.push_back(std::make_pair(outChannel, inChannel));
                        rect->SetInputPoint(info);
                    }
                }
                updateInputOutChannel();
                emit netChangedSignal();
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), rect->GetInputInChannels(), rect->GetIn());
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                // first channel
                if (!ellipse->AsInput())
                {
                    m_pMenuItem->CheckedAsInputAction(true);
                    ellipse->AsInput(true);

                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, ellipse->GetInputArrowBeginPos().x(), ellipse->GetInputArrowBeginPos().y(), ellipse->GetInputArrowEndPos().x(), ellipse->GetInputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = ellipse->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_input;
                    info.pos.setX(0);
                    info.pos.setY(0);
                    info.channels.push_back(std::make_pair(outChannel, inChannel));

                    addInputItem(ellipse);

                    ellipse->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    PointInfo info;
                    if (ellipse->GetInputPoint(info))
                    {
                        info.channels.push_back(std::make_pair(outChannel, inChannel));
                        ellipse->SetInputPoint(info);
                    }
                }
                updateInputOutChannel();
                emit netChangedSignal();
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), ellipse->GetInputInChannels(), ellipse->GetIn());
            }
        }
    }

    qDebug() << "CNetViewMgr::addInputChannelSlot: " << outChannel << ", " << inChannel;
}

void CNetViewMgr::delInputChannelSlot(int outChannel, int inChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                PointInfo info;
                if (rect->GetInputPoint(info))
                {
                    if (info.channels.size() == 1)
                    {
                        m_pMenuItem->CheckedAsInputAction(false);
                        delArrow(getArrow(info.arrow));
                    }
                    else
                    {
                        delChannelPair(info.channels, outChannel, inChannel);
                        rect->SetInputPoint(info);
                        updateInputOutChannel();
                    }
                    emit netChangedSignal();
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), rect->GetInputInChannels(), rect->GetIn());
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                PointInfo info;
                if (ellipse->GetInputPoint(info))
                {
                    if (info.channels.size() == 1)
                    {
                        m_pMenuItem->CheckedAsInputAction(false);
                        delArrow(getArrow(info.arrow));
                    }
                    else
                    {
                        delChannelPair(info.channels, outChannel, inChannel);
                        ellipse->SetInputPoint(info);
                        updateInputOutChannel();
                    }
                    emit netChangedSignal();
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), ellipse->GetInputInChannels(), ellipse->GetIn());
                }
            }
        }
    }

    qDebug() << "CNetViewMgr::delInputChannelSlot: " << outChannel << ", " << inChannel;
}

void CNetViewMgr::modifyInputChannelSlot(int oldOutChannel, int inChannel, int newOutChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                PointInfo info;
                if (rect->GetInputPoint(info))
                {
                    modifyOutChannel(info.channels, oldOutChannel, inChannel, newOutChannel);
                    rect->SetInputPoint(info);
                    updateInputOutChannel();
                    emit netChangedSignal();
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), rect->GetInputInChannels(), rect->GetIn());
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                PointInfo info;
                if (ellipse->GetInputPoint(info))
                {
                    modifyOutChannel(info.channels, oldOutChannel, inChannel, newOutChannel);
                    ellipse->SetInputPoint(info);
                    updateInputOutChannel();
                    emit netChangedSignal();
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), ellipse->GetInputInChannels(), ellipse->GetIn());
                }
            }
        }
    }

    qDebug() << "CNetViewMgr::modifyInputChannelSlot: " << oldOutChannel << ", " << inChannel<< ", " << newOutChannel;
}

void CNetViewMgr::swapInputInChannelSlot(int inChannel1, int inChannel2)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        swapItemInChannel(item, inChannel1, inChannel2);
        emit netChangedSignal();

        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), rect->GetInputInChannels(), rect->GetIn());
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), ellipse->GetInputInChannels(), ellipse->GetIn());
            }
        }
    }

    qDebug() << "CNetViewMgr::swapInputInChannelSlot: " << inChannel1 << ", " << inChannel2;
}

void CNetViewMgr::addOutputChannelSlot(int outChannel, int inChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                // first channel
                if (!rect->AsOutput())
                {
                    m_pMenuItem->CheckedAsOutputAction(true);
                    rect->AsOutput(true);

                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, rect->GetOutputArrowBeginPos().x(), rect->GetOutputArrowBeginPos().y(), rect->GetOutputArrowEndPos().x(), rect->GetOutputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = rect->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_output;
                    info.pos.setX(0);
                    info.pos.setY(0);
                    info.channels.push_back(std::make_pair(outChannel, inChannel));

                    addOutputItem(rect);

                    rect->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    PointInfo info;
                    if (rect->GetOutputPoint(info))
                    {
                        info.channels.push_back(std::make_pair(outChannel, inChannel));
                        rect->SetOutputPoint(info);
                    }
                }
                updateOutputInChannel();
                emit netChangedSignal();
                emit outputConfigSignal(rect->GetOut(), rect->GetOutputChannelPairs(), m_inChannelCnt);
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                // first channel
                if (!ellipse->AsOutput())
                {
                    m_pMenuItem->CheckedAsOutputAction(true);
                    ellipse->AsOutput(true);

                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, ellipse->GetOutputArrowBeginPos().x(), ellipse->GetOutputArrowBeginPos().y(), ellipse->GetOutputArrowEndPos().x(), ellipse->GetOutputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = ellipse->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_output;
                    info.pos.setX(0);
                    info.pos.setY(0);
                    info.channels.push_back(std::make_pair(outChannel, inChannel));

                    addOutputItem(ellipse);

                    ellipse->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    PointInfo info;
                    if (ellipse->GetOutputPoint(info))
                    {
                        info.channels.push_back(std::make_pair(outChannel, inChannel));
                        ellipse->SetOutputPoint(info);
                    }
                }
                updateOutputInChannel();
                emit netChangedSignal();
                emit outputConfigSignal(ellipse->GetOut(), ellipse->GetOutputChannelPairs(), m_inChannelCnt);
            }
        }
    }

    qDebug() << "CNetViewMgr::addOutputChannelSlot: " << outChannel << ", " << inChannel;
}

void CNetViewMgr::delOutputChannelSlot(int outChannel, int inChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                PointInfo info;
                if (rect->GetOutputPoint(info))
                {
                    if (info.channels.size() == 1)
                    {
                        m_pMenuItem->CheckedAsOutputAction(false);
                        delArrow(getArrow(info.arrow));
                    }
                    else
                    {
                        delChannelPair(info.channels, outChannel, inChannel);
                        rect->SetOutputPoint(info);
                        updateOutputInChannel();
                    }
                    emit netChangedSignal();
                    emit outputConfigSignal(rect->GetOut(), rect->GetOutputChannelPairs(), m_inChannelCnt);
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                PointInfo info;
                if (ellipse->GetOutputPoint(info))
                {
                    if (info.channels.size() == 1)
                    {
                        m_pMenuItem->CheckedAsOutputAction(false);
                        delArrow(getArrow(info.arrow));
                    }
                    else
                    {
                        delChannelPair(info.channels, outChannel, inChannel);
                        ellipse->SetOutputPoint(info);
                        updateOutputInChannel();
                    }
                    emit netChangedSignal();
                    emit outputConfigSignal(ellipse->GetOut(), ellipse->GetOutputChannelPairs(), m_inChannelCnt);
                }
            }
        }
    }

    qDebug() << "CNetViewMgr::delOutputChannelSlot: " << outChannel << ", " << inChannel;
}

void CNetViewMgr::modifyOutputChannelSlot(int oldOutChannel, int inChannel, int newOutChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                PointInfo info;
                if (rect->GetOutputPoint(info))
                {
                    modifyOutChannel(info.channels, oldOutChannel, inChannel, newOutChannel);
                    rect->SetOutputPoint(info);
                    updateOutputInChannel();
                    emit netChangedSignal();
                    emit outputConfigSignal(rect->GetOut(), rect->GetOutputChannelPairs(), m_inChannelCnt);
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                PointInfo info;
                if (ellipse->GetOutputPoint(info))
                {
                    modifyOutChannel(info.channels, oldOutChannel, inChannel, newOutChannel);
                    ellipse->SetOutputPoint(info);
                    updateOutputInChannel();
                    emit netChangedSignal();
                    emit outputConfigSignal(ellipse->GetOut(), ellipse->GetOutputChannelPairs(), m_inChannelCnt);
                }
            }
        }
    }

    qDebug() << "CNetViewMgr::modifyOutputChannelSlot: " << oldOutChannel << ", " << inChannel<< ", " << newOutChannel;
}

void CNetViewMgr::swapOutputInChannelSlot(int inChannel1, int inChannel2)
{
    for (auto outputItem : m_outputItems)
    {
        if (outputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(outputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetOutputPoint(point))
            {
                bool modify = false;
                for (auto &channelPair : point.channels)
                {
                    if (channelPair.second == inChannel1)
                    {
                        modify = true;
                        channelPair.second = inChannel2;
                    }
                    else if (channelPair.second == inChannel2)
                    {
                        modify = true;
                        channelPair.second = inChannel1;
                    }
                }

                if (!modify)
                    continue;

                rect->SetOutputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_output)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetOutputPoint(point);
                    arrowItem->update();
                }
            }
        }
        else if (outputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(outputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetOutputPoint(point))
            {
                bool modify = false;
                for (auto &channelPair : point.channels)
                {
                    if (channelPair.second == inChannel1)
                    {
                        modify = true;
                        channelPair.second = inChannel2;
                    }
                    else if (channelPair.second == inChannel2)
                    {
                        modify = true;
                        channelPair.second = inChannel1;
                    }
                }

                if (!modify)
                    continue;

                ellipse->SetOutputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_output)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetOutputPoint(point);
                    arrowItem->update();
                }
            }
        }
    }

    emit netChangedSignal();

    outputConfigSelectedItemsSlot();

    qDebug() << "CNetViewMgr::swapOutputInChannelSlot: " << inChannel1 << ", " << inChannel2;
}

void CNetViewMgr::delNetsSlot(QStringList const &delNetIds)
{
    for (auto const &netId : delNetIds)
        m_netMap.erase(netId);
}

void CNetViewMgr::checkNetInfoSlot(QString const &netId, bool &atomic)
{
    auto netInfo = getNetInfo(netId);
    if (netInfo)
        atomic = netInfo->atomic;
}

void CNetViewMgr::paramsChangedSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams)
{
    auto rootAiParams = getNetAiParams(m_editFocusItem);
    if (rootAiParams.isEmpty())
        return;

    if (m_extendNetMarkStack.empty())
    {
        if (sn == m_curEditNetMark.sn && netId == m_curEditNetMark.net && netName == m_curEditNetMark.name)
        {
            setNetAiExpressParams(m_editFocusItem, expressParams);
            rootAiParams["03_express"] = expressParams;
        }
        else
        {
            setNetAiExpressParams(m_showFocusItem, expressParams);
            setNetAiExpressParams(sn, netId, expressParams, rootAiParams);
        }
    }
    else
    {
        setNetAiExpressParams(m_showFocusItem, expressParams);

        auto aiParams = getNetAiParams(m_curShowNetMark, rootAiParams, m_extendNetMarkStack);
        if (aiParams.isEmpty())
            return;

        auto extendNetMarkStack = m_extendNetMarkStack;
        if (m_curShowNetMark == m_extendNetMarkStack.back())
        {
            aiParams["03_express"] = expressParams;
        }
        else
        {
            setNetAiExpressParams(sn, netId, expressParams, aiParams);
        }
        extendNetMarkStack.pop_back();

        rewindAiParams(aiParams, rootAiParams, extendNetMarkStack);
    }

    setNetAiParams(m_editFocusItem, rootAiParams);
    emit netChangedSignal();
    emit showAttrTreeSignal(rootAiParams);
    emit markNetSignal(sn, netId, netName, m_extendNetMarkStack);

    //emit showInfoSignal(netName + "(" + netId + "): CNetViewMgr::paramsChangedSlot...");
}

bool CNetViewMgr::matchStack(vector<NetMark> extendNetMarkStack) const
{
    bool match = false;

    do
    {
        if (extendNetMarkStack.size() < 2u)
            break;

        if (m_curEditNetMark != extendNetMarkStack.front())
            break;

        if (m_curShowNetMark != extendNetMarkStack.back())
            break;

        extendNetMarkStack.pop_back();
        extendNetMarkStack.erase(extendNetMarkStack.begin());

        if (m_extendNetMarkStack != extendNetMarkStack)
            break;

        match = true;
    } while (0);

    return match;
}

void CNetViewMgr::treeParamsChangedSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams, vector<NetMark> extendNetMarkStack)
{
    auto rootAiParams = getNetAiParams(m_editFocusItem);
    if (rootAiParams.isEmpty() || extendNetMarkStack.empty())
        return;

    NetMark curShowNetMark(sn, netId, netName);
    NetMark curEditNetMark(extendNetMarkStack.front());

    auto match = matchStack(extendNetMarkStack);

    extendNetMarkStack.erase(extendNetMarkStack.begin());
    if (!extendNetMarkStack.empty())
        extendNetMarkStack.pop_back();

    if (extendNetMarkStack.empty())
    {
        if (sn == curEditNetMark.sn && netId == curEditNetMark.net && netName == curEditNetMark.name)
        {
            setNetAiExpressParams(m_editFocusItem, expressParams);
            rootAiParams["03_express"] = expressParams;
        }
        else
        {
            if (match)
                setNetAiExpressParams(m_showFocusItem, expressParams);

            setNetAiExpressParams(sn, netId, expressParams, rootAiParams);
        }
    }
    else
    {
        if (match)
            setNetAiExpressParams(m_showFocusItem, expressParams);

        auto aiParams = getNetAiParams(curShowNetMark, rootAiParams, extendNetMarkStack);
        if (aiParams.isEmpty())
            return;

        if (curShowNetMark == extendNetMarkStack.back())
        {
            aiParams["03_express"] = expressParams;
        }
        else
        {
            setNetAiExpressParams(sn, netId, expressParams, aiParams);
        }
        extendNetMarkStack.pop_back();

        rewindAiParams(aiParams, rootAiParams, extendNetMarkStack);
    }

    setNetAiParams(m_editFocusItem, rootAiParams);
    emit netChangedSignal();
    emit showAttrTreeSignal(rootAiParams);
    emit markNetSignal(m_curShowNetMark.sn, m_curShowNetMark.net, m_curShowNetMark.name, m_extendNetMarkStack);

    if (match)
        emit showAttrSignal(false, sn, netId, netName, expressParams);

    //emit showInfoSignal(netName + "(" + netId + "): CNetViewMgr::treeParamsChangedSlot...");
}

void CNetViewMgr::enableSlot(bool enabled)
{
    for (auto &item : m_items)
    {
        item->setEnabled(enabled);
    }

    for (auto &item : m_arrows)
    {
        item->setEnabled(enabled);
    }
}

void CNetViewMgr::addArrowSlot(CGraphicsArrowItem *arrowItem, QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel, QGraphicsItem *endItem, QPointF const &endPos, int endChannel)
{
    if (hasRing(beginItem, endItem))
    {
        m_editView->scene()->removeItem(arrowItem);
        m_editView->update();
        delete arrowItem;
        emit showInfoSignal(tr("Arrow lead into ring!"));
        return;
    }

    arrowItem->SetSn(++m_sn);
    m_arrows.push_back(arrowItem);

    addArrowPoint(arrowItem, beginItem, beginPos, beginChannel, endItem, endPos, endChannel);
    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

    emit showInfoSignal(tr("Add arrow") + QString::number(arrowItem->GetSn()) + tr("."));
    emit netChangedSignal();
}

void CNetViewMgr::deleteKeySlot()
{
    deleteSelectedItems();
}

void CNetViewMgr::restoreSlot()
{
    emit netChangedSignal();
}

void CNetViewMgr::itemMovedSlot(bool edit, QGraphicsItem *itemMoved)
{
    if (itemMoved == nullptr)
        return;

    if (edit)
    {
        for (auto &arrowItem : m_arrows)
        {
            auto const &points = arrowItem->GetPoints();
            if (points.size() == 1)
            {
                auto item = m_itemPointsMap[points[0].owner].item;
                if (item->type() == CGraphicsRectItem::Type)
                {
                    auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                    if (rect != nullptr)
                    {
                        if (points[0].role == e_input)
                        {
                            arrowItem->SetBeginPoint(rect->GetInputArrowBeginPos());
                            arrowItem->SetEndPoint(rect->GetInputArrowEndPos());
                        }
                        else
                        {
                            arrowItem->SetBeginPoint(rect->GetOutputArrowBeginPos());
                            arrowItem->SetEndPoint(rect->GetOutputArrowEndPos());
                        }
                    }
                }
                else if (item->type() == CGraphicsEllipseItem::Type)
                {
                    auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                    if (ellipse != nullptr)
                    {
                        if (points[0].role == e_input)
                        {
                            arrowItem->SetBeginPoint(ellipse->GetInputArrowBeginPos());
                            arrowItem->SetEndPoint(ellipse->GetInputArrowEndPos());
                        }
                        else
                        {
                            arrowItem->SetBeginPoint(ellipse->GetOutputArrowBeginPos());
                            arrowItem->SetEndPoint(ellipse->GetOutputArrowEndPos());
                        }
                    }
                }

                arrowItem->update();
            }
            else if (points.size() == 2)
            {
                auto beginItem = m_itemPointsMap[points[0].owner].item;
                for (auto &beginPoint : m_itemPointsMap[points[0].owner].points)
                {
                    if (beginPoint.arrow != points[0].arrow)
                        continue;

                    auto endItem = m_itemPointsMap[points[1].owner].item;
                    for (auto &endPoint : m_itemPointsMap[points[1].owner].points)
                    {
                        if (endPoint.arrow != points[1].arrow)
                            continue;

                        arrowItem->SetBeginPoint(beginItem->mapToScene(beginPoint.pos));
                        arrowItem->SetEndPoint(endItem->mapToScene(endPoint.pos));
                        arrowItem->update();

                        break;
                    }

                    break;
                }
            }
        }

        m_editView->update();
        emit netChangedSignal();
    }
    else
    {
        for (auto &arrowItem : m_showArrows)
        {
            auto const &points = arrowItem->GetPoints();
            if (points.size() == 1)
            {
                auto item = m_showItemPointsMap[points[0].owner].item;
                if (item->type() == CGraphicsRectItem::Type)
                {
                    auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                    if (rect != nullptr)
                    {
                        if (points[0].role == e_input)
                        {
                            arrowItem->SetBeginPoint(rect->GetInputArrowBeginPos());
                            arrowItem->SetEndPoint(rect->GetInputArrowEndPos());
                        }
                        else
                        {
                            arrowItem->SetBeginPoint(rect->GetOutputArrowBeginPos());
                            arrowItem->SetEndPoint(rect->GetOutputArrowEndPos());
                        }
                    }
                }
                else if (item->type() == CGraphicsEllipseItem::Type)
                {
                    auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                    if (ellipse != nullptr)
                    {
                        if (points[0].role == e_input)
                        {
                            arrowItem->SetBeginPoint(ellipse->GetInputArrowBeginPos());
                            arrowItem->SetEndPoint(ellipse->GetInputArrowEndPos());
                        }
                        else
                        {
                            arrowItem->SetBeginPoint(ellipse->GetOutputArrowBeginPos());
                            arrowItem->SetEndPoint(ellipse->GetOutputArrowEndPos());
                        }
                    }
                }

                arrowItem->update();
            }
            else if (points.size() == 2)
            {
                auto beginItem = m_showItemPointsMap[points[0].owner].item;
                for (auto &beginPoint : m_showItemPointsMap[points[0].owner].points)
                {
                    if (beginPoint.arrow != points[0].arrow)
                        continue;

                    auto endItem = m_showItemPointsMap[points[1].owner].item;
                    for (auto &endPoint : m_showItemPointsMap[points[1].owner].points)
                    {
                        if (endPoint.arrow != points[1].arrow)
                            continue;

                        arrowItem->SetBeginPoint(beginItem->mapToScene(beginPoint.pos));
                        arrowItem->SetEndPoint(endItem->mapToScene(endPoint.pos));
                        arrowItem->update();

                        break;
                    }

                    break;
                }
            }
        }

        m_showView->update();
    }
}

void CNetViewMgr::contextMenuSlot(QPoint const &pos)
{
    m_pMenuItem->CheckedAsInputAction(false);
    m_pMenuItem->CheckedAsOutputAction(false);
    m_pMenuItem->EnableAsInputAction(false);
    m_pMenuItem->EnableAsOutputAction(false);
    m_pMenuItem->EnableInputConfigAction(false);
    m_pMenuItem->EnableOutputConfigAction(false);

    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                m_pMenuItem->EnableAsInputAction(rect->AsInput() || rect->nextInChannel() >= 0);
                m_pMenuItem->EnableInputConfigAction(rect->AsInput() || rect->nextInChannel() >= 0);
                m_pMenuItem->EnableOutputConfigAction(true);
                m_pMenuItem->EnableAsOutputAction(true);

                m_pMenuItem->CheckedAsInputAction(rect->AsInput());
                m_pMenuItem->CheckedAsOutputAction(rect->AsOutput());
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                m_pMenuItem->EnableAsInputAction(ellipse->AsInput() || ellipse->nextInChannel() >= 0);
                m_pMenuItem->EnableInputConfigAction(ellipse->AsInput() || ellipse->nextInChannel() >= 0);
                m_pMenuItem->EnableOutputConfigAction(true);
                m_pMenuItem->EnableAsOutputAction(true);

                m_pMenuItem->CheckedAsInputAction(ellipse->AsInput());
                m_pMenuItem->CheckedAsOutputAction(ellipse->AsOutput());
            }
        }
    }

    auto newPos = pos;
    newPos.setX(pos.x() - 20);
    newPos.setY(pos.y() - 20);
    m_pMenuItem->exec(newPos);
}

void CNetViewMgr::editArrowChannelSlot(int owner, int arrow, bool isIn, int channel, QPoint const &pos)
{
    m_isInChannel = isIn;
    m_editChannel = channel;
    m_editPointOwner = owner;
    m_editPointArrow = arrow;

    auto max = channel;
    auto item = m_itemPointsMap[owner].item;
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem*>(item);
        if (rect != nullptr)
        {
            max = (isIn ? rect->GetIn() - 1 : rect->GetOut() - 1);
        }
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
        if (ellipse != nullptr)
        {
            max = (isIn ? ellipse->GetIn() - 1 : ellipse->GetOut() - 1);
        }
    }

    emit editChannelSignal(0, max, channel, pos);
}

void CNetViewMgr::channelChangedSlot(int newChannel)
{
    auto items = m_editView->scene()->selectedItems();
    if (newChannel != m_editChannel && items.size() == 1)
    {
        auto item = m_itemPointsMap[m_editPointOwner].item;

        if (m_isInChannel)
        {
            swapItemInChannel(item, m_editChannel, newChannel);
            m_editChannel = newChannel;
            emit netChangedSignal();
        }
        else
        {
            if (item->type() == CGraphicsRectItem::Type)
            {
                auto rect = dynamic_cast<CGraphicsRectItem*>(item);
                if (rect != nullptr)
                {
                    auto points = rect->GetPoints();
                    for (auto &point : points)
                        if (point.role == e_src && point.arrow == m_editPointArrow && point.channel == m_editChannel)
                            point.channel = newChannel;

                    rect->SetPoints(points);
                    rect->update();
                    m_itemPointsMap[rect->GetSn()].points = points;

                    auto arrowItem = getArrow(m_editPointArrow);
                    if (arrowItem)
                    {
                        arrowItem->SetChannel(e_src, newChannel);
                        arrowItem->update();
                    }

                    m_editChannel = newChannel;
                    emit netChangedSignal();
                }
            }
            else if (item->type() == CGraphicsEllipseItem::Type)
            {
                auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
                if (ellipse != nullptr)
                {
                    auto points = ellipse->GetPoints();
                    for (auto &point : points)
                        if (point.role == e_src && point.arrow == m_editPointArrow && point.channel == m_editChannel)
                            point.channel = newChannel;

                    ellipse->SetPoints(points);
                    ellipse->update();
                    m_itemPointsMap[ellipse->GetSn()].points = points;

                    auto arrowItem = getArrow(m_editPointArrow);
                    if (arrowItem)
                    {
                        arrowItem->SetChannel(e_src, newChannel);
                        arrowItem->update();
                    }

                    m_editChannel = newChannel;
                    emit netChangedSignal();
                }
            }
        }
    }
}

void CNetViewMgr::editNetNameSlot(QString const &netId, QPoint const &pos)
{
    emit editNetNameSignal(netId, pos);
}

void CNetViewMgr::showNetSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams)
{
    m_curShowNetMark = NetMark(sn, netId, netName);
    showNetAiParams(true, sn, netId, netName, aiParams);
}

void CNetViewMgr::extendNetSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams)
{
    auto netInfo = getNetInfo(netId);
    if (netInfo->std)
        return;

    m_curShowNetMark = NetMark(sn, netId, netName);
    m_extendNetMarkStack.push_back(m_curShowNetMark);

    emit enableUpBtnSignal(true);
    emit markNetSignal(sn, netId, netName, m_extendNetMarkStack);
    showNetAiParams(true, sn, netId, netName, aiParams);

    QJsonObject netJsonObj;
    CDirFileMgr::ins().LoadCustomNet(netInfo->catalog, netId, netJsonObj);
    showView(netJsonObj, aiParams);
}

void CNetViewMgr::netNameChangedSlot(const QString &netName)
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                rect->SetNetName(netName);
                emit netChangedSignal();
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                ellipse->SetNetName(netName);
                emit netChangedSignal();
            }
        }
    }
}

void CNetViewMgr::asInputSelectedItemSlot()
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                auto asInput = !rect->AsInput();
                m_pMenuItem->CheckedAsInputAction(asInput);
                rect->AsInput(asInput);

                if (asInput)
                {
                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, rect->GetInputArrowBeginPos().x(), rect->GetInputArrowBeginPos().y(), rect->GetInputArrowEndPos().x(), rect->GetInputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = rect->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_input;
                    info.pos.setX(0);
                    info.pos.setY(0);

                    auto inChannels = rect->GetInputInChannels();
                    for (auto channel : inChannels)
                        info.channels.push_back(std::make_pair(m_outChannelCnt++, channel));

                    addInputItem(rect);

                    rect->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit netChangedSignal();
                    emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), inChannels, rect->GetIn());
                }
                else
                {
                    PointInfo info;
                    if (rect->GetInputPoint(info))
                    {
                        delArrow(getArrow(info.arrow));
                        emit netChangedSignal();
                    }
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                auto asInput = !ellipse->AsInput();
                m_pMenuItem->CheckedAsInputAction(asInput);
                ellipse->AsInput(asInput);

                if (asInput)
                {
                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, ellipse->GetInputArrowBeginPos().x(), ellipse->GetInputArrowBeginPos().y(), ellipse->GetInputArrowEndPos().x(), ellipse->GetInputArrowEndPos().y(), Qt::green);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = ellipse->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_input;
                    info.pos.setX(0);
                    info.pos.setY(0);

                    auto inChannels = ellipse->GetInputInChannels();
                    for (auto channel : inChannels)
                        info.channels.push_back(std::make_pair(m_outChannelCnt++, channel));

                    addInputItem(ellipse);

                    ellipse->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit netChangedSignal();
                    emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                    emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), inChannels, ellipse->GetIn());
                }
                else
                {
                    PointInfo info;
                    if (ellipse->GetInputPoint(info))
                    {
                        delArrow(getArrow(info.arrow));
                        emit netChangedSignal();
                    }
                }
            }
        }
    }
}

void CNetViewMgr::asOutputSelectedItemSlot()
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                auto asOutput = !rect->AsOutput();
                m_pMenuItem->CheckedAsInputAction(asOutput);
                rect->AsOutput(asOutput);

                if (asOutput)
                {
                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, rect->GetOutputArrowBeginPos().x(), rect->GetOutputArrowBeginPos().y(), rect->GetOutputArrowEndPos().x(), rect->GetOutputArrowEndPos().y(), Qt::blue);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = rect->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_output;
                    info.pos.setX(0);
                    info.pos.setY(0);

                    auto outChannelCnt = rect->GetOut();
                    for (auto channel = 0; channel < outChannelCnt; )
                        info.channels.push_back(std::make_pair(channel++, m_inChannelCnt++));

                    addOutputItem(rect);

                    rect->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit netChangedSignal();
                    emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                    emit outputConfigSignal(outChannelCnt, rect->GetOutputChannelPairs(), m_inChannelCnt);
                }
                else
                {
                    PointInfo info;
                    if (rect->GetOutputPoint(info))
                    {
                        delArrow(getArrow(info.arrow));
                        emit netChangedSignal();
                    }
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                auto asOutput = !ellipse->AsOutput();
                m_pMenuItem->CheckedAsInputAction(asOutput);
                ellipse->AsOutput(asOutput);

                if (asOutput)
                {
                    auto arrowItem = new CGraphicsArrowItem(true, ++m_sn, ellipse->GetOutputArrowBeginPos().x(), ellipse->GetOutputArrowBeginPos().y(), ellipse->GetOutputArrowEndPos().x(), ellipse->GetOutputArrowEndPos().y(), Qt::blue);
                    connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                    connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                    PointInfo info;
                    info.sn = ++m_sn;
                    info.owner = ellipse->GetSn();
                    info.peer = -1;
                    info.arrow = arrowItem->GetSn();
                    info.channel = -1;
                    info.role = e_output;
                    info.pos.setX(0);
                    info.pos.setY(0);

                    auto outChannelCnt = ellipse->GetOut();
                    for (auto channel = 0; channel < outChannelCnt; )
                        info.channels.push_back(std::make_pair(channel++, m_inChannelCnt++));

                    addOutputItem(ellipse);

                    ellipse->AddPoint(info);
                    arrowItem->AddPoint(info);
                    m_itemPointsMap[info.owner].points.push_back(info);

                    m_editView->scene()->addItem(arrowItem);
                    m_arrows.push_back(arrowItem);
                    arrowItem->update();

                    emit netChangedSignal();
                    emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                    emit outputConfigSignal(outChannelCnt, ellipse->GetOutputChannelPairs(), m_inChannelCnt);
                }
                else
                {
                    PointInfo info;
                    if (ellipse->GetOutputPoint(info))
                    {
                        delArrow(getArrow(info.arrow));
                        emit netChangedSignal();
                    }
                }
            }
        }
    }
}

void CNetViewMgr::inputConfigSelectedItemsSlot()
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), rect->GetInputChannelPairs(), rect->GetInputInChannels(), rect->GetIn());
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                emit inputConfigSignal(m_outChannelCnt, getInputOutChannelShareCnt(), ellipse->GetInputChannelPairs(), ellipse->GetInputInChannels(), ellipse->GetIn());
            }
        }
    }
}

void CNetViewMgr::outputConfigSelectedItemsSlot()
{
    auto items = m_editView->scene()->selectedItems();
    if (items.size() == 1)
    {
        auto &item = items[0];
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                emit outputConfigSignal(rect->GetOut(), rect->GetOutputChannelPairs(), m_inChannelCnt);
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                emit outputConfigSignal(ellipse->GetOut(), ellipse->GetOutputChannelPairs(), m_inChannelCnt);
            }
        }
    }
}

void CNetViewMgr::deleteSelectedItemsSlot()
{
    deleteSelectedItems();
}

void CNetViewMgr::editSceneSelectionChangedSlot()
{
    static vector<QGraphicsItem*> selectedItemsStack;

    auto items = m_editView->scene()->selectedItems();
    for (auto itor = items.begin(); itor != items.end();)
        if ((*itor)->type() == CGraphicsArrowItem::Type)
            itor = items.erase(itor);
        else
             ++itor;

    if (items.isEmpty())
    {
        selectedItemsStack.clear();
    }
    else if (items.size() == 1)
    {
        selectedItemsStack.clear();
        selectedItemsStack.push_back(items[0]);
    }
    else if (items.size() > (int)selectedItemsStack.size())
    {
        for (auto &item : items)
        {
            bool selected = false;
            for (auto selectedItem : selectedItemsStack)
                if (selectedItem == item)
                {
                    selected = true;
                    break;
                }
            if (selected)
                continue;

            selectedItemsStack.push_back(item);
            break;
        }
    }
    else if (items.size() < (int)selectedItemsStack.size())
    {
        for (auto itor = selectedItemsStack.begin(); itor != selectedItemsStack.end(); ++itor)
        {
            bool selected = false;
            for (auto &item : items)
                if (*itor == item)
                {
                    selected = true;
                    break;
                }
            if (selected)
                continue;

            selectedItemsStack.erase(itor);
            break;
        }
    }

    m_curEditNetMark.Reset();
    m_curShowNetMark.Reset();
    m_extendNetMarkStack.clear();
    m_editFocusItem = nullptr;
    emit enableUpBtnSignal(false);

    if (!selectedItemsStack.empty())
    {
        auto &item = selectedItemsStack.back();
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                m_editFocusItem = rect;

                auto aiParams = rect->GetAiParams();
                emit showAttrTreeSignal(aiParams);
                emit markNetSignal(rect->GetSn(), rect->GetNetId(), rect->GetNetName(), m_extendNetMarkStack);
                if (aiParams.contains("03_express") && aiParams.value("03_express").isObject())
                {
                    auto netId = rect->GetNetId();
                    auto netInfo = getNetInfo(netId);
                    if (netInfo != nullptr)
                    {
                        QJsonObject netJsonObj;
                        if (netInfo->std)
                        {
                            CDirFileMgr::ins().LoadStandardNet(netInfo->catalog, netId, netJsonObj);
                        }
                        else
                        {
                            CDirFileMgr::ins().LoadCustomNet(netInfo->catalog, netId, netJsonObj);
                        }

                        m_curEditNetMark = NetMark(rect->GetSn(), rect->GetNetId(), rect->GetNetName());
                        showNetAiParams(false, rect->GetSn(), rect->GetNetId(), rect->GetNetName(), aiParams);
                        showView(netJsonObj, aiParams);
                    }
                    else
                    {
                        emit showInfoSignal(tr("Error: ") + rect->GetNetId() + " : " + rect->GetNetName() + tr(" is missing!"));
                    }
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                m_editFocusItem = ellipse;

                auto aiParams = ellipse->GetAiParams();
                emit showAttrTreeSignal(aiParams);
                emit markNetSignal(ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName(), m_extendNetMarkStack);
                if (aiParams.contains("03_express") && aiParams.value("03_express").isObject())
                {
                    auto netId = ellipse->GetNetId();
                    auto netInfo = getNetInfo(netId);
                    if (netInfo != nullptr)
                    {
                        QJsonObject netJsonObj;
                        if (netInfo->std)
                        {
                            CDirFileMgr::ins().LoadStandardNet(netInfo->catalog, netId, netJsonObj);
                        }
                        else
                        {
                            CDirFileMgr::ins().LoadCustomNet(netInfo->catalog, netId, netJsonObj);
                        }

                        m_curEditNetMark = NetMark(ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName());
                        showNetAiParams(false, ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName(), aiParams);
                        showView(netJsonObj, aiParams);
                    }
                    else
                    {
                        emit showInfoSignal(tr("Error: ") + ellipse->GetNetId() + " : " + ellipse->GetNetName() + tr(" is missing!"));
                    }
                }
            }
        }
    }
    else
    {
        showViewClearAll();
        emit clearAttrSignal();
    }
}

void CNetViewMgr::editSceneFocusItemChangedSlot(QGraphicsItem *newFocusItem, QGraphicsItem *, Qt::FocusReason)
{
    if (newFocusItem == nullptr)
        return;
}

void CNetViewMgr::showSceneSelectionChangedSlot()
{
    static vector<QGraphicsItem*> selectedItemsStack;

    auto items = m_showView->scene()->selectedItems();
    for (auto itor = items.begin(); itor != items.end();)
        if ((*itor)->type() == CGraphicsArrowItem::Type)
            itor = items.erase(itor);
        else
             ++itor;

    if (items.isEmpty())
    {
        selectedItemsStack.clear();
    }
    else if (items.size() == 1)
    {
        selectedItemsStack.clear();
        selectedItemsStack.push_back(items[0]);
    }
    else if (items.size() > (int)selectedItemsStack.size())
    {
        for (auto &item : items)
        {
            bool selected = false;
            for (auto selectedItem : selectedItemsStack)
                if (selectedItem == item)
                {
                    selected = true;
                    break;
                }
            if (selected)
                continue;

            selectedItemsStack.push_back(item);
            break;
        }
    }
    else if (items.size() < (int)selectedItemsStack.size())
    {
        for (auto itor = selectedItemsStack.begin(); itor != selectedItemsStack.end(); ++itor)
        {
            bool selected = false;
            for (auto &item : items)
                if (*itor == item)
                {
                    selected = true;
                    break;
                }
            if (selected)
                continue;

            selectedItemsStack.erase(itor);
            break;
        }
    }

    m_showFocusItem = nullptr;
    if (!selectedItemsStack.empty())
    {
        auto &item = selectedItemsStack.back();
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                m_showFocusItem = rect;

                auto aiParams = rect->GetAiParams();
                emit markNetSignal(rect->GetSn(), rect->GetNetId(), rect->GetNetName(), m_extendNetMarkStack);
                if (aiParams.contains("03_express") && aiParams.value("03_express").isObject())
                {
                    auto netId = rect->GetNetId();
                    auto netInfo = getNetInfo(netId);
                    if (netInfo != nullptr)
                    {
                        m_curShowNetMark = NetMark(rect->GetSn(), rect->GetNetId(), rect->GetNetName());
                        showNetAiParams(true, rect->GetSn(), rect->GetNetId(), rect->GetNetName(), aiParams);
                    }
                    else
                    {
                        emit showInfoSignal(tr("Error: ") + rect->GetNetId() + " : " + rect->GetNetName() + tr(" is missing!"));
                    }
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                m_showFocusItem = ellipse;

                auto aiParams = ellipse->GetAiParams();
                emit markNetSignal(ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName(), m_extendNetMarkStack);
                if (aiParams.contains("03_express") && aiParams.value("03_express").isObject())
                {
                    auto netId = ellipse->GetNetId();
                    auto netInfo = getNetInfo(netId);
                    if (netInfo != nullptr)
                    {
                        m_curShowNetMark = NetMark(ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName());
                        showNetAiParams(true, ellipse->GetSn(), ellipse->GetNetId(), ellipse->GetNetName(), aiParams);
                    }
                    else
                    {
                        emit showInfoSignal(tr("Error: ") + ellipse->GetNetId() + " : " + ellipse->GetNetName() + tr(" is missing!"));
                    }
                }
            }
        }
    }
    else
    {
        emit clearAttrSignal();
    }
}

void CNetViewMgr::initGraph()
{
    m_graph.vertexNum = 0;
    memset(m_graph.arc, 0, sizeof(m_graph.arc));
}

void CNetViewMgr::initNetInfoMap()
{
    auto catalogs = CDirFileMgr::ins().GetStandardCatalogs();
    for (auto const &catalog : catalogs)
    {
        auto netIds = CDirFileMgr::ins().GetStandardNetIds(catalog);
        for (auto const &netId : netIds)
        {
            QJsonObject netJsonObj;
            CDirFileMgr::ins().LoadStandardNet(catalog, netId, netJsonObj);
            addNetInfo(catalog, netId, netJsonObj);
        }
    }

    catalogs = CDirFileMgr::ins().GetCustomCatalogs();
    for (auto const &catalog : catalogs)
    {
        auto netIds = CDirFileMgr::ins().GetCustomNetIds(catalog);
        for (auto const &netId : netIds)
        {
            QJsonObject netJsonObj;
            CDirFileMgr::ins().LoadCustomNet(catalog, netId, netJsonObj);
            addNetInfo(catalog, netId, netJsonObj);
        }
    }
}

void CNetViewMgr::addNetInfo(QString const &catalog, QString const &netId, QJsonObject const &netJsonObj)
{
    NetInfo info;

    info.catalog = catalog;

    if (!netJsonObj.contains("01_version") || !netJsonObj.value("01_version").isString())
        return;
    info.version = netJsonObj.value("01_version").toString();

    if (!netJsonObj.contains("02_net") || !netJsonObj.value("02_net").isObject())
        return;
    auto net = netJsonObj.value("02_net").toObject();

    if (!net.contains("01_express") || !net.value("01_express").isObject())
        return;
    auto express = net.value("01_express").toObject();

    if (!express.contains("01_net") || !express.value("01_net").isString())
        return;
    info.net = express.value("01_net").toString();
    if (info.net != netId)
        return;

    if (!express.contains("02_outline") || !express.value("02_outline").isString())
        return;
    info.outline = express.value("02_outline").toString();

    if (!express.contains("03_specify") || !express.value("03_specify").isString())
        return;
    info.specify = express.value("03_specify").toString();

    if (!express.contains("04_std") || !express.value("04_std").isBool())
        return;
    info.std = express.value("04_std").toBool();

    if (!express.contains("05_shape") || !express.value("05_shape").isString())
        return;
    info.shape = express.value("05_shape").toString();

    if (!express.contains("06_r") || !express.value("06_r").isDouble())
        return;
    info.r = express.value("06_r").toInt();

    if (!express.contains("07_g") || !express.value("07_g").isDouble())
        return;
    info.g = express.value("07_g").toInt();

    if (!express.contains("08_b") || !express.value("08_b").isDouble())
        return;
    info.b = express.value("08_b").toInt();

    if (!express.contains("09_in") || !express.value("09_in").isDouble())
        return;
    info.in = express.value("09_in").toInt();

    if (!express.contains("10_out") || !express.value("10_out").isDouble())
        return;
    info.out = express.value("10_out").toInt();

    if (!express.contains("11_params") || !express.value("11_params").isObject())
        return;
    info.expressParams = express.value("11_params").toObject();

    info.atomic = !(net.contains("02_logic") && net.value("02_logic").isObject());
    if (!info.atomic)
    {
        auto logic = net.value("02_logic").toObject();

        auto val = logic.find("01_items").value();
        if (!val.isArray())
            return;

        QJsonArray jsonParamsItems;
        auto items = val.toArray();
        for(auto itor = items.begin(); itor != items.end(); ++itor)
        {
            auto valRef = itor[0];
            if (!valRef.isObject())
                continue;

            auto jsonItem = valRef.toObject();

            if (!jsonItem.contains("17_params") || !jsonItem.value("17_params").isObject())
                continue;
            jsonParamsItems.append(jsonItem.value("17_params").toObject());
        }

        info.logicParams["01_items"] = jsonParamsItems;
    }

    m_netMap[info.net] = info;
}

void CNetViewMgr::clearAll()
{
    m_editView->scene()->clear();
    m_editView->update();

    m_items.clear();
    m_arrows.clear();

    m_itemPointsMap.clear();
    m_itemIndexs.clear();
    initGraph();

    m_arrowPointsMap.clear();

    m_sn = 0;

    m_inChannelCnt = 0;
    m_outChannelCnt = 0;

    m_inputItems.clear();
    m_outputItems.clear();

    m_netInuse.clear();
}

void CNetViewMgr::showItems(QJsonArray const &items)
{
    m_netInuse.clear();
    for(auto itor = items.begin(); itor != items.end(); ++itor)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_shape") || !jsonItem.value("01_shape").isString())
            continue;
        auto shape = jsonItem.value("01_shape").toString();


        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto sn = asSn(jsonItem.value("02_sn").toInt());


        if (!jsonItem.contains("03_x") || !jsonItem.value("03_x").isDouble())
            continue;
        auto x = jsonItem.value("03_x").toInt();

        if (!jsonItem.contains("04_y") || !jsonItem.value("04_y").isDouble())
            continue;
        auto y = jsonItem.value("04_y").toInt();

        if (!jsonItem.contains("05_w") || !jsonItem.value("05_w").isDouble())
            continue;
        auto w = jsonItem.value("05_w").toDouble();

        if (!jsonItem.contains("06_h") || !jsonItem.value("06_h").isDouble())
            continue;
        auto h = jsonItem.value("06_h").toDouble();


        if (!jsonItem.contains("07_net") || !jsonItem.value("07_net").isString())
            continue;
        auto netId = jsonItem.value("07_net").toString();
        m_netInuse.insert(netId);

        if (!jsonItem.contains("08_name") || !jsonItem.value("08_name").isString())
            continue;
        auto netName = jsonItem.value("08_name").toString();


        if (!jsonItem.contains("09_r") || !jsonItem.value("09_r").isDouble())
            continue;
        auto r = jsonItem.value("09_r").toInt();

        if (!jsonItem.contains("10_g") || !jsonItem.value("10_g").isDouble())
            continue;
        auto g = jsonItem.value("10_g").toInt();

        if (!jsonItem.contains("11_b") || !jsonItem.value("11_b").isDouble())
            continue;
        auto b = jsonItem.value("11_b").toInt();


        if (!jsonItem.contains("12_in") || !jsonItem.value("12_in").isDouble())
            continue;
        auto in = jsonItem.value("12_in").toInt();

        if (!jsonItem.contains("13_out") || !jsonItem.value("13_out").isDouble())
            continue;
        auto out = jsonItem.value("13_out").toInt();

        if (!jsonItem.contains("14_asInput") || !jsonItem.value("14_asInput").isBool())
            continue;
        auto asInput = jsonItem.value("14_asInput").toBool();

        if (!jsonItem.contains("15_asOutput") || !jsonItem.value("15_asOutput").isBool())
            continue;
        auto asOutput = jsonItem.value("15_asOutput").toBool();

        Points_t points;
        if (!jsonItem.contains("16_points") || !jsonItem.value("16_points").isArray())
            continue;
        auto jsonPoints = jsonItem.value("16_points").toArray();
        for(auto itor = jsonPoints.begin(); itor != jsonPoints.end(); ++itor)
        {
            auto valRef = itor[0];
            if (!valRef.isObject())
                continue;

            PointInfo point;
            auto jsonPoint = valRef.toObject();

            if (!jsonPoint.contains("01_sn") || !jsonPoint.value("01_sn").isDouble())
                continue;
            point.sn = asSn(jsonPoint.value("01_sn").toInt());

            if (!jsonPoint.contains("02_owner") || !jsonPoint.value("02_owner").isDouble())
                continue;
            point.owner = jsonPoint.value("02_owner").toInt();

            if (!jsonPoint.contains("03_peer") || !jsonPoint.value("03_peer").isDouble())
                continue;
            point.peer = jsonPoint.value("03_peer").toInt();

            if (!jsonPoint.contains("04_arrow") || !jsonPoint.value("04_arrow").isDouble())
                continue;
            point.arrow = jsonPoint.value("04_arrow").toInt();

            if (!jsonPoint.contains("05_channel") || !jsonPoint.value("05_channel").isDouble())
                continue;
            point.channel = jsonPoint.value("05_channel").toInt();

            if (!jsonPoint.contains("06_role") || !jsonPoint.value("06_role").isDouble())
                continue;
            point.role = static_cast<ROLE>(jsonPoint.value("06_role").toInt());

            if (!jsonPoint.contains("07_x") || !jsonPoint.value("07_x").isDouble())
                continue;
            point.pos.setX(jsonPoint.value("07_x").toDouble());

            if (!jsonPoint.contains("08_y") || !jsonPoint.value("08_y").isDouble())
                continue;
            point.pos.setY(jsonPoint.value("08_y").toDouble());

            if (!jsonPoint.contains("09_channels") || !jsonPoint.value("09_channels").isArray())
                continue;
            auto jsonChannelPairs = jsonPoint.value("09_channels").toArray();
            for(auto itor = jsonChannelPairs.begin(); itor != jsonChannelPairs.end(); ++itor)
            {
                auto valRef = itor[0];
                if (!valRef.isObject())
                    continue;

                pair<int, int> channelPair;
                auto jsonChannelPair = valRef.toObject();

                if (!jsonChannelPair.contains("1_outChannel") || !jsonChannelPair.value("1_outChannel").isDouble())
                    continue;
                channelPair.first = jsonChannelPair.value("1_outChannel").toInt();

                if (!jsonChannelPair.contains("2_inChannel") || !jsonChannelPair.value("2_inChannel").isDouble())
                    continue;
                channelPair.second = jsonChannelPair.value("2_inChannel").toInt();

                point.channels.push_back(channelPair);

                if (point.role == e_input)
                {
                    if (channelPair.first >= m_outChannelCnt)
                        m_outChannelCnt = channelPair.first + 1;
                }
                else if (point.role == e_output)
                {
                    if (channelPair.second >= m_inChannelCnt)
                        m_inChannelCnt = channelPair.second + 1;
                }
            }

            points.push_back(point);
            m_arrowPointsMap[point.arrow].push_back(point);
        }

        if (!jsonItem.contains("17_params") || !jsonItem.value("17_params").isObject())
            continue;
        auto aiParams = jsonItem.value("17_params").toObject();

        auto color = QColor(r, g, b);
        auto scenePos = m_editView->mapToScene(QPoint(x, y));

        if (shape == "rect")
        {
            auto rect = genRectItem(true, sn, netId, netName, color, w, h);
            rect->SetIn(in);
            rect->SetOut(out);
            rect->AsInput(asInput);
            rect->AsOutput(asOutput);
            rect->SetPoints(points);
            rect->SetAiParams(aiParams);

            if (asInput)
                m_inputItems.push_back(rect);

            if (asOutput)
                m_outputItems.push_back(rect);

            m_itemPointsMap[sn].item = rect;
            m_itemPointsMap[sn].points = points;

            addItem(true, rect, scenePos);

            showInfoSignal(tr("Add ") + rect->GetNetName() + tr("."));
        } //rect
        else if (shape == "circle")
        {
            auto ellipse = genEllipseItem(true, sn, netId, netName, color, w, h);
            ellipse->SetIn(in);
            ellipse->SetOut(out);
            ellipse->AsInput(asInput);
            ellipse->AsOutput(asOutput);
            ellipse->SetPoints(points);
            ellipse->SetAiParams(aiParams);

            if (asInput)
                m_inputItems.push_back(ellipse);

            if (asOutput)
                m_outputItems.push_back(ellipse);

            m_itemPointsMap[sn].item = ellipse;
            m_itemPointsMap[sn].points = points;

            addItem(true, ellipse, scenePos);

            showInfoSignal(tr("Add ") + ellipse->GetNetName() + tr("."));
        } //ellipse

    } //for
}

void CNetViewMgr::showArrows()
{
    for (auto &item : m_arrowPointsMap)
    {
        auto const &points = item.second;
        if (points.size() != 2)
            continue;

        auto beginItem = m_itemPointsMap[points[0].owner].item;
        for (auto &beginPoint : m_itemPointsMap[points[0].owner].points)
        {
            if (beginPoint.arrow != points[0].arrow)
                continue;

            auto endItem = m_itemPointsMap[points[1].owner].item;
            for (auto &endPoint : m_itemPointsMap[points[1].owner].points)
            {
                if (endPoint.arrow != points[1].arrow)
                    continue;

                auto beginPos = beginItem->mapToScene(beginPoint.pos);
                auto endPos = endItem->mapToScene(endPoint.pos);

                PointInfo beginInfo, endInfo;
                if (points[0].role == e_src && points[1].role == e_dst)
                {
                    beginInfo = beginPoint;
                    endInfo = endPoint;
                }
                else
                {
                    auto tempItem = beginItem;
                    beginItem = endItem;
                    endItem = tempItem;

                    auto tempPos = beginPos;
                    beginPos = endPos;
                    endPos = tempPos;

                    beginInfo = endPoint;
                    endInfo = beginPoint;
                }

                beginInfo.pos = beginPos;
                endInfo.pos = endPos;

                auto arrowItem = new CGraphicsArrowItem(true, asSn(item.first), beginPos.x(), beginPos.y(), endPos.x(), endPos.y(), QColor(18, 157, 221));
                arrowItem->AddPoint(beginInfo);
                arrowItem->AddPoint(endInfo);
                connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                m_editView->scene()->addItem(arrowItem);
                m_arrows.push_back(arrowItem);
                arrowItem->update();

                emit showInfoSignal(tr("Add arrow") + QString::number(arrowItem->GetSn()) + tr("."));

                break;
            }

            break;
        }
    }
}

void CNetViewMgr::showInputsOutputs()
{
    for (auto inputItem : m_inputItems)
    {
        if (inputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(inputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetInputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(true, asSn(point.arrow), rect->GetInputArrowBeginPos().x(), rect->GetInputArrowBeginPos().y(), rect->GetInputArrowEndPos().x(), rect->GetInputArrowEndPos().y(), Qt::green);
                connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_editView->scene()->addItem(arrowItem);
                m_arrows.push_back(arrowItem);
                arrowItem->update();

                emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
            }
        }
        else if (inputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(inputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetInputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(true, asSn(point.arrow), ellipse->GetInputArrowBeginPos().x(), ellipse->GetInputArrowBeginPos().y(), ellipse->GetInputArrowEndPos().x(), ellipse->GetInputArrowEndPos().y(), Qt::green);
                connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_editView->scene()->addItem(arrowItem);
                m_arrows.push_back(arrowItem);
                arrowItem->update();

                emit showInfoSignal(tr("Add input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
            }
        }
    }

    for (auto outputItem : m_outputItems)
    {
        if (outputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(outputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetOutputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(true, asSn(point.arrow), rect->GetOutputArrowBeginPos().x(), rect->GetOutputArrowBeginPos().y(), rect->GetOutputArrowEndPos().x(), rect->GetOutputArrowEndPos().y(), Qt::blue);
                connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_editView->scene()->addItem(arrowItem);
                m_arrows.push_back(arrowItem);
                arrowItem->update();

                emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
            }
        }
        else if (outputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(outputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetOutputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(true, asSn(point.arrow), ellipse->GetOutputArrowBeginPos().x(), ellipse->GetOutputArrowBeginPos().y(), ellipse->GetOutputArrowEndPos().x(), ellipse->GetOutputArrowEndPos().y(), Qt::blue);
                connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_editView->scene()->addItem(arrowItem);
                m_arrows.push_back(arrowItem);
                arrowItem->update();

                emit showInfoSignal(tr("Add output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
            }
        }
    }
}

void CNetViewMgr::showToView(QJsonObject const &netJsonObj)
{
    clearAll();

    auto val = netJsonObj.find("01_version").value();
    if (!val.isString())
        return;
    auto version = val.toString();

    val = netJsonObj.find("02_net").value();
    if (!val.isObject())
        return;
    auto net = val.toObject();

    val = net.find("01_express").value();
    if (!val.isObject())
        return;
    auto express = val.toObject();

    val = net.find("02_logic").value();
    if (!val.isObject())
        return;
    auto logic = val.toObject();

    val = logic.find("01_items").value();
    if (!val.isArray())
        return;

    showItems(val.toArray());
    updateVertexTable(m_itemPointsMap, m_itemIndexs, m_graph);
    showArrows();
    showInputsOutputs();

    m_editView->update();
}

int CNetViewMgr::asSn(int sn)
{
    if (sn > m_sn)
        m_sn = sn;

    return sn;
}

void CNetViewMgr::showViewClearAll()
{
    m_showView->scene()->clear();
    m_showView->update();

    m_showItems.clear();
    m_showArrows.clear();

    m_showItemPointsMap.clear();
    m_showArrowPointsMap.clear();

    m_showInputItems.clear();
    m_showOutputItems.clear();
}

void CNetViewMgr::showViewItems(QJsonArray const &items, QJsonArray const &aiParamItems)
{
    if (items.size() != aiParamItems.size())
        return;

    for(auto itor = items.begin(); itor != items.end(); ++itor)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_shape") || !jsonItem.value("01_shape").isString())
            continue;
        auto shape = jsonItem.value("01_shape").toString();


        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto sn = jsonItem.value("02_sn").toInt();


        if (!jsonItem.contains("03_x") || !jsonItem.value("03_x").isDouble())
            continue;
        auto x = jsonItem.value("03_x").toInt();

        if (!jsonItem.contains("04_y") || !jsonItem.value("04_y").isDouble())
            continue;
        auto y = jsonItem.value("04_y").toInt();

        if (!jsonItem.contains("05_w") || !jsonItem.value("05_w").isDouble())
            continue;
        auto w = jsonItem.value("05_w").toDouble();

        if (!jsonItem.contains("06_h") || !jsonItem.value("06_h").isDouble())
            continue;
        auto h = jsonItem.value("06_h").toDouble();


        if (!jsonItem.contains("07_net") || !jsonItem.value("07_net").isString())
            continue;
        auto netId = jsonItem.value("07_net").toString();

        if (!jsonItem.contains("08_name") || !jsonItem.value("08_name").isString())
            continue;
        auto netName = jsonItem.value("08_name").toString();


        if (!jsonItem.contains("09_r") || !jsonItem.value("09_r").isDouble())
            continue;
        auto r = jsonItem.value("09_r").toInt();

        if (!jsonItem.contains("10_g") || !jsonItem.value("10_g").isDouble())
            continue;
        auto g = jsonItem.value("10_g").toInt();

        if (!jsonItem.contains("11_b") || !jsonItem.value("11_b").isDouble())
            continue;
        auto b = jsonItem.value("11_b").toInt();


        if (!jsonItem.contains("12_in") || !jsonItem.value("12_in").isDouble())
            continue;
        auto in = jsonItem.value("12_in").toInt();

        if (!jsonItem.contains("13_out") || !jsonItem.value("13_out").isDouble())
            continue;
        auto out = jsonItem.value("13_out").toInt();

        if (!jsonItem.contains("14_asInput") || !jsonItem.value("14_asInput").isBool())
            continue;
        auto asInput = jsonItem.value("14_asInput").toBool();

        if (!jsonItem.contains("15_asOutput") || !jsonItem.value("15_asOutput").isBool())
            continue;
        auto asOutput = jsonItem.value("15_asOutput").toBool();

        Points_t points;
        if (!jsonItem.contains("16_points") || !jsonItem.value("16_points").isArray())
            continue;
        auto jsonPoints = jsonItem.value("16_points").toArray();
        for(auto itor = jsonPoints.begin(); itor != jsonPoints.end(); ++itor)
        {
            auto valRef = itor[0];
            if (!valRef.isObject())
                continue;

            PointInfo point;
            auto jsonPoint = valRef.toObject();

            if (!jsonPoint.contains("01_sn") || !jsonPoint.value("01_sn").isDouble())
                continue;
            point.sn = jsonPoint.value("01_sn").toInt();

            if (!jsonPoint.contains("02_owner") || !jsonPoint.value("02_owner").isDouble())
                continue;
            point.owner = jsonPoint.value("02_owner").toInt();

            if (!jsonPoint.contains("03_peer") || !jsonPoint.value("03_peer").isDouble())
                continue;
            point.peer = jsonPoint.value("03_peer").toInt();

            if (!jsonPoint.contains("04_arrow") || !jsonPoint.value("04_arrow").isDouble())
                continue;
            point.arrow = jsonPoint.value("04_arrow").toInt();

            if (!jsonPoint.contains("05_channel") || !jsonPoint.value("05_channel").isDouble())
                continue;
            point.channel = jsonPoint.value("05_channel").toInt();

            if (!jsonPoint.contains("06_role") || !jsonPoint.value("06_role").isDouble())
                continue;
            point.role = static_cast<ROLE>(jsonPoint.value("06_role").toInt());

            if (!jsonPoint.contains("07_x") || !jsonPoint.value("07_x").isDouble())
                continue;
            point.pos.setX(jsonPoint.value("07_x").toDouble());

            if (!jsonPoint.contains("08_y") || !jsonPoint.value("08_y").isDouble())
                continue;
            point.pos.setY(jsonPoint.value("08_y").toDouble());

            if (!jsonPoint.contains("09_channels") || !jsonPoint.value("09_channels").isArray())
                continue;
            auto jsonChannelPairs = jsonPoint.value("09_channels").toArray();
            for(auto itor = jsonChannelPairs.begin(); itor != jsonChannelPairs.end(); ++itor)
            {
                auto valRef = itor[0];
                if (!valRef.isObject())
                    continue;

                pair<int, int> channelPair;
                auto jsonChannelPair = valRef.toObject();

                if (!jsonChannelPair.contains("1_outChannel") || !jsonChannelPair.value("1_outChannel").isDouble())
                    continue;
                channelPair.first = jsonChannelPair.value("1_outChannel").toInt();

                if (!jsonChannelPair.contains("2_inChannel") || !jsonChannelPair.value("2_inChannel").isDouble())
                    continue;
                channelPair.second = jsonChannelPair.value("2_inChannel").toInt();

                point.channels.push_back(channelPair);
            }

            points.push_back(point);
            m_showArrowPointsMap[point.arrow].push_back(point);
        }

        auto aiParams = getNetAiParams(sn, netId, aiParamItems);
        if (aiParams.isEmpty())
            continue;

        auto color = QColor(r, g, b);
        auto scenePos = m_showView->mapToScene(QPoint(x, y));

        if (shape == "rect")
        {
            auto rect = genRectItem(false, sn, netId, netName, color, w, h);
            rect->SetIn(in);
            rect->SetOut(out);
            rect->AsInput(asInput);
            rect->AsOutput(asOutput);
            rect->SetPoints(points);
            rect->SetAiParams(aiParams);

            if (asInput)
                m_showInputItems.push_back(rect);

            if (asOutput)
                m_showOutputItems.push_back(rect);

            m_showItemPointsMap[sn].item = rect;
            m_showItemPointsMap[sn].points = points;

            addItem(false, rect, scenePos);
        } //rect
        else if (shape == "circle")
        {
            auto ellipse = genEllipseItem(false, sn, netId, netName, color, w, h);
            ellipse->SetIn(in);
            ellipse->SetOut(out);
            ellipse->AsInput(asInput);
            ellipse->AsOutput(asOutput);
            ellipse->SetPoints(points);
            ellipse->SetAiParams(aiParams);

            if (asInput)
                m_showInputItems.push_back(ellipse);

            if (asOutput)
                m_showOutputItems.push_back(ellipse);

            m_showItemPointsMap[sn].item = ellipse;
            m_showItemPointsMap[sn].points = points;

            addItem(false, ellipse, scenePos);
        } //ellipse

    } //for
}

void CNetViewMgr::showViewArrows()
{
    for (auto &item : m_showArrowPointsMap)
    {
        auto const &points = item.second;
        if (points.size() != 2)
            continue;

        auto beginItem = m_showItemPointsMap[points[0].owner].item;
        for (auto &beginPoint : m_showItemPointsMap[points[0].owner].points)
        {
            if (beginPoint.arrow != points[0].arrow)
                continue;

            auto endItem = m_showItemPointsMap[points[1].owner].item;
            for (auto &endPoint : m_showItemPointsMap[points[1].owner].points)
            {
                if (endPoint.arrow != points[1].arrow)
                    continue;

                auto beginPos = beginItem->mapToScene(beginPoint.pos);
                auto endPos = endItem->mapToScene(endPoint.pos);

                PointInfo beginInfo, endInfo;
                if (points[0].role == e_src && points[1].role == e_dst)
                {
                    beginInfo = beginPoint;
                    endInfo = endPoint;
                }
                else
                {
                    auto tempItem = beginItem;
                    beginItem = endItem;
                    endItem = tempItem;

                    auto tempPos = beginPos;
                    beginPos = endPos;
                    endPos = tempPos;

                    beginInfo = endPoint;
                    endInfo = beginPoint;
                }

                beginInfo.pos = beginPos;
                endInfo.pos = endPos;

                auto arrowItem = new CGraphicsArrowItem(false, item.first, beginPos.x(), beginPos.y(), endPos.x(), endPos.y(), QColor(18, 157, 221));
                arrowItem->AddPoint(beginInfo);
                arrowItem->AddPoint(endInfo);
                //connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                //connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                m_showView->scene()->addItem(arrowItem);
                m_showArrows.push_back(arrowItem);
                arrowItem->update();

                break;
            }

            break;
        }
    }
}

void CNetViewMgr::showViewInputsOutputs()
{
    for (auto inputItem : m_showInputItems)
    {
        if (inputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(inputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetInputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(false, point.arrow, rect->GetInputArrowBeginPos().x(), rect->GetInputArrowBeginPos().y(), rect->GetInputArrowEndPos().x(), rect->GetInputArrowEndPos().y(), Qt::green);
                //connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                //connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_showView->scene()->addItem(arrowItem);
                m_showArrows.push_back(arrowItem);
                arrowItem->update();
            }
        }
        else if (inputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(inputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetInputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(false, point.arrow, ellipse->GetInputArrowBeginPos().x(), ellipse->GetInputArrowBeginPos().y(), ellipse->GetInputArrowEndPos().x(), ellipse->GetInputArrowEndPos().y(), Qt::green);
                //connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                //connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_showView->scene()->addItem(arrowItem);
                m_showArrows.push_back(arrowItem);
                arrowItem->update();
            }
        }
    }

    for (auto outputItem : m_showOutputItems)
    {
        if (outputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(outputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetOutputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(false, point.arrow, rect->GetOutputArrowBeginPos().x(), rect->GetOutputArrowBeginPos().y(), rect->GetOutputArrowEndPos().x(), rect->GetOutputArrowEndPos().y(), Qt::blue);
                //connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                //connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_showView->scene()->addItem(arrowItem);
                m_showArrows.push_back(arrowItem);
                arrowItem->update();
            }
        }
        else if (outputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(outputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetOutputPoint(point))
            {
                auto arrowItem = new CGraphicsArrowItem(false, point.arrow, ellipse->GetOutputArrowBeginPos().x(), ellipse->GetOutputArrowBeginPos().y(), ellipse->GetOutputArrowEndPos().x(), ellipse->GetOutputArrowEndPos().y(), Qt::blue);
                //connect(arrowItem, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
                //connect(arrowItem, SIGNAL(signalEditChannel(int, int, bool, int, QPoint const&)), this, SLOT(editArrowChannelSlot(int, int, bool, int, QPoint const&)));

                arrowItem->AddPoint(point);
                m_showView->scene()->addItem(arrowItem);
                m_showArrows.push_back(arrowItem);
                arrowItem->update();
            }
        }
    }
}

void CNetViewMgr::showView(QJsonObject const &netJsonObj, QJsonObject const &aiParams)
{
    showViewClearAll();

    auto val = netJsonObj.find("01_version").value();
    if (!val.isString())
        return;
    auto version = val.toString();

    val = netJsonObj.find("02_net").value();
    if (!val.isObject())
        return;
    auto net = val.toObject();

    val = net.find("01_express").value();
    if (!val.isObject())
        return;
    auto express = val.toObject();

    val = net.find("02_logic").value();
    if (!val.isObject())
        return;
    auto logic = val.toObject();

    val = logic.find("01_items").value();
    if (!val.isArray())
        return;

    auto aiParamVal = aiParams.find("04_logic").value();
    if (!aiParamVal.isObject())
        return;
    auto aiParamLogic = aiParamVal.toObject();

    aiParamVal = aiParamLogic.find("01_items").value();
    if (!aiParamVal.isArray())
        return;

    showViewItems(val.toArray(), aiParamVal.toArray());
    updateVertexTable(m_showItemPointsMap, m_showItemIndexs, m_showGraph);
    tuoPuOrder(m_showGraph, m_showTuoPuOrders);
    showViewArrows();
    showViewInputsOutputs();

    m_showView->update();
}

QJsonObject CNetViewMgr::genJsonObj(NetInfo const &info)
{
    QJsonObject jsonNet;

    QJsonObject jsonExpress;

    jsonExpress["01_net"] = info.net;
    jsonExpress["02_outline"] = info.outline;
    jsonExpress["03_specify"] = info.specify;

    jsonExpress["04_std"] = info.std;

    jsonExpress["05_shape"] = info.shape;

    jsonExpress["06_r"] = info.r;
    jsonExpress["07_g"] = info.g;
    jsonExpress["08_b"] = info.b;

    jsonExpress["09_in"] = info.in;
    jsonExpress["10_out"] = info.out;

    jsonExpress["11_params"] = info.expressParams;


    jsonNet["01_express"] = jsonExpress;

    QJsonObject jsonLogic;
    QJsonArray jsonItems;

    m_netInuse.clear();
    tuoPuOrder(m_graph, m_tuoPuOrders);
    for (auto index = 0; index < (int)m_tuoPuOrders.size(); index++)
    {
        if (m_tuoPuOrders[index] < 0 || m_tuoPuOrders[index] >= (int)m_items.size())
            continue;

        auto item = m_items[m_tuoPuOrders[index]];
        if (item == nullptr)
            continue;

        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem const*>(item);
            if (rect == nullptr)
                continue;

            auto pos = m_editView->mapFromScene(rect->mapToScene(rect->rect().topLeft()));

            QJsonObject jsonItem;

            jsonItem["01_shape"] = "rect";

            jsonItem["02_sn"] = rect->GetSn();

            jsonItem["03_x"] = pos.x();
            jsonItem["04_y"] = pos.y();
            jsonItem["05_w"] = rect->rect().width();
            jsonItem["06_h"] = rect->rect().height();

            m_netInuse.insert(rect->GetNetId());
            jsonItem["07_net"] = rect->GetNetId();
            jsonItem["08_name"] = rect->GetNetName();

            jsonItem["09_r"] = rect->GetColor().red();
            jsonItem["10_g"] = rect->GetColor().green();
            jsonItem["11_b"] = rect->GetColor().blue();

            jsonItem["12_in"] = rect->GetIn();
            jsonItem["13_out"] = rect->GetOut();
            jsonItem["14_asInput"] = rect->AsInput();
            jsonItem["15_asOutput"] = rect->AsOutput();

            QJsonArray jsonPoints;
            for (auto const &point : rect->GetPoints())
            {
                QJsonObject jsonPoint;

                jsonPoint["01_sn"] = point.sn;
                jsonPoint["02_owner"] = point.owner;
                jsonPoint["03_peer"] = point.peer;
                jsonPoint["04_arrow"] = point.arrow;
                jsonPoint["05_channel"] = point.channel;
                jsonPoint["06_role"] = point.role;
                jsonPoint["07_x"] = point.pos.x();
                jsonPoint["08_y"] = point.pos.y();

                QJsonArray jsonChannelPairs;
                for (auto const &channelPair : point.channels)
                {
                    QJsonObject jsonChannelPair;

                    jsonChannelPair["1_outChannel"] = channelPair.first;
                    jsonChannelPair["2_inChannel"] = channelPair.second;

                    jsonChannelPairs.append(jsonChannelPair);
                }
                jsonPoint["09_channels"] = jsonChannelPairs;

                jsonPoints.append(jsonPoint);
            }
            jsonItem["16_points"] = jsonPoints;

            jsonItem["17_params"] = rect->GetAiParams();

            jsonItems.append(jsonItem);
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem const*>(item);
            if (ellipse == nullptr)
                continue;

            auto pos = m_editView->mapFromScene(ellipse->mapToScene(ellipse->rect().topLeft()));

            QJsonObject jsonItem;

            jsonItem["01_shape"] = "circle";

            jsonItem["02_sn"] = ellipse->GetSn();

            jsonItem["03_x"] = pos.x();
            jsonItem["04_y"] = pos.y();
            jsonItem["05_w"] = ellipse->rect().width();
            jsonItem["06_h"] = ellipse->rect().height();

            m_netInuse.insert(ellipse->GetNetId());
            jsonItem["07_net"] = ellipse->GetNetId();
            jsonItem["08_name"] = ellipse->GetNetName();

            jsonItem["09_r"] = ellipse->GetColor().red();
            jsonItem["10_g"] = ellipse->GetColor().green();
            jsonItem["11_b"] = ellipse->GetColor().blue();

            jsonItem["12_in"] = ellipse->GetIn();
            jsonItem["13_out"] = ellipse->GetOut();
            jsonItem["14_asInput"] = ellipse->AsInput();
            jsonItem["15_asOutput"] = ellipse->AsOutput();

            QJsonArray jsonPoints;
            for (auto const &point : ellipse->GetPoints())
            {
                QJsonObject jsonPoint;

                jsonPoint["01_sn"] = point.sn;
                jsonPoint["02_owner"] = point.owner;
                jsonPoint["03_peer"] = point.peer;
                jsonPoint["04_arrow"] = point.arrow;
                jsonPoint["05_channel"] = point.channel;
                jsonPoint["06_role"] = point.role;
                jsonPoint["07_x"] = point.pos.x();
                jsonPoint["08_y"] = point.pos.y();

                QJsonArray jsonChannelPairs;
                for (auto const &channelPair : point.channels)
                {
                    QJsonObject jsonChannelPair;

                    jsonChannelPair["1_outChannel"] = channelPair.first;
                    jsonChannelPair["2_inChannel"] = channelPair.second;

                    jsonChannelPairs.append(jsonChannelPair);
                }
                jsonPoint["09_channels"] = jsonChannelPairs;

                jsonPoints.append(jsonPoint);
            }
            jsonItem["16_points"] = jsonPoints;

            jsonItem["17_params"] = ellipse->GetAiParams();

            jsonItems.append(jsonItem);
        }
    }
    jsonLogic["01_items"] = jsonItems;
    jsonNet["02_logic"] = jsonLogic;

    QJsonObject jsonObj;
    jsonObj["01_version"] = info.version;
    jsonObj["02_net"] = jsonNet;

    return jsonObj;
}

CNetViewMgr::NetInfo const* CNetViewMgr::getNetInfo(QString const &netId) const
{
    auto itor = m_netMap.find(netId);
    if (itor != m_netMap.end())
        return &itor->second;

    return nullptr;
}

QJsonObject CNetViewMgr::getNetAiParams(QGraphicsItem const *item) const
{
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem const*>(item);
        if (rect != nullptr)
            return rect->GetAiParams();
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem const*>(item);
        if (ellipse != nullptr)
            return ellipse->GetAiParams();
    }

    return QJsonObject();
}

QJsonObject CNetViewMgr::getNetAiParams(int sn, QString const &netId) const
{
    QJsonObject aiParams;

    auto netInfo = getNetInfo(netId);
    if (netInfo)
    {
        aiParams["01_net"] = netId;
        aiParams["02_sn"] = sn;
        aiParams["03_express"] = netInfo->expressParams;
        if (!netInfo->atomic)
            aiParams["04_logic"] = netInfo->logicParams;
    }

    return aiParams;
}

QJsonObject CNetViewMgr::getNetAiParams(int sn, QString const &netId, QJsonArray const &aiParamItems) const
{
    QJsonObject aiParams;

    for(auto itor = aiParamItems.begin(); itor != aiParamItems.end(); ++itor)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_net") || !jsonItem.value("01_net").isString())
            continue;
        auto aiParamNetId = jsonItem.value("01_net").toString();

        if (netId != aiParamNetId)
            continue;

        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto aiParamSn = jsonItem.value("02_sn").toInt();

        if (sn == aiParamSn)
        {
            aiParams = jsonItem;
            break;
        }
    }

    return aiParams;
}

QJsonObject CNetViewMgr::getNetAiParams(NetMark const &extendNetMark, QJsonObject const &rootAiParams, vector<NetMark> const &extendNetMarkStack) const
{
    if (extendNetMarkStack.empty())
        return rootAiParams;

    QJsonObject aiParams = rootAiParams;
    for (auto citor = extendNetMarkStack.cbegin(); citor != extendNetMarkStack.cend(); ++citor)
    {
        auto val = aiParams.find("04_logic").value();
        if (!val.isObject())
            return QJsonObject();

        auto logic = val.toObject();

        val = logic.find("01_items").value();
        if (!val.isArray())
            return QJsonObject();

        aiParams = getNetAiParams(citor->sn, citor->net, val.toArray());

        if (extendNetMark == *citor)
            break;
    }
    return aiParams;
}

void CNetViewMgr::setNetAiExpressParams(QGraphicsItem *item, QJsonObject const &expressParams)
{
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem*>(item);
        if (rect != nullptr)
        {
            auto aiParams = rect->GetAiParams();
            aiParams["03_express"] = expressParams;
            rect->SetAiParams(aiParams);
        }
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
        if (ellipse != nullptr)
        {
            auto aiParams = ellipse->GetAiParams();
            aiParams["03_express"] = expressParams;
            ellipse->SetAiParams(aiParams);
        }
    }
}

void CNetViewMgr::setNetAiParams(QGraphicsItem *item, QJsonObject const &aiParams)
{
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem*>(item);
        if (rect != nullptr)
            rect->SetAiParams(aiParams);
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
        if (ellipse != nullptr)
            ellipse->SetAiParams(aiParams);
    }
}

void CNetViewMgr::setNetAiExpressParams(int sn, QString const &netId, QJsonObject const &expressParams, QJsonObject &aiParams)
{
    auto val = aiParams.find("04_logic").value();
    if (!val.isObject())
        return;

    auto logic = val.toObject();

    val = logic.find("01_items").value();
    if (!val.isArray())
        return;

    auto index = 0;
    auto aiParamItems = val.toArray();
    for(auto itor = aiParamItems.begin(); itor != aiParamItems.end(); ++itor, index++)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_net") || !jsonItem.value("01_net").isString())
            continue;
        auto aiParamNetId = jsonItem.value("01_net").toString();

        if (netId != aiParamNetId)
            continue;

        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto aiParamSn = jsonItem.value("02_sn").toInt();

        if (sn == aiParamSn)
        {
            jsonItem["03_express"] = expressParams;
            aiParamItems[index] = jsonItem;
            break;
        }
    }

    logic["01_items"] = aiParamItems;
    aiParams["04_logic"] = logic;
}

void CNetViewMgr::setNetAiParams(QJsonObject const &aiParams, QJsonObject &upperAiParams)
{
    if (!aiParams.contains("01_net") || !aiParams.value("01_net").isString())
        return;
    auto netId = aiParams.value("01_net").toString();

    if (!aiParams.contains("02_sn") || !aiParams.value("02_sn").isDouble())
        return;
    auto sn = aiParams.value("02_sn").toInt();


    auto val = upperAiParams.find("04_logic").value();
    if (!val.isObject())
        return;

    auto logic = val.toObject();

    val = logic.find("01_items").value();
    if (!val.isArray())
        return;

    auto index = 0;
    auto aiParamItems = val.toArray();
    for(auto itor = aiParamItems.begin(); itor != aiParamItems.end(); ++itor, index++)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_net") || !jsonItem.value("01_net").isString())
            continue;
        auto aiParamNetId = jsonItem.value("01_net").toString();

        if (netId != aiParamNetId)
            continue;

        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto aiParamSn = jsonItem.value("02_sn").toInt();

        if (sn == aiParamSn)
        {
            aiParamItems[index] = aiParams;
            break;
        }
    }

    logic["01_items"] = aiParamItems;
    upperAiParams["04_logic"] = logic;
}

void CNetViewMgr::rewindAiParams(QJsonObject aiParams, QJsonObject &rootAiParams, vector<NetMark> extendNetMarkStack)
{
    while(!extendNetMarkStack.empty())
    {
        auto extendNetMark = extendNetMarkStack.back();
        auto upperAiParams = getNetAiParams(extendNetMark, rootAiParams, extendNetMarkStack);
        setNetAiParams(aiParams, upperAiParams);

        aiParams = upperAiParams;
        extendNetMarkStack.pop_back();
    }

    setNetAiParams(aiParams, rootAiParams);
}

void CNetViewMgr::showNetAiParams(bool show, int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams)
{
    auto expressParams = aiParams.value("03_express").toObject();
    emit showAttrSignal(show, sn, netId, netName, expressParams);
}

void CNetViewMgr::addItem(bool edit, QGraphicsItem *item, QPointF const &scenePos)
{
    item->setPos(scenePos);

    if (edit)
    {
        m_editView->scene()->addItem(item);
        m_items.push_back(item);
    }
    else
    {
        m_showView->scene()->addItem(item);
        m_showItems.push_back(item);
    }
}

QGraphicsItem* CNetViewMgr::genItem(QString const &netId, QColor const &color)
{
    QGraphicsItem *item = nullptr;
    if (m_pNetInfo->shape == "rect")
    {
        ++m_sn;
        auto rect = genRectItem(true, m_sn, netId, netId + "_" + QString::number(m_sn), color, ITEM_WIDTH, ITEM_HEIGHT);
        rect->SetIn(m_pNetInfo->in);
        rect->SetOut(m_pNetInfo->out);
        rect->SetAiParams(getNetAiParams(rect->GetSn(), netId));
        addVertex(rect->GetSn(), rect);

        emit showInfoSignal(tr("Add ") + rect->GetNetName() + tr("."));

        return rect;
    }
    else if (m_pNetInfo->shape == "circle")
    {
        ++m_sn;
        auto ellipse = genEllipseItem(true, m_sn, netId, netId + "_" + QString::number(m_sn), color, ITEM_WIDTH, ITEM_WIDTH);
        ellipse->SetIn(m_pNetInfo->in);
        ellipse->SetOut(m_pNetInfo->out);
        ellipse->SetAiParams(getNetAiParams(ellipse->GetSn(), netId));
        addVertex(ellipse->GetSn(), ellipse);

        emit showInfoSignal(tr("Add ") + ellipse->GetNetName() + tr("."));

        return ellipse;
    }

    m_pNetInfo = nullptr;

    return item;
}

CGraphicsRectItem* CNetViewMgr::genRectItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, int width, int height)
{
    auto rect = new CGraphicsRectItem(edit, sn, 0, 0, width, height, netId, netName, color);
    connect(rect, SIGNAL(signalMoved(bool, QGraphicsItem*)), this, SLOT(itemMovedSlot(bool, QGraphicsItem*)));

    if (edit)
    {
        connect(rect, SIGNAL(signalArrow(QGraphicsItem*, QPointF const&, int)), m_editView, SLOT(arrowSlot(QGraphicsItem*, QPointF const&, int)));
        connect(rect, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
        connect(rect, SIGNAL(signalEditNetName(QString const&, QPoint const&)), this, SLOT(editNetNameSlot(QString const&, QPoint const&)));
    }
    else
    {
        connect(rect, SIGNAL(signalShowNet(int, QString const&, QString const&, QJsonObject const&)), this, SLOT(showNetSlot(int, QString const&, QString const&, QJsonObject const&)));
        connect(rect, SIGNAL(signalExtendNet(int, QString const&, QString const&, QJsonObject const&)), this, SLOT(extendNetSlot(int, QString const&, QString const&, QJsonObject const&)));
    }

    return rect;
}

CGraphicsEllipseItem* CNetViewMgr::genEllipseItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, int width, int height)
{
    auto ellipse = new CGraphicsEllipseItem(edit, sn, 0, 0, width, height, netId, netName, color);
    connect(ellipse, SIGNAL(signalMoved(bool, QGraphicsItem*)), this, SLOT(itemMovedSlot(bool, QGraphicsItem*)));

    if (edit)
    {
        connect(ellipse, SIGNAL(signalArrow(QGraphicsItem*, QPointF const&, int)), m_editView, SLOT(arrowSlot(QGraphicsItem*, QPointF const&, int)));
        connect(ellipse, SIGNAL(signalContextMenu(QPoint const&)), this, SLOT(contextMenuSlot(QPoint const&)));
        connect(ellipse, SIGNAL(signalEditNetName(QString const&, QPoint const&)), this, SLOT(editNetNameSlot(QString const&, QPoint const&)));
    }
    else
    {
        connect(ellipse, SIGNAL(signalShowNet(int, QString const&, QString const&, QJsonObject const&)), this, SLOT(showNetSlot(int, QString const&, QString const&, QJsonObject const&)));
        connect(ellipse, SIGNAL(signalExtendNet(int, QString const&, QString const&, QJsonObject const&)), this, SLOT(extendNetSlot(int, QString const&, QString const&, QJsonObject const&)));
    }

    return ellipse;
}

void CNetViewMgr::delItem(QGraphicsItem *item)
{
    m_editView->scene()->removeItem(item);

    for (auto itor = m_items.begin(); itor != m_items.end(); ++itor)
        if (*itor == item)
        {
            m_items.erase(itor);
            break;
        }

    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem*>(item);
        if (rect != nullptr)
        {
            m_itemPointsMap.erase(rect->GetSn());
            emit showInfoSignal(tr("Del ") + rect->GetNetName() + tr("."));
        }
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
        if (ellipse != nullptr)
        {
            m_itemPointsMap.erase(ellipse->GetSn());
            emit showInfoSignal(tr("Del ") + ellipse->GetNetName() + tr("."));
        }
    }

    delete item;
}

void CNetViewMgr::addArrowPoint(CGraphicsArrowItem *arrowItem, QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel, QGraphicsItem *endItem, QPointF const &endPos, int endChannel)
{
    PointInfo info;
    info.arrow = arrowItem->GetSn();

    if (beginItem->type() == CGraphicsRectItem::Type)
    {
        auto beginRect = dynamic_cast<CGraphicsRectItem*>(beginItem);
        if (beginRect != nullptr)
        {
            if (endItem->type() == CGraphicsRectItem::Type)
            {
                auto endRect = dynamic_cast<CGraphicsRectItem*>(endItem);
                if (endRect != nullptr)
                {
                    info.sn = ++m_sn;
                    info.owner = beginRect->GetSn();
                    info.peer = endRect->GetSn();
                    info.channel = beginChannel;
                    info.role = e_src;
                    info.pos = beginRect->mapFromScene(beginPos);
                    beginRect->AddPoint(info);

                    ArrowPoint beginArrowPoint;
                    beginArrowPoint.sn = info.sn;
                    beginArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);
                    ++m_graph.arc[getIndex(m_itemIndexs, info.owner)][getIndex(m_itemIndexs, info.peer)];

                    info.pos = beginPos;
                    arrowItem->AddPoint(info);


                    info.sn = ++m_sn;
                    info.owner = endRect->GetSn();
                    info.peer = beginRect->GetSn();
                    info.channel = endChannel;
                    info.role = e_dst;
                    info.pos = endRect->mapFromScene(endPos);
                    endRect->AddPoint(info);

                    ArrowPoint endArrowPoint;
                    endArrowPoint.sn = info.sn;
                    endArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);

                    info.pos = endPos;
                    arrowItem->AddPoint(info);
                }
            }
            else if (endItem->type() == CGraphicsEllipseItem::Type)
            {
                auto endEllipse = dynamic_cast<CGraphicsEllipseItem*>(endItem);
                if (endEllipse != nullptr)
                {
                    info.sn = ++m_sn;
                    info.owner = beginRect->GetSn();
                    info.peer = endEllipse->GetSn();
                    info.channel = beginChannel;
                    info.role = e_src;
                    info.pos = beginRect->mapFromScene(beginPos);
                    beginRect->AddPoint(info);

                    ArrowPoint beginArrowPoint;
                    beginArrowPoint.sn = info.sn;
                    beginArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);
                    ++m_graph.arc[getIndex(m_itemIndexs, info.owner)][getIndex(m_itemIndexs, info.peer)];

                    info.pos = beginPos;
                    arrowItem->AddPoint(info);


                    info.sn = ++m_sn;
                    info.owner = endEllipse->GetSn();
                    info.peer = beginRect->GetSn();
                    info.channel = endChannel;
                    info.role = e_dst;
                    info.pos = endEllipse->mapFromScene(endPos);
                    endEllipse->AddPoint(info);

                    ArrowPoint endArrowPoint;
                    endArrowPoint.sn = info.sn;
                    endArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);

                    info.pos = endPos;
                    arrowItem->AddPoint(info);
                }
            }
        }
    }
    else if (beginItem->type() == CGraphicsEllipseItem::Type)
    {
        auto beginEllipse = dynamic_cast<CGraphicsEllipseItem*>(beginItem);
        if (beginEllipse != nullptr)
        {
            if (endItem->type() == CGraphicsRectItem::Type)
            {
                auto endRect = dynamic_cast<CGraphicsRectItem*>(endItem);
                if (endRect != nullptr)
                {
                    info.sn = ++m_sn;
                    info.owner = beginEllipse->GetSn();
                    info.peer = endRect->GetSn();
                    info.channel = beginChannel;
                    info.role = e_src;
                    info.pos = beginEllipse->mapFromScene(beginPos);
                    beginEllipse->AddPoint(info);

                    ArrowPoint beginArrowPoint;
                    beginArrowPoint.sn = info.sn;
                    beginArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);
                    ++m_graph.arc[getIndex(m_itemIndexs, info.owner)][getIndex(m_itemIndexs, info.peer)];

                    info.pos = beginPos;
                    arrowItem->AddPoint(info);


                    info.sn = ++m_sn;
                    info.owner = endRect->GetSn();
                    info.peer = beginEllipse->GetSn();
                    info.channel = endChannel;
                    info.role = e_dst;
                    info.pos = endRect->mapFromScene(endPos);
                    endRect->AddPoint(info);

                    ArrowPoint endArrowPoint;
                    endArrowPoint.sn = info.sn;
                    endArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);

                    info.pos = endPos;
                    arrowItem->AddPoint(info);
                }
            }
            else if (endItem->type() == CGraphicsEllipseItem::Type)
            {
                auto endEllipse = dynamic_cast<CGraphicsEllipseItem*>(endItem);
                if (endEllipse != nullptr)
                {
                    info.sn = ++m_sn;
                    info.owner = beginEllipse->GetSn();
                    info.peer = endEllipse->GetSn();
                    info.channel = beginChannel;
                    info.role = e_src;
                    info.pos = beginEllipse->mapFromScene(beginPos);
                    beginEllipse->AddPoint(info);

                    ArrowPoint beginArrowPoint;
                    beginArrowPoint.sn = info.sn;
                    beginArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);
                    ++m_graph.arc[getIndex(m_itemIndexs, info.owner)][getIndex(m_itemIndexs, info.peer)];

                    info.pos = beginPos;
                    arrowItem->AddPoint(info);


                    info.sn = ++m_sn;
                    info.owner = endEllipse->GetSn();
                    info.peer = beginEllipse->GetSn();
                    info.channel = endChannel;
                    info.role = e_dst;
                    info.pos = endEllipse->mapFromScene(endPos);
                    endEllipse->AddPoint(info);

                    ArrowPoint endArrowPoint;
                    endArrowPoint.sn = info.sn;
                    endArrowPoint.pos = info.pos;

                    m_itemPointsMap[info.owner].points.push_back(info);

                    info.pos = endPos;
                    arrowItem->AddPoint(info);
                }
            } //endEllipse
        } //beginEllipse
    } //CGraphicsEllipseItem::Type
}

void CNetViewMgr::delArrow(CGraphicsArrowItem *arrowItem)
{
    m_editView->scene()->removeItem(arrowItem);

    for (auto itor = m_arrows.begin(); itor != m_arrows.end(); ++itor)
        if (*itor == arrowItem)
        {
            m_arrows.erase(itor);
            break;
        }

    auto const &points = arrowItem->GetPoints();
    if (points.size() == 2)
    {
        auto beginItem = m_itemPointsMap[points[0].owner].item;
        delPoint(points[0].sn, m_itemPointsMap[points[0].owner].points);
        if (beginItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(beginItem);
            if (rect != nullptr)
                rect->DelPoint(points[0].sn);
        }
        else if (beginItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(beginItem);
            if (ellipse != nullptr)
                ellipse->DelPoint(points[0].sn);
        }

        auto endItem = m_itemPointsMap[points[1].owner].item;
        delPoint(points[1].sn, m_itemPointsMap[points[1].owner].points);
        if (endItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(endItem);
            if (rect != nullptr)
                rect->DelPoint(points[1].sn);
        }
        else if (endItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(endItem);
            if (ellipse != nullptr)
                ellipse->DelPoint(points[1].sn);
        }

        --m_graph.arc[getIndex(m_itemIndexs, points[0].owner)][getIndex(m_itemIndexs, points[1].owner)];

        emit showInfoSignal(tr("Del arrow") + QString::number(arrowItem->GetSn()) + tr("."));
    }
    else if (points.size() == 1)
    {
        auto item = m_itemPointsMap[points[0].owner].item;
        delPoint(points[0].sn, m_itemPointsMap[points[0].owner].points);
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                rect->DelPoint(points[0].sn);
                if (points[0].role == e_input)
                {
                    rect->AsInput(false);
                    delInputItem(rect);
                    updateInputOutChannel();

                    emit showInfoSignal(tr("Del input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    rect->AsOutput(false);
                    delOutputItem(rect);
                    updateOutputInChannel();

                    emit showInfoSignal(tr("Del output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                ellipse->DelPoint(points[0].sn);
                if (points[0].role == e_input)
                {
                    ellipse->AsInput(false);
                    delInputItem(ellipse);
                    updateInputOutChannel();

                    emit showInfoSignal(tr("Del input arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
                else
                {
                    ellipse->AsOutput(false);
                    delOutputItem(ellipse);
                    updateOutputInChannel();

                    emit showInfoSignal(tr("Del output arrow") + QString::number(arrowItem->GetSn()) + tr("."));
                }
            }
        }
    }

    delete arrowItem;
}

int CNetViewMgr::getIndex(vector<int> const &itemIndexs, int itemSn) const
{
    for (auto index = 0u; index < itemIndexs.size(); index++)
        if (itemIndexs[index] == itemSn)
            return index;

    return -1;
}

void CNetViewMgr::delPoint(int sn, Points_t &points) const
{
    for (auto itor = points.begin(); itor != points.end(); ++itor)
        if (itor->sn == sn)
        {
            points.erase(itor);
            break;
        }
}

void CNetViewMgr::addVertex(int sn, QGraphicsItem *item)
{
    m_itemPointsMap[sn].item = item;
    m_itemIndexs.push_back(sn);
    m_graph.vertexNum++;
}

void CNetViewMgr::updateVertexTable(map<int, ItemPoints> const &itemPointsMap, vector<int> &itemIndexs, Graph &graph)
{
    itemIndexs.clear();
    for (auto &itemPoints : itemPointsMap)
        itemIndexs.push_back(itemPoints.first);

    graph.vertexNum = (int)itemPointsMap.size();
    memset(graph.arc, 0, sizeof(graph.arc));
    for (auto &itemPoints : itemPointsMap)
        for (auto &point : itemPoints.second.points)
            if (point.role == e_src)
                ++graph.arc[getIndex(itemIndexs, point.owner)][getIndex(itemIndexs, point.peer)];
}

void CNetViewMgr::rebuildVertexTable()
{
    m_itemPointsMap.clear();
    for (auto &item : m_items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect == nullptr)
                continue;

            m_itemPointsMap[rect->GetSn()].item = rect;
            m_itemPointsMap[rect->GetSn()].points = rect->GetPoints();
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse == nullptr)
                continue;

            m_itemPointsMap[ellipse->GetSn()].item = ellipse;
            m_itemPointsMap[ellipse->GetSn()].points = ellipse->GetPoints();
        }
    }

    updateVertexTable(m_itemPointsMap, m_itemIndexs, m_graph);
}

set<int> CNetViewMgr::getRelatedArrowSns(QList<QGraphicsItem*> const &items) const
{
    set<int> arrowSns;
    for (auto item : items)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                auto points = rect->GetPoints();
                for (auto &point : points)
                    arrowSns.insert(point.arrow);
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                auto points = ellipse->GetPoints();
                for (auto &point : points)
                    arrowSns.insert(point.arrow);
            }
        }
    }

    return arrowSns;
}

vector<CGraphicsArrowItem*> CNetViewMgr::getArrowItems(set<int> const &arrowSns) const
{
    vector<CGraphicsArrowItem*> arrowItems;
    for (auto arrow : m_arrows)
        if (arrowSns.count(arrow->GetSn()))
            arrowItems.push_back(arrow);

    return arrowItems;
}

CGraphicsArrowItem* CNetViewMgr::getArrow(int sn) const
{
    for (auto arrowItem : m_arrows)
        if (arrowItem->GetSn() == sn)
            return arrowItem;

    return nullptr;
}

void CNetViewMgr::deleteSelectedItems()
{
    size_t cnt = 0;
    auto items = m_editView->scene()->selectedItems();
    for (auto itor = items.begin(); itor != items.end();)
        if ((*itor)->type() == CGraphicsArrowItem::Type)
        {
            auto arrowItem = dynamic_cast<CGraphicsArrowItem*>(*itor);
            if (arrowItem != nullptr)
            {
                delArrow(arrowItem);
            }
            itor = items.erase(itor);
            cnt++;
        }
        else
             ++itor;

    auto arrowItems = getArrowItems(getRelatedArrowSns(items));
    for (auto arrow : arrowItems)
    {
        delArrow(arrow);
        cnt++;
    }

    for (auto item : items)
    {
        delItem(item);
        cnt++;
    }

    updateVertexTable(m_itemPointsMap, m_itemIndexs, m_graph);

    m_editView->update();

    if (cnt)
    {
        emit showInfoSignal(tr("Del selected items: ") + QString::number(cnt) + tr("."));
        emit netChangedSignal();
    }
}

bool CNetViewMgr::hasRing(QGraphicsItem *beginItem, QGraphicsItem *endItem) const
{
    bool result = false;
    if (beginItem->type() == CGraphicsRectItem::Type)
    {
        auto beginRect = dynamic_cast<CGraphicsRectItem*>(beginItem);
        if (beginRect != nullptr)
        {
            if (endItem->type() == CGraphicsRectItem::Type)
            {
                auto endRect = dynamic_cast<CGraphicsRectItem*>(endItem);
                if (endRect != nullptr)
                {
                    auto beginId = beginRect->GetSn();
                    auto endId = endRect->GetSn();

                    ++m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                    result = tuoPu(m_graph);
                    --m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                }
            }
            else if (endItem->type() == CGraphicsEllipseItem::Type)
            {
                auto endEllipse = dynamic_cast<CGraphicsEllipseItem*>(endItem);
                if (endEllipse != nullptr)
                {
                    auto beginId = beginRect->GetSn();
                    auto endId = endEllipse->GetSn();

                    ++m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                    result = tuoPu(m_graph);
                    --m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                }
            }
        }
    }
    else if (beginItem->type() == CGraphicsEllipseItem::Type)
    {
        auto beginEllipse = dynamic_cast<CGraphicsEllipseItem*>(beginItem);
        if (beginEllipse != nullptr)
        {
            if (endItem->type() == CGraphicsRectItem::Type)
            {
                auto endRect = dynamic_cast<CGraphicsRectItem*>(endItem);
                if (endRect != nullptr)
                {
                    auto beginId = beginEllipse->GetSn();
                    auto endId = endRect->GetSn();

                    ++m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                    result = tuoPu(m_graph);
                    --m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                }
            }
            else if (endItem->type() == CGraphicsEllipseItem::Type)
            {
                auto endEllipse = dynamic_cast<CGraphicsEllipseItem*>(endItem);
                if (endEllipse != nullptr)
                {
                    auto beginId = beginEllipse->GetSn();
                    auto endId = endEllipse->GetSn();

                    ++m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                    result = tuoPu(m_graph);
                    --m_graph.arc[getIndex(m_itemIndexs, beginId)][getIndex(m_itemIndexs, endId)];
                }
            } //endEllipse
        } //beginEllipse
    } //CGraphicsEllipseItem::Type

    return !result;
}

bool CNetViewMgr::tuoPu(Graph const &g) const
{
    bool visited[MAX_VERTEX_NUM];
    int indegree[MAX_VERTEX_NUM];

    memset(visited, 0, sizeof(visited));
    memset(indegree, 0, sizeof(indegree));
    for (int i = 0; i < g.vertexNum; i++)
        for (int j = 0; j < g.vertexNum; j++)
            if (i != j && g.arc[i][j])
                indegree[j]++;

    std::stack<int> s;
    for (int i = 0; i < g.vertexNum; i++)
        if (indegree[i] == 0)
            s.push(i), visited[i] = true;

    int cnt = 0;
    while (!s.empty())
    {
        int v = s.top();
        s.pop();
        cnt++;

        for (int i = 0; i < g.vertexNum; i++)
            if (v != i && g.arc[v][i] && !visited[i])
                if (--indegree[i] == 0)
                    s.push(i), visited[i] = true;

    }

    return cnt == g.vertexNum;
}

void CNetViewMgr::tuoPuOrder(Graph const &g, vector<int> &tuoPuOrders)
{
    bool visited[MAX_VERTEX_NUM];
    int indegree[MAX_VERTEX_NUM];

    memset(visited, 0, sizeof(visited));
    memset(indegree, 0, sizeof(indegree));
    for (int i = 0; i < g.vertexNum; i++)
        for (int j = 0; j < g.vertexNum; j++)
            if (i != j && g.arc[i][j])
                indegree[j]++;

    std::deque<int> que;
    for (int i = 0; i < g.vertexNum; i++)
        if (indegree[i] == 0)
            que.push_back(i), visited[i] = true;

    tuoPuOrders.clear();
    while (!que.empty())
    {
        int v = que.front();
        tuoPuOrders.push_back(v);
        que.pop_front();

        for (int i = 0; i < g.vertexNum; i++)
            if (v != i && g.arc[v][i] && !visited[i])
                if (--indegree[i] == 0)
                    que.push_back(i), visited[i] = true;
    }
}

int CNetViewMgr::tuoPuOrder(int index) const
{
    for (auto order = 0; order < (int)m_tuoPuOrders.size(); order++)
        if (m_tuoPuOrders[order] == index)
            return order;

    return -1;
}

void CNetViewMgr::addInputItem(QGraphicsItem *item)
{
    m_inputItems.push_back(item);
}

void CNetViewMgr::delInputItem(QGraphicsItem *item)
{
    for (auto itor = m_inputItems.begin(); itor != m_inputItems.end(); ++itor)
        if (*itor == item)
        {
            m_inputItems.erase(itor);
            break;
        }
}

void CNetViewMgr::updateInputOutChannel()
{
    set<int> sorted;
    for (auto inputItem : m_inputItems)
    {
        if (inputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(inputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetInputPoint(point))
                for (auto const &channelPair : point.channels)
                    sorted.insert(channelPair.first);
        }
        else if (inputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(inputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetInputPoint(point))
                for (auto const &channelPair : point.channels)
                    sorted.insert(channelPair.first);
        }
    }

    m_channelOrders.clear();
    for (auto channel : sorted)
        m_channelOrders.push_back(channel);

    for (auto inputItem : m_inputItems)
    {
        if (inputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(inputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetInputPoint(point))
            {
                for (auto &channelPair : point.channels)
                    channelPair.first = getOrder(channelPair.first);

                rect->SetInputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_input)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetInputPoint(point);
                    arrowItem->update();
                }
            }
        }
        else if (inputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(inputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetInputPoint(point))
            {
                for (auto &channelPair : point.channels)
                    channelPair.first = getOrder(channelPair.first);

                ellipse->SetInputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_input)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetInputPoint(point);
                    arrowItem->update();
                }
            }
        }
    }

    m_outChannelCnt = (int)m_channelOrders.size();
}

void CNetViewMgr::addOutputItem(QGraphicsItem *item)
{
    m_outputItems.push_back(item);
}

void CNetViewMgr::delOutputItem(QGraphicsItem *item)
{
    for (auto itor = m_outputItems.begin(); itor != m_outputItems.end(); ++itor)
        if (*itor == item)
        {
            m_outputItems.erase(itor);
            break;
        }
}

void CNetViewMgr::updateOutputInChannel()
{
    set<int> sorted;
    for (auto outputItem : m_outputItems)
    {
        if (outputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(outputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetOutputPoint(point))
                for (auto const &channelPair : point.channels)
                    sorted.insert(channelPair.second);
        }
        else if (outputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(outputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetOutputPoint(point))
                for (auto const &channelPair : point.channels)
                    sorted.insert(channelPair.second);
        }
    }

    m_channelOrders.clear();
    for (auto channel : sorted)
        m_channelOrders.push_back(channel);

    for (auto outputItem : m_outputItems)
    {
        if (outputItem->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(outputItem);
            if (rect == nullptr)
                continue;

            PointInfo point;
            if (rect->GetOutputPoint(point))
            {
                for (auto &channelPair : point.channels)
                    channelPair.second = getOrder(channelPair.second);

                rect->SetOutputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_output)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetOutputPoint(point);
                    arrowItem->update();
                }
            }

        }
        else if (outputItem->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(outputItem);
            if (ellipse == nullptr)
                continue;

            PointInfo point;
            if (ellipse->GetOutputPoint(point))
            {
                for (auto &channelPair : point.channels)
                    channelPair.second = getOrder(channelPair.second);

                ellipse->SetOutputPoint(point);

                for (auto &itemPoint: m_itemPointsMap[point.owner].points)
                    if (itemPoint.role == e_output)
                        itemPoint = point;

                auto arrowItem = getArrow(point.arrow);
                if (arrowItem)
                {
                    arrowItem->SetOutputPoint(point);
                    arrowItem->update();
                }
            }
        }
    }

    m_inChannelCnt = (int)m_channelOrders.size();
}

int CNetViewMgr::getOrder(int channel) const
{
    for (auto index = 0u; index < m_channelOrders.size(); index++)
        if (m_channelOrders[index] == channel)
            return index;

    return -1;
}

void CNetViewMgr::delChannelPair(ChannelPairs_t &channels, int outChannel, int inChannel) const
{
    for (auto itor = channels.begin(); itor != channels.end(); ++itor)
    {
        if (itor->first == outChannel && itor->second == inChannel)
        {
            channels.erase(itor);
            break;
        }
    }
}

void CNetViewMgr::swapItemInChannel(QGraphicsItem *item, int inChannel1, int inChannel2)
{
    if (item->type() == CGraphicsRectItem::Type)
    {
        auto rect = dynamic_cast<CGraphicsRectItem*>(item);
        if (rect != nullptr)
        {
            auto points = rect->GetPoints();
            for (auto &point : points)
            {
                swapArrowInPoint(point, inChannel1, inChannel2);
            }

            rect->SetPoints(points);
            rect->update();
            m_itemPointsMap[rect->GetSn()].points = points;
        }
    }
    else if (item->type() == CGraphicsEllipseItem::Type)
    {
        auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
        if (ellipse != nullptr)
        {
            auto points = ellipse->GetPoints();
            for (auto &point : points)
            {
                swapArrowInPoint(point, inChannel1, inChannel2);
            }

            ellipse->SetPoints(points);
            ellipse->update();
            m_itemPointsMap[ellipse->GetSn()].points = points;
        }
    }
}

void CNetViewMgr::swapArrowInPoint(PointInfo &point, int inChannel1, int inChannel2)
{
    if (point.role == e_dst)
    {
        bool modifyArrow = false;
        if (point.channel == inChannel1)
        {
            modifyArrow = true;
            point.channel = inChannel2;
        }
        else if (point.channel == inChannel2)
        {
            modifyArrow = true;
            point.channel = inChannel1;
        }

        if (modifyArrow)
        {
            auto arrowItem = getArrow(point.arrow);
            if (arrowItem)
            {
                arrowItem->SetChannel(e_dst, point.channel);
                arrowItem->update();
            }
        }
    }
    else if (point.role == e_input)
    {
        swapInChannel(point.channels, inChannel1, inChannel2);
        auto inputArrowItem = getArrow(point.arrow);
        if (inputArrowItem)
        {
            inputArrowItem->SetInputPoint(point);
            inputArrowItem->update();
        }
    }
}

void CNetViewMgr::swapInChannel(ChannelPairs_t &channels, int inChannel1, int inChannel2) const
{
    for (auto &channelPair : channels)
    {
        if (channelPair.second == inChannel1)
        {
            channelPair.second = inChannel2;
        }
        else if (channelPair.second == inChannel2)
        {
            channelPair.second = inChannel1;
        }
    }
}

void CNetViewMgr::modifyOutChannel(ChannelPairs_t &channels, int oldOutChannel, int inChannel, int newOutChannel) const
{
    for (auto &channelPair : channels)
    {
        if (channelPair.first == oldOutChannel && channelPair.second == inChannel)
        {
            channelPair.first = newOutChannel;
            break;
        }
    }
}

vector<int> CNetViewMgr::getInputOutChannelShareCnt() const
{
    vector<int> shareCnt;
    shareCnt.resize(m_outChannelCnt);

    for (auto item : m_inputItems)
    {
        if (item->type() == CGraphicsRectItem::Type)
        {
            auto rect = dynamic_cast<CGraphicsRectItem*>(item);
            if (rect != nullptr)
            {
                PointInfo point;
                if (rect->GetInputPoint(point))
                {
                    for (auto const &channelPair : point.channels)
                        ++shareCnt[channelPair.first];
                }
            }
        }
        else if (item->type() == CGraphicsEllipseItem::Type)
        {
            auto ellipse = dynamic_cast<CGraphicsEllipseItem*>(item);
            if (ellipse != nullptr)
            {
                PointInfo point;
                if (ellipse->GetInputPoint(point))
                {
                    for (auto const &channelPair : point.channels)
                        ++shareCnt[channelPair.first];
                }
            }
        }
    }

    return shareCnt;
}
