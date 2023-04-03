#include "AttrViewMgr.h"
#include <algorithm>
#include <QListWidget>
#include <QTreeWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <list>
#include "WgAttrList.h"

using std::list;

CAttrViewMgr& CAttrViewMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CAttrViewMgr());
	return *kp.get();
}

CAttrViewMgr::CAttrViewMgr()
{

}

CAttrViewMgr::~CAttrViewMgr()
{
    m_treeWidget->disconnect();
}

void CAttrViewMgr::Init(QListWidget *listwgAttr)
{
    m_listwgAttr = listwgAttr;

    // 网络单元: netunit
    m_netTypeMap["conv"] = e_NetConvType;
    m_netTypeMap["maxpool"] = e_NetMaxPoolType;
    m_netTypeMap["avgpool"] = e_NetAvgPoolType;
    m_netTypeMap["adaptiveavgpool"] = e_NetAdaptiveAvgPoolType;
    m_netTypeMap["convtranspose"] = e_NetConvTransposeType;
    m_netTypeMap["upsample"] = e_NetUpSampleType;
    m_netTypeMap["linear"] = e_NetLinearType;
    m_netTypeMap["dropout"] = e_NetDropOutType;
    m_netTypeMap["droppath"] = e_NetDropPathType;
    m_netTypeMap["batchnorm"] = e_NetBatchNormType;
    m_netTypeMap["groupnorm"] = e_NetGroupNormType;
    m_netTypeMap["instancenorm"] = e_NetInstanceNormType;
    m_netTypeMap["layernorm"] = e_NetLayerNormType;
    m_netTypeMap["active"] = e_NetActiveType;
    m_netTypeMap["softmax"] = e_NetSoftMaxType;
    m_netTypeMap["flatten"] = e_NetFlattenType;
    m_netTypeMap["channelshuffle"] = e_NetChannelShuffleType;
    m_netTypeMap["detectlayer2d"] = e_NetDetectLayer2DType;
    m_netTypeMap["cat"] = e_NetCatType;

    // 计算单元: calunit
    m_netTypeMap["scaled"] = e_NetScaledType;
    m_netTypeMap["add"] = e_NetAddType;
    m_netTypeMap["const"] = e_NetConstType;
    m_netTypeMap["permute"] = e_NetPermuteType;
    m_netTypeMap["view"] = e_NetViewType;
    m_netTypeMap["split"] = e_NetSplitType;
    m_netTypeMap["identity"] = e_NetIdentityType;

    // 运行单元: rununit
    m_netTypeMap["showresult"] = e_NetShowResultType;
    m_netTypeMap["loss"] = e_NetBaseLossType;
    m_netTypeMap["diceloss"] = e_NetDiceLossType;
    m_netTypeMap["detectyololoss"] = e_NetDetectYoloLossType;
    m_netTypeMap["optadadelta"] = e_NetOptAdaDeltaType;
    m_netTypeMap["optadagrad"] = e_NetOptAdaGradType;
    m_netTypeMap["optadam"] = e_NetOptAdamType;
    m_netTypeMap["optasgd"] = e_NetOptASGDType;
    m_netTypeMap["optrmsprop"] = e_NetOptRMSpropType;
    m_netTypeMap["optsgd"] = e_NetOptSGDType;
    m_netTypeMap["train2d"] = e_NetTrain2DType;
    m_netTypeMap["dataloader2d"] = e_NetDataLoader2DType;
    m_netTypeMap["datainputtest"] = e_NetDataInputTestType;
    m_netTypeMap["dataoutput"] = e_NetDataOutputType;

    // 定制网元: custom
}

NetType CAttrViewMgr::GetNetType(QString const &netId) const
{
    auto itor = m_netTypeMap.find(netId);
    if (itor !=  m_netTypeMap.end())
    {
        // std
        return itor->second;
    }
    else
    {
        // custom
        return e_NetCustomType;
    }
}

void CAttrViewMgr::SetAiParamsView(QTreeWidget *treeWidget)
{
    m_treeWidget = treeWidget;

    connect(m_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemDoubleClickedSlot(QTreeWidgetItem*, int)));
}

void CAttrViewMgr::clearAttr()
{
    m_listwgAttr->clear();
}

void CAttrViewMgr::showAttr(int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams)
{
    m_listwgAttr->clear();

    NetAiParams aiParams;
    decodeParams(netId, expressParams, aiParams);
    showParams(sn, netId, netName, aiParams);
}

// 网络单元: netunit
/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_paddingMode": "zeros",
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetConvParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_inChannels") && expressParams.value("02_inChannels").isDouble())
    {
        aiParams.inChannels = expressParams.value("02_inChannels").toInt();
    }

    if (expressParams.contains("03_outChannels") && expressParams.value("03_outChannels").isDouble())
    {
        aiParams.outChannels = expressParams.value("03_outChannels").toInt();
    }

    if (expressParams.contains("04_group") && expressParams.value("04_group").isDouble())
    {
        aiParams.group = expressParams.value("04_group").toInt();
    }

    if (expressParams.contains("05_kernelSize") && expressParams.value("05_kernelSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("05_kernelSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.kernelSize[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("06_stride") && expressParams.value("06_stride").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("06_stride").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.stride[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("07_dilation") && expressParams.value("07_dilation").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("07_dilation").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.dilation[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("08_padding") && expressParams.value("08_padding").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("08_padding").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.padding[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("09_paddingMode") && expressParams.value("09_paddingMode").isString())
    {
        auto paddingMode = expressParams.value("09_paddingMode").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), paddingMode.size());
        memcpy(aiParams.paddingMode, paddingMode.data(), sz);
    }

    if (expressParams.contains("10_requiresGrad") && expressParams.value("10_requiresGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("10_requiresGrad").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_dilation": [1, 1, 1],
    "05_padding": [0, 0, 0],
    "06_requiresGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetMaxPoolParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_kernelSize") && expressParams.value("02_kernelSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("02_kernelSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.kernelSize[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("03_stride") && expressParams.value("03_stride").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("03_stride").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.stride[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("04_dilation") && expressParams.value("04_dilation").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("04_dilation").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.dilation[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("05_padding") && expressParams.value("05_padding").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("05_padding").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.padding[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("06_requiresGrad") && expressParams.value("06_requiresGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("06_requiresGrad").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_padding": [0, 0, 0],
    "05_requiresGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetAvgPoolParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_kernelSize") && expressParams.value("02_kernelSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("02_kernelSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.kernelSize[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("03_stride") && expressParams.value("03_stride").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("03_stride").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.stride[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("04_padding") && expressParams.value("04_padding").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("04_padding").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.padding[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("05_requiresGrad") && expressParams.value("05_requiresGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("05_requiresGrad").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_outputSize": [32, 32, 32]
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetAdaptiveAvgPoolParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_outputSize") && expressParams.value("02_outputSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("02_outputSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.outputSize[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }
}

/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_outputPadding": [0, 0, 0],
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetConvTransposeParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_inChannels") && expressParams.value("02_inChannels").isDouble())
    {
        aiParams.inChannels = expressParams.value("02_inChannels").toInt();
    }

    if (expressParams.contains("03_outChannels") && expressParams.value("03_outChannels").isDouble())
    {
        aiParams.outChannels = expressParams.value("03_outChannels").toInt();
    }

    if (expressParams.contains("04_group") && expressParams.value("04_group").isDouble())
    {
        aiParams.group = expressParams.value("04_group").toInt();
    }

    if (expressParams.contains("05_kernelSize") && expressParams.value("05_kernelSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("05_kernelSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.kernelSize[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("06_stride") && expressParams.value("06_stride").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("06_stride").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.stride[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("07_dilation") && expressParams.value("07_dilation").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("07_dilation").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.dilation[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("08_padding") && expressParams.value("08_padding").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("08_padding").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.padding[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("09_outputPadding") && expressParams.value("09_outputPadding").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("09_outputPadding").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.outputPadding[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("10_requiresGrad") && expressParams.value("10_requiresGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("10_requiresGrad").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_scaleFactor": [2.0, 2.0, 2.0],
    "03_mode": "nearest"
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetUpSampleParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_scaleFactor") && expressParams.value("02_scaleFactor").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("02_scaleFactor").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toDouble();
            aiParams.scaleFactor[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("03_mode") && expressParams.value("03_mode").isString())
    {
        auto mode = expressParams.value("03_mode").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), mode.size());
        memcpy(aiParams.mode, mode.data(), sz);
    }
}

/*{
    "01_inFeatures": 64,
    "02_outFeatures": 64,
    "03_bias": true,
    "04_requiresGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetLinearParams &aiParams)
{
    if (expressParams.contains("01_inFeatures") && expressParams.value("01_inFeatures").isDouble())
    {
        aiParams.inFeatures = expressParams.value("01_inFeatures").toInt();
    }

    if (expressParams.contains("02_outFeatures") && expressParams.value("02_outFeatures").isDouble())
    {
        aiParams.outFeatures = expressParams.value("02_outFeatures").toInt();
    }

    if (expressParams.contains("03_bias") && expressParams.value("03_bias").isBool())
    {
        aiParams.bias = expressParams.value("03_bias").toBool();
    }

    if (expressParams.contains("04_requiresGrad") && expressParams.value("04_requiresGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("04_requiresGrad").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_prob": 0.5
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetDropOutParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_prob") && expressParams.value("02_prob").isDouble())
    {
        aiParams.prob = expressParams.value("02_prob").toDouble();
    }
}

/*{
    "01_prob": 0.5
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetDropPathParams &aiParams)
{
    if (expressParams.contains("01_prob") && expressParams.value("01_prob").isDouble())
    {
        aiParams.prob = expressParams.value("01_prob").toDouble();
    }
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetBatchNormParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_numFeatures") && expressParams.value("02_numFeatures").isDouble())
    {
        aiParams.numFeatures = expressParams.value("02_numFeatures").toInt();
    }
}

/*{
    "01_numGroups": 2,
    "02_numChannels": 64
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetGroupNormParams &aiParams)
{
    if (expressParams.contains("01_numGroups") && expressParams.value("01_numGroups").isDouble())
    {
        aiParams.numGroups = expressParams.value("01_numGroups").toInt();
    }

    if (expressParams.contains("02_numChannels") && expressParams.value("02_numChannels").isDouble())
    {
        aiParams.numChannels = expressParams.value("02_numChannels").toInt();
    }
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetInstanceNormParams &aiParams)
{
    if (expressParams.contains("01_dim") && expressParams.value("01_dim").isDouble())
    {
        aiParams.dim = expressParams.value("01_dim").toInt();
    }

    if (expressParams.contains("02_numFeatures") && expressParams.value("02_numFeatures").isDouble())
    {
        aiParams.numFeatures = expressParams.value("02_numFeatures").toInt();
    }
}

/*{
    "01_normalizedShape": [0, 1, 3, 2]
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetLayerNormParams &aiParams)
{
    if (expressParams.contains("01_normalizedShape") && expressParams.value("01_normalizedShape").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("01_normalizedShape").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.normalizedShape[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }
}

/*{
    "01_function": "SiLU"
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetActiveParams &aiParams)
{
    if (expressParams.contains("01_function") && expressParams.value("01_function").isString())
    {
        auto func = expressParams.value("01_function").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), func.size());
        memcpy(aiParams.function, func.data(), sz);
    }
}

/*{
    "01_softMaxDim": 2
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetSoftMaxParams &aiParams)
{
    if (expressParams.contains("01_softMaxDim") && expressParams.value("01_softMaxDim").isDouble())
    {
        aiParams.softMaxDim = expressParams.value("01_softMaxDim").toInt();
    }
}

/*{
    "01_startDim": 1,
    "02_endDim": -1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetFlattenParams &aiParams)
{
    if (expressParams.contains("01_startDim") && expressParams.value("01_startDim").isDouble())
    {
        aiParams.startDim = expressParams.value("01_startDim").toInt();
    }

    if (expressParams.contains("02_endDim") && expressParams.value("02_endDim").isDouble())
    {
        aiParams.endDim = expressParams.value("02_endDim").toInt();
    }
}

/*{
    "01_groups": 2
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetChannelShuffleParams &aiParams)
{
    if (expressParams.contains("01_groups") && expressParams.value("01_groups").isDouble())
    {
        aiParams.groups = expressParams.value("01_groups").toInt();
    }
}

/*{
    "01_numberClass": 2,
    "02_anchorSize": [[210, 210], [300, 400], [300, 400]],
    "03_imageSize": 416
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetDetectLayer2DParams &aiParams)
{
    if (expressParams.contains("01_numberClass") && expressParams.value("01_numberClass").isDouble())
    {
        aiParams.numberClass = expressParams.value("01_numberClass").toInt();
    }

    if (expressParams.contains("02_anchorSize") && expressParams.value("02_anchorSize").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("02_anchorSize").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isArray())
                continue;

            if (index > 0)
                pntText += ", ";
            pntText += "[";

            auto intIndex = 0;
            auto intValues = valRef.toArray();
            for (auto intItor = intValues.begin(); intItor != intValues.end(); ++intItor, intIndex++)
            {
                auto intValRef = intItor[0];
                if (!intValRef.isDouble())
                    continue;

                auto intValue = intValRef.toInt();
                aiParams.anchorSize[index][intIndex] = intValue;

                if (intIndex > 0)
                    pntText += ", ";
                pntText += QString::number(intValue);
            }
            pntText +="]";
        }
        pntText +="]";
    }

    if (expressParams.contains("03_imageSize") && expressParams.value("03_imageSize").isDouble())
    {
        aiParams.imageSize = expressParams.value("03_imageSize").toInt();
    }
}

/*{
    "01_catDim": 0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetCatParams &aiParams)
{
    if (expressParams.contains("01_catDim") && expressParams.value("01_catDim").isDouble())
    {
        aiParams.catDim = expressParams.value("01_catDim").toInt();
    }
}


// 计算单元: calunit
/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetScaledParams &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetAddParams &)
{
    // no-op
}

/*{
    "01_input": 0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetConstParams &aiParams)
{
    if (expressParams.contains("01_input") && expressParams.value("01_input").isDouble())
    {
        aiParams.input = expressParams.value("01_input").toDouble();
    }
}

/*{
    "01_swapDims": [0, 1, 3, 2]
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetPermuteParams &aiParams)
{
    if (expressParams.contains("01_swapDims") && expressParams.value("01_swapDims").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("01_swapDims").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.swapDims[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }
}

/*{
    "01_changedims": 0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetViewParams &aiParams)
{
    if (expressParams.contains("01_changedims") && expressParams.value("01_changedims").isDouble())
    {
        aiParams.changeDims = expressParams.value("01_changedims").toInt();
    }
}

/*{
    "01_splitsize": 2
    "02_dim": 0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetSplitParams &aiParams)
{
    if (expressParams.contains("01_splitsize") && expressParams.value("01_splitsize").isDouble())
    {
        aiParams.splitSize = expressParams.value("01_splitsize").toInt();
    }

    if (expressParams.contains("02_dim") && expressParams.value("02_dim").isDouble())
    {
        aiParams.dim = expressParams.value("02_dim").toInt();
    }
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetIdentityParams &)
{
    // no-op
}


// 运行单元: rununit
/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetShowResultParams &)
{
    // no-op
}

/*{
    "01_function": "CrossEntropyLoss"
    "02_p": 1.0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetBaseLossParams &aiParams)
{
    if (expressParams.contains("01_function") && expressParams.value("01_function").isString())
    {
        auto func = expressParams.value("01_function").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), func.size());
        memcpy(aiParams.function, func.data(), sz);
    }

    if (expressParams.contains("02_p") && expressParams.value("02_p").isDouble())
    {
        aiParams.p = expressParams.value("02_p").toDouble();
    }
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetDiceLossParams &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetDetectYoloLossParams &)
{
    // no-op
}

/*{
    "01_lr": 1.0,
    "02_rho": 0.9,
    "03_weightDecay": 0.0,
    "04_eps": 1.00E-06
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetOptAdaDeltaParams &aiParams)
{
    if (expressParams.contains("01_lr") && expressParams.value("01_lr").isDouble())
    {
        aiParams.lr = expressParams.value("01_lr").toDouble();
    }

    if (expressParams.contains("02_rho") && expressParams.value("02_rho").isDouble())
    {
        aiParams.rho = expressParams.value("02_rho").toDouble();
    }

    if (expressParams.contains("03_weightDecay") && expressParams.value("03_weightDecay").isDouble())
    {
        aiParams.weightDecay = expressParams.value("03_weightDecay").toDouble();
    }

    if (expressParams.contains("04_eps") && expressParams.value("04_eps").isDouble())
    {
        aiParams.eps = expressParams.value("04_eps").toDouble();
    }
}

/*{
    "01_lr": 0.01,
    "02_lrDecay": 0.0,
    "03_weightDecay": 0.0,
    "04_initialAccumulatorValue": 0.0,
    "05_eps": 1.00E-06
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetOptAdaGradParams &aiParams)
{
    if (expressParams.contains("01_lr") && expressParams.value("01_lr").isDouble())
    {
        aiParams.lr = expressParams.value("01_lr").toDouble();
    }

    if (expressParams.contains("02_lrDecay") && expressParams.value("02_lrDecay").isDouble())
    {
        aiParams.lrDecay = expressParams.value("02_lrDecay").toDouble();
    }

    if (expressParams.contains("03_weightDecay") && expressParams.value("03_weightDecay").isDouble())
    {
        aiParams.weightDecay = expressParams.value("03_weightDecay").toDouble();
    }

    if (expressParams.contains("04_initialAccumulatorValue") && expressParams.value("04_initialAccumulatorValue").isDouble())
    {
        aiParams.initialAccumulatorValue = expressParams.value("04_initialAccumulatorValue").toDouble();
    }

    if (expressParams.contains("05_eps") && expressParams.value("05_eps").isDouble())
    {
        aiParams.eps = expressParams.value("05_eps").toDouble();
    }
}

/*{
    "01_lr": 0.01,
    "02_eps": 1.00E-06,
    "03_weightDecay": 0.0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetOptAdamParams &aiParams)
{
    if (expressParams.contains("01_lr") && expressParams.value("01_lr").isDouble())
    {
        aiParams.lr = expressParams.value("01_lr").toDouble();
    }

    if (expressParams.contains("02_eps") && expressParams.value("02_eps").isDouble())
    {
        aiParams.eps = expressParams.value("02_eps").toDouble();
    }

    if (expressParams.contains("03_weightDecay") && expressParams.value("03_weightDecay").isDouble())
    {
        aiParams.weightDecay = expressParams.value("03_weightDecay").toDouble();
    }
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetOptASGDParams &)
{
    // no-op
}

/*{
    "01_lr": 0.01,
    "02_alpha": 9.90E-01,
    "03_eps": 1.00E-06,
    "04_weightDecay": 0.0,
    "05_momentum": 0.0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetOptRMSpropParams &aiParams)
{
    if (expressParams.contains("01_lr") && expressParams.value("01_lr").isDouble())
    {
        aiParams.lr = expressParams.value("01_lr").toDouble();
    }

    if (expressParams.contains("02_alpha") && expressParams.value("02_alpha").isDouble())
    {
        aiParams.alpha = expressParams.value("02_alpha").toDouble();
    }

    if (expressParams.contains("03_eps") && expressParams.value("03_eps").isDouble())
    {
        aiParams.eps = expressParams.value("03_eps").toDouble();
    }

    if (expressParams.contains("04_weightDecay") && expressParams.value("04_weightDecay").isDouble())
    {
        aiParams.weightDecay = expressParams.value("04_weightDecay").toDouble();
    }

    if (expressParams.contains("05_momentum") && expressParams.value("05_momentum").isDouble())
    {
        aiParams.momentum = expressParams.value("05_momentum").toDouble();
    }
}

/*{
    "01_lr": 0.01,
    "02_momentum": 0.0,
    "03_dampening": 0.0,
    "04_weightDecay": 0.0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetOptSGDParams &aiParams)
{
    if (expressParams.contains("01_lr") && expressParams.value("01_lr").isDouble())
    {
        aiParams.lr = expressParams.value("01_lr").toDouble();
    }

    if (expressParams.contains("02_momentum") && expressParams.value("02_momentum").isDouble())
    {
        aiParams.momentum = expressParams.value("02_momentum").toDouble();
    }

    if (expressParams.contains("03_dampening") && expressParams.value("03_dampening").isDouble())
    {
        aiParams.dampening = expressParams.value("03_dampening").toDouble();
    }

    if (expressParams.contains("04_weightDecay") && expressParams.value("04_weightDecay").isDouble())
    {
        aiParams.weightDecay = expressParams.value("04_weightDecay").toDouble();
    }
}


/*{
    "01_savePthPath": ".",
    "02_preweightsPath": ".",
    "03_logDir": "",
    "04_epochNum": 100,
    "05_batchSize": 16,
    "06_isCuda": false,
    "07_classNums": [1, 1, 1, 1, 1],
    "08_criteriaFunction": "Dice",
    "09_isTrainDataCriteria": true,
    "10_showLossFigure": true,
    "11_showCriteriaFigure": true,
    "12_epochSaveModel": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetTrain2DParams &aiParams)
{
    if (expressParams.contains("01_savePthPath") && expressParams.value("01_savePthPath").isString())
    {
        auto path = expressParams.value("01_savePthPath").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), path.size());
        memcpy(aiParams.savePthPath, path.data(), sz);
    }

    if (expressParams.contains("02_preweightsPath") && expressParams.value("02_preweightsPath").isString())
    {
        auto path = expressParams.value("02_preweightsPath").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), path.size());
        memcpy(aiParams.preweightsPath, path.data(), sz);
    }

    if (expressParams.contains("03_logDir") && expressParams.value("03_logDir").isString())
    {
        auto dir = expressParams.value("03_logDir").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), dir.size());
        memcpy(aiParams.logDir, dir.data(), sz);
    }

    if (expressParams.contains("04_epochNum") && expressParams.value("04_epochNum").isDouble())
    {
        aiParams.epochNum = expressParams.value("04_epochNum").toInt();
    }

    if (expressParams.contains("05_batchSize") && expressParams.value("05_batchSize").isDouble())
    {
        aiParams.batchSize = expressParams.value("05_batchSize").toInt();
    }

    if (expressParams.contains("06_isCuda") && expressParams.value("06_isCuda").isBool())
    {
        aiParams.isCuda = expressParams.value("06_isCuda").toBool();
    }

    if (expressParams.contains("07_classNums") && expressParams.value("07_classNums").isArray())
    {
        auto index = 0;
        QString pntText = "[";
        auto values = expressParams.value("07_classNums").toArray();
        for(auto itor = values.begin(); itor != values.end(); ++itor, index++)
        {
            auto valRef = itor[0];
            if (!valRef.isDouble())
                continue;

            auto value = valRef.toInt();
            aiParams.classNums[index] = value;

            if (index > 0)
                pntText += ", ";
            pntText += QString::number(value);
        }
        pntText +="]";
    }

    if (expressParams.contains("08_criteriaFunction") && expressParams.value("08_criteriaFunction").isString())
    {
        auto func = expressParams.value("08_criteriaFunction").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), func.size());
        memcpy(aiParams.criteriaFunction, func.data(), sz);
    }

    if (expressParams.contains("09_isTrainDataCriteria") && expressParams.value("09_isTrainDataCriteria").isBool())
    {
        aiParams.isTrainDataCriteria = expressParams.value("09_isTrainDataCriteria").toBool();
    }

    if (expressParams.contains("10_showLossFigure") && expressParams.value("10_showLossFigure").isBool())
    {
        aiParams.showLossFigure = expressParams.value("10_showLossFigure").toBool();
    }

    if (expressParams.contains("11_showCriteriaFigure") && expressParams.value("11_showCriteriaFigure").isBool())
    {
        aiParams.showCriteriaFigure = expressParams.value("11_showCriteriaFigure").toBool();
    }

    if (expressParams.contains("12_epochSaveModel") && expressParams.value("12_epochSaveModel").isDouble())
    {
        aiParams.epochSaveModel = expressParams.value("12_epochSaveModel").toInt();
    }
}

/*{
    "01_inputW": 256,
    "02_inputH": 256,
    "03_dataJsonPath": "",
    "04_dataLabelCodeJsonPath": "",
    "05_isRgb": true,
    "06_batchSize": 16,
    "07_numWorkers": 1,
    "08_shuffle": true
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetDataLoader2DParams &aiParams)
{
    if (expressParams.contains("01_inputW") && expressParams.value("01_inputW").isDouble())
    {
        aiParams.inputW = expressParams.value("01_inputW").toInt();
    }

    if (expressParams.contains("02_inputH") && expressParams.value("02_inputH").isDouble())
    {
        aiParams.inputH = expressParams.value("02_inputH").toInt();
    }

    if (expressParams.contains("03_dataJsonPath") && expressParams.value("03_dataJsonPath").isString())
    {
        auto func = expressParams.value("03_dataJsonPath").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), func.size());
        memcpy(aiParams.dataJsonPath, func.data(), sz);
    }

    if (expressParams.contains("04_dataLabelCodeJsonPath") && expressParams.value("04_dataLabelCodeJsonPath").isString())
    {
        auto func = expressParams.value("04_dataLabelCodeJsonPath").toString().toStdString();
        auto sz = std::min(size_t(STRLEN), func.size());
        memcpy(aiParams.dataLabelCodeJsonPath, func.data(), sz);
    }

    if (expressParams.contains("05_isRgb") && expressParams.value("05_isRgb").isBool())
    {
        aiParams.isRgb = expressParams.value("05_isRgb").toBool();
    }

    if (expressParams.contains("06_batchSize") && expressParams.value("06_batchSize").isDouble())
    {
        aiParams.batchSize = expressParams.value("06_batchSize").toInt();
    }

    if (expressParams.contains("07_numWorkers") && expressParams.value("07_numWorkers").isDouble())
    {
        aiParams.numWorkers = expressParams.value("07_numWorkers").toInt();
    }

    if (expressParams.contains("08_shuffle") && expressParams.value("08_shuffle").isBool())
    {
        aiParams.shuffle = expressParams.value("08_shuffle").toBool();
    }
}

/*{
    "01_inputDim": 0
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetDataInputTestParams &aiParams)
{
    if (expressParams.contains("01_inputDim") && expressParams.value("01_inputDim").isDouble())
    {
        aiParams.inputDim = expressParams.value("01_inputDim").toInt();
    }
}

/*{

}*/
void CAttrViewMgr::decode(QJsonObject const &, NetDataOutputParams &)
{
    // no-op
}


// 定制网元: custom
/*{
    "01_cycle": 1,
    "02_reqGrad": 1
}*/
void CAttrViewMgr::decode(QJsonObject const &expressParams, NetCustomParams &aiParams)
{
    if (expressParams.contains("01_cycle") && expressParams.value("01_cycle").isDouble())
    {
        aiParams.cycle = expressParams.value("01_cycle").toInt();
    }

    if (expressParams.contains("02_reqGrad") && expressParams.value("02_reqGrad").isDouble())
    {
        aiParams.requiresGrad = expressParams.value("02_reqGrad").toInt();
    }
}

void CAttrViewMgr::decodeParams(QString const &netId, QJsonObject const &expressParams, NetAiParams &aiParams)
{    
    aiParams.type = GetNetType(netId);

    switch (aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        decode(expressParams, aiParams.ai.convParams);
        break;

    case e_NetMaxPoolType:
        decode(expressParams, aiParams.ai.maxPoolParams);
        break;

    case e_NetAvgPoolType:
        decode(expressParams, aiParams.ai.avgPoolParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        decode(expressParams, aiParams.ai.adaptiveAvgPoolParams);
        break;

    case e_NetConvTransposeType:
        decode(expressParams, aiParams.ai.convTransposeParams);
        break;

    case e_NetUpSampleType:
        decode(expressParams, aiParams.ai.upSampleParams);
        break;

    case e_NetLinearType:
        decode(expressParams, aiParams.ai.linearParams);
        break;

    case e_NetDropOutType:
        decode(expressParams, aiParams.ai.dropOutParams);
        break;

    case e_NetDropPathType:
        decode(expressParams, aiParams.ai.dropPathParams);
        break;

    case e_NetBatchNormType:
        decode(expressParams, aiParams.ai.batchNormParams);
        break;

    case e_NetGroupNormType:
        decode(expressParams, aiParams.ai.groupNormParams);
        break;

    case e_NetInstanceNormType:
        decode(expressParams, aiParams.ai.instanceNormParams);
        break;

    case e_NetLayerNormType:
        decode(expressParams, aiParams.ai.layerNormParams);
        break;

    case e_NetActiveType:
        decode(expressParams, aiParams.ai.activeParams);
        break;

    case e_NetSoftMaxType:
        decode(expressParams, aiParams.ai.softMaxParams);
        break;

    case e_NetFlattenType:
        decode(expressParams, aiParams.ai.flattenParams);
        break;

    case e_NetChannelShuffleType:
        decode(expressParams, aiParams.ai.channelShuffleParams);
        break;

    case e_NetDetectLayer2DType:
        decode(expressParams, aiParams.ai.detectLayer2DParams);
        break;

    case e_NetCatType:
        decode(expressParams, aiParams.ai.catParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        decode(expressParams, aiParams.ai.scaledParams);
        break;

    case e_NetAddType:
        decode(expressParams, aiParams.ai.addParams);
        break;

    case e_NetConstType:
        decode(expressParams, aiParams.ai.constParams);
        break;

    case e_NetPermuteType:
        decode(expressParams, aiParams.ai.permuteParams);
        break;

    case e_NetViewType:
        decode(expressParams, aiParams.ai.viewParams);
        break;

    case e_NetSplitType:
        decode(expressParams, aiParams.ai.splitParams);
        break;

    case e_NetIdentityType:
        decode(expressParams, aiParams.ai.identityParams);
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        decode(expressParams, aiParams.ai.showResultParams);
        break;

    case e_NetBaseLossType:
        decode(expressParams, aiParams.ai.baseLossParams);
        break;

    case e_NetDiceLossType:
        decode(expressParams, aiParams.ai.diceLossParams);
        break;

    case e_NetDetectYoloLossType:
        decode(expressParams, aiParams.ai.detectYoloLossParams);
        break;

    case e_NetOptAdaDeltaType:
        decode(expressParams, aiParams.ai.optAdaDeltaParams);
        break;

    case e_NetOptAdaGradType:
        decode(expressParams, aiParams.ai.optAdaGradParams);
        break;

    case e_NetOptAdamType:
        decode(expressParams, aiParams.ai.optAdamParams);
        break;

    case e_NetOptASGDType:
        decode(expressParams, aiParams.ai.optASGDParams);
        break;

    case e_NetOptRMSpropType:
        decode(expressParams, aiParams.ai.optRMSpropParams);
        break;

    case e_NetOptSGDType:
        decode(expressParams, aiParams.ai.optSGDParams);
        break;

    case e_NetTrain2DType:
        decode(expressParams, aiParams.ai.train2DParams);
        break;

    case e_NetDataLoader2DType:
        decode(expressParams, aiParams.ai.dataLoader2DParams);
        break;

    case e_NetDataInputTestType:
        decode(expressParams, aiParams.ai.dataInputTestParams);
        break;

    case e_NetDataOutputType:
        decode(expressParams, aiParams.ai.dataOutputParams);
        break;


    // 定制网元: custom
    case e_NetCustomType:
        decode(expressParams, aiParams.ai.customParams);
        break;
    }
}

void CAttrViewMgr::showParams(int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams)
{
    auto wgAttrList = new WgAttrList(m_listwgAttr);

    wgAttrList->Init(sn, netId, netName, aiParams);
    wgAttrList->show();

    connect(wgAttrList, SIGNAL(signalUpdate(int, QString const&, QString const&)), this, SLOT(updateListSlot(int, QString const&, QString const&)));
    connect(wgAttrList, SIGNAL(signalParamsChanged(int, QString const&, QString const&, NetAiParams const&)), this, SLOT(paramsChangedSlot(int, QString const&, QString const&, NetAiParams const&)));

    auto item = new QListWidgetItem(m_listwgAttr);
    item->setSizeHint(QSize(m_listwgAttr->width(), wgAttrList->height()));

    m_listwgAttr->addItem(item);
    m_listwgAttr->setItemWidget(item, wgAttrList);
}

void CAttrViewMgr::showAttrTreeSlot(QJsonObject const &aiParams)
{
    auto val = aiParams.find("04_logic").value();
    if (!val.isObject())
        return;

    if (!aiParams.contains("01_net") || !aiParams.value("01_net").isString())
        return;
    auto netId = aiParams.value("01_net").toString();

    if (!aiParams.contains("02_sn") || !aiParams.value("02_sn").isDouble())
        return;
    auto sn = aiParams.value("02_sn").toInt();

    if (!aiParams.contains("03_express") || !aiParams.value("03_express").isObject())
        return;
    auto expressParams = aiParams.value("03_express").toObject();

    NetAiParams netAiParams;
    decodeParams(netId, expressParams, netAiParams);
    auto root = addParamstoTree(nullptr, sn, netId, netId + "_" + QString::number(sn), netAiParams);

    auto logicParams = val.toObject();
    val = logicParams.find("01_items").value();
    if (!val.isArray())
        return;

    doShowAttrTree(root, val.toArray());
}

void CAttrViewMgr::markNetSlot(int sn, QString const &netId, QString const &netName, vector<NetMark> extendNetMarkStack)
{
    netName;

    auto root = m_treeWidget->topLevelItem(0);
    if (root == nullptr)
        return;

    clearMark(root);

    root->setTextColor(0, Qt::green);

    markNet(root, sn, netId, netName, extendNetMarkStack);
}

void CAttrViewMgr::updateListSlot(int sn, QString const &, QString const &)
{
    for (auto i = 0; i < m_listwgAttr->count(); i++)
    {
        auto item = m_listwgAttr->item(i);
        auto wgAttrList = dynamic_cast<WgAttrList const*>(m_listwgAttr->itemWidget(item));
        if (wgAttrList == nullptr || wgAttrList->GetSn() != sn)
            continue;

        //qDebug() << i << ": " << item->sizeHint().height() << ", " << wgAttrList->height();
        //auto w = m_listwgAttr->verticalScrollBar()->width();

        item->setSizeHint(QSize(m_listwgAttr->width(), wgAttrList->height()));
    }
}

void CAttrViewMgr::paramsChangedSlot(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams)
{
    QJsonObject expressParams;
    encodeParams(aiParams, expressParams);
    emit paramsChangedSignal(sn, netId, netName, expressParams);
}

void CAttrViewMgr::encodeParams(NetAiParams const &aiParams, QJsonObject &expressParams)
{
    switch (aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        encode(aiParams.ai.convParams, expressParams);
        break;

    case e_NetMaxPoolType:
        encode(aiParams.ai.maxPoolParams, expressParams);
        break;

    case e_NetAvgPoolType:
        encode(aiParams.ai.avgPoolParams, expressParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        encode(aiParams.ai.adaptiveAvgPoolParams, expressParams);
        break;

    case e_NetConvTransposeType:
        encode(aiParams.ai.convTransposeParams, expressParams);
        break;

    case e_NetUpSampleType:
        encode(aiParams.ai.upSampleParams, expressParams);
        break;

    case e_NetLinearType:
        encode(aiParams.ai.linearParams, expressParams);
        break;

    case e_NetDropOutType:
        encode(aiParams.ai.dropOutParams, expressParams);
        break;

    case e_NetDropPathType:
        encode(aiParams.ai.dropPathParams, expressParams);
        break;

    case e_NetBatchNormType:
        encode(aiParams.ai.batchNormParams, expressParams);
        break;

    case e_NetGroupNormType:
        encode(aiParams.ai.groupNormParams, expressParams);
        break;

    case e_NetInstanceNormType:
        encode(aiParams.ai.instanceNormParams, expressParams);
        break;

    case e_NetLayerNormType:
        encode(aiParams.ai.layerNormParams, expressParams);
        break;

    case e_NetActiveType:
        encode(aiParams.ai.activeParams, expressParams);
        break;

    case e_NetSoftMaxType:
        encode(aiParams.ai.softMaxParams, expressParams);
        break;

    case e_NetFlattenType:
        encode(aiParams.ai.flattenParams, expressParams);
        break;

    case e_NetChannelShuffleType:
        encode(aiParams.ai.channelShuffleParams, expressParams);
        break;

    case e_NetDetectLayer2DType:
        encode(aiParams.ai.detectLayer2DParams, expressParams);
        break;

    case e_NetCatType:
        encode(aiParams.ai.catParams, expressParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        encode(aiParams.ai.scaledParams, expressParams);
        break;

    case e_NetAddType:
        encode(aiParams.ai.addParams, expressParams);
        break;

    case e_NetConstType:
        encode(aiParams.ai.constParams, expressParams);
        break;

    case e_NetPermuteType:
        encode(aiParams.ai.permuteParams, expressParams);
        break;

    case e_NetViewType:
        encode(aiParams.ai.viewParams, expressParams);
        break;

    case e_NetSplitType:
        encode(aiParams.ai.splitParams, expressParams);
        break;

    case e_NetIdentityType:
        encode(aiParams.ai.identityParams, expressParams);
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        encode(aiParams.ai.showResultParams, expressParams);
        break;

    case e_NetBaseLossType:
        encode(aiParams.ai.baseLossParams, expressParams);
        break;

    case e_NetDiceLossType:
        encode(aiParams.ai.diceLossParams, expressParams);
        break;

    case e_NetDetectYoloLossType:
        encode(aiParams.ai.detectYoloLossParams, expressParams);
        break;

    case e_NetOptAdaDeltaType:
        encode(aiParams.ai.optAdaDeltaParams, expressParams);
        break;

    case e_NetOptAdaGradType:
        encode(aiParams.ai.optAdaGradParams, expressParams);
        break;

    case e_NetOptAdamType:
        encode(aiParams.ai.optAdamParams, expressParams);
        break;

    case e_NetOptASGDType:
        encode(aiParams.ai.optASGDParams, expressParams);
        break;

    case e_NetOptRMSpropType:
        encode(aiParams.ai.optRMSpropParams, expressParams);
        break;

    case e_NetOptSGDType:
        encode(aiParams.ai.optSGDParams, expressParams);
        break;

    case e_NetTrain2DType:
        encode(aiParams.ai.train2DParams, expressParams);
        break;

    case e_NetDataLoader2DType:
        encode(aiParams.ai.dataLoader2DParams, expressParams);
        break;

    case e_NetDataInputTestType:
        encode(aiParams.ai.dataInputTestParams, expressParams);
        break;

    case e_NetDataOutputType:
        encode(aiParams.ai.dataOutputParams, expressParams);
        break;


    // 定制网元: custom
    case e_NetCustomType:
        encode(aiParams.ai.customParams, expressParams);
        break;
    }
}

// 网络单元: netunit
/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_paddingMode": "zeros",
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::encode(NetConvParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;
    expressParams["02_inChannels"] = aiParams.inChannels;
    expressParams["03_outChannels"] = aiParams.outChannels;
    expressParams["04_group"] = std::min(aiParams.group, aiParams.inChannels);

    QJsonArray jsonKernelSize;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonKernelSize.append(aiParams.kernelSize[i]);
    }
    expressParams["05_kernelSize"] = jsonKernelSize;

    QJsonArray jsonStride;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonStride.append(aiParams.stride[i]);
    }
    expressParams["06_stride"] = jsonStride;

    QJsonArray jsonDilation;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonDilation.append(aiParams.dilation[i]);
    }
    expressParams["07_dilation"] = jsonDilation;

    QJsonArray jsonPadding;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonPadding.append(aiParams.padding[i]);
    }
    expressParams["08_padding"] = jsonPadding;

    expressParams["09_paddingMode"] = aiParams.paddingMode;
    expressParams["10_requiresGrad"] = aiParams.requiresGrad;
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_dilation": [1, 1, 1],
    "05_padding": [0, 0, 0],
    "06_requiresGrad": 1
}*/
void CAttrViewMgr::encode(NetMaxPoolParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;

    QJsonArray jsonKernelSize;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonKernelSize.append(aiParams.kernelSize[i]);
    }
    expressParams["02_kernelSize"] = jsonKernelSize;

    QJsonArray jsonStride;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonStride.append(aiParams.stride[i]);
    }
    expressParams["03_stride"] = jsonStride;

    QJsonArray jsonDilation;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonDilation.append(aiParams.dilation[i]);
    }
    expressParams["04_dilation"] = jsonDilation;

    QJsonArray jsonPadding;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonPadding.append(aiParams.padding[i]);
    }
    expressParams["05_padding"] = jsonPadding;

    expressParams["06_requiresGrad"] = aiParams.requiresGrad;
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_padding": [0, 0, 0],
    "05_requiresGrad": 1
}*/
void CAttrViewMgr::encode(NetAvgPoolParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;

    QJsonArray jsonKernelSize;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonKernelSize.append(aiParams.kernelSize[i]);
    }
    expressParams["02_kernelSize"] = jsonKernelSize;

    QJsonArray jsonStride;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonStride.append(aiParams.stride[i]);
    }
    expressParams["03_stride"] = jsonStride;

    QJsonArray jsonPadding;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonPadding.append(aiParams.padding[i]);
    }
    expressParams["04_padding"] = jsonPadding;

    expressParams["05_requiresGrad"] = aiParams.requiresGrad;
}

/*{
    "01_dim": 2,
    "02_outputSize": [32, 32, 32]
}*/
void CAttrViewMgr::encode(NetAdaptiveAvgPoolParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;

    QJsonArray jsonOutputSize;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonOutputSize.append(aiParams.outputSize[i]);
    }
    expressParams["02_outputSize"] = jsonOutputSize;
}

/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_outputPadding": [0, 0, 0],
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::encode(NetConvTransposeParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;
    expressParams["02_inChannels"] = aiParams.inChannels;
    expressParams["03_outChannels"] = aiParams.outChannels;
    expressParams["04_group"] = std::min(aiParams.group, aiParams.inChannels);

    QJsonArray jsonKernelSize;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonKernelSize.append(aiParams.kernelSize[i]);
    }
    expressParams["05_kernelSize"] = jsonKernelSize;

    QJsonArray jsonStride;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonStride.append(aiParams.stride[i]);
    }
    expressParams["06_stride"] = jsonStride;

    QJsonArray jsonDilation;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonDilation.append(aiParams.dilation[i]);
    }
    expressParams["07_dilation"] = jsonDilation;

    QJsonArray jsonPadding;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonPadding.append(aiParams.padding[i]);
    }
    expressParams["08_padding"] = jsonPadding;

    QJsonArray jsonOutputPadding;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonOutputPadding.append(aiParams.outputPadding[i]);
    }
    expressParams["09_outputPadding"] = jsonOutputPadding;

    expressParams["10_requiresGrad"] = aiParams.requiresGrad;
}

/*{
    "01_dim": 2,
    "02_scaleFactor": [2.0, 2.0, 2.0],
    "03_mode": "nearest"
}*/
void CAttrViewMgr::encode(NetUpSampleParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;

    QJsonArray jsonScaleFactor;
    for (auto i = 0; i < aiParams.dim; i++)
    {
        jsonScaleFactor.append(aiParams.scaleFactor[i]);
    }
    expressParams["02_scaleFactor"] = jsonScaleFactor;

    expressParams["03_mode"] = aiParams.mode;
}

/*{
    "01_inFeatures": 64,
    "02_outFeatures": 64,
    "03_bias": true,
    "04_requiresGrad": 1
}*/
void CAttrViewMgr::encode(NetLinearParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_inFeatures"] = aiParams.inFeatures;
    expressParams["02_outFeatures"] = aiParams.outFeatures;
    expressParams["03_bias"] = aiParams.bias;
    expressParams["04_requiresGrad"] = aiParams.requiresGrad;
}

/*{
    "01_dim": 2,
    "02_prob": 0.5
}*/
void CAttrViewMgr::encode(NetDropOutParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;
    expressParams["02_prob"] = aiParams.prob;
}

/*{
    "01_prob": 0.5
}*/
void CAttrViewMgr::encode(NetDropPathParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_prob"] = aiParams.prob;
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::encode(NetBatchNormParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;
    expressParams["02_numFeatures"] = aiParams.numFeatures;
}

/*{
    "01_numGroups": 2,
    "02_numChannels": 64
}*/
void CAttrViewMgr::encode(NetGroupNormParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_numGroups"] = aiParams.numGroups;
    expressParams["02_numChannels"] = aiParams.numChannels;
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::encode(NetInstanceNormParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_dim"] = aiParams.dim;
    expressParams["02_numFeatures"] = aiParams.numFeatures;
}

/*{
    "01_normalizedShape": [0, 1, 3, 2]
}*/
void CAttrViewMgr::encode(NetLayerNormParams const &aiParams, QJsonObject &expressParams)
{
    QJsonArray jsonNormalizedShape;
    int cnt = sizeof(aiParams.normalizedShape) / sizeof(aiParams.normalizedShape[0]);
    for (auto i = 0; i < cnt; i++)
    {
        jsonNormalizedShape.append(aiParams.normalizedShape[i]);
    }
    expressParams["01_normalizedShape"] = jsonNormalizedShape;
}

/*{
    "01_function": "SiLU"
}*/
void CAttrViewMgr::encode(NetActiveParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_function"] = aiParams.function;
}

/*{
    "01_softMaxDim": 2
}*/
void CAttrViewMgr::encode(NetSoftMaxParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_softMaxDim"] = aiParams.softMaxDim;
}

/*{
    "01_startDim": 1,
    "02_endDim": -1
}*/
void CAttrViewMgr::encode(NetFlattenParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_startDim"] = aiParams.startDim;
    expressParams["02_endDim"] = aiParams.endDim;
}

/*{
    "01_groups": 2
}*/
void CAttrViewMgr::encode(NetChannelShuffleParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_groups"] = aiParams.groups;
}

/*{
    "01_numberClass": 2,
    "02_anchorSize": [[210, 210], [300, 400], [300, 400]],
    "03_imageSize": 416
}*/
void CAttrViewMgr::encode(NetDetectLayer2DParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_numberClass"] = aiParams.numberClass;

    QJsonArray jsonAnchorSize;
    int rows = sizeof(aiParams.anchorSize) / sizeof(aiParams.anchorSize[0]);
    int cols = sizeof(aiParams.anchorSize[0]) / sizeof(aiParams.anchorSize[0][0]);
    for (auto rIndex = 0; rIndex < rows; rIndex++)
    {
        QJsonArray jsonAnchorSizeRow;
        for (auto cIndex = 0; cIndex < cols; cIndex++)
        {
            jsonAnchorSizeRow.append(aiParams.anchorSize[rIndex][cIndex]);
        }

        jsonAnchorSize.append(jsonAnchorSizeRow);
    }
    expressParams["02_anchorSize"] = jsonAnchorSize;

    expressParams["03_imageSize"] = aiParams.imageSize;
}

/*{
    "01_catDim": 0
}*/
void CAttrViewMgr::encode(NetCatParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_catDim"] = aiParams.catDim;
}


// 计算单元: calunit
/*{

}*/
void CAttrViewMgr::encode(NetScaledParams const &, QJsonObject &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::encode(NetAddParams const &, QJsonObject &)
{
    // no-op
}

/*{
    "01_input": 0
}*/
void CAttrViewMgr::encode(NetConstParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_input"] = aiParams.input;
}

/*{
    "01_swapDims": [0, 1, 3, 2]
}*/
void CAttrViewMgr::encode(NetPermuteParams const &aiParams, QJsonObject &expressParams)
{
    QJsonArray jsonSwapdims;
    int cnt = sizeof(aiParams.swapDims) / sizeof(aiParams.swapDims[0]);
    for (auto i = 0; i < cnt; i++)
    {
        jsonSwapdims.append(aiParams.swapDims[i]);
    }
    expressParams["01_swapDims"] = jsonSwapdims;
}

/*{
    "01_changedims": 0
}*/
void CAttrViewMgr::encode(NetViewParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_changedims"] = aiParams.changeDims;
}

/*{
    "01_splitsize": 2
    "02_dim": 0
}*/
void CAttrViewMgr::encode(NetSplitParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_splitsize"] = aiParams.splitSize;
    expressParams["02_dim"] = aiParams.dim;
}

/*{

}*/
void CAttrViewMgr::encode(NetIdentityParams const &, QJsonObject &)
{
    // no-op
}


// 运行单元: rununit
/*{

}*/
void CAttrViewMgr::encode(NetShowResultParams const &, QJsonObject &)
{
    // no-op
}

/*{
    "01_function": "CrossEntropyLoss"
    "02_p": 1.0
}*/
void CAttrViewMgr::encode(NetBaseLossParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_function"] = aiParams.function;
    expressParams["02_p"] = aiParams.p;
}

/*{

}*/
void CAttrViewMgr::encode(NetDiceLossParams const &, QJsonObject &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::encode(NetDetectYoloLossParams const &, QJsonObject &)
{
    // no-op
}

/*{
    "01_lr": 1.0,
    "02_rho": 0.9,
    "03_weightDecay": 0.0,
    "04_eps": 1.00E-06
}*/
void CAttrViewMgr::encode(NetOptAdaDeltaParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_lr"] = aiParams.lr;
    expressParams["02_rho"] = aiParams.rho;
    expressParams["03_weightDecay"] = aiParams.weightDecay;
    expressParams["04_eps"] = aiParams.eps;
}

/*{
    "01_lr": 0.01,
    "02_lrDecay": 0.0,
    "03_weightDecay": 0.0,
    "04_initialAccumulatorValue": 0.0,
    "05_eps": 1.00E-06
}*/
void CAttrViewMgr::encode(NetOptAdaGradParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_lr"] = aiParams.lr;
    expressParams["02_lrDecay"] = aiParams.lrDecay;
    expressParams["03_weightDecay"] = aiParams.weightDecay;
    expressParams["04_initialAccumulatorValue"] = aiParams.initialAccumulatorValue;
    expressParams["05_eps"] = aiParams.eps;
}

/*{
    "01_lr": 0.01,
    "02_eps": 1.00E-06,
    "03_weightDecay": 0.0
}*/
void CAttrViewMgr::encode(NetOptAdamParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_lr"] = aiParams.lr;
    expressParams["02_eps"] = aiParams.eps;
    expressParams["03_weightDecay"] = aiParams.weightDecay;
}

/*{

}*/
void CAttrViewMgr::encode(NetOptASGDParams const &, QJsonObject &)
{
    // no-op
}

/*{
    "01_lr": 0.01,
    "02_alpha": 9.90E-01,
    "03_eps": 1.00E-06,
    "04_weightDecay": 0.0,
    "05_momentum": 0.0
}*/
void CAttrViewMgr::encode(NetOptRMSpropParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_lr"] = aiParams.lr;
    expressParams["02_alpha"] = aiParams.alpha;
    expressParams["03_eps"] = aiParams.eps;
    expressParams["04_weightDecay"] = aiParams.weightDecay;
    expressParams["05_momentum"] = aiParams.momentum;
}

/*{
    "01_lr": 0.01,
    "02_momentum": 0.0,
    "03_dampening": 0.0,
    "04_weightDecay": 0.0
}*/
void CAttrViewMgr::encode(NetOptSGDParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_lr"] = aiParams.lr;
    expressParams["02_momentum"] = aiParams.momentum;
    expressParams["03_dampening"] = aiParams.dampening;
    expressParams["04_weightDecay"] = aiParams.weightDecay;
}

/*{
    "01_savePthPath": ".",
    "02_preweightsPath": ".",
    "03_logDir": "",
    "04_epochNum": 100,
    "05_batchSize": 16,
    "06_isCuda": false,
    "07_classNums": [1, 1, 1, 1, 1],
    "08_criteriaFunction": "Dice",
    "09_isTrainDataCriteria": true,
    "10_showLossFigure": true,
    "11_showCriteriaFigure": true,
    "12_epochSaveModel": 1
}*/
void CAttrViewMgr::encode(NetTrain2DParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_savePthPath"] = aiParams.savePthPath;
    expressParams["02_preweightsPath"] = aiParams.preweightsPath;
    expressParams["03_logDir"] = aiParams.logDir;
    expressParams["04_epochNum"] = aiParams.epochNum;
    expressParams["05_batchSize"] = aiParams.batchSize;
    expressParams["06_isCuda"] = aiParams.isCuda;

    QJsonArray jsonClassNums;
    int cnt = sizeof(aiParams.classNums) / sizeof(aiParams.classNums[0]);
    for (auto i = 0; i < cnt; i++)
    {
        jsonClassNums.append(aiParams.classNums[i]);
    }
    expressParams["07_classNums"] = jsonClassNums;

    expressParams["08_criteriaFunction"] = aiParams.criteriaFunction;
    expressParams["09_isTrainDataCriteria"] = aiParams.isTrainDataCriteria;
    expressParams["10_showLossFigure"] = aiParams.showLossFigure;
    expressParams["11_showCriteriaFigure"] = aiParams.showCriteriaFigure;
    expressParams["12_epochSaveModel"] = aiParams.epochSaveModel;
}

/*{
    "01_inputW": 256,
    "02_inputH": 256,
    "03_dataJsonPath": "",
    "04_dataLabelCodeJsonPath": "",
    "05_isRgb": true,
    "06_batchSize": 16,
    "07_numWorkers": 1,
    "08_shuffle": true
}*/
void CAttrViewMgr::encode(NetDataLoader2DParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_inputW"] = aiParams.inputW;
    expressParams["02_inputH"] = aiParams.inputH;
    expressParams["03_dataJsonPath"] = aiParams.dataJsonPath;
    expressParams["04_dataLabelCodeJsonPath"] = aiParams.dataLabelCodeJsonPath;
    expressParams["05_isRgb"] = aiParams.isRgb;
    expressParams["06_batchSize"] = aiParams.batchSize;
    expressParams["07_numWorkers"] = aiParams.numWorkers;
    expressParams["08_shuffle"] = aiParams.shuffle;
}

/*{
    "01_inputDim": 0
}*/
void CAttrViewMgr::encode(NetDataInputTestParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_inputDim"] = aiParams.inputDim;
}

/*{

}*/
void CAttrViewMgr::encode(NetDataOutputParams const &, QJsonObject &)
{
    // no-op
}


// 定制网元: custom
/*{
    "01_cycle": 1,
    "02_reqGrad": 1
}*/
void CAttrViewMgr::encode(NetCustomParams const &aiParams, QJsonObject &expressParams)
{
    expressParams["01_cycle"] = aiParams.cycle;
    expressParams["02_reqGrad"] = aiParams.requiresGrad;
}

QString CAttrViewMgr::toStr(bool val) const
{
    return val ? "true" : "false";
}

QString CAttrViewMgr::toStr(char const* val) const
{
    return val;
}

QString CAttrViewMgr::toStr(int val) const
{
    return QString::number(val);
}

QString CAttrViewMgr::toStr(int const* arr, int n) const
{
    QString str = "[";
    for (auto i = 0; i < n; i++)
    {
        if (i > 0)
            str += ", ";

        str += QString::number(arr[i]);
    }
    str += "]";

    return str;
}

QString CAttrViewMgr::toStr(int const *tab, int rows, int cols) const
{
    QString str = "[";
    for (auto i = 0; i < rows; i++)
    {
        str += "[";
        for (auto j = 0; j < cols; j++)
        {
            if (j > 0)
                str += ", ";

            str += QString::number(tab[i * cols + j]);
        }
        str += "]";
    }
    str += "]";

    return str;
}

QString CAttrViewMgr::toStr(double val) const
{
    return QString::number(val);
}

QString CAttrViewMgr::toStr(double const *arr, int n) const
{
    QString str = "[";
    for (auto i = 0; i < n; i++)
    {
        if (i > 0)
            str += ", ";

        str += QString::number(arr[i]);
    }
    str += "]";

    return str;
}

void CAttrViewMgr::doShowAttrTree(QTreeWidgetItem *parent, QJsonArray const &items)
{
    for(auto itor = items.begin(); itor != items.end(); ++itor)
    {
        auto valRef = itor[0];
        if (!valRef.isObject())
            continue;

        auto jsonItem = valRef.toObject();

        if (!jsonItem.contains("01_net") || !jsonItem.value("01_net").isString())
            continue;
        auto netId = jsonItem.value("01_net").toString();

        if (!jsonItem.contains("02_sn") || !jsonItem.value("02_sn").isDouble())
            continue;
        auto sn = jsonItem.value("02_sn").toInt();

        if (!jsonItem.contains("03_express") || !jsonItem.value("03_express").isObject())
            continue;
        auto expressParams = jsonItem.value("03_express").toObject();

        NetAiParams netAiParams;
        decodeParams(netId, expressParams, netAiParams);
        auto child = addParamstoTree(parent, sn, netId, netId + "_" + QString::number(sn), netAiParams);

        if (!jsonItem.contains("04_logic") || !jsonItem.value("04_logic").isObject())
            continue;
        auto logicParams = jsonItem.value("04_logic").toObject();

        auto val = logicParams.find("01_items").value();
        if (!val.isArray())
            continue;

        doShowAttrTree(child, val.toArray());
    }
}

QTreeWidgetItem* CAttrViewMgr::addParamstoTree(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams)
{
    auto newNode = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    newNode->setToolTip(0, netName);
    newNode->setText(0, netId + "_" + QString::number(sn));

    if (parent == nullptr)
    {
        m_treeWidget->clear();
        m_treeWidget->setColumnCount(1);
        m_treeWidget->addTopLevelItem(newNode);
        addNodeParams(newNode, aiParams);
    }
    else
    {
        parent->addChild(newNode);
        addNodeParams(newNode, aiParams);
    }

    m_treeWidget->expandItem(newNode);
    //m_treeWidget->collapseItem(newNode);

    return newNode;
}

void CAttrViewMgr::addNodeParams(QTreeWidgetItem *node, NetAiParams const &aiParams)
{
    switch (aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        addParams(node, aiParams.ai.convParams);
        break;

    case e_NetMaxPoolType:
        addParams(node, aiParams.ai.maxPoolParams);
        break;

    case e_NetAvgPoolType:
        addParams(node, aiParams.ai.avgPoolParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        addParams(node, aiParams.ai.adaptiveAvgPoolParams);
        break;

    case e_NetConvTransposeType:
        addParams(node, aiParams.ai.convTransposeParams);
        break;

    case e_NetUpSampleType:
        addParams(node, aiParams.ai.upSampleParams);
        break;

    case e_NetLinearType:
        addParams(node, aiParams.ai.linearParams);
        break;

    case e_NetDropOutType:
        addParams(node, aiParams.ai.dropOutParams);
        break;

    case e_NetDropPathType:
        addParams(node, aiParams.ai.dropPathParams);
        break;

    case e_NetBatchNormType:
        addParams(node, aiParams.ai.batchNormParams);
        break;

    case e_NetGroupNormType:
        addParams(node, aiParams.ai.groupNormParams);
        break;

    case e_NetInstanceNormType:
        addParams(node, aiParams.ai.instanceNormParams);
        break;

    case e_NetLayerNormType:
        addParams(node, aiParams.ai.layerNormParams);
        break;

    case e_NetActiveType:
        addParams(node, aiParams.ai.activeParams);
        break;

    case e_NetSoftMaxType:
        addParams(node, aiParams.ai.softMaxParams);
        break;

    case e_NetFlattenType:
        addParams(node, aiParams.ai.flattenParams);
        break;

    case e_NetChannelShuffleType:
        addParams(node, aiParams.ai.channelShuffleParams);
        break;

    case e_NetDetectLayer2DType:
        addParams(node, aiParams.ai.detectLayer2DParams);
        break;

    case e_NetCatType:
        addParams(node, aiParams.ai.catParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        addParams(node, aiParams.ai.scaledParams);
        break;

    case e_NetAddType:
        addParams(node, aiParams.ai.addParams);
        break;

    case e_NetConstType:
        addParams(node, aiParams.ai.constParams);
        break;

    case e_NetPermuteType:
        addParams(node, aiParams.ai.permuteParams);
        break;

    case e_NetViewType:
        addParams(node, aiParams.ai.viewParams);
        break;

    case e_NetSplitType:
        addParams(node, aiParams.ai.splitParams);
        break;

    case e_NetIdentityType:
        addParams(node, aiParams.ai.identityParams);
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        addParams(node, aiParams.ai.showResultParams);
        break;

    case e_NetBaseLossType:
        addParams(node, aiParams.ai.baseLossParams);
        break;

    case e_NetDiceLossType:
        addParams(node, aiParams.ai.diceLossParams);
        break;

    case e_NetDetectYoloLossType:
        addParams(node, aiParams.ai.detectYoloLossParams);
        break;

    case e_NetOptAdaDeltaType:
        addParams(node, aiParams.ai.optAdaDeltaParams);
        break;

    case e_NetOptAdaGradType:
        addParams(node, aiParams.ai.optAdaGradParams);
        break;

    case e_NetOptAdamType:
        addParams(node, aiParams.ai.optAdamParams);
        break;

    case e_NetOptASGDType:
        addParams(node, aiParams.ai.optASGDParams);
        break;

    case e_NetOptRMSpropType:
        addParams(node, aiParams.ai.optRMSpropParams);
        break;

    case e_NetOptSGDType:
        addParams(node, aiParams.ai.optSGDParams);
        break;

    case e_NetTrain2DType:
        addParams(node, aiParams.ai.train2DParams);
        break;

    case e_NetDataLoader2DType:
        addParams(node, aiParams.ai.dataLoader2DParams);
        break;

    case e_NetDataInputTestType:
        addParams(node, aiParams.ai.dataInputTestParams);
        break;

    case e_NetDataOutputType:
        addParams(node, aiParams.ai.dataOutputParams);
        break;


    // 定制网元: custom
    case e_NetCustomType:
        addParams(node, aiParams.ai.customParams);
        break;
    }
}

// 网络单元: netunit
/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_paddingMode": "zeros",
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetConvParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "inChannels", toStr(aiParams.inChannels));
    doAddParam(node, "outChannels", toStr(aiParams.outChannels));
    doAddParam(node, "kernelSize", toStr(aiParams.kernelSize, aiParams.dim));
    doAddParam(node, "stride", toStr(aiParams.stride, aiParams.dim));
    doAddParam(node, "dilation", toStr(aiParams.dilation, aiParams.dim));
    doAddParam(node, "padding", toStr(aiParams.padding, aiParams.dim));
    doAddParam(node, "paddingMode", toStr(aiParams.paddingMode));
    doAddParam(node, "group", toStr(aiParams.group));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_dilation": [1, 1, 1],
    "05_padding": [0, 0, 0],
    "06_requiresGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetMaxPoolParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "kernelSize", toStr(aiParams.kernelSize, aiParams.dim));
    doAddParam(node, "stride", toStr(aiParams.stride, aiParams.dim));
    doAddParam(node, "dilation", toStr(aiParams.dilation, aiParams.dim));
    doAddParam(node, "padding", toStr(aiParams.padding, aiParams.dim));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_padding": [0, 0, 0],
    "05_requiresGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetAvgPoolParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "kernelSize", toStr(aiParams.kernelSize, aiParams.dim));
    doAddParam(node, "stride", toStr(aiParams.stride, aiParams.dim));
    doAddParam(node, "padding", toStr(aiParams.padding, aiParams.dim));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

/*{
    "01_dim": 2,
    "02_outputSize": [32, 32, 32]
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetAdaptiveAvgPoolParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "outputSize", toStr(aiParams.outputSize, aiParams.dim));
}

/*{
    "01_dim": 2,
    "02_inChannels": 64,
    "03_outChannels": 64,
    "04_group": 1,
    "05_kernelSize": [3, 3, 3],
    "06_stride": [1, 1, 1],
    "07_dilation": [1, 1, 1],
    "08_padding": [0, 0, 0],
    "09_outputPadding": [0, 0, 0],
    "10_requiresGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetConvTransposeParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "inChannels", toStr(aiParams.inChannels));
    doAddParam(node, "outChannels", toStr(aiParams.outChannels));
    doAddParam(node, "kernelSize", toStr(aiParams.kernelSize, aiParams.dim));
    doAddParam(node, "stride", toStr(aiParams.stride, aiParams.dim));
    doAddParam(node, "dilation", toStr(aiParams.dilation, aiParams.dim));
    doAddParam(node, "padding", toStr(aiParams.padding, aiParams.dim));
    doAddParam(node, "outputPadding", toStr(aiParams.outputPadding, aiParams.dim));
    doAddParam(node, "group", toStr(aiParams.group));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

/*{
    "01_dim": 2,
    "02_scaleFactor": [2.0, 2.0, 2.0],
    "03_mode": "nearest"
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetUpSampleParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "scaleFactor", toStr(aiParams.scaleFactor, aiParams.dim));
    doAddParam(node, "mode", toStr(aiParams.mode));
}

/*{
    "01_inFeatures": 64,
    "02_outFeatures": 64,
    "03_bias": true,
    "04_requiresGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetLinearParams const &aiParams)
{
    doAddParam(node, "inFeatures", toStr(aiParams.inFeatures));
    doAddParam(node, "outFeatures", toStr(aiParams.outFeatures));
    doAddParam(node, "bias", toStr(aiParams.bias));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

/*{
    "01_dim": 2,
    "02_prob": 0.5
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetDropOutParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "prob", toStr(aiParams.prob));
}

/*{
    "01_prob": 0.5
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetDropPathParams const &aiParams)
{
    doAddParam(node, "prob", toStr(aiParams.prob));
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetBatchNormParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "numFeatures", toStr(aiParams.numFeatures));
}

/*{
    "01_numGroups": 2,
    "02_numChannels": 64
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetGroupNormParams const &aiParams)
{
    doAddParam(node, "numGroups", toStr(aiParams.numGroups));
    doAddParam(node, "numChannels", toStr(aiParams.numChannels));
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetInstanceNormParams const &aiParams)
{
    doAddParam(node, "dim", toStr(aiParams.dim));
    doAddParam(node, "numFeatures", toStr(aiParams.numFeatures));
}

/*{
    "01_normalizedShape": [0, 1, 3, 2]
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetLayerNormParams const &aiParams)
{
    int cnt = sizeof(aiParams.normalizedShape) / sizeof(aiParams.normalizedShape[0]);
    doAddParam(node, "normalizedShape", toStr(aiParams.normalizedShape, cnt));
}

/*{
    "01_function": "SiLU"
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetActiveParams const &aiParams)
{
    doAddParam(node, "function", toStr(aiParams.function));
}

/*{
    "01_softMaxDim": 2
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetSoftMaxParams const &aiParams)
{
    doAddParam(node, "softMaxDim", toStr(aiParams.softMaxDim));
}

/*{
    "01_startDim": 1,
    "02_endDim": -1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetFlattenParams const &aiParams)
{
    doAddParam(node, "startDim", toStr(aiParams.startDim));
    doAddParam(node, "endDim", toStr(aiParams.endDim));
}

/*{
    "01_groups": 2
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetChannelShuffleParams const &aiParams)
{
    doAddParam(node, "groups", toStr(aiParams.groups));
}

/*{
    "01_numberClass": 2,
    "02_anchorSize": [[210, 210], [300, 400], [300, 400]],
    "03_imageSize": 416
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetDetectLayer2DParams const &aiParams)
{
    doAddParam(node, "numberClass", toStr(aiParams.numberClass));

    int rows = sizeof(aiParams.anchorSize) / sizeof(aiParams.anchorSize[0]);
    int cols = sizeof(aiParams.anchorSize[0]) / sizeof(aiParams.anchorSize[0][0]);
    doAddParam(node, "anchorSize", toStr(aiParams.anchorSize[0], rows, cols));

    doAddParam(node, "imageSize", toStr(aiParams.imageSize));
}

/*{
    "01_catDim": 0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetCatParams const &aiParams)
{
    doAddParam(node, "catDim", toStr(aiParams.catDim));
}


// 计算单元: calunit
/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetScaledParams const &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetAddParams const &)
{
    // no-op
}

/*{
    "01_input": 0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetConstParams const &aiParams)
{
    doAddParam(node, "input", toStr(aiParams.input));
}

/*{
    "01_swapDims": [0, 1, 3, 2]
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetPermuteParams const &aiParams)
{
    int cnt = sizeof(aiParams.swapDims) / sizeof(aiParams.swapDims[0]);
    doAddParam(node, "swapDims", toStr(aiParams.swapDims, cnt));
}

/*{
    "01_changedims": 0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetViewParams const &aiParams)
{
    doAddParam(node, "changeDims", toStr(aiParams.changeDims));
}

/*{
    "01_splitsize": 2
    "02_dim": 0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetSplitParams const &aiParams)
{
    doAddParam(node, "splitSize", toStr(aiParams.splitSize));
    doAddParam(node, "dim", toStr(aiParams.dim));
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetIdentityParams const &)
{
    // no-op
}


// 运行单元: rununit
/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetShowResultParams const &)
{
    // no-op
}

/*{
    "01_function": "CrossEntropyLoss"
    "02_p": 1.0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetBaseLossParams const &aiParams)
{
    doAddParam(node, "function", toStr(aiParams.function));
    doAddParam(node, "p", toStr(aiParams.p));
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetDiceLossParams const &)
{
    // no-op
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetDetectYoloLossParams const &)
{
    // no-op
}

/*{
    "01_lr": 1.0,
    "02_rho": 0.9,
    "03_weightDecay": 0.0,
    "04_eps": 1.00E-06
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetOptAdaDeltaParams const &aiParams)
{
    doAddParam(node, "lr", toStr(aiParams.lr));
    doAddParam(node, "rho", toStr(aiParams.rho));
    doAddParam(node, "weightDecay", toStr(aiParams.weightDecay));
    doAddParam(node, "eps", toStr(aiParams.eps));
}

/*{
    "01_lr": 0.01,
    "02_lrDecay": 0.0,
    "03_weightDecay": 0.0,
    "04_initialAccumulatorValue": 0.0,
    "05_eps": 1.00E-06
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetOptAdaGradParams const &aiParams)
{
    doAddParam(node, "lr", toStr(aiParams.lr));
    doAddParam(node, "lrDecay", toStr(aiParams.lrDecay));
    doAddParam(node, "weightDecay", toStr(aiParams.weightDecay));
    doAddParam(node, "initialAccumulatorValue", toStr(aiParams.initialAccumulatorValue));
    doAddParam(node, "eps", toStr(aiParams.eps));
}

/*{
    "01_lr": 0.01,
    "02_eps": 1.00E-06,
    "03_weightDecay": 0.0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetOptAdamParams const &aiParams)
{
    doAddParam(node, "lr", toStr(aiParams.lr));
    doAddParam(node, "weightDecay", toStr(aiParams.weightDecay));
    doAddParam(node, "eps", toStr(aiParams.eps));
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetOptASGDParams const &)
{
    // no-op
}

/*{
    "01_lr": 0.01,
    "02_alpha": 9.90E-01,
    "03_eps": 1.00E-06,
    "04_weightDecay": 0.0,
    "05_momentum": 0.0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetOptRMSpropParams const &aiParams)
{
    doAddParam(node, "lr", toStr(aiParams.lr));
    doAddParam(node, "alpha", toStr(aiParams.alpha));
    doAddParam(node, "eps", toStr(aiParams.eps));
    doAddParam(node, "weightDecay", toStr(aiParams.weightDecay));
    doAddParam(node, "momentum", toStr(aiParams.momentum));
}

/*{
    "01_lr": 0.01,
    "02_momentum": 0.0,
    "03_dampening": 0.0,
    "04_weightDecay": 0.0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetOptSGDParams const &aiParams)
{
    doAddParam(node, "lr", toStr(aiParams.lr));
    doAddParam(node, "momentum", toStr(aiParams.momentum));
    doAddParam(node, "dampening", toStr(aiParams.dampening));
    doAddParam(node, "weightDecay", toStr(aiParams.weightDecay));
}

/*{
    "01_savePthPath": ".",
    "02_preweightsPath": ".",
    "03_logDir": "",
    "04_epochNum": 100,
    "05_batchSize": 16,
    "06_isCuda": false,
    "07_classNums": [1, 1, 1, 1, 1],
    "08_criteriaFunction": "Dice",
    "09_isTrainDataCriteria": true,
    "10_showLossFigure": true,
    "11_showCriteriaFigure": true,
    "12_epochSaveModel": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetTrain2DParams const &aiParams)
{
    doAddParam(node, "savePthPath", toStr(aiParams.savePthPath));
    doAddParam(node, "preweightsPath", toStr(aiParams.preweightsPath));
    doAddParam(node, "logDir", toStr(aiParams.logDir));
    doAddParam(node, "epochNum", toStr(aiParams.epochNum));
    doAddParam(node, "batchSize", toStr(aiParams.batchSize));
    doAddParam(node, "isCuda", toStr(aiParams.isCuda));

    int cnt = sizeof(aiParams.classNums) / sizeof(aiParams.classNums[0]);
    doAddParam(node, "classNums", toStr(aiParams.classNums, cnt));

    doAddParam(node, "criteriaFunction", toStr(aiParams.criteriaFunction));
    doAddParam(node, "isTrainDataCriteria", toStr(aiParams.isTrainDataCriteria));
    doAddParam(node, "showLossFigure", toStr(aiParams.showLossFigure));
    doAddParam(node, "showCriteriaFigure", toStr(aiParams.showCriteriaFigure));
    doAddParam(node, "epochSaveModel", toStr(aiParams.epochSaveModel));
}

/*{
    "01_inputW": 256,
    "02_inputH": 256,
    "03_dataJsonPath": "",
    "04_dataLabelCodeJsonPath": "",
    "05_isRgb": true,
    "06_batchSize": 16,
    "07_numWorkers": 1,
    "08_shuffle": true
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetDataLoader2DParams const &aiParams)
{
    doAddParam(node, "inputW", toStr(aiParams.inputW));
    doAddParam(node, "inputH", toStr(aiParams.inputH));
    doAddParam(node, "dataJsonPath", toStr(aiParams.dataJsonPath));
    doAddParam(node, "dataLabelCodeJsonPath", toStr(aiParams.dataLabelCodeJsonPath));
    doAddParam(node, "isRgb", toStr(aiParams.isRgb));
    doAddParam(node, "batchSize", toStr(aiParams.batchSize));
    doAddParam(node, "numWorkers", toStr(aiParams.numWorkers));
    doAddParam(node, "shuffle", toStr(aiParams.shuffle));
}

/*{
    "01_inputDim": 0
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetDataInputTestParams const &aiParams)
{
    doAddParam(node, "inputDim", toStr(aiParams.inputDim));
}

/*{

}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *, NetDataOutputParams const &)
{
    // no-op
}


// 定制网元: custom
/*{
    "01_cycle": 1,
    "02_reqGrad": 1
}*/
void CAttrViewMgr::addParams(QTreeWidgetItem *node, NetCustomParams const &aiParams)
{
    doAddParam(node, "cycle", toStr(aiParams.cycle));
    doAddParam(node, "requiresGrad", toStr(aiParams.requiresGrad));
}

void CAttrViewMgr::doAddParam(QTreeWidgetItem *node, QString const &name, QString const &value)
{
    auto child = new QTreeWidgetItem(QTreeWidgetItem::UserType + 1);
    child->setText(0, name + ": " + value);
    node->addChild(child);
    m_treeWidget->expandItem(child);
}

void CAttrViewMgr::clearMark(QTreeWidgetItem *parent)
{
    for (auto index = 0; index < parent->childCount(); index++)
    {
        auto child = parent->child(index);
        child->setBackgroundColor(0, Qt::white);
        child->setTextColor(0, Qt::black);
        clearMark(child);
    }
}

void CAttrViewMgr::markNet(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName, vector<NetMark> extendNetMarkStack)
{
    if (extendNetMarkStack.empty())
    {
        bool match = false;
        for (auto index = 0; index < parent->childCount(); index++)
        {
            auto child = parent->child(index);
            auto text = child->text(0);
            if (text == netId + "_" + QString::number(sn))
            {
                child->setTextColor(0, Qt::green);
                markParams(child, sn, netId, netName);
                match = true;
                break;
            }
        }

        if (match)
            return;

        markParams(parent, sn, netId, netName);
    }
    else
    {
        auto extendNetMark = extendNetMarkStack.front();
        vector<NetMark> nextExtendNetMarkStack(++extendNetMarkStack.begin(), extendNetMarkStack.end());

        for (auto index = 0; index < parent->childCount(); index++)
        {
            auto child = parent->child(index);
            auto text = child->text(0);
            if (text == extendNetMark.net + "_" + QString::number(extendNetMark.sn))
            {
                child->setTextColor(0, Qt::green);
                if (text == netId + "_" + QString::number(sn))
                {
                    markParams(child, sn, netId, netName);
                }
                else
                {
                    markNet(child, sn, netId, netName, nextExtendNetMarkStack);
                }
            }
        }
    }
}

void CAttrViewMgr::markParams(QTreeWidgetItem *parent, int sn, QString const &netId, QString const &netName)
{
    sn, netName;

    bool atomic = false;
    emit checkNetInfoSignal(netId, atomic);

    auto cnt = parent->childCount();
    if (!atomic && cnt > 2)
        cnt = 2;

    for (auto index = 0; index < cnt; index++)
    {
        auto child = parent->child(index);

        QColor color(255, 255, 191);
        child->setBackgroundColor(0, color);
        child->setTextColor(0, Qt::red);
    }
}

void CAttrViewMgr::treeItemDoubleClickedSlot(QTreeWidgetItem *item, int column)
{
    if (item->type() == QTreeWidgetItem::UserType)
        return;

    m_item = item;

    list<QTreeWidgetItem*> itemsLink;
    do {
        itemsLink.push_front(item);
        item = item->parent();
    } while (item != nullptr);

    vector<QString> params;
    m_extendNetMarkStack.clear();
    for (auto item : itemsLink)
    {
        auto text = item->text(column);
        if (item->type() == QTreeWidgetItem::UserType)
        {
            NetMark netMark;
            auto pos = text.lastIndexOf("_");
            netMark.sn = text.mid(pos + 1).toInt();
            netMark.net = text.mid(0, pos);
            netMark.name = item->toolTip(0);
            m_extendNetMarkStack.push_back(netMark);
        }
        else
        {
            auto pos = text.lastIndexOf(": ");
            m_field = text.mid(0, pos);
            m_value = text.mid(pos + 2);

            auto parent = item->parent();
            if (parent)
            {
                auto cnt = parent->childCount();
                for (auto i = 0; i < cnt; i++)
                {
                    auto child = parent->child(i);
                    if (child == nullptr || child->type() == QTreeWidgetItem::UserType)
                        continue;

                    text = child->text(column);
                    params.push_back(text);
                }
            }
        }
    }

    emit editTreeParamSignal(m_extendNetMarkStack.back().sn, m_extendNetMarkStack.back().net, m_extendNetMarkStack.back().name, m_field, m_value, params);
}

void CAttrViewMgr::treeParamsChangedSlot(int sn, QString const &netName, QString const &netId, NetAiParams const &aiParams)
{
    if (sn != m_extendNetMarkStack.back().sn || netName != m_extendNetMarkStack.back().name || netId != m_extendNetMarkStack.back().net)
        return;

    emit showInfoSignal("Net " + netName + " param changed.");

    m_item, m_field, m_value;

    QJsonObject expressParams;
    encodeParams(aiParams, expressParams);
    emit treeParamsChangedSignal(sn, netId, netName, expressParams, m_extendNetMarkStack);
}
