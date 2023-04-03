#ifndef ATTRVIEWMGR_H
#define ATTRVIEWMGR_H

#include <QObject>
#include <vector>
#include <unordered_map>
#include "NetAiParamsDefine.h"
#include "NetMark.h"

using std::vector;
using std::unordered_map;

class QJsonObject;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;

class CAttrViewMgr: public QObject
{
    Q_OBJECT

public:
    static CAttrViewMgr& ins();

    void Init(QListWidget *listwgAttr);
    void SetAiParamsView(QTreeWidget *treeWidget);

    NetType GetNetType(QString const &netId) const;

    void clearAttr();
    void showAttr(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams);

    // 网络单元: netunit
    void decode(QJsonObject const &expressParams, NetConvParams &aiParams);
    void decode(QJsonObject const &expressParams, NetMaxPoolParams &aiParams);
    void decode(QJsonObject const &expressParams, NetAvgPoolParams &aiParams);
    void decode(QJsonObject const &expressParams, NetAdaptiveAvgPoolParams &aiParams);
    void decode(QJsonObject const &expressParams, NetConvTransposeParams &aiParams);
    void decode(QJsonObject const &expressParams, NetUpSampleParams &aiParams);
    void decode(QJsonObject const &expressParams, NetLinearParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDropOutParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDropPathParams &aiParams);
    void decode(QJsonObject const &expressParams, NetBatchNormParams &aiParams);
    void decode(QJsonObject const &expressParams, NetGroupNormParams &aiParams);
    void decode(QJsonObject const &expressParams, NetInstanceNormParams &aiParams);
    void decode(QJsonObject const &expressParams, NetLayerNormParams &aiParams);
    void decode(QJsonObject const &expressParams, NetActiveParams &aiParams);
    void decode(QJsonObject const &expressParams, NetSoftMaxParams &aiParams);
    void decode(QJsonObject const &expressParams, NetFlattenParams &aiParams);
    void decode(QJsonObject const &expressParams, NetChannelShuffleParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDetectLayer2DParams &aiParams);
    void decode(QJsonObject const &expressParams, NetCatParams &aiParams);

    // 计算单元: calunit
    void decode(QJsonObject const &expressParams, NetScaledParams &aiParams);
    void decode(QJsonObject const &expressParams, NetAddParams &aiParams);
    void decode(QJsonObject const &expressParams, NetConstParams &aiParams);
    void decode(QJsonObject const &expressParams, NetPermuteParams &aiParams);
    void decode(QJsonObject const &expressParams, NetViewParams &aiParams);
    void decode(QJsonObject const &expressParams, NetSplitParams &aiParams);
    void decode(QJsonObject const &expressParams, NetIdentityParams &aiParams);

    // 运行单元: rununit
    void decode(QJsonObject const &expressParams, NetShowResultParams &aiParams);
    void decode(QJsonObject const &expressParams, NetBaseLossParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDiceLossParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDetectYoloLossParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptAdaDeltaParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptAdaGradParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptAdamParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptASGDParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptRMSpropParams &aiParams);
    void decode(QJsonObject const &expressParams, NetOptSGDParams &aiParams);
    void decode(QJsonObject const &expressParams, NetTrain2DParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDataLoader2DParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDataInputTestParams &aiParams);
    void decode(QJsonObject const &expressParams, NetDataOutputParams &aiParams);

signals:
    void showInfoSignal(QString const &info);
    void checkNetInfoSignal(QString const &netId, bool &atomic);
    void paramsChangedSignal(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams);
    void treeParamsChangedSignal(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams, vector<NetMark> extendNetMarkStack);
    void editTreeParamSignal(int sn, QString const &netId, QString const &netName, QString const &field, QString const &value, vector<QString> const &params);

public slots:
    void showAttrTreeSlot(QJsonObject const &aiParams);
    void markNetSlot(int sn, QString const &netId, QString const &netName, vector<NetMark> extendNetMarkStack);
    void treeParamsChangedSlot(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams);

private slots:
    void updateListSlot(int sn, QString const &netName, QString const &netId);
    void paramsChangedSlot(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams);
    void treeItemDoubleClickedSlot(QTreeWidgetItem *item, int column);

private:
    CAttrViewMgr();
    ~CAttrViewMgr();

    CAttrViewMgr(CAttrViewMgr &&rhs) = delete;
    CAttrViewMgr(CAttrViewMgr const &rhs) = delete;

    CAttrViewMgr& operator=(CAttrViewMgr &&rhs) = delete;
    CAttrViewMgr& operator=(CAttrViewMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CAttrViewMgr *pt = 0) : p(pt){}
        CAttrViewMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CAttrViewMgr *p;
    };

    // 定制网元: custom
    void decode(QJsonObject const &expressParams, NetCustomParams &aiParams);

    void decodeParams(QString const &netId, QJsonObject const &expressParams, NetAiParams &aiParams);
    void showParams(int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams);
    void encodeParams(NetAiParams const &aiParams, QJsonObject &expressParams);

    // 网络单元: netunit
    void encode(NetConvParams const &aiParams, QJsonObject &expressParams);
    void encode(NetMaxPoolParams const &aiParams, QJsonObject &expressParams);
    void encode(NetAvgPoolParams const &aiParams, QJsonObject &expressParams);
    void encode(NetAdaptiveAvgPoolParams const &aiParams, QJsonObject &expressParams);
    void encode(NetConvTransposeParams const &aiParams, QJsonObject &expressParams);
    void encode(NetUpSampleParams const &aiParams, QJsonObject &expressParams);
    void encode(NetLinearParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDropOutParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDropPathParams const &aiParams, QJsonObject &expressParams);
    void encode(NetBatchNormParams const &aiParams, QJsonObject &expressParams);
    void encode(NetGroupNormParams const &aiParams, QJsonObject &expressParams);
    void encode(NetInstanceNormParams const &aiParams, QJsonObject &expressParams);
    void encode(NetLayerNormParams const &aiParams, QJsonObject &expressParams);
    void encode(NetActiveParams const &aiParams, QJsonObject &expressParams);
    void encode(NetSoftMaxParams const &aiParams, QJsonObject &expressParams);
    void encode(NetFlattenParams const &aiParams, QJsonObject &expressParams);
    void encode(NetChannelShuffleParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDetectLayer2DParams const &aiParams, QJsonObject &expressParams);
    void encode(NetCatParams const &aiParams, QJsonObject &expressParams);

    // 计算单元: calunit
    void encode(NetScaledParams const &aiParams, QJsonObject &expressParams);
    void encode(NetAddParams const &aiParams, QJsonObject &expressParams);
    void encode(NetConstParams const &aiParams, QJsonObject &expressParams);
    void encode(NetPermuteParams const &aiParams, QJsonObject &expressParams);
    void encode(NetViewParams const &aiParams, QJsonObject &expressParams);
    void encode(NetSplitParams const &aiParams, QJsonObject &expressParams);
    void encode(NetIdentityParams const &aiParams, QJsonObject &expressParams);

    // 运行单元: rununit
    void encode(NetShowResultParams const &aiParams, QJsonObject &expressParams);
    void encode(NetBaseLossParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDiceLossParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDetectYoloLossParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptAdaDeltaParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptAdaGradParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptAdamParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptASGDParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptRMSpropParams const &aiParams, QJsonObject &expressParams);
    void encode(NetOptSGDParams const &aiParams, QJsonObject &expressParams);
    void encode(NetTrain2DParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDataLoader2DParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDataInputTestParams const &aiParams, QJsonObject &expressParams);
    void encode(NetDataOutputParams const &aiParams, QJsonObject &expressParams);

    // 定制网元: custom
    void encode(NetCustomParams const &aiParams, QJsonObject &expressParams);


    QString toStr(bool val) const;
    QString toStr(char const* val) const;
    QString toStr(int val) const;
    QString toStr(int const* arr, int n) const;
    QString toStr(int const *tab, int rows, int cols) const;
    QString toStr(double val) const;
    QString toStr(double const *arr, int n) const;

    void doShowAttrTree(QTreeWidgetItem *parent, QJsonArray const &items);
    QTreeWidgetItem* addParamstoTree(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams);
    void addNodeParams(QTreeWidgetItem *node, NetAiParams const &aiParams);

    // 网络单元: netunit
    void addParams(QTreeWidgetItem *node, NetConvParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetMaxPoolParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetAvgPoolParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetAdaptiveAvgPoolParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetConvTransposeParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetUpSampleParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetLinearParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDropOutParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDropPathParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetBatchNormParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetGroupNormParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetInstanceNormParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetLayerNormParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetActiveParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetSoftMaxParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetFlattenParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetChannelShuffleParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDetectLayer2DParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetCatParams const &aiParams);

    // 计算单元: calunit
    void addParams(QTreeWidgetItem *node, NetScaledParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetAddParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetConstParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetPermuteParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetViewParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetSplitParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetIdentityParams const &aiParams);

    // 运行单元: rununit
    void addParams(QTreeWidgetItem *node, NetShowResultParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetBaseLossParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDiceLossParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDetectYoloLossParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptAdaDeltaParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptAdaGradParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptAdamParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptASGDParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptRMSpropParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetOptSGDParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetTrain2DParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDataLoader2DParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDataInputTestParams const &aiParams);
    void addParams(QTreeWidgetItem *node, NetDataOutputParams const &aiParams);

    // 定制网元: custom
    void addParams(QTreeWidgetItem *node, NetCustomParams const &aiParams);

    void doAddParam(QTreeWidgetItem *node, QString const &name, QString const &value);

    void clearMark(QTreeWidgetItem *parent);
    void markNet(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName, vector<NetMark> extendNetMarkStack);
    void markParams(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName);

private:
    QListWidget*                        m_listwgAttr = {nullptr};
    QTreeWidget*                        m_treeWidget = {nullptr};

    unordered_map<QString, NetType>     m_netTypeMap;

    QTreeWidgetItem*                    m_item;
    QString                             m_field;
    QString                             m_value;
    vector<NetMark>                     m_extendNetMarkStack;
};

#endif // ATTRVIEWMGR_H
