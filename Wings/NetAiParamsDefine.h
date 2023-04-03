#ifndef NETPARAMSDEFINE_H
#define NETPARAMSDEFINE_H

#include <string>

constexpr int   STRLEN  = 128;

enum NetType
{
    // 网络单元: netunit
    e_NetConvType,
    e_NetMaxPoolType,
    e_NetAvgPoolType,
    e_NetAdaptiveAvgPoolType,
    e_NetConvTransposeType,
    e_NetUpSampleType,
    e_NetLinearType,
    e_NetDropOutType,
    e_NetDropPathType,
    e_NetBatchNormType,
    e_NetGroupNormType,
    e_NetInstanceNormType,
    e_NetLayerNormType,
    e_NetActiveType,
    e_NetSoftMaxType,
    e_NetFlattenType,
    e_NetChannelShuffleType,
    e_NetDetectLayer2DType,
    e_NetCatType,

    // 计算单元: calunit
    e_NetScaledType,
    e_NetAddType,
    e_NetConstType,
    e_NetPermuteType,
    e_NetViewType,
    e_NetSplitType,
    e_NetIdentityType,

    // 运行单元: rununit
    e_NetShowResultType,
    e_NetBaseLossType,
    e_NetDiceLossType,
    e_NetDetectYoloLossType,
    e_NetOptAdaDeltaType,
    e_NetOptAdaGradType,
    e_NetOptAdamType,
    e_NetOptASGDType,
    e_NetOptRMSpropType,
    e_NetOptSGDType,
    e_NetTrain2DType,
    e_NetDataLoader2DType,
    e_NetDataInputTestType,
    e_NetDataOutputType,

    // 定制网元: custom
    e_NetCustomType,

};


// 网络单元: netunit

struct NetConvParams
{
    int             dim                     = {2};
    int             inChannels              = {64};
    int             outChannels             = {64};
    int             group                   = {1};
    int             kernelSize[3]           = {3, 3, 3};
    int             stride[3]               = {1, 1, 1};
    int             dilation[3]             = {1, 1, 1};
    int             padding[3]              = {0, 0, 0};
    char            paddingMode[STRLEN + 1] = {"zeros"};
    int             requiresGrad            = {1};
};

struct NetMaxPoolParams
{
    int             dim                     = {2};
    int             kernelSize[3]           = {3, 3, 3};
    int             stride[3]               = {1, 1, 1};
    int             dilation[3]             = {1, 1, 1};
    int             padding[3]              = {0, 0, 0};
    int             requiresGrad            = {1};
};

struct NetAvgPoolParams
{
    int             dim                     = {2};
    int             kernelSize[3]           = {3, 3, 3};
    int             stride[3]               = {1, 1, 1};
    int             padding[3]              = {0, 0, 0};
    int             requiresGrad            = {1};
};

struct NetAdaptiveAvgPoolParams
{
    int             dim                     = {2};
    int             outputSize[3]           = {32, 32, 32};
};

struct NetConvTransposeParams
{
    int             dim                     = {2};
    int             inChannels              = {64};
    int             outChannels             = {64};
    int             group                   = {1};
    int             kernelSize[3]           = {3, 3, 3};
    int             stride[3]               = {1, 1, 1};
    int             dilation[3]             = {1, 1, 1};
    int             padding[3]              = {0, 0, 0};
    int             outputPadding[3]        = {0, 0, 0};
    int             requiresGrad            = {1};
};

struct NetUpSampleParams
{
    int             dim                     = {2};
    double          scaleFactor[3]          = {2.0, 2.0, 2.0};
    char            mode[STRLEN + 1]        = "nearest";
};

struct NetLinearParams
{
    int             inFeatures              = {64};
    int             outFeatures             = {64};
    bool            bias                    = {true};
    int             requiresGrad            = {1};
};

struct NetDropOutParams
{
    int             dim                     = {2};
    double          prob                    = {0.5};
};

struct NetDropPathParams
{
    double          prob                    = {0.5};
};

struct NetBatchNormParams
{
    int             dim                     = {2};
    int             numFeatures             = {64};
};

struct NetGroupNormParams
{
    int             numGroups               = {2};
    int             numChannels             = {64};
};

struct NetInstanceNormParams
{
    int             dim                     = {2};
    int             numFeatures             = {64};
};

struct NetLayerNormParams
{
    int             normalizedShape[4]      = {0, 1, 3, 2};
};

struct NetActiveParams
{
    char            function[STRLEN + 1]    = {"SiLU"};
};

struct NetSoftMaxParams
{
    int             softMaxDim              = {2};
};

struct NetFlattenParams
{
    int             startDim                = {1};
    int             endDim                  = {-1};
};

struct NetChannelShuffleParams
{
    int             groups                  = {2};
};

struct NetDetectLayer2DParams
{
    int             numberClass             = {2};
    int             anchorSize[3][2]        = {{210, 210}, {300, 400}, {300, 400}};
    int             imageSize               = {416};
};

struct NetCatParams
{
    int             catDim                  = {0};
};


// 计算单元: calunit

struct NetScaledParams
{

};

struct NetAddParams
{

};

struct NetConstParams
{
    double          input                   = {0.0};
};

struct NetPermuteParams
{
    int             swapDims[4]             = {0, 1, 3, 2};
};

struct NetViewParams
{
    int             changeDims              = {0};
};

struct NetSplitParams
{
    int             splitSize               = {0};
    int             dim                     = {0};
};

struct NetIdentityParams
{

};


// 运行单元: rununit

struct NetShowResultParams
{

};

struct NetBaseLossParams
{
    char            function[STRLEN + 1]    = {"CrossEntropyLoss"};
    double          p = {1.0};
};

struct NetDiceLossParams
{

};

struct NetDetectYoloLossParams
{

};

struct NetOptAdaDeltaParams
{
    double          lr                      = {1.0};
    double          rho                     = {0.9};
    double          weightDecay             = {0.0};
    double          eps                     = {1.00E-06};
};

struct NetOptAdaGradParams
{
    double          lr                      = {0.01};
    double          lrDecay                 = {0.0};
    double          weightDecay             = {0.0};
    double          initialAccumulatorValue = {0.0};
    double          eps                     = {1.00E-06};
};

struct NetOptAdamParams
{
    double          lr                      = {0.01};
    double          eps                     = {1.00E-06};
    double          weightDecay             = {0.0};
};

struct NetOptASGDParams
{

};

struct NetOptRMSpropParams
{
    double          lr                      = {0.01};
    double          alpha                   = {9.90E-01};
    double          eps                     = {1.00E-06};
    double          weightDecay             = {0.0};
    double          momentum                = {0.0};
};

struct NetOptSGDParams
{
    double          lr                      = {0.01};
    double          momentum                = {0.0};
    double          dampening               = {0.0};
    double          weightDecay             = {0.0};
};

struct NetTrain2DParams
{
    char            savePthPath[STRLEN + 1]         = {"."};
    char            preweightsPath[STRLEN + 1]      = {"."};
    char            logDir[STRLEN + 1]              = {""};
    int             epochNum                        = {100};
    int             batchSize                       = {16};
    bool            isCuda                          = {false};
    int             classNums[5]                    = {1, 1, 1, 1, 1};
    char            criteriaFunction[STRLEN + 1]    = {"Dice"};
    bool            isTrainDataCriteria             = {true};
    bool            showLossFigure                  = {true};
    bool            showCriteriaFigure              = {true};
    int             epochSaveModel                  = {1};
};

struct NetDataLoader2DParams
{
    int             inputW                              = {256};
    int             inputH                              = {256};
    char            dataJsonPath[STRLEN + 1]            = {""};
    char            dataLabelCodeJsonPath[STRLEN + 1]   = {""};
    bool            isRgb                               = {true};
    int             batchSize                           = {16};
    int             numWorkers                          = {1};
    bool            shuffle                             = {true};
};

struct NetDataInputTestParams
{
    int             inputDim                = {0};
};

struct NetDataOutputParams
{

};


// 定制网元: custom

struct NetCustomParams
{
    int             cycle                   = {1};
    int             requiresGrad            = {1};
};

struct NetAiParams
{
    NetType                 type;

    union AiParams
    {
        // 网络单元: netunit
        NetConvParams               convParams;
        NetMaxPoolParams            maxPoolParams;
        NetAvgPoolParams            avgPoolParams;
        NetAdaptiveAvgPoolParams    adaptiveAvgPoolParams;
        NetConvTransposeParams      convTransposeParams;
        NetUpSampleParams           upSampleParams;
        NetLinearParams             linearParams;
        NetDropOutParams            dropOutParams;
        NetDropPathParams           dropPathParams;
        NetBatchNormParams          batchNormParams;
        NetGroupNormParams          groupNormParams;
        NetInstanceNormParams       instanceNormParams;
        NetLayerNormParams          layerNormParams;
        NetActiveParams             activeParams;
        NetSoftMaxParams            softMaxParams;
        NetFlattenParams            flattenParams;
        NetChannelShuffleParams     channelShuffleParams;
        NetDetectLayer2DParams      detectLayer2DParams;
        NetCatParams                catParams;

        // 计算单元: calunit
        NetScaledParams             scaledParams;
        NetAddParams                addParams;
        NetConstParams              constParams;
        NetPermuteParams            permuteParams;
        NetViewParams               viewParams;
        NetSplitParams              splitParams;
        NetIdentityParams           identityParams;

        // 运行单元: rununit
        NetShowResultParams         showResultParams;
        NetBaseLossParams           baseLossParams;
        NetDiceLossParams           diceLossParams;
        NetDetectYoloLossParams     detectYoloLossParams;
        NetOptAdaDeltaParams        optAdaDeltaParams;
        NetOptAdaGradParams         optAdaGradParams;
        NetOptAdamParams            optAdamParams;
        NetOptASGDParams            optASGDParams;
        NetOptRMSpropParams         optRMSpropParams;
        NetOptSGDParams             optSGDParams;
        NetTrain2DParams            train2DParams;
        NetDataLoader2DParams       dataLoader2DParams;
        NetDataInputTestParams      dataInputTestParams;
        NetDataOutputParams         dataOutputParams;

        // 定制网元: custom
        NetCustomParams             customParams;

        AiParams(){ memset(this, 0 , sizeof (AiParams)); }
        ~AiParams(){ memset(this, 0 , sizeof (AiParams)); }
    } ai;
};

#endif // NETPARAMSDEFINE_H
