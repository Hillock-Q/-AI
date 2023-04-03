#ifndef WGATTRLIST_H
#define WGATTRLIST_H

#include <QWidget>
#include <map>
#include "NetAiParamsDefine.h"

using std::map;

namespace Ui {
class WgAttrList;
}

class WgAttrList : public QWidget
{
    Q_OBJECT

public:
    explicit WgAttrList(QWidget *parent = nullptr);
    ~WgAttrList();

    void Init(int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams);
    int GetSn() const;

signals:
    void signalUpdate(int sn, QString const &netName, QString const &netId);
    void signalParamsChanged(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams);

private slots:
    void on_tbtnAttr_clicked(bool checked);

    void TextChangedSlot(QString const &name, QString const &text);

    void selectedChangeSlot(QString const &name, QString const &selected);

    void valueChangeSlot(QString const &name, int val);

    void indexChangeSlot(QString const &name, int index, int &val);
    void valueChangeSlot(QString const &name, int index, int val);

    void rowChangeSlot(QString const &name, int rIndex, int &cIndex, int &val);
    void colChangeSlot(QString const &name, int rIndex, int cIndex, int &val);
    void valueChangeSlot(QString const &name, int rIndex, int cIndex, int val);

    void valueChangeSlot(QString const &name, double val);

    void indexChangeSlot(QString const &name, int index, double &val);
    void valueChangeSlot(QString const &name, int index, double val);

private:
    void resizeHeight();

    void addTextEditor(QString const &name, QString const &text);
    void addStrSelector(QString const &name, QStringList const &rows, QString const &selected);
    void addIntEditor(QString const &name, int min, int max, int val, int step);
    void addIntListEditor(QString const &name, int cols, int index, int min, int max, int val, int step);
    void addIntTableEditor(QString const &name, int rows, int rIndex, int cols, int cIndex, int min, int max, int val, int step);
    void addFloatEditor(QString const &name, double min, double max, double val, double step, int prec);
    void addFloatListEditor(QString const &name, int cols, int index, double min, double max, double val, double step, int prec);

    void intEditorSetMax(QString const &name, int max);
    void intListEditorReset(QString const &name, int cols);
    void floatListEditorReset(QString const &name, int cols);

    // 网络单元: netunit
    void extend(NetConvParams &aiParams);
    void handleSelectedChanged(NetConvParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetConvParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetConvParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetConvParams &aiParams, QString const &name, int index, int val);

    void extend(NetMaxPoolParams &aiParams);
    void handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int val);

    void extend(NetAvgPoolParams &aiParams);
    void handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int val);

    void extend(NetAdaptiveAvgPoolParams &aiParams);
    void handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int val);

    void extend(NetConvTransposeParams &aiParams);
    void handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int val);

    void extend(NetUpSampleParams &aiParams);
    void handleSelectedChanged(NetUpSampleParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetUpSampleParams &aiParams, QString const &name, int index, double &val);
    void handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int index, double val);

    void extend(NetLinearParams &aiParams);
    void handleSelectedChanged(NetLinearParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetLinearParams &aiParams, QString const &name, int val);

    void extend(NetDropOutParams &aiParams);
    void handleValueChanged(NetDropOutParams &aiParams, QString const &name, int val);
    void handleValueChanged(NetDropOutParams &aiParams, QString const &name, double val);

    void extend(NetDropPathParams &aiParams);
    void handleValueChanged(NetDropPathParams &aiParams, QString const &name, double val);

    void extend(NetBatchNormParams &aiParams);
    void handleValueChanged(NetBatchNormParams &aiParams, QString const &name, int val);

    void extend(NetGroupNormParams &aiParams);
    void handleValueChanged(NetGroupNormParams &aiParams, QString const &name, int val);

    void extend(NetInstanceNormParams &aiParams);
    void handleValueChanged(NetInstanceNormParams &aiParams, QString const &name, int val);

    void extend(NetLayerNormParams &aiParams);
    void handleIndexChanged(NetLayerNormParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetLayerNormParams &aiParams, QString const &name, int index, int val);

    void extend(NetActiveParams &aiParams);
    void handleSelectedChanged(NetActiveParams &aiParams, QString const &name, QString const &selected);

    void extend(NetSoftMaxParams &aiParams);
    void handleValueChanged(NetSoftMaxParams &aiParams, QString const &name, int val);

    void extend(NetFlattenParams &aiParams);
    void handleValueChanged(NetFlattenParams &aiParams, QString const &name, int val);

    void extend(NetChannelShuffleParams &aiParams);
    void handleValueChanged(NetChannelShuffleParams &aiParams, QString const &name, int val);

    void extend(NetDetectLayer2DParams &aiParams);
    void handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int val);
    void handleRowChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int &cIndex, int &val);
    void handleColChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int &val);
    void handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int val);

    void extend(NetCatParams &aiParams);
    void handleValueChanged(NetCatParams &aiParams, QString const &name, int val);


    // 计算单元: calunit
    void extend(NetScaledParams &aiParams);

    void extend(NetAddParams &aiParams);

    void extend(NetConstParams &aiParams);
    void handleValueChanged(NetConstParams &aiParams, QString const &name, double val);

    void extend(NetPermuteParams &aiParams);
    void handleIndexChanged(NetPermuteParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetPermuteParams &aiParams, QString const &name, int index, int val);

    void extend(NetViewParams &aiParams);
    void handleValueChanged(NetViewParams &aiParams, QString const &name, int val);

    void extend(NetSplitParams &aiParams);
    void handleValueChanged(NetSplitParams &aiParams, QString const &name, int val);

    void extend(NetIdentityParams &aiParams);


    // 运行单元: rununit
    void extend(NetShowResultParams &aiParams);

    void extend(NetBaseLossParams &aiParams);
    void handleSelectedChanged(NetBaseLossParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetBaseLossParams &aiParams, QString const &name, double val);

    void extend(NetDiceLossParams &aiParams);

    void extend(NetDetectYoloLossParams &aiParams);

    void extend(NetOptAdaDeltaParams &aiParams);
    void handleValueChanged(NetOptAdaDeltaParams &aiParams, QString const &name, double val);

    void extend(NetOptAdaGradParams &aiParams);
    void handleValueChanged(NetOptAdaGradParams &aiParams, QString const &name, double val);

    void extend(NetOptAdamParams &aiParams);
    void handleValueChanged(NetOptAdamParams &aiParams, QString const &name, double val);

    void extend(NetOptASGDParams &aiParams);

    void extend(NetOptRMSpropParams &aiParams);
    void handleValueChanged(NetOptRMSpropParams &aiParams, QString const &name, double val);

    void extend(NetOptSGDParams &aiParams);
    void handleValueChanged(NetOptSGDParams &aiParams, QString const &name, double val);

    void extend(NetTrain2DParams &aiParams);
    void handleTextChanged(NetTrain2DParams &aiParams, QString const &name, QString const &text);
    void handleSelectedChanged(NetTrain2DParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetTrain2DParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int index, int val);

    void extend(NetDataLoader2DParams &aiParams);
    void handleTextChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &text);
    void handleSelectedChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetDataLoader2DParams &aiParams, QString const &name, int val);

    void extend(NetDataInputTestParams &aiParams);
    void handleValueChanged(NetDataInputTestParams &aiParams, QString const &name, int val);

    void extend(NetDataOutputParams &aiParams);


    // 定制网元: custom
    void extend(NetCustomParams &aiParams);
    void handleValueChanged(NetCustomParams &aiParams, QString const &name, int val);


private:
    Ui::WgAttrList*                 ui;

    int                             m_sn;
    QString                         m_Net;
    QString                         m_Name;

    NetAiParams                     m_aiParams;

    map<QString, QWidget*>          m_nameWgMap;
};

#endif // WGATTRLIST_H
