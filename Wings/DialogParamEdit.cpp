#include "DialogParamEdit.h"
#include "ui_DialogParamEdit.h"
#include <algorithm>
#include "AttrViewMgr.h"

constexpr int       ROW_HEIGHT  = 50 + 2;

constexpr int       ISTEP       = 1;
constexpr int       IPREC       = 1;
constexpr double    DSTEP       = 1.0 / 10;

CDialogParamEdit::CDialogParamEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogParamEdit)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);

    initTextEditor();
    initStrSelector();
    initIntEditor();
    initIntListEditor();
    initIntTableEditor();
    initFloatEditor();
    initFloatListEditor();

    initWgs();
}

CDialogParamEdit::~CDialogParamEdit()
{
    delete ui;
}

void CDialogParamEdit::initTextEditor()
{
    ui->wgTextEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgTextEditor->show();

    connect(ui->wgTextEditor, SIGNAL(signalTextChanged(QString const&, QString const&)), this, SLOT(TextChangedSlot(QString const&, QString const&)));
}

void CDialogParamEdit::initStrSelector()
{
    ui->wgStrSelector->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgStrSelector->show();

    connect(ui->wgStrSelector, SIGNAL(signalSelectedChanged(QString const&, QString const&)), this, SLOT(selectedChangeSlot(QString const&, QString const&)));
}

void CDialogParamEdit::initIntEditor()
{
    ui->wgIntEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgIntEditor->show();

    connect(ui->wgIntEditor, SIGNAL(signalValueChanged(QString const&, int)), this, SLOT(valueChangeSlot(QString const&, int)));
}

void CDialogParamEdit::initIntListEditor()
{
    ui->wgIntListEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgIntListEditor->show();

    connect(ui->wgIntListEditor, SIGNAL(signalIndexChanged(QString const&, int, int&)), this, SLOT(indexChangeSlot(QString const&, int, int&)));
    connect(ui->wgIntListEditor, SIGNAL(signalValueChanged(QString const&, int, int)), this, SLOT(valueChangeSlot(QString const&, int, int)));
}

void CDialogParamEdit::initIntTableEditor()
{
    ui->wgIntTableEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgIntTableEditor->show();

    connect(ui->wgIntTableEditor, SIGNAL(signalRowChanged(QString const&, int, int&, int&)), this, SLOT(rowChangeSlot(QString const&, int, int&, int&)));
    connect(ui->wgIntTableEditor, SIGNAL(signalColChanged(QString const&, int, int, int&)), this, SLOT(colChangeSlot(QString const&, int, int, int&)));
    connect(ui->wgIntTableEditor, SIGNAL(signalValueChanged(QString const&, int, int, int)), this, SLOT(valueChangeSlot(QString const&, int, int, int)));
}

void CDialogParamEdit::initFloatEditor()
{
    ui->wgFloatEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgFloatEditor->show();

    connect(ui->wgFloatEditor, SIGNAL(signalValueChanged(QString const&, double)), this, SLOT(valueChangeSlot(QString const&, double)));
}

void CDialogParamEdit::initFloatListEditor()
{
    ui->wgFloatListEditor->setFixedHeight(ROW_HEIGHT - 2);
    ui->wgFloatListEditor->show();

    connect(ui->wgFloatListEditor, SIGNAL(signalIndexChanged(QString const&, int, double&)), this, SLOT(indexChangeSlot(QString const&, int, double&)));
    connect(ui->wgFloatListEditor, SIGNAL(signalValueChanged(QString const&, int, double)), this, SLOT(valueChangeSlot(QString const&, int, double)));
}

void CDialogParamEdit::initWgs()
{
    m_wgs.push_back(ui->wgIntEditor);
    m_wgs.push_back(ui->wgTextEditor);
    m_wgs.push_back(ui->wgFloatEditor);
    m_wgs.push_back(ui->wgStrSelector);
    m_wgs.push_back(ui->wgIntListEditor);
    m_wgs.push_back(ui->wgIntTableEditor);
    m_wgs.push_back(ui->wgFloatListEditor);

    showWg(nullptr);
}

void CDialogParamEdit::showWg(QWidget *wg)
{
    for (auto item : m_wgs)
    {
        if (wg == item)
            item->show();
        else
            item->hide();
    }
}

void CDialogParamEdit::showTextEditor(QString const &name, QString const &text)
{
    ui->wgTextEditor->Init(name, text);
    showWg(ui->wgTextEditor);
}

void CDialogParamEdit::showStrSelector(QString const &name, QStringList const &rows, QString const &selected)
{
    ui->wgStrSelector->Init(name, rows, selected);
    showWg(ui->wgStrSelector);
}

void CDialogParamEdit::showIntEditor(QString const &name, int min, int max, int val, int step)
{
    ui->wgIntEditor->Init(name, min, max, val, step);
    showWg(ui->wgIntEditor);
}

void CDialogParamEdit::showIntListEditor(QString const &name, int cols, int index, int min, int max, int val, int step)
{
    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    ui->wgIntListEditor->Init(name, strCols, index, min, max, val, step);
    showWg(ui->wgIntListEditor);
}

void CDialogParamEdit::showIntTableEditor(QString const &name, int rows, int rIndex, int cols, int cIndex, int min, int max, int val, int step)
{
    QStringList strRows;
    for (auto i = 0; i < rows; i++)
        strRows << QString::number(i);

    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    ui->wgIntTableEditor->Init(name, strRows, rIndex, strCols, cIndex, min, max, val, step);
    showWg(ui->wgIntTableEditor);
}

void CDialogParamEdit::showFloatEditor(QString const &name, double min, double max, double val, double step, int prec)
{
    ui->wgFloatEditor->Init(name, min, max, val, step, prec);
    showWg(ui->wgFloatEditor);
}

void CDialogParamEdit::showFloatListEditor(QString const &name, int cols, int index, double min, double max, double val, double step, int prec)
{
    QStringList strCols;
    for (auto i = 0; i < cols; i++)
        strCols << QString::number(i);

    ui->wgFloatListEditor->Init(name, strCols, index, min, max, val, step, prec);
    showWg(ui->wgFloatListEditor);
}

bool CDialogParamEdit::decode(QString const &value, int* arr, int n) const
{
    auto begin = value.indexOf(QString("["));
    for (auto i = 0; i < n; i++)
    {
        if (begin == -1)
            return false;

        auto end = value.indexOf(QString(","), ++begin);
        if (end == -1 && (end = value.indexOf(QString("]"), begin)) == -1)
            return false;

        arr[i] = value.mid(begin, end - begin).trimmed().toInt();
        begin = value.indexOf(QString(","), begin);
    }

    return true;
}

bool CDialogParamEdit::decode(QString const &value, int *tab, int rows, int cols) const
{
    auto begin = value.indexOf(QString("["));
    for (auto row = 0; row < rows; row++)
    {
        if (begin == -1)
            return false;

        begin = value.indexOf(QString("["), ++begin);
        for (auto col = 0; col < cols; col++)
        {
            if (begin == -1)
                return false;

            auto end1 = value.indexOf(QString(","), ++begin);
            auto end2 = value.indexOf(QString("]"), begin);
            auto end = std::min(end1, end2);
            if (end1 == -1)
                end = end2;

            if (end == -1)
                return false;

            auto str = value.mid(begin, end - begin).trimmed();
            tab[row * cols + col] = value.mid(begin, end - begin).trimmed().toInt();

            begin = std::min(value.indexOf(QString(","), begin), value.indexOf(QString("]"), begin));
        }
    }

    return true;
}

bool CDialogParamEdit::decode(QString const &value, double *arr, int n) const
{
    auto begin = value.indexOf(QString("["));
    for (auto i = 0; i < n; i++)
    {
        if (begin == -1)
            return false;

        auto end = value.indexOf(QString(","), ++begin);
        if (end == -1 && (end = value.indexOf(QString("]"), begin)) == -1)
            return false;

        arr[i] = value.mid(begin, end - begin).trimmed().toDouble();
        begin = value.indexOf(QString(","), begin);
    }

    return true;
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
void CDialogParamEdit::initParams(vector<QString> const &params, NetConvParams &aiParams)
{
    aiParams = NetConvParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "inChannels")
        {
            aiParams.inChannels = value.toInt();
        }
        else if (field == "outChannels")
        {
            aiParams.outChannels = value.toInt();
        }
        else if (field == "kernelSize")
        {
            decode(value, aiParams.kernelSize, aiParams.dim);
        }
        else if (field == "stride")
        {
            decode(value, aiParams.stride, aiParams.dim);
        }
        else if (field == "dilation")
        {
            decode(value, aiParams.dilation, aiParams.dim);
        }
        else if (field == "padding")
        {
            decode(value, aiParams.padding, aiParams.dim);
        }
        else if (field == "paddingMode")
        {
            auto paddingMode = value.toStdString();
            auto sz = std::min(size_t(STRLEN), paddingMode.size());
            memcpy(aiParams.paddingMode, paddingMode.data(), sz);
            aiParams.paddingMode[sz] = '\0';
        }
        else if (field == "group")
        {
            aiParams.group = value.toInt();
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &value, NetConvParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "inChannels" || field == "outChannels")
    {
        showIntEditor(field, 1, 9999, value.toInt(), ISTEP);
    }
    else if (field == "kernelSize")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);
    }
    else if (field == "stride")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);
    }
    else if (field == "dilation")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);
    }
    else if (field == "padding")
    {
        showIntListEditor(field, aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);
    }
    else if (field == "paddingMode")
    {
        QStringList strRows;
        strRows << "zeros" << "reflect" << "replicate" << "circular";
        showStrSelector(field, strRows, aiParams.paddingMode);
    }
    else if (field == "group")
    {
        showIntEditor(field, 1, aiParams.inChannels, aiParams.group, ISTEP);
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleSelectedChanged(NetConvParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "paddingMode")
    {
        auto paddingMode = selected.toStdString();
        auto sz = (size_t(STRLEN) < paddingMode.size() ? size_t(STRLEN) : paddingMode.size());
        memcpy(aiParams.paddingMode, paddingMode.data(), sz);
        aiParams.paddingMode[sz] = '\0';
    }
}

void CDialogParamEdit::handleValueChanged(NetConvParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "inChannels")
    {
        aiParams.inChannels = val;
        aiParams.group = std::min(aiParams.group, aiParams.inChannels);
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

void CDialogParamEdit::handleIndexChanged(NetConvParams &aiParams, QString const &name, int index, int &val)
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

void CDialogParamEdit::handleValueChanged(NetConvParams &aiParams, QString const &name, int index, int val)
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

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_dilation": [1, 1, 1],
    "05_padding": [0, 0, 0],
    "06_requiresGrad": 1
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetMaxPoolParams &aiParams)
{
    aiParams = NetMaxPoolParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "kernelSize")
        {
            decode(value, aiParams.kernelSize, aiParams.dim);
        }
        else if (field == "stride")
        {
            decode(value, aiParams.stride, aiParams.dim);
        }
        else if (field == "dilation")
        {
            decode(value, aiParams.dilation, aiParams.dim);
        }
        else if (field == "padding")
        {
            decode(value, aiParams.padding, aiParams.dim);
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetMaxPoolParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "kernelSize")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);
    }
    else if (field == "stride")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);
    }
    else if (field == "dilation")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);
    }
    else if (field == "padding")
    {
        showIntListEditor(field, aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void CDialogParamEdit::handleIndexChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int &val)
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

void CDialogParamEdit::handleValueChanged(NetMaxPoolParams &aiParams, QString const &name, int index, int val)
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

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_padding": [0, 0, 0],
    "05_requiresGrad": 1
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetAvgPoolParams &aiParams)
{
    aiParams = NetAvgPoolParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "kernelSize")
        {
            decode(value, aiParams.kernelSize, aiParams.dim);
        }
        else if (field == "stride")
        {
            decode(value, aiParams.stride, aiParams.dim);
        }
        else if (field == "padding")
        {
            decode(value, aiParams.padding, aiParams.dim);
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetAvgPoolParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "kernelSize")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);
    }
    else if (field == "stride")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);
    }
    else if (field == "padding")
    {
        showIntListEditor(field, aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "requiresGrad")
    {
        aiParams.requiresGrad = val;
    }
}

void CDialogParamEdit::handleIndexChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int &val)
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

void CDialogParamEdit::handleValueChanged(NetAvgPoolParams &aiParams, QString const &name, int index, int val)
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

/*{
    "01_dim": 2,
    "02_outputSize": [32, 32, 32]
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetAdaptiveAvgPoolParams &aiParams)
{
    aiParams = NetAdaptiveAvgPoolParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "outputSize")
        {
            decode(value, aiParams.outputSize, aiParams.dim);
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetAdaptiveAvgPoolParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "outputSize")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9999, aiParams.outputSize[0], ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
}

void CDialogParamEdit::handleIndexChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "outputSize")
    {
        val = aiParams.outputSize[index];
    }
}

void CDialogParamEdit::handleValueChanged(NetAdaptiveAvgPoolParams &aiParams, QString const &name, int index, int val)
{
    if (name == "outputSize")
    {
        aiParams.outputSize[index] = val;
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
void CDialogParamEdit::initParams(vector<QString> const &params, NetConvTransposeParams &aiParams)
{
    aiParams = NetConvTransposeParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "inChannels")
        {
            aiParams.inChannels = value.toInt();
        }
        else if (field == "outChannels")
        {
            aiParams.outChannels = value.toInt();
        }
        else if (field == "group")
        {
            aiParams.group = value.toInt();
        }
        else if (field == "kernelSize")
        {
            decode(value, aiParams.kernelSize, aiParams.dim);
        }
        else if (field == "stride")
        {
            decode(value, aiParams.stride, aiParams.dim);
        }
        else if (field == "dilation")
        {
            decode(value, aiParams.dilation, aiParams.dim);
        }
        else if (field == "padding")
        {
            decode(value, aiParams.padding, aiParams.dim);
        }
        else if (field == "outputPadding")
        {
            decode(value, aiParams.outputPadding, aiParams.dim);
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &value, NetConvTransposeParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "inChannels" || field == "outChannels")
    {
        showIntEditor(field, 1, 9999, value.toInt(), ISTEP);
    }
    else if (field == "kernelSize")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.kernelSize[0], ISTEP);
    }
    else if (field == "stride")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 9, aiParams.stride[0], ISTEP);
    }
    else if (field == "dilation")
    {
        showIntListEditor(field, aiParams.dim, 0, 1, 64, aiParams.dilation[0], ISTEP);
    }
    else if (field == "padding")
    {
        showIntListEditor(field, aiParams.dim, 0, 0, 99, aiParams.padding[0], ISTEP);
    }
    else if (field == "group")
    {
        showIntEditor(field, 1, aiParams.inChannels, aiParams.group, ISTEP);
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
    else if (name == "inChannels")
    {
        aiParams.inChannels = val;
        aiParams.group = std::min(aiParams.group, aiParams.inChannels);
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

void CDialogParamEdit::handleIndexChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int &val)
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

void CDialogParamEdit::handleValueChanged(NetConvTransposeParams &aiParams, QString const &name, int index, int val)
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

/*{
    "01_dim": 2,
    "02_scaleFactor": [2.0, 2.0, 2.0],
    "03_mode": "nearest"
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetUpSampleParams &aiParams)
{
    aiParams = NetUpSampleParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "scaleFactor")
        {
            decode(value, aiParams.scaleFactor, aiParams.dim);
        }
        else if (field == "mode")
        {
            auto mode = value.toStdString();
            auto sz = std::min(size_t(STRLEN), mode.size());
            memcpy(aiParams.mode, mode.data(), sz);
            aiParams.mode[sz] = '\0';
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetUpSampleParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "scaleFactor")
    {
        showFloatListEditor(field, aiParams.dim, 0, 1.0, 10.0, aiParams.scaleFactor[0], DSTEP, IPREC);
    }
    else if (field == "mode")
    {
        QStringList strRows;
        strRows << "nearest" << "linear" << "bilinear" << "bicubic";
        showStrSelector(field, strRows, aiParams.mode);
    }
}

void CDialogParamEdit::handleSelectedChanged(NetUpSampleParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "mode")
    {
        auto mode = selected.toStdString();
        auto sz = (size_t(STRLEN) < mode.size() ? size_t(STRLEN) : mode.size());
        memcpy(aiParams.mode, mode.data(), sz);
        aiParams.mode[sz] = '\0';
    }
}

void CDialogParamEdit::handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
}

void CDialogParamEdit::handleIndexChanged(NetUpSampleParams &aiParams, QString const &name, int index, double &val)
{
    if (name == "scaleFactor")
    {
        val = aiParams.scaleFactor[index];
    }
}

void CDialogParamEdit::handleValueChanged(NetUpSampleParams &aiParams, QString const &name, int index, double val)
{
    if (name == "scaleFactor")
    {
        aiParams.scaleFactor[index] = val;
    }
}

/*{
    "01_inFeatures": 64,
    "02_outFeatures": 64,
    "03_bias": true,
    "04_requiresGrad": 1
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetLinearParams &aiParams)
{
    aiParams = NetLinearParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "inFeatures")
        {
            aiParams.inFeatures = value.toInt();
        }
        else if (field == "inChannels")
        {
            aiParams.outFeatures = value.toInt();
        }
        else if (field == "bias")
        {
            aiParams.bias = (value == "true" ? true : false);
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetLinearParams const &aiParams)
{
    if (field == "inFeatures")
    {
        showIntEditor(field, 1, 9999, aiParams.inFeatures, ISTEP);
    }
    else if (field == "outFeatures")
    {
        showIntEditor(field, 1, 9999, aiParams.outFeatures, ISTEP);
    }
    else if (field == "bias")
    {
        QStringList strRows;
        strRows << "true" << "false";
        showStrSelector(field, strRows, (aiParams.bias ? "true" : "false"));
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleSelectedChanged(NetLinearParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "bias")
    {
        auto bias = selected.toStdString();
        aiParams.bias = (bias == "true" ? true : false);
    }
}

void CDialogParamEdit::handleValueChanged(NetLinearParams &aiParams, QString const &name, int val)
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

/*{
    "01_dim": 2,
    "02_prob": 0.5
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetDropOutParams &aiParams)
{
    aiParams = NetDropOutParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "prob")
        {
            aiParams.prob = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetDropOutParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "prob")
    {
        showFloatEditor(field, 0.1, 0.9, aiParams.prob, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetDropOutParams &aiParams, QString const &name, int val)
{
    if (name == "dim")
    {
        aiParams.dim = val;
    }
}

void CDialogParamEdit::handleValueChanged(NetDropOutParams &aiParams, QString const &name, double val)
{
    if (name == "prob")
    {
        aiParams.prob = val;
    }
}

/*{
    "01_prob": 0.5
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetDropPathParams &aiParams)
{
    aiParams = NetDropPathParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "prob")
        {
            aiParams.prob = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetDropPathParams const &aiParams)
{
    if (field == "prob")
    {
        showFloatEditor(field, 0.1, 0.9, aiParams.prob, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetDropPathParams &aiParams, QString const &name, double val)
{
    if (name == "prob")
    {
        aiParams.prob = val;
    }
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetBatchNormParams &aiParams)
{
    aiParams = NetBatchNormParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "numFeatures")
        {
            aiParams.numFeatures = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetBatchNormParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "numFeatures")
    {
        showIntEditor(field, 1, 9999, aiParams.numFeatures, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetBatchNormParams &aiParams, QString const &name, int val)
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

/*{
    "01_numGroups": 2,
    "02_numChannels": 64
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetGroupNormParams &aiParams)
{
    aiParams = NetGroupNormParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "numGroups")
        {
            aiParams.numGroups = value.toInt();
        }
        else if (field == "numChannels")
        {
            aiParams.numChannels = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetGroupNormParams const &aiParams)
{
    if (field == "numGroups")
    {
        showIntEditor(field, 1, 9999, aiParams.numGroups, ISTEP);
    }
    else if (field == "numChannels")
    {
        showIntEditor(field, 1, 9999, aiParams.numChannels, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetGroupNormParams &aiParams, QString const &name, int val)
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

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetInstanceNormParams &aiParams)
{
    aiParams = NetInstanceNormParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
        else if (field == "numFeatures")
        {
            aiParams.numFeatures = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetInstanceNormParams const &aiParams)
{
    if (field == "dim")
    {
        showIntEditor(field, 2, 3, aiParams.dim, ISTEP);
    }
    else if (field == "numFeatures")
    {
        showIntEditor(field, 1, 9999, aiParams.numFeatures, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetInstanceNormParams &aiParams, QString const &name, int val)
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

/*{
    "01_normalizedShape": [0, 1, 3, 2]
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetLayerNormParams &aiParams)
{
    aiParams = NetLayerNormParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "normalizedShape")
        {
            int cnt = sizeof(aiParams.normalizedShape) / sizeof(aiParams.normalizedShape[0]);
            decode(value, aiParams.normalizedShape, cnt);
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetLayerNormParams const &aiParams)
{
    if (field == "normalizedShape")
    {
        int cnt = sizeof(aiParams.normalizedShape) / sizeof(aiParams.normalizedShape[0]);
        showIntListEditor(field, cnt, 0, 0, 9999, aiParams.normalizedShape[0], ISTEP);
    }
}

void CDialogParamEdit::handleIndexChanged(NetLayerNormParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "normalizedShape")
    {
        val = aiParams.normalizedShape[index];
    }
}

void CDialogParamEdit::handleValueChanged(NetLayerNormParams &aiParams, QString const &name, int index, int val)
{
    if (name == "normalizedShape")
    {
        aiParams.normalizedShape[index] = val;
    }
}

/*{
    "01_function": "SiLU"
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetActiveParams &aiParams)
{
    aiParams = NetActiveParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "function")
        {
            auto function = value.toStdString();
            auto sz = std::min(size_t(STRLEN), function.size());
            memcpy(aiParams.function, function.data(), sz);
            aiParams.function[sz] = '\0';
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetActiveParams const &aiParams)
{
    if (field == "function")
    {
        QStringList strRows;
        strRows << "ELU" << "Hardshrink" << "Hardsigmoid" << "Hardtanh" << "Hardswish";
        strRows << "LeakyReLU" << "LogSigmoid" << "PReLU" << "ReLU" << "ReLU6";
        strRows << "RReLU" << "SELU" << "CELU" << "GELU" << "Sigmoid";
        strRows << "SiLU" << "Mish" << "Softplus" << "Softshrink" << "Softsign";
        strRows << "Tanh" << "Tanhshrink" << "GLU";
        showStrSelector(field, strRows, aiParams.function);
    }
}

void CDialogParamEdit::handleSelectedChanged(NetActiveParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "function")
    {
        auto function = selected.toStdString();
        auto sz = (size_t(STRLEN) < function.size() ? size_t(STRLEN) : function.size());
        memcpy(aiParams.function, function.data(), sz);
        aiParams.function[sz] = '\0';
    }
}

/*{
    "01_softMaxDim": 2
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetSoftMaxParams &aiParams)
{
    aiParams = NetSoftMaxParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "softMaxDim")
        {
            aiParams.softMaxDim = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetSoftMaxParams const &aiParams)
{
    if (field == "softMaxDim")
    {
        showIntEditor(field, 2, 3, aiParams.softMaxDim, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetSoftMaxParams &aiParams, QString const &name, int val)
{
    if (name == "softMaxDim")
    {
        aiParams.softMaxDim = val;
    }
}

/*{
    "01_startDim": 1,
    "02_endDim": -1
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetFlattenParams &aiParams)
{
    aiParams = NetFlattenParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "startDim")
        {
            aiParams.startDim = value.toInt();
        }
        else if (field == "endDim")
        {
            aiParams.endDim = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetFlattenParams const &aiParams)
{
    if (field == "startDim")
    {
        showIntEditor(field, -10, 10, aiParams.startDim, ISTEP);
    }
    else if (field == "endDim")
    {
        showIntEditor(field, -10, 10, aiParams.endDim, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetFlattenParams &aiParams, QString const &name, int val)
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

/*{
    "01_groups": 2
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetChannelShuffleParams &aiParams)
{
    aiParams = NetChannelShuffleParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "groups")
        {
            aiParams.groups = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetChannelShuffleParams const &aiParams)
{
    if (field == "groups")
    {
        showIntEditor(field, 1, 9999, aiParams.groups, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetChannelShuffleParams &aiParams, QString const &name, int val)
{
    if (name == "groups")
    {
        aiParams.groups = val;
    }
}

/*{
    "01_numberClass": 2,
    "02_anchorSize": [[210, 210], [300, 400], [300, 400]],
    "03_imageSize": 416
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetDetectLayer2DParams &aiParams)
{
    aiParams = NetDetectLayer2DParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "numberClass")
        {
            aiParams.numberClass = value.toInt();
        }
        else if (field == "anchorSize")
        {
            int rows = sizeof(aiParams.anchorSize) / sizeof(aiParams.anchorSize[0]);
            int cols = sizeof(aiParams.anchorSize[0]) / sizeof(aiParams.anchorSize[0][0]);
            decode(value, aiParams.anchorSize[0], rows, cols);
        }
        else if (field == "imageSize")
        {
            aiParams.imageSize = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetDetectLayer2DParams const &aiParams)
{
    if (field == "numberClass")
    {
        showIntEditor(field, 1, 9999, aiParams.numberClass, ISTEP);
    }
    else if (field == "anchorSize")
    {
        int rows = sizeof(aiParams.anchorSize) / sizeof(aiParams.anchorSize[0]);
        int cols = sizeof(aiParams.anchorSize[0]) / sizeof(aiParams.anchorSize[0][0]);
        showIntTableEditor("anchorSize", rows, 0, cols, 0, 1, 9999, aiParams.anchorSize[0][0], ISTEP);
    }
    else if (field == "imageSize")
    {
        showIntEditor(field, 64, 9999, aiParams.imageSize, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int val)
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

void CDialogParamEdit::handleRowChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int &cIndex, int &val)
{
    if (name == "anchorSize")
    {
        cIndex = 0;
        val = aiParams.anchorSize[rIndex][cIndex];
    }
}

void CDialogParamEdit::handleColChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int &val)
{
    if (name == "anchorSize")
    {
        val = aiParams.anchorSize[rIndex][cIndex];
    }
}

void CDialogParamEdit::handleValueChanged(NetDetectLayer2DParams &aiParams, QString const &name, int rIndex, int cIndex, int val)
{
    if (name == "anchorSize")
    {
        aiParams.anchorSize[rIndex][cIndex] = val;
    }
}

/*{
    "01_catDim": 0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetCatParams &aiParams)
{
    aiParams = NetCatParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "catDim")
        {
            aiParams.catDim = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetCatParams const &aiParams)
{
    if (field == "catDim")
    {
        showIntEditor(field, 0, 6, aiParams.catDim, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetCatParams &aiParams, QString const &name, int val)
{
    if (name == "catDim")
    {
        aiParams.catDim = val;
    }
}


// 计算单元: calunit
/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetScaledParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetScaledParams const &)
{
    // no-op
    return;
}

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetAddParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetAddParams const &)
{
    // no-op
    return;
}

/*{
    "01_input": 0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetConstParams &aiParams)
{
    aiParams = NetConstParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "input")
        {
            aiParams.input = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetConstParams const &aiParams)
{
    if (field == "input")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.input, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetConstParams &aiParams, QString const &name, double val)
{
    if (name == "input")
    {
        aiParams.input = val;
    }
}

/*{
    "01_swapDims": [0, 1, 3, 2]
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetPermuteParams &aiParams)
{
    aiParams = NetPermuteParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "swapDims")
        {
            int cnt = sizeof(aiParams.swapDims) / sizeof(aiParams.swapDims[0]);
            decode(value, aiParams.swapDims, cnt);
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetPermuteParams const &aiParams)
{
    if (field == "swapDims")
    {
        int cnt = sizeof(aiParams.swapDims) / sizeof(aiParams.swapDims[0]);
        showIntListEditor(field, cnt, 0, 0, 9999, aiParams.swapDims[0], ISTEP);
    }
}

void CDialogParamEdit::handleIndexChanged(NetPermuteParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "swapDims")
    {
        val = aiParams.swapDims[index];
    }
}

void CDialogParamEdit::handleValueChanged(NetPermuteParams &aiParams, QString const &name, int index, int val)
{
    if (name == "swapDims")
    {
        aiParams.swapDims[index] = val;
    }
}

/*{
    "01_changedims": 0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetViewParams &aiParams)
{
    aiParams = NetViewParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "changeDims")
        {
            aiParams.changeDims = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetViewParams const &aiParams)
{
    if (field == "changeDims")
    {
        showIntEditor(field, 0, 6, aiParams.changeDims, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetViewParams &aiParams, QString const &name, int val)
{
    if (name == "changeDims")
    {
        aiParams.changeDims = val;
    }
}

/*{
    "01_splitsize": 2
    "02_dim": 0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetSplitParams &aiParams)
{
    aiParams = NetSplitParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "splitSize")
        {
            aiParams.splitSize = value.toInt();
        }
        else if (field == "dim")
        {
            aiParams.dim = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetSplitParams const &aiParams)
{
    if (field == "splitSize")
    {
        showIntEditor(field, 2, 9999, aiParams.splitSize, ISTEP);
    }
    else if (field == "dim")
    {
        showIntEditor(field, 0, 3, aiParams.dim, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetSplitParams &aiParams, QString const &name, int val)
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

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetIdentityParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetIdentityParams const &)
{
    // no-op
    return;
}


// 运行单元: rununit
/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetShowResultParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetShowResultParams const &)
{
    // no-op
    return;
}

/*{
    "01_function": "CrossEntropyLoss"
    "02_p": 1.0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetBaseLossParams &aiParams)
{
    aiParams = NetBaseLossParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "function")
        {
            auto function = value.toStdString();
            auto sz = std::min(size_t(STRLEN), function.size());
            memcpy(aiParams.function, function.data(), sz);
            aiParams.function[sz] = '\0';
        }
        else if (field == "p")
        {
            aiParams.p = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetBaseLossParams const &aiParams)
{
    if (field == "function")
    {
        QStringList strRows;
        strRows << "L1Loss" << "MSELoss" << "CrossEntropyLoss" << "NLLLoss";
        strRows << "CTCLoss" << "KLDivLoss" << "BCELoss" << "BCEWithLogitsLoss";
        showStrSelector(field, strRows, aiParams.function);
    }
    else if (field == "p")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.p, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleSelectedChanged(NetBaseLossParams &aiParams, QString const &name, QString const &selected)
{
    if (name == "function")
    {
        auto function = selected.toStdString();
        auto sz = (size_t(STRLEN) < function.size() ? size_t(STRLEN) : function.size());
        memcpy(aiParams.function, function.data(), sz);
        aiParams.function[sz] = '\0';
    }
}

void CDialogParamEdit::handleValueChanged(NetBaseLossParams &aiParams, QString const &name, double val)
{
    if (name == "p")
    {
        aiParams.p = val;
    }
}

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetDiceLossParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetDiceLossParams const &)
{
    // no-op
    return;
}

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetDetectYoloLossParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetDetectYoloLossParams const &)
{
    // no-op
    return;
}

/*{
    "01_lr": 1.0,
    "02_rho": 0.9,
    "03_weightDecay": 0.0,
    "04_eps": 1.00E-06
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetOptAdaDeltaParams &aiParams)
{
    aiParams = NetOptAdaDeltaParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "lr")
        {
            aiParams.lr = value.toDouble();
        }
        else if (field == "rho")
        {
            aiParams.rho = value.toDouble();
        }
        else if (field == "weightDecay")
        {
            aiParams.weightDecay = value.toDouble();
        }
        else if (field == "eps")
        {
            aiParams.eps = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetOptAdaDeltaParams const &aiParams)
{
    if (field == "lr")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.lr, 0.001, 3);
    }
    else if (field == "rho")
    {
        showFloatEditor(field, 0.1, 1.0, aiParams.rho, DSTEP, IPREC);
    }
    else if (field == "weightDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    }
    else if (field == "eps")
    {
        showFloatEditor(field, 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    }
}

void CDialogParamEdit::handleValueChanged(NetOptAdaDeltaParams &aiParams, QString const &name, double val)
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

/*{
    "01_lr": 0.01,
    "02_lrDecay": 0.0,
    "03_weightDecay": 0.0,
    "04_initialAccumulatorValue": 0.0,
    "05_eps": 1.00E-06
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetOptAdaGradParams &aiParams)
{
    aiParams = NetOptAdaGradParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "lr")
        {
            aiParams.lr = value.toDouble();
        }
        else if (field == "lrDecay")
        {
            aiParams.lrDecay = value.toDouble();
        }
        else if (field == "weightDecay")
        {
            aiParams.weightDecay = value.toDouble();
        }
        else if (field == "initialAccumulatorValue")
        {
            aiParams.initialAccumulatorValue = value.toDouble();
        }
        else if (field == "eps")
        {
            aiParams.eps = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetOptAdaGradParams const &aiParams)
{
    if (field == "lr")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.lr, 0.001, 3);
    }
    else if (field == "lrDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.lrDecay, DSTEP, IPREC);
    }
    else if (field == "weightDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    }
    else if (field == "initAccumulatorValue")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.initialAccumulatorValue, DSTEP, IPREC);
    }
    else if (field == "eps")
    {
        showFloatEditor(field, 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    }
}

void CDialogParamEdit::handleValueChanged(NetOptAdaGradParams &aiParams, QString const &name, double val)
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

/*{
    "01_lr": 0.01,
    "02_eps": 1.00E-06,
    "03_weightDecay": 0.0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetOptAdamParams &aiParams)
{
    aiParams = NetOptAdamParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "lr")
        {
            aiParams.lr = value.toDouble();
        }
        else if (field == "eps")
        {
            aiParams.eps = value.toDouble();
        }
        else if (field == "weightDecay")
        {
            aiParams.weightDecay = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetOptAdamParams const &aiParams)
{
    if (field == "lr")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.lr, 0.001, 3);
    }
    else if (field == "eps")
    {
        showFloatEditor(field, 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    }
    else if (field == "weightDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetOptAdamParams &aiParams, QString const &name, double val)
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

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetOptASGDParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetOptASGDParams const &)
{
    // no-op
    return;
}

/*{
    "01_lr": 0.01,
    "02_alpha": 9.90E-01,
    "03_eps": 1.00E-06,
    "04_weightDecay": 0.0,
    "05_momentum": 0.0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetOptRMSpropParams &aiParams)
{
    aiParams = NetOptRMSpropParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "lr")
        {
            aiParams.lr = value.toDouble();
        }
        else if (field == "alpha")
        {
            aiParams.alpha = value.toDouble();
        }
        else if (field == "eps")
        {
            aiParams.eps = value.toDouble();
        }
        else if (field == "weightDecay")
        {
            aiParams.weightDecay = value.toDouble();
        }
        else if (field == "momentum")
        {
            aiParams.momentum = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetOptRMSpropParams const &aiParams)
{
    if (field == "lr")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.lr, 0.001, 3);
    }
    else if (field == "alpha")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.alpha, 0.001, 3);
    }
    else if (field == "eps")
    {
        showFloatEditor(field, 0.0000001, 1.0, aiParams.eps, 0.0000001, 7);
    }
    else if (field == "weightDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    }
    else if (field == "momentum")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.momentum, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetOptRMSpropParams &aiParams, QString const &name, double val)
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

/*{
    "01_lr": 0.01,
    "02_momentum": 0.0,
    "03_dampening": 0.0,
    "04_weightDecay": 0.0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetOptSGDParams &aiParams)
{
    aiParams = NetOptSGDParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "lr")
        {
            aiParams.lr = value.toDouble();
        }
        else if (field == "momentum")
        {
            aiParams.momentum = value.toDouble();
        }
        else if (field == "dampening")
        {
            aiParams.dampening = value.toDouble();
        }
        else if (field == "weightDecay")
        {
            aiParams.weightDecay = value.toDouble();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetOptSGDParams const &aiParams)
{
    if (field == "lr")
    {
        showFloatEditor(field, 0.001, 10.0, aiParams.lr, 0.001, 3);
    }
    else if (field == "momentum")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.momentum, DSTEP, IPREC);
    }
    else if (field == "dampening")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.dampening, DSTEP, IPREC);
    }
    else if (field == "weightDecay")
    {
        showFloatEditor(field, 0.0, 1.0, aiParams.weightDecay, DSTEP, IPREC);
    }
}

void CDialogParamEdit::handleValueChanged(NetOptSGDParams &aiParams, QString const &name, double val)
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
void CDialogParamEdit::initParams(vector<QString> const &params, NetTrain2DParams &aiParams)
{
    aiParams = NetTrain2DParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "savePthPath")
        {
            auto savePthPath = value.toStdString();
            auto sz = std::min(size_t(STRLEN), savePthPath.size());
            memcpy(aiParams.savePthPath, savePthPath.data(), sz);
            aiParams.savePthPath[sz] = '\0';
        }
        else if (field == "preweightsPath")
        {
            auto preweightsPath = value.toStdString();
            auto sz = std::min(size_t(STRLEN), preweightsPath.size());
            memcpy(aiParams.preweightsPath, preweightsPath.data(), sz);
            aiParams.preweightsPath[sz] = '\0';
        }
        else if (field == "logDir")
        {
            auto logDir = value.toStdString();
            auto sz = std::min(size_t(STRLEN), logDir.size());
            memcpy(aiParams.logDir, logDir.data(), sz);
            aiParams.logDir[sz] = '\0';
        }
        else if (field == "epochNum")
        {
            aiParams.epochNum = value.toInt();
        }
        else if (field == "batchSize")
        {
            aiParams.batchSize = value.toInt();
        }
        else if (field == "isCuda")
        {
            aiParams.isCuda = (value == "true" ? true : false);
        }
        else if (field == "classNums")
        {
            int cnt = sizeof(aiParams.classNums) / sizeof(aiParams.classNums[0]);
            decode(value, aiParams.classNums, cnt);
        }
        else if (field == "criteriaFunction")
        {
            auto criteriaFunction = value.toStdString();
            auto sz = std::min(size_t(STRLEN), criteriaFunction.size());
            memcpy(aiParams.criteriaFunction, criteriaFunction.data(), sz);
            aiParams.criteriaFunction[sz] = '\0';
        }
        else if (field == "isTrainDataCriteria")
        {
            aiParams.isTrainDataCriteria = (value == "true" ? true : false);
        }
        else if (field == "showLossFigure")
        {
            aiParams.showLossFigure = (value == "true" ? true : false);
        }
        else if (field == "showCriteriaFigure")
        {
            aiParams.showCriteriaFigure = (value == "true" ? true : false);
        }
        else if (field == "epochSaveModel")
        {
            aiParams.epochSaveModel = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &value, NetTrain2DParams const &aiParams)
{
    if (field == "savePthPath" || field == "preweightsPath" || field == "logDir")
    {
        showTextEditor(field, value);
    }
    else if (field == "epochNum")
    {
        showIntEditor(field, 20, 10000, aiParams.epochNum, ISTEP);
    }
    else if (field == "batchSize")
    {
        showIntEditor(field, 1, 128, aiParams.batchSize, ISTEP);
    }
    else if (field == "isCuda" || field == "isTrainDataCriteria" || field == "showLossFigure" || field == "showCriteriaFigure")
    {
        QStringList strRows;
        strRows << "true" << "false";
        showStrSelector(field, strRows, value);
    }
    else if (field == "classNums")
    {
        int cnt = sizeof(aiParams.classNums) / sizeof(aiParams.classNums[0]);
        showIntListEditor(field, cnt, 0, 1, 1000, aiParams.classNums[0], ISTEP);
    }
    else if (field == "criteriaFunction")
    {
        QStringList strRows;
        strRows << "Dice" << "IOU" << "Accuracy" << "L1" << "L2" << "Jaccard";
        showStrSelector(field, strRows, aiParams.criteriaFunction);
    }
    else if (field == "epochSaveModel")
    {
        showIntEditor(field, 1, 50, aiParams.epochSaveModel, ISTEP);
    }
}

void CDialogParamEdit::handleTextChanged(NetTrain2DParams &aiParams, QString const &name, QString const &text)
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

void CDialogParamEdit::handleSelectedChanged(NetTrain2DParams &aiParams, QString const &name, QString const &selected)
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

void CDialogParamEdit::handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int val)
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

void CDialogParamEdit::handleIndexChanged(NetTrain2DParams &aiParams, QString const &name, int index, int &val)
{
    if (name == "classNums")
    {
        val = aiParams.classNums[index];
    }
}

void CDialogParamEdit::handleValueChanged(NetTrain2DParams &aiParams, QString const &name, int index, int val)
{
    if (name == "classNums")
    {
        aiParams.classNums[index] = val;
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
void CDialogParamEdit::initParams(vector<QString> const &params, NetDataLoader2DParams &aiParams)
{
    aiParams = NetDataLoader2DParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "inputW")
        {
            aiParams.inputW = value.toInt();
        }
        else if (field == "inputH")
        {
            aiParams.inputH = value.toInt();
        }
        else if (field == "dataJsonPath")
        {
            auto dataJsonPath = value.toStdString();
            auto sz = std::min(size_t(STRLEN), dataJsonPath.size());
            memcpy(aiParams.dataJsonPath, dataJsonPath.data(), sz);
            aiParams.dataJsonPath[sz] = '\0';
        }
        else if (field == "dataLabelCodeJsonPath")
        {
            auto dataLabelCodeJsonPath = value.toStdString();
            auto sz = std::min(size_t(STRLEN), dataLabelCodeJsonPath.size());
            memcpy(aiParams.dataLabelCodeJsonPath, dataLabelCodeJsonPath.data(), sz);
            aiParams.dataLabelCodeJsonPath[sz] = '\0';
        }
        else if (field == "isRgb")
        {
            aiParams.isRgb = (value == "true" ? true : false);
        }
        else if (field == "batchSize")
        {
            aiParams.batchSize = value.toInt();
        }
        else if (field == "numWorkers")
        {
            aiParams.numWorkers = value.toInt();
        }
        else if (field == "shuffle")
        {
            aiParams.shuffle = (value == "true" ? true : false);
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &value, NetDataLoader2DParams const &aiParams)
{
    if (field == "inputW")
    {
        showIntEditor(field, 1, 1024, aiParams.inputW, ISTEP);
    }
    else if (field == "inputH")
    {
        showIntEditor(field, 1, 1024, aiParams.inputH, ISTEP);
    }
    else if (field == "dataJsonPath" || field == "dataLabelCodeJsonPath")
    {
        showTextEditor(field, value);
    }
    else if (field == "isRgb" || field == "shuffle")
    {
        QStringList strRows;
        strRows << "true" << "false";
        showStrSelector(field, strRows, value);
    }
    else if (field == "batchSize")
    {
        showIntEditor(field, 1, 128, aiParams.batchSize, ISTEP);
    }
    else if (field == "numWorkers")
    {
        showIntEditor(field, 1, 16, aiParams.numWorkers, ISTEP);
    }
}

void CDialogParamEdit::handleTextChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &text)
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

void CDialogParamEdit::handleSelectedChanged(NetDataLoader2DParams &aiParams, QString const &name, QString const &selected)
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

void CDialogParamEdit::handleValueChanged(NetDataLoader2DParams &aiParams, QString const &name, int val)
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

/*{
    "01_inputDim": 0
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetDataInputTestParams &aiParams)
{
    aiParams = NetDataInputTestParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "inputDim")
        {
            aiParams.inputDim = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetDataInputTestParams const &aiParams)
{
    if (field == "inputDim")
    {
        showIntEditor(field, 0, 6, aiParams.inputDim, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetDataInputTestParams &aiParams, QString const &name, int val)
{
    if (name == "inputDim")
    {
        aiParams.inputDim = val;
    }
}

/*{

}*/
void CDialogParamEdit::initParams(vector<QString> const &, NetDataOutputParams &)
{
    // no-op
    return;
}

void CDialogParamEdit::showParam(QString const &, QString const &, NetDataOutputParams const &)
{
    // no-op
    return;
}


// 定制网元: custom
/*{
    "01_cycle": 1,
    "02_reqGrad": 1
}*/
void CDialogParamEdit::initParams(vector<QString> const &params, NetCustomParams &aiParams)
{
    aiParams = NetCustomParams();

    for (auto const &param : params)
    {
        auto pos = param.lastIndexOf(": ");
        auto field = param.mid(0, pos);
        auto value = param.mid(pos + 2).trimmed();

        if (field == "cycle")
        {
            aiParams.cycle = value.toInt();
        }
        else if (field == "requiresGrad")
        {
            aiParams.requiresGrad = value.toInt();
        }
    }
}

void CDialogParamEdit::showParam(QString const &field, QString const &, NetCustomParams const &aiParams)
{
    if (field == "cycle")
    {
        showIntEditor(field, 1, 9999, aiParams.cycle, ISTEP);
    }
    else if (field == "requiresGrad")
    {
        showIntEditor(field, -1, 1, aiParams.requiresGrad, ISTEP);
    }
}

void CDialogParamEdit::handleValueChanged(NetCustomParams &aiParams, QString const &name, int val)
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


void CDialogParamEdit::Show(int sn, QString const &netId, QString const &netName, QString const &field, QString const &value, vector<QString> const &params)
{
    m_sn = sn;
    m_netId = netId;
    m_netName = netName;

    m_field = field;
    m_value = value;

    m_aiParams.type = CAttrViewMgr::ins().GetNetType(netId);

    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        initParams(params, m_aiParams.ai.convParams);
        showParam(field, value, m_aiParams.ai.convParams);
        break;

    case e_NetMaxPoolType:
        initParams(params, m_aiParams.ai.maxPoolParams);
        showParam(field, value, m_aiParams.ai.maxPoolParams);
        break;

    case e_NetAvgPoolType:
        initParams(params, m_aiParams.ai.avgPoolParams);
        showParam(field, value, m_aiParams.ai.avgPoolParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        initParams(params, m_aiParams.ai.adaptiveAvgPoolParams);
        showParam(field, value, m_aiParams.ai.adaptiveAvgPoolParams);
        break;

    case e_NetConvTransposeType:
        initParams(params, m_aiParams.ai.convTransposeParams);
        showParam(field, value, m_aiParams.ai.convTransposeParams);
        break;

    case e_NetUpSampleType:
        initParams(params, m_aiParams.ai.upSampleParams);
        showParam(field, value, m_aiParams.ai.upSampleParams);
        break;

    case e_NetLinearType:
        initParams(params, m_aiParams.ai.linearParams);
        showParam(field, value, m_aiParams.ai.linearParams);
        break;

    case e_NetDropOutType:
        initParams(params, m_aiParams.ai.dropOutParams);
        showParam(field, value, m_aiParams.ai.dropOutParams);
        break;

    case e_NetDropPathType:
        initParams(params, m_aiParams.ai.dropPathParams);
        showParam(field, value, m_aiParams.ai.dropPathParams);
        break;

    case e_NetBatchNormType:
        initParams(params, m_aiParams.ai.batchNormParams);
        showParam(field, value, m_aiParams.ai.batchNormParams);
        break;

    case e_NetGroupNormType:
        initParams(params, m_aiParams.ai.groupNormParams);
        showParam(field, value, m_aiParams.ai.groupNormParams);
        break;

    case e_NetInstanceNormType:
        initParams(params, m_aiParams.ai.instanceNormParams);
        showParam(field, value, m_aiParams.ai.instanceNormParams);
        break;

    case e_NetLayerNormType:
        initParams(params, m_aiParams.ai.layerNormParams);
        showParam(field, value, m_aiParams.ai.layerNormParams);
        break;

    case e_NetActiveType:
        initParams(params, m_aiParams.ai.activeParams);
        showParam(field, value, m_aiParams.ai.activeParams);
        break;

    case e_NetSoftMaxType:
        initParams(params, m_aiParams.ai.softMaxParams);
        showParam(field, value, m_aiParams.ai.softMaxParams);
        break;

    case e_NetFlattenType:
        initParams(params, m_aiParams.ai.flattenParams);
        showParam(field, value, m_aiParams.ai.flattenParams);
        break;

    case e_NetChannelShuffleType:
        initParams(params, m_aiParams.ai.channelShuffleParams);
        showParam(field, value, m_aiParams.ai.channelShuffleParams);
        break;

    case e_NetDetectLayer2DType:
        initParams(params, m_aiParams.ai.detectLayer2DParams);
        showParam(field, value, m_aiParams.ai.detectLayer2DParams);
        break;

    case e_NetCatType:
        initParams(params, m_aiParams.ai.catParams);
        showParam(field, value, m_aiParams.ai.catParams);
        break;


    // 计算单元: calunit
    case e_NetScaledType:
        initParams(params, m_aiParams.ai.scaledParams);
        showParam(field, value, m_aiParams.ai.scaledParams);
        break;

    case e_NetAddType:
        initParams(params, m_aiParams.ai.addParams);
        showParam(field, value, m_aiParams.ai.addParams);
        break;

    case e_NetConstType:
        initParams(params, m_aiParams.ai.constParams);
        showParam(field, value, m_aiParams.ai.constParams);
        break;

    case e_NetPermuteType:
        initParams(params, m_aiParams.ai.permuteParams);
        showParam(field, value, m_aiParams.ai.permuteParams);
        break;

    case e_NetViewType:
        initParams(params, m_aiParams.ai.viewParams);
        showParam(field, value, m_aiParams.ai.viewParams);
        break;

    case e_NetSplitType:
        initParams(params, m_aiParams.ai.splitParams);
        showParam(field, value, m_aiParams.ai.splitParams);
        break;

    case e_NetIdentityType:
        initParams(params, m_aiParams.ai.identityParams);
        showParam(field, value, m_aiParams.ai.identityParams);
        break;


    // 运行单元: rununit
    case e_NetShowResultType:
        initParams(params, m_aiParams.ai.showResultParams);
        showParam(field, value, m_aiParams.ai.showResultParams);
        break;

    case e_NetBaseLossType:
        initParams(params, m_aiParams.ai.baseLossParams);
        showParam(field, value, m_aiParams.ai.baseLossParams);
        break;

    case e_NetDiceLossType:
        initParams(params, m_aiParams.ai.diceLossParams);
        showParam(field, value, m_aiParams.ai.diceLossParams);
        break;

    case e_NetDetectYoloLossType:
        initParams(params, m_aiParams.ai.detectYoloLossParams);
        showParam(field, value, m_aiParams.ai.detectYoloLossParams);
        break;

    case e_NetOptAdaDeltaType:
        initParams(params, m_aiParams.ai.optAdaDeltaParams);
        showParam(field, value, m_aiParams.ai.optAdaDeltaParams);
        break;

    case e_NetOptAdaGradType:
        initParams(params, m_aiParams.ai.optAdaGradParams);
        showParam(field, value, m_aiParams.ai.optAdaGradParams);
        break;

    case e_NetOptAdamType:
        initParams(params, m_aiParams.ai.optAdamParams);
        showParam(field, value, m_aiParams.ai.optAdamParams);
        break;

    case e_NetOptASGDType:
        initParams(params, m_aiParams.ai.optASGDParams);
        showParam(field, value, m_aiParams.ai.optASGDParams);
        break;

    case e_NetOptRMSpropType:
        initParams(params, m_aiParams.ai.optRMSpropParams);
        showParam(field, value, m_aiParams.ai.optRMSpropParams);
        break;

    case e_NetOptSGDType:
        initParams(params, m_aiParams.ai.optSGDParams);
        showParam(field, value, m_aiParams.ai.optSGDParams);
        break;

    case e_NetTrain2DType:
        initParams(params, m_aiParams.ai.train2DParams);
        showParam(field, value, m_aiParams.ai.train2DParams);
        break;

    case e_NetDataLoader2DType:
        initParams(params, m_aiParams.ai.dataLoader2DParams);
        showParam(field, value, m_aiParams.ai.dataLoader2DParams);
        break;

    case e_NetDataInputTestType:
        initParams(params, m_aiParams.ai.dataInputTestParams);
        showParam(field, value, m_aiParams.ai.dataInputTestParams);
        break;

    case e_NetDataOutputType:
        initParams(params, m_aiParams.ai.dataOutputParams);
        showParam(field, value, m_aiParams.ai.dataOutputParams);
        break;

    // 定制网元: custom
    case e_NetCustomType:
        initParams(params, m_aiParams.ai.customParams);
        showParam(field, value, m_aiParams.ai.customParams);
        break;
    }

    emit showInfoSignal("Edit " + netId + "_" + QString::number(sn) + " param " + field + ": " + value);
}


void CDialogParamEdit::TextChangedSlot(QString const &name, QString const &text)
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleTextChanged(m_aiParams.ai.dataLoader2DParams, name, text);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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


void CDialogParamEdit::selectedChangeSlot(QString const &name, QString const &selected)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleSelectedChanged(m_aiParams.ai.convParams, name, selected);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetLinearType:
        handleSelectedChanged(m_aiParams.ai.linearParams, name, selected);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleSelectedChanged(m_aiParams.ai.dataLoader2DParams, name, selected);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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


void CDialogParamEdit::valueChangeSlot(QString const &name, int val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleValueChanged(m_aiParams.ai.convParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetMaxPoolType:
        handleValueChanged(m_aiParams.ai.maxPoolParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetAvgPoolType:
        handleValueChanged(m_aiParams.ai.avgPoolParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        handleValueChanged(m_aiParams.ai.adaptiveAvgPoolParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetConvTransposeType:
        handleValueChanged(m_aiParams.ai.convTransposeParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetUpSampleType:
        handleValueChanged(m_aiParams.ai.upSampleParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetLinearType:
        handleValueChanged(m_aiParams.ai.linearParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDropOutType:
        handleValueChanged(m_aiParams.ai.dropOutParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDropPathType:
        break;

    case e_NetBatchNormType:
        handleValueChanged(m_aiParams.ai.batchNormParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetGroupNormType:
        handleValueChanged(m_aiParams.ai.groupNormParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetInstanceNormType:
        handleValueChanged(m_aiParams.ai.instanceNormParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetLayerNormType:
        break;

    case e_NetActiveType:
        break;

    case e_NetSoftMaxType:
        handleValueChanged(m_aiParams.ai.softMaxParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetFlattenType:
        handleValueChanged(m_aiParams.ai.flattenParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetChannelShuffleType:
        handleValueChanged(m_aiParams.ai.channelShuffleParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDetectLayer2DType:
        handleValueChanged(m_aiParams.ai.detectLayer2DParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetCatType:
        handleValueChanged(m_aiParams.ai.catParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetSplitType:
        handleValueChanged(m_aiParams.ai.splitParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDataLoader2DType:
        handleValueChanged(m_aiParams.ai.dataLoader2DParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDataInputTestType:
        handleValueChanged(m_aiParams.ai.dataInputTestParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDataOutputType:
        break;


    // 定制网元: custom
    case e_NetCustomType:
        handleValueChanged(m_aiParams.ai.customParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;
    }
}


void CDialogParamEdit::indexChangeSlot(QString const &name, int index, int &val)
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

void CDialogParamEdit::valueChangeSlot(QString const &name, int index, int val)
{
    switch (m_aiParams.type)
    {
    // 网络单元: netunit
    case e_NetConvType:
        handleValueChanged(m_aiParams.ai.convParams, name, index, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetMaxPoolType:
        handleValueChanged(m_aiParams.ai.maxPoolParams, name, index, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetAvgPoolType:
        handleValueChanged(m_aiParams.ai.avgPoolParams, name, index, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetAdaptiveAvgPoolType:
        handleValueChanged(m_aiParams.ai.adaptiveAvgPoolParams, name, index, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetConvTransposeType:
        handleValueChanged(m_aiParams.ai.convTransposeParams, name, index, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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


void CDialogParamEdit::rowChangeSlot(QString const &name, int rIndex, int &cIndex, int &val)
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

void CDialogParamEdit::colChangeSlot(QString const &name, int rIndex, int cIndex, int &val)
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

void CDialogParamEdit::valueChangeSlot(QString const &name, int rIndex, int cIndex, int val)
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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


void CDialogParamEdit::valueChangeSlot(QString const &name, double val)
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDropPathType:
        handleValueChanged(m_aiParams.ai.dropPathParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetDiceLossType:
        break;

    case e_NetDetectYoloLossType:
        break;

    case e_NetOptAdaDeltaType:
        handleValueChanged(m_aiParams.ai.optAdaDeltaParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetOptAdaGradType:
        handleValueChanged(m_aiParams.ai.optAdaGradParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetOptAdamType:
        handleValueChanged(m_aiParams.ai.optAdamParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetOptASGDType:
        break;

    case e_NetOptRMSpropType:
        handleValueChanged(m_aiParams.ai.optRMSpropParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
        break;

    case e_NetOptSGDType:
        handleValueChanged(m_aiParams.ai.optSGDParams, name, val);
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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


void CDialogParamEdit::indexChangeSlot(QString const &name, int index, double &val)
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

void CDialogParamEdit::valueChangeSlot(QString const &name, int index, double val)
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
        emit paramsChangedSignal(m_sn, m_netName, m_netId, m_aiParams);
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

void CDialogParamEdit::on_btnClose_clicked()
{
    this->close();
}
