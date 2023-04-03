#include "WgAttrList.h"
#include "ui_WgAttrList.h"
#include "WgIntEditor.h"
#include "WgIntListEditor.h"
#include "WgIntTableEditor.h"
#include "WgFloatEditor.h"
#include "WgFloatListEditor.h"
#include "WgStrSelector.h"
#include "WgTextEditor.h"

constexpr int       ROW_HEIGHT  = 50 + 2;

constexpr int       ISTEP       = 1;
constexpr int       IPREC       = 1;
constexpr double    DSTEP       = 1.0 / 10;

WgAttrList::WgAttrList(QWidget *parent)
    :
      QWidget(parent),
      ui(new Ui::WgAttrList)
{
    ui->setupUi(this);
    resizeHeight();
}

WgAttrList::~WgAttrList()
{
    delete ui;
}

void WgAttrList::Init(int sn, QString const &netId, QString const &netName, NetAiParams const &aiParams)
{
    m_sn = sn;
    m_Net = netId;
    m_Name = netName;
    m_aiParams = aiParams;

    ui->tbtnAttr->setText(m_Name + "(" + m_Net + ")");

    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        extend(m_aiParams.ai.convParams);
        break;

    case e_NetMaxPoolType:
        extend(m_aiParams.ai.maxPoolParams);
        break;

    case e_NetAvgPoolType:
        extend(m_aiParams.ai.avgPoolParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        extend(m_aiParams.ai.adaptiveAvgPoolParams);
        break;

    case e_NetConvTransposeType:
        extend(m_aiParams.ai.convTransposeParams);
        break;

    case e_NetUpSampleType:
        extend(m_aiParams.ai.upSampleParams);
        break;

    case e_NetLinearType:
        extend(m_aiParams.ai.linearParams);
        break;

    case e_NetDropOutType:
        extend(m_aiParams.ai.dropOutParams);
        break;

    case e_NetDropPathType:
        extend(m_aiParams.ai.dropPathParams);
        break;

    case e_NetBatchNormType:
        extend(m_aiParams.ai.batchNormParams);
        break;

    case e_NetGroupNormType:
        extend(m_aiParams.ai.groupNormParams);
        break;

    case e_NetInstanceNormType:
        extend(m_aiParams.ai.instanceNormParams);
        break;

    case e_NetLayerNormType:
        extend(m_aiParams.ai.layerNormParams);
        break;

    case e_NetActiveType:
        extend(m_aiParams.ai.activeParams);
        break;

    case e_NetSoftMaxType:
        extend(m_aiParams.ai.softMaxParams);
        break;

    case e_NetFlattenType:
        extend(m_aiParams.ai.flattenParams);
        break;

    case e_NetChannelShuffleType:
        extend(m_aiParams.ai.channelShuffleParams);
        break;

    case e_NetDetectLayer2DType:
        extend(m_aiParams.ai.detectLayer2DParams);
        break;

    case e_NetCatType:
        extend(m_aiParams.ai.catParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        extend(m_aiParams.ai.scaledParams);
        break;

    case e_NetAddType:
        extend(m_aiParams.ai.addParams);
        break;

    case e_NetConstType:
        extend(m_aiParams.ai.constParams);
        break;

    case e_NetPermuteType:
        extend(m_aiParams.ai.permuteParams);
        break;

    case e_NetViewType:
        extend(m_aiParams.ai.viewParams);
        break;

    case e_NetSplitType:
        extend(m_aiParams.ai.splitParams);
        break;

    case e_NetIdentityType:
        extend(m_aiParams.ai.identityParams);
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        extend(m_aiParams.ai.showResultParams);
        break;

    case e_NetBaseLossType:
        extend(m_aiParams.ai.baseLossParams);
        break;

    case e_NetDiceLossType:
        extend(m_aiParams.ai.diceLossParams);
        break;

    case e_NetDetectYoloLossType:
        extend(m_aiParams.ai.detectYoloLossParams);
        break;

    case e_NetOptAdaDeltaType:
        extend(m_aiParams.ai.optAdaDeltaParams);
        break;

    case e_NetOptAdaGradType:
        extend(m_aiParams.ai.optAdaGradParams);
        break;

    case e_NetOptAdamType:
        extend(m_aiParams.ai.optAdamParams);
        break;

    case e_NetOptASGDType:
        extend(m_aiParams.ai.optASGDParams);
        break;

    case e_NetOptRMSpropType:
        extend(m_aiParams.ai.optRMSpropParams);
        break;

    case e_NetOptSGDType:
        extend(m_aiParams.ai.optSGDParams);
        break;

    case e_NetTrain2DType:
        extend(m_aiParams.ai.train2DParams);
        break;

    case e_NetDataLoader2DType:
        extend(m_aiParams.ai.dataLoader2DParams);
        break;

    case e_NetDataInputTestType:
        extend(m_aiParams.ai.dataInputTestParams);
        break;

    case e_NetDataOutputType:
        extend(m_aiParams.ai.dataOutputParams);
        break;


    // 定制网元: custom
    case e_NetCustomType:
        extend(m_aiParams.ai.customParams);
        break;
    }
}

int WgAttrList::GetSn() const
{
    return m_sn;
}

void WgAttrList::resizeHeight()
{
    ui->listWidget->setFixedHeight(ROW_HEIGHT * ui->listWidget->count());
    setFixedHeight(ui->tbtnAttr->height() + ui->listWidget->height());
    update();
}

void WgAttrList::addTextEditor(QString const &name, QString const &text)
{
    auto textEditor = new WgTextEditor(this);
    textEditor->Init(name, text);
    textEditor->setFixedHeight(ROW_HEIGHT - 2);
    textEditor->show();

    connect(textEditor, SIGNAL(signalTextChanged(QString const&, QString const&)), this, SLOT(TextChangedSlot(QString const&, QString const&)));

    m_nameWgMap[name] = textEditor;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, textEditor);

    resizeHeight();
}

void WgAttrList::addStrSelector(QString const &name, QStringList const &rows, QString const &selected)
{
    auto strSelector = new WgStrSelector(this);
    strSelector->Init(name, rows, selected);
    strSelector->setFixedHeight(ROW_HEIGHT - 2);
    strSelector->show();

    connect(strSelector, SIGNAL(signalSelectedChanged(QString const&, QString const&)), this, SLOT(selectedChangeSlot(QString const&, QString const&)));

    m_nameWgMap[name] = strSelector;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, strSelector);

    resizeHeight();
}

void WgAttrList::addIntEditor(QString const &name, int min, int max, int val, int step)
{
    auto intEditor = new WgIntEditor(this);
    intEditor->Init(name, min, max, val, step);
    intEditor->setFixedHeight(ROW_HEIGHT - 2);
    intEditor->show();

    connect(intEditor, SIGNAL(signalValueChanged(QString const&, int)), this, SLOT(valueChangeSlot(QString const&, int)));

    m_nameWgMap[name] = intEditor;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, intEditor);

    resizeHeight();
}

void WgAttrList::addIntListEditor(QString const &name, int cols, int index, int min, int max, int val, int step)
{
    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    auto intListEditor = new WgIntListEditor(this);
    intListEditor->Init(name, strCols, index, min, max, val, step);
    intListEditor->setFixedHeight(ROW_HEIGHT - 2);
    intListEditor->show();

    connect(intListEditor, SIGNAL(signalIndexChanged(QString const&, int, int&)), this, SLOT(indexChangeSlot(QString const&, int, int&)));
    connect(intListEditor, SIGNAL(signalValueChanged(QString const&, int, int)), this, SLOT(valueChangeSlot(QString const&, int, int)));

    m_nameWgMap[name] = intListEditor;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, intListEditor);

    resizeHeight();
}

void WgAttrList::addIntTableEditor(QString const &name, int rows, int rIndex, int cols, int cIndex, int min, int max, int val, int step)
{
    QStringList strRows;
    for (auto i = 0; i < rows; i++)
        strRows << QString::number(i);

    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    auto intTableEditor = new WgIntTableEditor(this);
    intTableEditor->Init(name, strRows, rIndex, strCols, cIndex, min, max, val, step);
    intTableEditor->setFixedHeight(ROW_HEIGHT - 2);
    intTableEditor->show();

    m_nameWgMap[name] = intTableEditor;

    connect(intTableEditor, SIGNAL(signalRowChanged(QString const&, int, int&, int&)), this, SLOT(rowChangeSlot(QString const&, int, int&, int&)));
    connect(intTableEditor, SIGNAL(signalColChanged(QString const&, int, int, int&)), this, SLOT(colChangeSlot(QString const&, int, int, int&)));
    connect(intTableEditor, SIGNAL(signalValueChanged(QString const&, int, int, int)), this, SLOT(valueChangeSlot(QString const&, int, int, int)));

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, intTableEditor);

    resizeHeight();
}

void WgAttrList::addFloatEditor(QString const &name, double min, double max, double val, double step, int prec)
{
    auto floatEditor = new WgFloatEditor(this);
    floatEditor->Init(name, min, max, val, step, prec);
    floatEditor->setFixedHeight(ROW_HEIGHT - 2);
    floatEditor->show();

    connect(floatEditor, SIGNAL(signalValueChanged(QString const&, double)), this, SLOT(valueChangeSlot(QString const&, double)));

    m_nameWgMap[name] = floatEditor;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, floatEditor);

    resizeHeight();
}

void WgAttrList::addFloatListEditor(QString const &name, int cols, int index, double min, double max, double val, double step, int prec)
{
    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    auto floatListEditor = new WgFloatListEditor(this);
    floatListEditor->Init(name, strCols, index, min, max, val, step, prec);
    floatListEditor->setFixedHeight(ROW_HEIGHT - 2);
    floatListEditor->show();

    connect(floatListEditor, SIGNAL(signalIndexChanged(QString const&, int, double&)), this, SLOT(indexChangeSlot(QString const&, int, double&)));
    connect(floatListEditor, SIGNAL(signalValueChanged(QString const&, int, double)), this, SLOT(valueChangeSlot(QString const&, int, double)));

    m_nameWgMap[name] = floatListEditor;

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, floatListEditor);

    resizeHeight();
}

void WgAttrList::intEditorSetMax(QString const &name, int max)
{
    auto intEditor = dynamic_cast<WgIntEditor*>(m_nameWgMap[name]);
    if (intEditor)
    {
        intEditor->SetMax(max);
    }
}

void WgAttrList::intListEditorReset(QString const &name, int cols)
{
    auto intListEditor = dynamic_cast<WgIntListEditor*>(m_nameWgMap[name]);
    if (intListEditor)
    {
        QStringList strCols;
        for (auto i = 0; i < cols; i++)
            strCols << QString::number(i);

        intListEditor->Reset(strCols);
    }
}

void WgAttrList::floatListEditorReset(QString const &name, int cols)
{
    auto floatListEditor = dynamic_cast<WgFloatListEditor*>(m_nameWgMap[name]);
    if (floatListEditor)
    {
        QStringList strCols;
        for (auto i = 0; i < cols; i++)
            strCols << QString::number(i);

        floatListEditor->Reset(strCols);
    }
}

// 网络单元: netunit
void WgAttrList::extend(NetConvParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);
    addIntEditor("inChannels", 1, 9999, aiParams.inChannels, ISTEP);
    addIntEditor("outChannels", 1, 9999, aiParams.outChannels, ISTEP);

    addIntListEditor("kernelSize", aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);///
    addIntListEditor("stride", aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);///
    addIntListEditor("dilation", aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);///
    addIntListEditor("padding", aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);///

    QStringList strRows;
    strRows << "zeros" << "reflect" << "replicate" << "circular";
    addStrSelector("paddingMode", strRows, aiParams.paddingMode);

    addIntEditor("group", 1, aiParams.inChannels, aiParams.group, ISTEP);///
    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleSelectedChanged(NetConvParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "paddingMode")
    {
        auto paddingMode = selected.toStdString();
        auto sz = (size_t(STRLEN) < paddingMode.size() ? size_t(STRLEN) : paddingMode.size());
        memcpy(aiParams.paddingMode, paddingMode.data(), sz);
        aiParams.paddingMode[sz] = '\0';
    }
}

void WgAttrList::handleValueChanged(NetConvParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        intListEditorReset("kernelSize", aiParams.dim);
        intListEditorReset("stride", aiParams.dim);
        intListEditorReset("dilation", aiParams.dim);
        intListEditorReset("padding", aiParams.dim);
    }
    else if (name == "inChannels")
    {
        aiParams.inChannels = val;

        intEditorSetMax("group", aiParams.inChannels);
    }
    else if (name == "outChannels")
    {
        aiParams.outChannels = val;
    }
    else if (name == "group")
    {
        aiParams.group = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void WgAttrList::handleIndexChanged(NetConvParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "kernelSize")
    {
        val = aiParams.kernelSize[index];
    }
    else if (name == "stride")
    {
        val = aiParams.stride[index];
    }
    else if (name == "dilation")
    {
        val = aiParams.dilation[index];
    }
    else if (name == "padding")
    {
        val = aiParams.padding[index];
    }
}

void WgAttrList::handleValueChanged(NetConvParams &aiParams, QString const &name, int index, int val)
{
    if (name == "kernelSize")
    {
        aiParams.kernelSize[index] = val;
    }
    else if (name == "stride")
    {
        aiParams.stride[index] = val;
    }
    else if (name == "dilation")
    {
        aiParams.dilation[index] = val;
    }
    else if (name == "padding")
    {
        aiParams.padding[index] = val;
    }
}


void WgAttrList::extend(NetMaxPoolParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);

    addIntListEditor("kernelSize", aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);///
    addIntListEditor("stride", aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);///
    addIntListEditor("dilation", aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);///
    addIntListEditor("padding", aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);///

    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        intListEditorReset("kernelSize", aiParams.dim);
        intListEditorReset("stride", aiParams.dim);
        intListEditorReset("dilation", aiParams.dim);
        intListEditorReset("padding", aiParams.dim);
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void WgAttrList::handleIndexChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "kernelSize")
    {
        val = aiParams.kernelSize[index];
    }
    else if (name == "stride")
    {
        val = aiParams.stride[index];
    }
    else if (name == "dilation")
    {
        val = aiParams.dilation[index];
    }
    else if (name == "padding")
    {
        val = aiParams.padding[index];
    }
}

void WgAttrList::handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int val)
{
    if (name == "kernelSize")
    {
        aiParams.kernelSize[index] = val;
    }
    else if (name == "stride")
    {
        aiParams.stride[index] = val;
    }
    else if (name == "dilation")
    {
        aiParams.dilation[index] = val;
    }
    else if (name == "padding")
    {
        aiParams.padding[index] = val;
    }
}


void WgAttrList::extend(NetAvgPoolParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);

    addIntListEditor("kernelSize", aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);///
    addIntListEditor("stride", aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);///
    addIntListEditor("padding", aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);///

    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        intListEditorReset("kernelSize", aiParams.dim);
        intListEditorReset("stride", aiParams.dim);
        intListEditorReset("padding", aiParams.dim);
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void WgAttrList::handleIndexChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "kernelSize")
    {
        val = aiParams.kernelSize[index];
    }
    else if (name == "stride")
    {
        val = aiParams.stride[index];
    }
    else if (name == "padding")
    {
        val = aiParams.padding[index];
    }
}

void WgAttrList::handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int val)
{
    if (name == "kernelSize")
    {
        aiParams.kernelSize[index] = val;
    }
    else if (name == "stride")
    {
        aiParams.stride[index] = val;
    }
    else if (name == "padding")
    {
        aiParams.padding[index] = val;
    }
}


void WgAttrList::extend(NetAdaptiveAvgPoolParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);

    addIntListEditor("outputSize", aiParams.dim, 0, 1, 9999, aiParams.outputSize[0], ISTEP);///
}

void WgAttrList::handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        intListEditorReset("outputSize", aiParams.dim);
    }
}

void WgAttrList::handleIndexChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "outputSize")
    {
        val = aiParams.outputSize[index];
    }
}

void WgAttrList::handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int val)
{
    if (name == "outputSize")
    {
        aiParams.outputSize[index] = val;
    }
}


void WgAttrList::extend(NetConvTransposeParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);
    addIntEditor("inChannels", 1, 9999, aiParams.inChannels, ISTEP);
    addIntEditor("outChannels", 1, 9999, aiParams.outChannels, ISTEP);

    addIntListEditor("kernelSize", aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);///
    addIntListEditor("stride", aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);///
    addIntListEditor("dilation", aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);///
    addIntListEditor("padding", aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);///
    addIntListEditor("outputPadding", aiParams.dim, 0, 0, 99, aiParams.outputPadding[0], ISTEP);///

    addIntEditor("group", 1, aiParams.inChannels, aiParams.group, ISTEP);///
    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        intListEditorReset("kernelSize", aiParams.dim);
        intListEditorReset("stride", aiParams.dim);
        intListEditorReset("dilation", aiParams.dim);
        intListEditorReset("padding", aiParams.dim);
        intListEditorReset("outputPadding", aiParams.dim);
    }
    else if (name == "inChannels")
    {
        aiParams.inChannels = val;

        intEditorSetMax("group", aiParams.inChannels);
    }
    else if (name == "outChannels")
    {
        aiParams.outChannels = val;
    }
    else if (name == "group")
    {
        aiParams.group = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void WgAttrList::handleIndexChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "kernelSize")
    {
        val = aiParams.kernelSize[index];
    }
    else if (name == "stride")
    {
        val = aiParams.stride[index];
    }
    else if (name == "dilation")
    {
        val = aiParams.dilation[index];
    }
    else if (name == "padding")
    {
        val = aiParams.padding[index];
    }
    else if (name == "outputPadding")
    {
        val = aiParams.outputPadding[index];
    }
}

void WgAttrList::handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int val)
{
    if (name == "kernelSize")
    {
        aiParams.kernelSize[index] = val;
    }
    else if (name == "stride")
    {
        aiParams.stride[index] = val;
    }
    else if (name == "dilation")
    {
        aiParams.dilation[index] = val;
    }
    else if (name == "padding")
    {
        aiParams.padding[index] = val;
    }
    else if (name == "outputPadding")
    {
        aiParams.outputPadding[index] = val;
    }
}


void WgAttrList::extend(NetUpSampleParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);

    addFloatListEditor("scaleFactor", aiParams.dim, 0, 1.0, 10.0, aiParams.scaleFactor[0], DSTEP, IPREC);///

    QStringList strRows;
    strRows << "nearest" << "linear" << "bilinear" << "bicubic";
    addStrSelector("mode", strRows, aiParams.mode);
}

void WgAttrList::handleSelectedChanged(NetUpSampleParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "mode")
    {
        auto mode = selected.toStdString();
        auto sz = (size_t(STRLEN) < mode.size() ? size_t(STRLEN) : mode.size());
        memcpy(aiParams.mode, mode.data(), sz);
        aiParams.mode[sz] = '\0';
    }
}

void WgAttrList::handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;

        floatListEditorReset("scaleFactor", aiParams.dim);
    }
}

void WgAttrList::handleIndexChanged(NetUpSampleParams &aiParams, QString const &name, int index, double &val)
{
    if (name == "scaleFactor")
    {
        val = aiParams.scaleFactor[index];
    }
}

void WgAttrList::handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int index, double val)
{
    if (name == "scaleFactor")
    {
        aiParams.scaleFactor[index] = val;
    }
}


void WgAttrList::extend(NetLinearParams &aiParams)
{
    addIntEditor("inFeatures", 1, 9999, aiParams.inFeatures, ISTEP);
    addIntEditor("outFeatures", 1, 9999, aiParams.outFeatures, ISTEP);

    QStringList strRows;
    strRows << "true" << "false";
    addStrSelector("bias", strRows, (aiParams.bias ? "true" : "false"));

    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleSelectedChanged(NetLinearParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "bias")
    {
        auto bias = selected.toStdString();
        aiParams.bias = (bias == "true" ? true : false);
    }
}

void WgAttrList::handleValueChanged(NetLinearParams &aiParams, QString const &name, int val)
{
    if (name == "inFeatures")
    {
        aiParams.inFeatures = val;
    }
    else if (name == "outFeatures")
    {
        aiParams.outFeatures = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}


void WgAttrList::extend(NetDropOutParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);
    addFloatEditor("prob", 0.1, 0.9, aiParams.prob, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetDropOutParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
}

void WgAttrList::handleValueChanged(NetDropOutParams &aiParams, QString const &name, double val)
{
    if (name == "prob")
    {
        aiParams.prob = val;
    }
}


void WgAttrList::extend(NetDropPathParams &aiParams)
{
    addFloatEditor("prob", 0.1, 0.9, aiParams.prob, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetDropPathParams &aiParams, QString const &name, double val)
{
    if (name == "prob")
    {
        aiParams.prob = val;
    }
}


void WgAttrList::extend(NetBatchNormParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);
    addIntEditor("numFeatures", 1, 9999, aiParams.numFeatures, ISTEP);
}

void WgAttrList::handleValueChanged(NetBatchNormParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "numFeatures")
    {
        aiParams.numFeatures = val;
    }
}


void WgAttrList::extend(NetGroupNormParams &aiParams)
{
    addIntEditor("numGroups", 1, 9999, aiParams.numGroups, ISTEP);
    addIntEditor("numChannels", 1, 9999, aiParams.numChannels, ISTEP);
}

void WgAttrList::handleValueChanged(NetGroupNormParams &aiParams, QString const &name, int val)
{
    if (name == "numGroups")
    {
        aiParams.numGroups = val;
    }
    else if (name == "numChannels")
    {
        aiParams.numChannels = val;
    }
}


void WgAttrList::extend(NetInstanceNormParams &aiParams)
{
    addIntEditor("dim", 2, 3, aiParams.dim, ISTEP);
    addIntEditor("numFeatures", 1, 9999, aiParams.numFeatures, ISTEP);
}

void WgAttrList::handleValueChanged(NetInstanceNormParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "numFeatures")
    {
        aiParams.numFeatures = val;
    }
}


void WgAttrList::extend(NetLayerNormParams &aiParams)
{
    int cnt = sizeof(aiParams.normalizedShape) / sizeof(aiParams.normalizedShape[0]);
    addIntListEditor("normalizedShape", cnt, 0, 0, 9999, aiParams.normalizedShape[0], ISTEP);
}

void WgAttrList::handleIndexChanged(NetLayerNormParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "normalizedShape")
    {
        val = aiParams.normalizedShape[index];
    }
}

void WgAttrList::handleValueChanged(NetLayerNormParams &aiParams, QString const &name, int index, int val)
{
    if (name == "normalizedShape")
    {
        aiParams.normalizedShape[index] = val;
    }
}


void WgAttrList::extend(NetActiveParams &aiParams)
{
    QStringList strRows;
    strRows << "ELU" << "Hardshrink" << "Hardsigmoid" << "Hardtanh" << "Hardswish";
    strRows << "LeakyReLU" << "LogSigmoid" << "PReLU" << "ReLU" << "ReLU6";
    strRows << "RReLU" << "SELU" << "CELU" << "GELU" << "Sigmoid";
    strRows << "SiLU" << "Mish" << "Softplus" << "Softshrink" << "Softsign";
    strRows << "Tanh" << "Tanhshrink" << "GLU";
    addStrSelector("function", strRows, aiParams.function);
}

void WgAttrList::handleSelectedChanged(NetActiveParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "function")
    {
        auto function = selected.toStdString();
        auto sz = (size_t(STRLEN) < function.size() ? size_t(STRLEN) : function.size());
        memcpy(aiParams.function, function.data(), sz);
        aiParams.function[sz] = '\0';
    }
}


void WgAttrList::extend(NetSoftMaxParams &aiParams)
{
    addIntEditor("softMaxDim", 2, 3, aiParams.softMaxDim, ISTEP);
}

void WgAttrList::handleValueChanged(NetSoftMaxParams &aiParams, QString const &name, int val)
{
    if (name == "softMaxDim")
    {
        aiParams.softMaxDim = val;
    }
}


void WgAttrList::extend(NetFlattenParams &aiParams)
{
    addIntEditor("startDim", -10, 10, aiParams.startDim, ISTEP);
    addIntEditor("endDim", -10, 10, aiParams.endDim, ISTEP);
}

void WgAttrList::handleValueChanged(NetFlattenParams &aiParams, QString const &name, int val)
{
    if (name == "startDim")
    {
        aiParams.startDim = val;
    }
    else if (name == "endDim")
    {
        aiParams.endDim = val;
    }
}


void WgAttrList::extend(NetChannelShuffleParams &aiParams)
{
    addIntEditor("groups", 1, 9999, aiParams.groups, ISTEP);
}

void WgAttrList::handleValueChanged(NetChannelShuffleParams &aiParams, QString const &name, int val)
{
    if (name == "groups")
    {
        aiParams.groups = val;
    }
}


void WgAttrList::extend(NetDetectLayer2DParams &aiParams)
{
    addIntEditor("numberClass", 1, 9999, aiParams.numberClass, ISTEP);

    int rows = sizeof(aiParams.anchorSize) / sizeof(aiParams.anchorSize[0]);
    int cols = sizeof(aiParams.anchorSize[0]) / sizeof(aiParams.anchorSize[0][0]);
    addIntTableEditor("anchorSize", rows, 0, cols, 0, 1, 9999, aiParams.anchorSize[0][0], ISTEP);

    addIntEditor("imageSize", 64, 9999, aiParams.imageSize, ISTEP);
}

void WgAttrList::handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int val)
{
    if (name == "numberClass")
    {
        aiParams.numberClass = val;
    }
    else if (name == "imageSize")
    {
        aiParams.imageSize = val;
    }
}

void WgAttrList::handleRowChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int &cIndex, int &val)
{
    if (name == "anchorSize")
    {
        cIndex = 0;
        val = aiParams.anchorSize[rIndex][cIndex];
    }
}

void WgAttrList::handleColChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int &val)
{
    if (name == "anchorSize")
    {
        val = aiParams.anchorSize[rIndex][cIndex];
    }
}

void WgAttrList::handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int val)
{
    if (name == "anchorSize")
    {
        aiParams.anchorSize[rIndex][cIndex] = val;
    }
}


void WgAttrList::extend(NetCatParams &aiParams)
{
    addIntEditor("catDim", 0, 6, aiParams.catDim, ISTEP);
}

void WgAttrList::handleValueChanged(NetCatParams &aiParams, QString const &name, int val)
{
    if (name == "catDim")
    {
        aiParams.catDim = val;
    }
}


// 计算单元: calunit
void WgAttrList::extend(NetScaledParams &)
{
    return;
}


void WgAttrList::extend(NetAddParams &)
{
    return;
}


void WgAttrList::extend(NetConstParams &aiParams)
{
    addFloatEditor("input", 0.0, 1.0, aiParams.input, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetConstParams &aiParams, QString const &name, double val)
{
    if (name == "input")
    {
        aiParams.input = val;
    }
}


void WgAttrList::extend(NetPermuteParams &aiParams)
{
    int cnt = sizeof(aiParams.swapDims) / sizeof(aiParams.swapDims[0]);
    addIntListEditor("swapDims", cnt, 0, 0, 9999, aiParams.swapDims[0], ISTEP);
}

void WgAttrList::handleIndexChanged(NetPermuteParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "swapDims")
    {
        val = aiParams.swapDims[index];
    }
}

void WgAttrList::handleValueChanged(NetPermuteParams &aiParams, QString const &name, int index, int val)
{
    if (name == "swapDims")
    {
        aiParams.swapDims[index] = val;
    }
}


void WgAttrList::extend(NetViewParams &aiParams)
{
    addIntEditor("changeDims", 0, 6, aiParams.changeDims, ISTEP);
}

void WgAttrList::handleValueChanged(NetViewParams &aiParams, QString const &name, int val)
{
    if (name == "changeDims")
    {
        aiParams.changeDims = val;
    }
}


void WgAttrList::extend(NetSplitParams &aiParams)
{
    addIntEditor("splitSize", 2, 9999, aiParams.splitSize, ISTEP);
    addIntEditor("dim", 0, 3, aiParams.dim, ISTEP);
}

void WgAttrList::handleValueChanged(NetSplitParams &aiParams, QString const &name, int val)
{
    if (name == "splitSize")
    {
        aiParams.splitSize = val;
    }
    else if (name == "dim")
    {
        aiParams.dim = val;
    }
}


void WgAttrList::extend(NetIdentityParams &)
{

}



// 运行单元: rununit
void WgAttrList::extend(NetShowResultParams &)
{

}


void WgAttrList::extend(NetBaseLossParams &aiParams)
{
    QStringList strRows;
    strRows << "L1Loss" << "MSELoss" << "CrossEntropyLoss" << "NLLLoss";
    strRows << "CTCLoss" << "KLDivLoss" << "BCELoss" << "BCEWithLogitsLoss";
    addStrSelector("function", strRows, aiParams.function);

    addFloatEditor("p", 0.0, 1.0, aiParams.p, DSTEP, IPREC);
}

void WgAttrList::handleSelectedChanged(NetBaseLossParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "function")
    {
        auto function = selected.toStdString();
        auto sz = (size_t(STRLEN) < function.size() ? size_t(STRLEN) : function.size());
        memcpy(aiParams.function, function.data(), sz);
        aiParams.function[sz] = '\0';
    }
}

void WgAttrList::handleValueChanged(NetBaseLossParams &aiParams, QString const &name, double val)
{
    if (name == "p")
    {
        aiParams.p = val;
    }
}


void WgAttrList::extend(NetDiceLossParams &)
{

}


void WgAttrList::extend(NetDetectYoloLossParams &)
{

}


void WgAttrList::extend(NetOptAdaDeltaParams &aiParams)
{
    addFloatEditor("lr", 0.001, 10.0, aiParams.lr, 0.001, 3);
    addFloatEditor("rho", 0.1, 1.0, aiParams.rho, DSTEP, IPREC);
    addFloatEditor("weightDecay", 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    addFloatEditor("eps", 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
}

void WgAttrList::handleValueChanged(NetOptAdaDeltaParams &aiParams, QString const &name, double val)
{
    if (name == "lr")
    {
        aiParams.lr = val;
    }
    else if (name == "rho")
    {
        aiParams.rho = val;
    }
    else if (name == "weightDecay")
    {
        aiParams.weightDecay = val;
    }
    else if (name == "eps")
    {
        aiParams.eps = val;
    }
}


void WgAttrList::extend(NetOptAdaGradParams &aiParams)
{
    addFloatEditor("lr", 0.001, 10.0, aiParams.lr, 0.001, 3);
    addFloatEditor("lrDecay", 0.0, 1.0, aiParams.lrDecay, DSTEP, IPREC);
    addFloatEditor("weightDecay", 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    addFloatEditor("initAccumulatorValue", 0.0, 1.0, aiParams.initialAccumulatorValue, DSTEP, IPREC);
    addFloatEditor("eps", 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
}

void WgAttrList::handleValueChanged(NetOptAdaGradParams &aiParams, QString const &name, double val)
{
    if (name == "lr")
    {
        aiParams.lr = val;
    }
    else if (name == "lrDecay")
    {
        aiParams.lrDecay = val;
    }
    else if (name == "weightDecay")
    {
        aiParams.weightDecay = val;
    }
    else if (name == "initAccumulatorValue")
    {
        aiParams.initialAccumulatorValue = val;
    }
    else if (name == "eps")
    {
        aiParams.eps = val;
    }
}


void WgAttrList::extend(NetOptAdamParams &aiParams)
{
    addFloatEditor("lr", 0.001, 10.0, aiParams.lr, 0.001, 3);
    addFloatEditor("eps", 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    addFloatEditor("weightDecay", 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetOptAdamParams &aiParams, QString const &name, double val)
{
    if (name == "lr")
    {
        aiParams.lr = val;
    }
    else if (name == "eps")
    {
        aiParams.eps = val;
    }
    else if (name == "weightDecay")
    {
        aiParams.weightDecay = val;
    }
}


void WgAttrList::extend(NetOptASGDParams &)
{

}


void WgAttrList::extend(NetOptRMSpropParams &aiParams)
{
    addFloatEditor("lr", 0.001, 10.0, aiParams.lr, 0.001, 3);
    addFloatEditor("alpha", 0.001, 10.0, aiParams.alpha, 0.001, 3);
    addFloatEditor("eps", 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    addFloatEditor("weightDecay", 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    addFloatEditor("momentum", 0.0, 1.0, aiParams.momentum, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetOptRMSpropParams &aiParams, QString const &name, double val)
{
    if (name == "lr")
    {
        aiParams.lr = val;
    }
    else if (name == "alpha")
    {
        aiParams.alpha = val;
    }
    else if (name == "eps")
    {
        aiParams.eps = val;
    }
    else if (name == "weightDecay")
    {
        aiParams.weightDecay = val;
    }
    else if (name == "momentum")
    {
        aiParams.momentum = val;
    }
}


void WgAttrList::extend(NetOptSGDParams &aiParams)
{
    addFloatEditor("lr", 0.001, 10.0, aiParams.lr, 0.001, 3);
    addFloatEditor("momentum", 0.0, 1.0, aiParams.momentum, DSTEP, IPREC);
    addFloatEditor("dampening", 0.0, 1.0, aiParams.dampening, DSTEP, IPREC);
    addFloatEditor("weightDecay", 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
}

void WgAttrList::handleValueChanged(NetOptSGDParams &aiParams, QString const &name, double val)
{
    if (name == "lr")
    {
        aiParams.lr = val;
    }
    else if (name == "momentum")
    {
        aiParams.momentum = val;
    }
    else if (name == "dampening")
    {
        aiParams.dampening = val;
    }
    else if (name == "weightDecay")
    {
        aiParams.weightDecay = val;
    }
}


void WgAttrList::extend(NetTrain2DParams &aiParams)
{
    addTextEditor("savePthPath", aiParams.savePthPath);
    addTextEditor("preweightsPath", aiParams.preweightsPath);
    addTextEditor("logDir", aiParams.logDir);

    addIntEditor("epochNum", 20, 10000, aiParams.epochNum, ISTEP);
    addIntEditor("batchSize", 1, 128, aiParams.batchSize, ISTEP);

    QStringList strRows;
    strRows << "true" << "false";
    addStrSelector("isCuda", strRows, (aiParams.isCuda ? "true" : "false"));
    addStrSelector("isTrainDataCriteria", strRows, (aiParams.isTrainDataCriteria ? "true" : "false"));
    addStrSelector("showLossFigure", strRows, (aiParams.showLossFigure ? "true" : "false"));
    addStrSelector("showCriteriaFigure", strRows, (aiParams.showCriteriaFigure ? "true" : "false"));

    int cnt = sizeof(aiParams.classNums) / sizeof(aiParams.classNums[0]);
    addIntListEditor("classNums", cnt, 0, 1, 1000, aiParams.classNums[0], ISTEP);

    strRows.clear();
    strRows << "Dice" << "IOU" << "Accuracy" << "L1" << "L2" << "Jaccard";
    addStrSelector("criteriaFunction", strRows, aiParams.criteriaFunction);

    addIntEditor("epochSaveModel", 1, 50, aiParams.epochSaveModel, ISTEP);
}

void WgAttrList::handleTextChanged(NetTrain2DParams &aiParams, QString const &name, QString const &text)
{
    if (name == "savePthPath")
    {
        auto savePthPath = text.toStdString();
        auto sz = (size_t(STRLEN) < savePthPath.size() ? size_t(STRLEN) : savePthPath.size());
        memcpy(aiParams.savePthPath, savePthPath.data(), sz);
        aiParams.savePthPath[sz] = '\0';
    }
    else if (name == "preweightsPath")
    {
        auto preweightsPath = text.toStdString();
        auto sz = (size_t(STRLEN) < preweightsPath.size() ? size_t(STRLEN) : preweightsPath.size());
        memcpy(aiParams.preweightsPath, preweightsPath.data(), sz);
        aiParams.preweightsPath[sz] = '\0';
    }
    else if (name == "logDir")
    {
        auto logDir = text.toStdString();
        auto sz = (size_t(STRLEN) < logDir.size() ? size_t(STRLEN) : logDir.size());
        memcpy(aiParams.logDir, logDir.data(), sz);
        aiParams.logDir[sz] = '\0';
    }
}

void WgAttrList::handleSelectedChanged(NetTrain2DParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "isCuda")
    {
        auto isCuda = selected.toStdString();
        aiParams.isCuda = (isCuda == "true" ? true : false);
    }
    else if (name == "isTrainDataCriteria")
    {
        auto isTrainDataCriteria = selected.toStdString();
        aiParams.isTrainDataCriteria = (isTrainDataCriteria == "true" ? true : false);
    }
    else if (name == "showLossFigure")
    {
        auto showLossFigure = selected.toStdString();
        aiParams.showLossFigure = (showLossFigure == "true" ? true : false);
    }
    else if (name == "showCriteriaFigure")
    {
        auto showCriteriaFigure = selected.toStdString();
        aiParams.showCriteriaFigure = (showCriteriaFigure == "true" ? true : false);
    }
    else if (name == "criteriaFunction")
    {
        auto criteriaFunction = selected.toStdString();
        auto sz = (size_t(STRLEN) < criteriaFunction.size() ? size_t(STRLEN) : criteriaFunction.size());
        memcpy(aiParams.criteriaFunction, criteriaFunction.data(), sz);
        aiParams.criteriaFunction[sz] = '\0';
    }
}

void WgAttrList::handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int val)
{
    if (name == "epochNum")
    {
        aiParams.epochNum = val;
    }
    else if (name == "batchSize")
    {
        aiParams.batchSize = val;
    }
    else if (name == "epochSaveModel")
    {
        aiParams.epochSaveModel = val;
    }
}

void WgAttrList::handleIndexChanged(NetTrain2DParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "classNums")
    {
        val = aiParams.classNums[index];
    }
}

void WgAttrList::handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int index, int val)
{
    if (name == "classNums")
    {
        aiParams.classNums[index] = val;
    }
}


void WgAttrList::extend(NetDataLoader2DParams &aiParams)
{
    addIntEditor("inputW", 1, 1024, aiParams.inputW, ISTEP);
    addIntEditor("inputH", 1, 1024, aiParams.inputH, ISTEP);

    addTextEditor("dataJsonPath", aiParams.dataJsonPath);
    addTextEditor("dataLabelCodeJsonPath", aiParams.dataLabelCodeJsonPath);

    QStringList strRows;
    strRows << "true" << "false";
    addStrSelector("isRgb", strRows, (aiParams.isRgb ? "true" : "false"));

    addIntEditor("batchSize", 1, 128, aiParams.batchSize, ISTEP);
    addIntEditor("numWorkers", 1, 16, aiParams.numWorkers, ISTEP);

    addStrSelector("shuffle", strRows, (aiParams.shuffle ? "true" : "false"));
}

void WgAttrList::handleTextChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &text)
{
    if (name == "dataJsonPath")
    {
        auto dataJsonPath = text.toStdString();
        auto sz = (size_t(STRLEN) < dataJsonPath.size() ? size_t(STRLEN) : dataJsonPath.size());
        memcpy(aiParams.dataJsonPath, dataJsonPath.data(), sz);
        aiParams.dataJsonPath[sz] = '\0';
    }
    else if (name == "dataLabelCodeJsonPath")
    {
        auto dataLabelCodeJsonPath = text.toStdString();
        auto sz = (size_t(STRLEN) < dataLabelCodeJsonPath.size() ? size_t(STRLEN) : dataLabelCodeJsonPath.size());
        memcpy(aiParams.dataLabelCodeJsonPath, dataLabelCodeJsonPath.data(), sz);
        aiParams.dataLabelCodeJsonPath[sz] = '\0';
    }
}

void WgAttrList::handleSelectedChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "isRgb")
    {
        auto isRgb = selected.toStdString();
        aiParams.isRgb = (isRgb == "true" ? true : false);
    }
    else if (name == "shuffle")
    {
        auto shuffle = selected.toStdString();
        aiParams.shuffle = (shuffle == "true" ? true : false);
    }
}

void WgAttrList::handleValueChanged(NetDataLoader2DParams &aiParams, QString const &name, int val)
{
    if (name == "inputW")
    {
        aiParams.inputW = val;
    }
    else if (name == "inputH")
    {
        aiParams.inputH = val;
    }
    else if (name == "batchSize")
    {
        aiParams.batchSize = val;
    }
    else if (name == "numWorkers")
    {
        aiParams.numWorkers = val;
    }
}


void WgAttrList::extend(NetDataInputTestParams &aiParams)
{
    addIntEditor("inputDim", 0, 6, aiParams.inputDim, ISTEP);
}

void WgAttrList::handleValueChanged(NetDataInputTestParams &aiParams, QString const &name, int val)
{
    if (name == "inputDim")
    {
        aiParams.inputDim = val;
    }
}


void WgAttrList::extend(NetDataOutputParams &)
{

}


// 定制网元: custom
void WgAttrList::extend(NetCustomParams &aiParams)
{
    addIntEditor("cycle", 1, 9999, aiParams.cycle, ISTEP);
    addIntEditor("requiresGrad", -1, 1, aiParams.requiresGrad, ISTEP);
}

void WgAttrList::handleValueChanged(NetCustomParams &aiParams, QString const &name, int val)
{
    if (name == "cycle")
    {
        aiParams.cycle = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void WgAttrList::on_tbtnAttr_clicked(bool checked)
{
    if (checked)
    {
        ui->tbtnAttr->setArrowType(Qt::DownArrow);
        ui->listWidget->show();
        setFixedHeight(ui->tbtnAttr->height() + ui->listWidget->height());
    }
    else
    {
        ui->tbtnAttr->setArrowType(Qt::RightArrow);
        ui->listWidget->hide();
        setFixedHeight(ui->tbtnAttr->height());
    }

    emit signalUpdate(m_sn, m_Name, m_Net);
}

void WgAttrList::TextChangedSlot(QString const &name, QString const &text)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        handleTextChanged(m_aiParams.ai.train2DParams, name, text);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleTextChanged(m_aiParams.ai.dataLoader2DParams, name, text);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::selectedChangeSlot(QString const &name, QString const &selected)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleSelectedChanged(m_aiParams.ai.convParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        handleSelectedChanged(m_aiParams.ai.upSampleParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetLinearType:
        handleSelectedChanged(m_aiParams.ai.linearParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        handleSelectedChanged(m_aiParams.ai.activeParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        handleSelectedChanged(m_aiParams.ai.baseLossParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        handleSelectedChanged(m_aiParams.ai.train2DParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleSelectedChanged(m_aiParams.ai.dataLoader2DParams, name, selected);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::valueChangeSlot(QString const &name, int val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleValueChanged(m_aiParams.ai.convParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetMaxPoolType:
        handleValueChanged(m_aiParams.ai.maxPoolParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetAvgPoolType:
        handleValueChanged(m_aiParams.ai.avgPoolParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        handleValueChanged(m_aiParams.ai.adaptiveAvgPoolParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetConvTransposeType:
        handleValueChanged(m_aiParams.ai.convTransposeParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetUpSampleType:
        handleValueChanged(m_aiParams.ai.upSampleParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetLinearType:
        handleValueChanged(m_aiParams.ai.linearParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDropOutType:
        handleValueChanged(m_aiParams.ai.dropOutParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        handleValueChanged(m_aiParams.ai.batchNormParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetGroupNormType:
        handleValueChanged(m_aiParams.ai.groupNormParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetInstanceNormType:
        handleValueChanged(m_aiParams.ai.instanceNormParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        handleValueChanged(m_aiParams.ai.softMaxParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetFlattenType:
        handleValueChanged(m_aiParams.ai.flattenParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetChannelShuffleType:
        handleValueChanged(m_aiParams.ai.channelShuffleParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDetectLayer2DType:
        handleValueChanged(m_aiParams.ai.detectLayer2DParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetCatType:
        handleValueChanged(m_aiParams.ai.catParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        handleValueChanged(m_aiParams.ai.viewParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetSplitType:
        handleValueChanged(m_aiParams.ai.splitParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        handleValueChanged(m_aiParams.ai.train2DParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleValueChanged(m_aiParams.ai.dataLoader2DParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataInputTestType:
        handleValueChanged(m_aiParams.ai.dataInputTestParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        handleValueChanged(m_aiParams.ai.customParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;
    }
}

void WgAttrList::indexChangeSlot(QString const &name, int index, int &val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleIndexChanged(m_aiParams.ai.convParams, name, index, val);
        break;

    case e_NetMaxPoolType:
        handleIndexChanged(m_aiParams.ai.maxPoolParams, name, index, val);
        break;

    case e_NetAvgPoolType:
        handleIndexChanged(m_aiParams.ai.avgPoolParams, name, index, val);
        break;

    case e_NetAdaptiveAvgPoolType:
        handleIndexChanged(m_aiParams.ai.adaptiveAvgPoolParams, name, index, val);
        break;

    case e_NetConvTransposeType:
        handleIndexChanged(m_aiParams.ai.convTransposeParams, name, index, val);
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        handleIndexChanged(m_aiParams.ai.layerNormParams, name, index, val);
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        handleIndexChanged(m_aiParams.ai.permuteParams, name, index, val);
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        handleIndexChanged(m_aiParams.ai.train2DParams, name, index, val);
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::valueChangeSlot(QString const &name, int index, int val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleValueChanged(m_aiParams.ai.convParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetMaxPoolType:
        handleValueChanged(m_aiParams.ai.maxPoolParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetAvgPoolType:
        handleValueChanged(m_aiParams.ai.avgPoolParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        handleValueChanged(m_aiParams.ai.adaptiveAvgPoolParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetConvTransposeType:
        handleValueChanged(m_aiParams.ai.convTransposeParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        handleValueChanged(m_aiParams.ai.layerNormParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        handleValueChanged(m_aiParams.ai.permuteParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        handleValueChanged(m_aiParams.ai.train2DParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::rowChangeSlot(QString const &name, int rIndex, int &cIndex, int &val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        handleRowChanged(m_aiParams.ai.detectLayer2DParams, name, rIndex, cIndex, val);
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::colChangeSlot(QString const &name, int rIndex, int cIndex, int &val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        handleColChanged(m_aiParams.ai.detectLayer2DParams, name, rIndex, cIndex, val);
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::valueChangeSlot(QString const &name, int rIndex, int cIndex, int val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        handleValueChanged(m_aiParams.ai.detectLayer2DParams, name, rIndex, cIndex, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::valueChangeSlot(QString const &name, double val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        handleValueChanged(m_aiParams.ai.dropOutParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDropPathType:
        handleValueChanged(m_aiParams.ai.dropPathParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        handleValueChanged(m_aiParams.ai.constParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        handleValueChanged(m_aiParams.ai.baseLossParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        handleValueChanged(m_aiParams.ai.optAdaDeltaParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetOptAdaGradType:
        handleValueChanged(m_aiParams.ai.optAdaGradParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetOptAdamType:
        handleValueChanged(m_aiParams.ai.optAdamParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        handleValueChanged(m_aiParams.ai.optRMSpropParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetOptSGDType:
        handleValueChanged(m_aiParams.ai.optSGDParams, name, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::indexChangeSlot(QString const &name, int index, double &val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        handleIndexChanged(m_aiParams.ai.upSampleParams, name, index, val);
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}

void WgAttrList::valueChangeSlot(QString const &name, int index, double val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        break;

    case e_NetMaxPoolType:
        break;

    case e_NetAvgPoolType:
        break;

    case e_NetAdaptiveAvgPoolType:
        break;

    case e_NetConvTransposeType:
        break;

    case e_NetUpSampleType:
        handleValueChanged(m_aiParams.ai.upSampleParams, name, index, val);
        emit signalParamsChanged(m_sn, m_Name, m_Net, m_aiParams);
        break;

    case e_NetLinearType:
        break;

    case e_NetDropOutType:
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        break;

    case e_NetGroupNormType:
        break;

    case e_NetInstanceNormType:
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        break;

    case e_NetFlattenType:
        break;

    case e_NetChannelShuffleType:
        break;

    case e_NetDetectLayer2DType:
        break;

    case e_NetCatType:
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        break;

    case e_NetAddType:
        break;

    case e_NetConstType:
        break;

    case e_NetPermuteType:
        break;

    case e_NetViewType:
        break;

    case e_NetSplitType:
        break;

    case e_NetIdentityType:
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        break;

    case e_NetBaseLossType:
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        break;

    case e_NetOptAdaGradType:
        break;

    case e_NetOptAdamType:
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        break;

    case e_NetOptSGDType:
        break;

    case e_NetTrain2DType:
        break;

    case e_NetDataLoader2DType:
        break;

    case e_NetDataInputTestType:
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        break;
    }
}
