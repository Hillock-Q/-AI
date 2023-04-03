#ifndef DIALOGPARAMEDIT_H
#define DIALOGPARAMEDIT_H

#include <QDialog>
#include <vector>
#include "NetAiParamsDefine.h"

using std::vector;

namespace Ui {
class CDialogParamEdit;
}

class CDialogParamEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogParamEdit(QWidget *parent = nullptr);
    ~CDialogParamEdit();

    void Show(int sn, QString const &netId, QString const &netName, QString const &field, QString const &value, vector<QString> const &params);

signals:
    void showInfoSignal(QString const &info);
    void paramsChangedSignal(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams);

private slots:
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

    void on_btnClose_clicked();

private:
    void initTextEditor();
    void initStrSelector();
    void initIntEditor();
    void initIntListEditor();
    void initIntTableEditor();
    void initFloatEditor();
    void initFloatListEditor();

    void initWgs();

    void showWg(QWidget *wg);

    void showTextEditor(QString const &name, QString const &text);
    void showStrSelector(QString const &name, QStringList const &rows, QString const &selected);
    void showIntEditor(QString const &name, int min, int max, int val, int step);
    void showIntListEditor(QString const &name, int cols, int index, int min, int max, int val, int step);
    void showIntTableEditor(QString const &name, int rows, int rIndex, int cols, int cIndex, int min, int max, int val, int step);
    void showFloatEditor(QString const &name, double min, double max, double val, double step, int prec);
    void showFloatListEditor(QString const &name, int cols, int index, double min, double max, double val, double step, int prec);

    bool decode(QString const &value, int* arr, int n) const;
    bool decode(QString const &value, int *tab, int rows, int cols) const;
    bool decode(QString const &value, double *arr, int n) const;

    // 网络单元: netunit
    void initParams(vector<QString> const &params, NetConvParams &aiParams);
    void showParam(QString const &field, QString const &value, NetConvParams const &aiParams);
    void handleSelectedChanged(NetConvParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetConvParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetConvParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetConvParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetMaxPoolParams &aiParams);
    void showParam(QString const &field, QString const &value, NetMaxPoolParams const &aiParams);
    void handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetAvgPoolParams &aiParams);
    void showParam(QString const &field, QString const &value, NetAvgPoolParams const &aiParams);
    void handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetAdaptiveAvgPoolParams &aiParams);
    void showParam(QString const &field, QString const &value, NetAdaptiveAvgPoolParams const &aiParams);
    void handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetConvTransposeParams &aiParams);
    void showParam(QString const &field, QString const &value, NetConvTransposeParams const &aiParams);
    void handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetUpSampleParams &aiParams);
    void showParam(QString const &field, QString const &value, NetUpSampleParams const &aiParams);
    void handleSelectedChanged(NetUpSampleParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetUpSampleParams &aiParams, QString const &name, int index, double &val);
    void handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int index, double val);

    void initParams(vector<QString> const &params, NetLinearParams &aiParams);
    void showParam(QString const &field, QString const &value, NetLinearParams const &aiParams);
    void handleSelectedChanged(NetLinearParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetLinearParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetDropOutParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDropOutParams const &aiParams);
    void handleValueChanged(NetDropOutParams &aiParams, QString const &name, int val);
    void handleValueChanged(NetDropOutParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetDropPathParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDropPathParams const &aiParams);
    void handleValueChanged(NetDropPathParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetBatchNormParams &aiParams);
    void showParam(QString const &field, QString const &value, NetBatchNormParams const &aiParams);
    void handleValueChanged(NetBatchNormParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetGroupNormParams &aiParams);
    void showParam(QString const &field, QString const &value, NetGroupNormParams const &aiParams);
    void handleValueChanged(NetGroupNormParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetInstanceNormParams &aiParams);
    void showParam(QString const &field, QString const &value, NetInstanceNormParams const &aiParams);

    void handleValueChanged(NetInstanceNormParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetLayerNormParams &aiParams);
    void showParam(QString const &field, QString const &value, NetLayerNormParams const &aiParams);
    void handleIndexChanged(NetLayerNormParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetLayerNormParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetActiveParams &aiParams);
    void showParam(QString const &field, QString const &value, NetActiveParams const &aiParams);
    void handleSelectedChanged(NetActiveParams &aiParams, QString const &name, QString const &selected);

    void initParams(vector<QString> const &params, NetSoftMaxParams &aiParams);
    void showParam(QString const &field, QString const &value, NetSoftMaxParams const &aiParams);
    void handleValueChanged(NetSoftMaxParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetFlattenParams &aiParams);
    void showParam(QString const &field, QString const &value, NetFlattenParams const &aiParams);

    void handleValueChanged(NetFlattenParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetChannelShuffleParams &aiParams);
    void showParam(QString const &field, QString const &value, NetChannelShuffleParams const &aiParams);
    void handleValueChanged(NetChannelShuffleParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetDetectLayer2DParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDetectLayer2DParams const &aiParams);
    void handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int val);
    void handleRowChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int &cIndex, int &val);
    void handleColChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int &val);
    void handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int val);

    void initParams(vector<QString> const &params, NetCatParams &aiParams);
    void showParam(QString const &field, QString const &value, NetCatParams const &aiParams);
    void handleValueChanged(NetCatParams &aiParams, QString const &name, int val);


    // 计算单元: calunit
    void initParams(vector<QString> const &params, NetScaledParams &aiParams);
    void showParam(QString const &field, QString const &value, NetScaledParams const &aiParams);

    void initParams(vector<QString> const &params, NetAddParams &aiParams);
    void showParam(QString const &field, QString const &value, NetAddParams const &aiParams);

    void initParams(vector<QString> const &params, NetConstParams &aiParams);
    void showParam(QString const &field, QString const &value, NetConstParams const &aiParams);
    void handleValueChanged(NetConstParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetPermuteParams &aiParams);
    void showParam(QString const &field, QString const &value, NetPermuteParams const &aiParams);
    void handleIndexChanged(NetPermuteParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetPermuteParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetViewParams &aiParams);
    void showParam(QString const &field, QString const &value, NetViewParams const &aiParams);
    void handleValueChanged(NetViewParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetSplitParams &aiParams);
    void showParam(QString const &field, QString const &value, NetSplitParams const &aiParams);
    void handleValueChanged(NetSplitParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetIdentityParams &aiParams);
    void showParam(QString const &field, QString const &value, NetIdentityParams const &aiParams);


    // 运行单元: rununit
    void initParams(vector<QString> const &params, NetShowResultParams &aiParams);
    void showParam(QString const &field, QString const &value, NetShowResultParams const &aiParams);

    void initParams(vector<QString> const &params, NetBaseLossParams &aiParams);
    void showParam(QString const &field, QString const &value, NetBaseLossParams const &aiParams);
    void handleSelectedChanged(NetBaseLossParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetBaseLossParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetDiceLossParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDiceLossParams const &aiParams);

    void initParams(vector<QString> const &params, NetDetectYoloLossParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDetectYoloLossParams const &aiParams);

    void initParams(vector<QString> const &params, NetOptAdaDeltaParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptAdaDeltaParams const &aiParams);
    void handleValueChanged(NetOptAdaDeltaParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetOptAdaGradParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptAdaGradParams const &aiParams);
    void handleValueChanged(NetOptAdaGradParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetOptAdamParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptAdamParams const &aiParams);
    void handleValueChanged(NetOptAdamParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetOptASGDParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptASGDParams const &aiParams);

    void initParams(vector<QString> const &params, NetOptRMSpropParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptRMSpropParams const &aiParams);
    void handleValueChanged(NetOptRMSpropParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetOptSGDParams &aiParams);
    void showParam(QString const &field, QString const &value, NetOptSGDParams const &aiParams);
    void handleValueChanged(NetOptSGDParams &aiParams, QString const &name, double val);

    void initParams(vector<QString> const &params, NetTrain2DParams &aiParams);
    void showParam(QString const &field, QString const &value, NetTrain2DParams const &aiParams);
    void handleTextChanged(NetTrain2DParams &aiParams, QString const &name, QString const &text);
    void handleSelectedChanged(NetTrain2DParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int val);
    void handleIndexChanged(NetTrain2DParams &aiParams, QString const &name, int index, int &val);
    void handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int index, int val);

    void initParams(vector<QString> const &params, NetDataLoader2DParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDataLoader2DParams const &aiParams);
    void handleTextChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &text);
    void handleSelectedChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &selected);
    void handleValueChanged(NetDataLoader2DParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetDataInputTestParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDataInputTestParams const &aiParams);
    void handleValueChanged(NetDataInputTestParams &aiParams, QString const &name, int val);

    void initParams(vector<QString> const &params, NetDataOutputParams &aiParams);
    void showParam(QString const &field, QString const &value, NetDataOutputParams const &aiParams);


    // 定制网元: custom
    void initParams(vector<QString> const &params, NetCustomParams &aiParams);
    void showParam(QString const &field, QString const &value, NetCustomParams const &aiParams);
    void handleValueChanged(NetCustomParams &aiParams, QString const &name, int val);


private:
    Ui::CDialogParamEdit*               ui;

    vector<QWidget*>                    m_wgs;

    int                                 m_sn;
    QString                             m_netId;
    QString                             m_netName;

    QString                             m_field;
    QString                             m_value;

    NetAiParams                         m_aiParams;
};

#endif // DIALOGPARAMEDIT_H
