#ifndef NETVIEWMGR_H
#define NETVIEWMGR_H

#include <QObject>
#include <QJsonObject>
#include <map>
#include <set>
#include "ItemInfo.h"
#include "NetMark.h"
#include "CompileMgr.h"
#include "ProjectInfo.h"

using std::map;
using std::set;
using std::pair;

class CGraphicsView;
class QGraphicsItem;
class CGraphicsRectItem;
class CGraphicsEllipseItem;
class CGraphicsArrowItem;
class CMenuItem;

constexpr int MAX_VERTEX_NUM = 1000;

class CNetViewMgr: public QObject
{
    Q_OBJECT

    struct NetInfo
    {
        QString             version;

        QString             net;
        QString             catalog;
        QString             outline;
        QString             specify;

        bool                std;
        bool                atomic;

        QString             shape;

        int                 r;
        int                 g;
        int                 b;

        int                 in;
        int                 out;

        QJsonObject         expressParams;
        QJsonObject         logicParams;
    };

    struct ItemPoints
    {
        QGraphicsItem*      item;
        Points_t            points;
    };

    struct ArrowPoint
    {
        int                 sn;
        QPointF             pos;
    };

    using ArrowPoints = pair<ArrowPoint, ArrowPoint>;

    struct Graph
    {
        int                 vertexNum;
        int                 arc[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    };

public:
    static CNetViewMgr& ins();

    void Init(CGraphicsView *editView, CGraphicsView *showView, QString const &menuQss, QString const &version);

    void AddNet(QString const &netId, QColor const &color, QPointF const &pos);

    QJsonObject GetNet();
    void SetNet(QJsonObject const &netJsonObj);

    bool inUse(QString const &name) const;
    int GetInputChannelCnt() const;
    int GetOutputChannelCnt() const;

    set<QString> GetMissingNetIds() const;
    vector<QString> GetMatchNetNames (QString const &netId) const;
    vector<QString> GetNullInputItems() const;
    vector<QString> GetNeedFullInputItems() const;
    ProjectInfo GetProjectInfo() const;

    QString GenProjectCode(QString const &projectName, bool isTest);
    void GenCode(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString &paramCode, QString &netCode);
    QJsonObject GenNetCode(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString const &shape, QColor const &color, QString &paramCode, QString &netCode);

    bool RewindNet();

signals:
    void netChangedSignal();

    void showInfoSignal(QString const &info) const;

    void editNetNameSignal(QString const &netId, QPoint const &pos);
    void editChannelSignal(int min, int max, int channel, QPoint const &pos);

    void inputConfigSignal(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt);
    void outputConfigSignal(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt);

    void enableUpBtnSignal(bool enabled);

    void clearAttrSignal();
    void showAttrSignal(bool show, int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams);
    void showAttrTreeSignal(QJsonObject const &aiParams);
    void markNetSignal(int sn, QString const &netId, QString const &netName, vector<NetMark> extendNetMarkStack);

public slots:
    // slot for channel-edit
    void channelChangedSlot(int newChannel);

    // slot for netName-edit
    void netNameChangedSlot(const QString &netName);

    // slot for input-config
    void addInputChannelSlot(int outChannel, int inChannel);
    void delInputChannelSlot(int outChannel, int inChannel);
    void modifyInputChannelSlot(int oldOutChannel, int inChannel, int newOutChannel);
    void swapInputInChannelSlot(int inChannel1, int inChannel2);

    // slot for output-config
    void addOutputChannelSlot(int outChannel, int inChannel);
    void delOutputChannelSlot(int outChannel, int inChannel);
    void modifyOutputChannelSlot(int oldOutChannel, int inChannel, int newOutChannel);
    void swapOutputInChannelSlot(int inChannel1, int inChannel2);

    // slot for ListViewMgr
    void delNetsSlot(QStringList const &delNetIds);

    // slot for AttrViewMgr
    void checkNetInfoSlot(QString const &netId, bool &atomic);
    void paramsChangedSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams);
    void treeParamsChangedSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams, vector<NetMark> extendNetMarkStack);

    // slot for CompileMgr
    void getNetDirSlot(QString const &itemNetId, QString &itemDir);

private slots:
    // slot for edit-view
    void enableSlot(bool enabled);

    // slot for edit-view
    void addArrowSlot(CGraphicsArrowItem *arrowItem, QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel, QGraphicsItem *endItem, QPointF const &endPos, int endChannel);
    void deleteKeySlot();
    void restoreSlot();

    // slot for item
    void itemMovedSlot(bool edit, QGraphicsItem *item);

    // slot for edit-item
    void contextMenuSlot(QPoint const &pos);
    void editArrowChannelSlot(int owner, int arrow, bool isIn, int channel, QPoint const &pos);
    void editNetNameSlot(QString const &netId, QPoint const &pos);

    // slot for show-item
    void showNetSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams);
    void extendNetSlot(int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams);

    // slot for menu
    void asInputSelectedItemSlot();
    void asOutputSelectedItemSlot();
    void inputConfigSelectedItemsSlot();
    void outputConfigSelectedItemsSlot();
    void deleteSelectedItemsSlot();

    // slot for edit-scene
    void editSceneSelectionChangedSlot();
    void editSceneFocusItemChangedSlot(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);

    // slot for show-scene
    void showSceneSelectionChangedSlot();

private:
    CNetViewMgr();
    ~CNetViewMgr();

    CNetViewMgr(CNetViewMgr &&rhs) = delete;
    CNetViewMgr(CNetViewMgr const &rhs) = delete;

    CNetViewMgr& operator=(CNetViewMgr &&rhs) = delete;
    CNetViewMgr& operator=(CNetViewMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CNetViewMgr *pt = 0) : p(pt){}
        CNetViewMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CNetViewMgr *p;
    };

    void initGraph();
    void initNetInfoMap();
    void addNetInfo(QString const &catalog, QString const &netId, QJsonObject const &netJsonObj);

    void clearAll();
    void showItems(QJsonArray const &items);
    void showArrows();
    void showInputsOutputs();
    void showToView(QJsonObject const &netJsonObj);
    int asSn(int sn);

    void showViewClearAll();
    void showViewItems(QJsonArray const &items, QJsonArray const &aiParamItems);
    void showViewArrows();
    void showViewInputsOutputs();
    void showView(QJsonObject const &netJsonObj, QJsonObject const &aiParams);

    QString const& getItemNetId(QGraphicsItem const *item) const;
    CItemInfo const* getItemInfo(QGraphicsItem const *item) const;
    CItemInfo const* getItemInfo(QGraphicsItem const *item, int owner, int peer, int arrow, int &channel) const;
    CItemInfo const* getItemInfo(vector<QGraphicsItem*> items, int owner, int peer, int arrow, int &channel) const;
    vector<QGraphicsItem*> getPreposeItems(int sn) const;
    ForwardInfo genForwardInfo(int index, QString const &netId, CItemInfo const &itemInfo, vector<QGraphicsItem*> const &preposeItems) const;
    FinishInfo genFinishInfo(int outputCnt, vector<QGraphicsItem*> const &outputItems) const;
    QJsonObject genNet(QString const &catalog, QString const &netId, QString const &outline, QString const &specify, QString const &shape, QColor const &color);
    QJsonObject genJsonObj(NetInfo const &info);

    NetInfo const* getNetInfo(QString const &netId) const;

    QJsonObject getNetAiParams(QGraphicsItem const *item) const;
    QJsonObject getNetAiParams(int sn, QString const &netId) const;
    QJsonObject getNetAiParams(int sn, QString const &netId, QJsonArray const &aiParamItems) const;
    QJsonObject getNetAiParams(NetMark const &extendNetMark, QJsonObject const &rootAiParams, vector<NetMark> const &extendNetMarkStack) const;

    void setNetAiExpressParams(QGraphicsItem *item, QJsonObject const &expressParams);
    void setNetAiParams(QGraphicsItem *item, QJsonObject const &aiParams);
    void setNetAiExpressParams(int sn, QString const &netId, QJsonObject const &expressParams, QJsonObject &aiParams);
    void setNetAiParams(QJsonObject const &aiParams, QJsonObject &upperAiParams);
    void rewindAiParams(QJsonObject aiParams, QJsonObject &rootAiParams, vector<NetMark> extendNetMarkStack);

    void showNetAiParams(bool show, int sn, QString const &netId, QString const &netName, QJsonObject const &aiParams);

    void addItem(bool edit, QGraphicsItem *item, QPointF const &scenePos);
    QGraphicsItem* genItem(QString const &netId, QColor const &color);
    CGraphicsRectItem* genRectItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, int width, int height);
    CGraphicsEllipseItem* genEllipseItem(bool edit, int sn, QString const &netId, QString const &netName, QColor const &color, int width, int height);
    void delItem(QGraphicsItem *item);

    void addArrowPoint(CGraphicsArrowItem *arrowItem, QGraphicsItem *beginItem, QPointF const &beginPos, int beginChannel, QGraphicsItem *endItem, QPointF const &endPos, int endChannel);
    void delArrow(CGraphicsArrowItem *arrowItem);

    int getIndex(vector<int> const &itemIndexs, int itemSn) const;
    void delPoint(int sn, Points_t &points) const;

    void addVertex(int sn, QGraphicsItem *item);
    void updateVertexTable(map<int, ItemPoints> const &itemPointsMap, vector<int> &itemIndexs, Graph &graph);
    void rebuildVertexTable();

    set<int> getRelatedArrowSns(QList<QGraphicsItem*> const &items) const;
    vector<CGraphicsArrowItem*> getArrowItems(set<int> const &arrowSns) const;
    CGraphicsArrowItem* getArrow(int sn) const;
    void deleteSelectedItems();

    bool hasRing(QGraphicsItem *beginItem, QGraphicsItem *endItem) const;
    bool tuoPu(Graph const &g) const;
    void tuoPuOrder(Graph const &g, vector<int> &tuoPuOrders);
    int tuoPuOrder(int index) const;

    void addInputItem(QGraphicsItem *item);
    void delInputItem(QGraphicsItem *item);
    void updateInputOutChannel();

    void addOutputItem(QGraphicsItem *item);
    void delOutputItem(QGraphicsItem *item);
    void updateOutputInChannel();

    int getOrder(int channel) const;
    void delChannelPair(ChannelPairs_t &channels, int outChannel, int inChannel) const;
    void swapItemInChannel(QGraphicsItem *item, int inChannel1, int inChannel2);
    void swapArrowInPoint(PointInfo &point, int inChannel1, int inChannel2);
    void swapInChannel(ChannelPairs_t &channels, int inChannel1, int inChannel2) const;
    void modifyOutChannel(ChannelPairs_t &channels, int oldOutChannel, int inChannel, int newOutChannel) const;
    vector<int> getInputOutChannelShareCnt() const;

    bool loss(int owner, Points_t const &points) const;
    bool inflow(int owner, Points_t const &points, QString const &dstNetId) const;
    void updateProjectInfo(int sn, QString const &netId, QString const &netName, Points_t const &points, ProjectInfo &info) const;
    void getLossInfo(Points_t const &points, LossInfo &info, set<std::pair<QString, int>> &unets) const;

    bool matchStack(vector<NetMark> extendNetMarkStack) const;

private:
    CGraphicsView*                      m_editView = {nullptr};
    CGraphicsView*                      m_showView = {nullptr};

    CMenuItem*                          m_pMenuItem = {nullptr};

    QString                             m_verson;
    QString                             m_menuQss;

    int                                 m_sn = {0};

    vector<QGraphicsItem*>              m_items;
    vector<CGraphicsArrowItem*>         m_arrows;

    mutable map<int, ItemPoints>        m_itemPointsMap;
    vector<int>                         m_itemIndexs;
    mutable Graph                       m_graph;

    map<int, Points_t>                  m_arrowPointsMap;

    int                                 m_inChannelCnt = {0};
    int                                 m_outChannelCnt = {0};

    vector<QGraphicsItem*>              m_inputItems;
    vector<QGraphicsItem*>              m_outputItems;
    vector<int>                         m_channelOrders;

    bool                                m_isInChannel;
    int                                 m_editChannel;
    int                                 m_editPointOwner;
    int                                 m_editPointArrow;

    NetInfo const*                      m_pNetInfo = {nullptr};
    map<QString, NetInfo>               m_netMap;
    set<QString>                        m_netInuse;

    vector<QGraphicsItem*>              m_showItems;
    vector<CGraphicsArrowItem*>         m_showArrows;

    map<int, ItemPoints>                m_showItemPointsMap;
    vector<int>                         m_showItemIndexs;
    mutable Graph                       m_showGraph;

    map<int, Points_t>                  m_showArrowPointsMap;

    vector<QGraphicsItem*>              m_showInputItems;
    vector<QGraphicsItem*>              m_showOutputItems;

    QGraphicsItem*                      m_editFocusItem = {nullptr};
    QGraphicsItem*                      m_showFocusItem = {nullptr};

    NetMark                             m_curEditNetMark;
    NetMark                             m_curShowNetMark;
    vector<NetMark>                     m_extendNetMarkStack;

    vector<int>                         m_tuoPuOrders;
    vector<int>                         m_showTuoPuOrders;
};

#endif // NETVIEWMGR_H
