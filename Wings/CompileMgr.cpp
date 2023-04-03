#include "CompileMgr.h"
#include <QJsonObject>
#include <QJsonArray>
#include <map>
#include "AttrViewMgr.h"

using std::map;

CCompileMgr& CCompileMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CCompileMgr());
	return *kp.get();
}

CCompileMgr::CCompileMgr()
{

}

CCompileMgr::~CCompileMgr()
{

}


void CCompileMgr::initParamImportCode()
{
    m_paramImportCode.clear();

    m_paramImportCode.push_back("import os");
    m_paramImportCode.push_back("import importlib, sys");
    m_paramImportCode.push_back("Current_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))");
    m_paramImportCode.push_back("sys.path.append(Current_DIR)");
    m_paramImportCode.push_back("Base_DIR=os.path.abspath(os.path.join(os.getcwd(),\"..\"))");
    m_paramImportCode.push_back("sys.path.append(Base_DIR)");
    m_paramImportCode.push_back("");
}

void CCompileMgr::initParamClassInitCode(QString const &netId)
{
    m_paramClassCode.clear();

    m_paramClassCode.push_back("class " + netId + "_param():");
    m_paramClassCode.push_back(m_ind1 + "def __init__(self):");
    m_paramClassCode.push_back(m_ind2 + "self.requires_grad = 1");
    m_paramClassCode.push_back(m_ind2 + "self.repetnum = 1");
    m_paramClassCode.push_back("");
}

void CCompileMgr::initNetImportCode()
{
    m_netImportCode.clear();

    m_netImportCode.push_back("import os");
    m_netImportCode.push_back("import importlib, sys");
    m_netImportCode.push_back("Current_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))");
    m_netImportCode.push_back("sys.path.append(Current_DIR)");
    m_netImportCode.push_back("Base_DIR=os.path.abspath(os.path.join(os.getcwd(),\"..\"))");
    m_netImportCode.push_back("sys.path.append(Base_DIR)");
    m_netImportCode.push_back("");
}

void CCompileMgr::initNetClassInitCode(QString const &netId)
{
    m_netInitCode.clear();

    m_netInitCode.push_back("class " + netId + "_net(nn.Module):");
    m_netInitCode.push_back(m_ind1 + "def __init__(self, " + netId + "_param):");
    m_netInitCode.push_back(m_ind2 + "super(" + netId + "_net, self).__init__()");
    m_netInitCode.push_back("");
    m_netInitCode.push_back(m_ind2 + "self.repetnum = " + netId + "_param.repetnum");
    m_netInitCode.push_back(m_ind2 + "self.requires_grad = " + netId + "_param.requires_grad");
    m_netInitCode.push_back("");
}

void CCompileMgr::initNetRequiresGradCode()
{
    m_netRequiresGradCode.clear();

    m_netRequiresGradCode.push_back(m_ind1 + "def requires_grad(self):");
    m_netRequiresGradCode.push_back(m_ind2 + "if self.requires_grad == False:");
    m_netRequiresGradCode.push_back(m_ind3 + "for p in self.parameters():");
    m_netRequiresGradCode.push_back(m_ind4 + "p.requires_grad = False");
}

void CCompileMgr::initNetForwardCode()
{
    m_netForwardCode.clear();

    m_netForwardCode.push_back(m_ind1 + "def forward(self, x):");
    m_netForwardCode.push_back(m_ind2 + "for i in range(0, self.repetnum):");
}

void CCompileMgr::GenCodeInit(QString const &catalog, QString const &netId, QString const &outline, QString const &specify)
{
    m_paramCode.clear();
    m_paramCode.push_back("# param@" + catalog);
    m_paramCode.push_back("# outline: " + outline);
    m_paramCode.push_back("# specify: " + specify);
    m_paramCode.push_back("");

    initParamImportCode();
    initParamClassInitCode(netId);

    m_netCode.clear();
    m_netCode.push_back("# net@" + catalog);
    m_netCode.push_back("# outline: " + outline);
    m_netCode.push_back("# specify: " + specify);
    m_netCode.push_back("");

    initNetImportCode();
    initNetClassInitCode(netId);
    initNetRequiresGradCode();
    initNetForwardCode();
}

void CCompileMgr::GenParamImportCode(QString const &itemDir, QString const &itemNetId)
{
    auto code = "from " + itemDir + "." + itemNetId + "_param import " + itemNetId + "_param";

    for (auto const &item : m_paramImportCode)
        if (item == code)
            return;

    m_paramImportCode.push_back(code);
}

void CCompileMgr::GenParamInitCode(int index, QString const &itemNetId, int itemSn)
{
    if (index > 0  && index % 5 == 0)
        m_paramClassCode.push_back("");

    auto lvalue = "self." + itemNetId.toLower() + "_" + QString::number(itemSn);
    auto rvalue = itemNetId + "_param()";
    auto code = lvalue + " = " + rvalue;
    m_paramClassCode.push_back(m_ind2 + code);
}

void CCompileMgr::GenNetImportCode(QString const &itemDir, QString const &itemNetId)
{
    auto code = "from " + itemDir + "." + itemNetId + "_param import " + itemNetId + "_param";

    for (auto const &item : m_netImportCode)
        if (item == code)
            return;

    m_netImportCode.push_back(code);

    code = "from " + itemDir + "." + itemNetId + "_net import " + itemNetId + "_net";
    m_netImportCode.push_back(code);
}

void CCompileMgr::GenNetInitCode(int index, QString const &netId, QString const &itemNetId, int itemSn)
{
    if (index > 0  && index % 5 == 0)
        m_netInitCode.push_back("");

    auto lvalue = "self." + itemNetId.toLower() + "_" + QString::number(itemSn);
    auto rvalue = itemNetId + "_net(" + netId + "_param." + itemNetId.toLower() + "_" + QString::number(itemSn) + ")";
    auto code = lvalue + " = " + rvalue;
    m_netInitCode.push_back(m_ind2 + code);
}

void CCompileMgr::GenNetForwardCode(ForwardInfo const &info)
{
    QString lvalue, rvalue;
    QString strIndex = QString::number(info.index + 1);

    if (info.inCnt == 1)
    {
        lvalue = "y" + strIndex;
        rvalue = "self." + info.netId.toLower() + "_" + QString::number(info.sn) + "(";
        if (info.inputs.empty())
        {
            if (info.inChannels.empty() || info.outCnts.empty() || info.outIndexs.empty() || info.outChannels.empty())
            {
                //error
                return;
            }
            else
            {
                if (info.outCnts[0] == 1)
                {
                    rvalue += "y" + QString::number(info.outIndexs[0] + 1) + ")";
                }
                else
                {
                    rvalue += "y" + QString::number(info.outIndexs[0] + 1) + "[" + QString::number(info.outChannels[0]) + "])";
                }
            }
        }
        else
        {
            if (info.inputCnt == 1)
            {
                rvalue += "x)";
            }
            else
            {
                rvalue += "x[" + QString::number(info.inputs[0].first) + "])";
            }
        }

        auto code = lvalue + " = " + rvalue;
        m_netForwardCode.push_back(m_ind3 + code);
    }
    else
    {
        map<int, QString> rvalueMap;
        for (auto &outIn : info.inputs)
        {
            if (info.inputCnt == 1)
            {
                rvalue = "x";
            }
            else
            {
                rvalue = "x[" + QString::number(outIn.first) + "]";
            }

            rvalueMap[outIn.second] = rvalue;
        }
        for (auto index = 0; index < (int)info.inChannels.size(); index++)
        {
            if (info.outCnts[index] == 1)
            {
                rvalue = "y" + QString::number(info.outIndexs[index] + 1);
            }
            else
            {
                rvalue = "y" + QString::number(info.outIndexs[index] + 1) + "[" + QString::number(info.outChannels[index]) + "]";
            }

            rvalueMap[info.inChannels[index]] = rvalue;
        }

        if (rvalueMap.empty())
        {
            lvalue = "y" + strIndex;
            rvalue = "self." + info.netId.toLower() + "_" + QString::number(info.sn) + "()";
            auto code = lvalue + " = " + rvalue;
            m_netForwardCode.push_back(m_ind3 + code);
        }
        else
        {
            lvalue = "x" + strIndex;
            rvalue = "[";
            for (auto itor = rvalueMap.begin(); itor != rvalueMap.end(); ++itor)
            {
                if (itor != rvalueMap.begin())
                    rvalue += ", ";

                rvalue += itor->second;
            }
            rvalue += "]";
            auto code = lvalue + " = " + rvalue;
            m_netForwardCode.push_back(m_ind3 + code);

            lvalue = "y" + strIndex;
            rvalue = "self." + info.netId.toLower() + "_" + QString::number(info.sn) + "(x" + strIndex + ")";
            code = lvalue + " = " + rvalue;
            m_netForwardCode.push_back(m_ind3 + code);
        }
    }
}

void CCompileMgr::finishParamImportCode()
{
    m_paramImportCode.push_back("");
    m_paramImportCode.push_back("import numpy as np");
    m_paramImportCode.push_back("");
}

void CCompileMgr::finishParamInitCode()
{
    m_paramClassCode.push_back("");
}

void CCompileMgr::finishNetImportCode()
{
    m_netImportCode.push_back("");
    //m_netImportCode.push_back("import numpy as np");
    m_netImportCode.push_back("import torch");
    m_netImportCode.push_back("from torch import nn");
    m_netImportCode.push_back("");
}

void CCompileMgr::finishNetInitCode()
{
    m_netInitCode.push_back("");
}

void CCompileMgr::finishNetRequiresGradCode()
{
    m_netRequiresGradCode.push_back("");
}

void CCompileMgr::finishNetForwardCode(FinishInfo const &info)
{
    m_netForwardCode.push_back("");

    if (info.outputCnt == 1)
    {
        if (info.outputs.empty() || info.outCnts.empty() || info.outIndexs.empty())
        {
            //error
            return;
        }
        else
        {
            if (info.outCnts[0] == 1)
            {
                auto code = "return y" + QString::number(info.outIndexs[0] + 1);
                m_netForwardCode.push_back(m_ind2 + code);
            }
            else
            {
                auto code = "return y" + QString::number(info.outIndexs[0] + 1) + "[" + QString::number(info.outputs[0].first) + "]";
                m_netForwardCode.push_back(m_ind2 + code);
            }
        }
    }
    else
    {
        map<int, QString> rvalueMap;
        for (auto index = 0u; index < info.outputs.size(); index++)
        {
            auto rvalue = "y" + QString::number(info.outIndexs[index] + 1);
            if (info.outCnts[index] > 1)
                rvalue += "[" + QString::number(info.outputs[index].first) + "]";

            rvalueMap[info.outputs[index].second] = rvalue;
        }

        auto code = QString("return [");
        for (auto itor = rvalueMap.begin(); itor != rvalueMap.end(); ++itor)
        {
            if (itor != rvalueMap.begin())
                code += ", ";

            code += itor->second;
        }
        code += "]";
        m_netForwardCode.push_back(m_ind2 + code);
    }

    m_netForwardCode.push_back("");
}

void CCompileMgr::splice()
{
    m_paramCode.insert(m_paramCode.end(), m_paramImportCode.begin(), m_paramImportCode.end());
    m_paramCode.insert(m_paramCode.end(), m_paramClassCode.begin(), m_paramClassCode.end());

    m_netCode.insert(m_netCode.end(), m_netImportCode.begin(), m_netImportCode.end());
    m_netCode.insert(m_netCode.end(), m_netInitCode.begin(), m_netInitCode.end());
    m_netCode.insert(m_netCode.end(), m_netRequiresGradCode.begin(), m_netRequiresGradCode.end());
    m_netCode.insert(m_netCode.end(), m_netForwardCode.begin(), m_netForwardCode.end());
}

void CCompileMgr::GenCodeFinish(FinishInfo const &info)
{
    finishParamImportCode();
    finishParamInitCode();

    finishNetImportCode();
    finishNetInitCode();
    finishNetRequiresGradCode();
    finishNetForwardCode(info);

    splice();
}

void CCompileMgr::GetCode(QString &paramCode, QString &netCode) const
{
    for (auto const &item : m_paramCode)
    {
        paramCode += item + m_return;
    }

    for (auto const &item : m_netCode)
    {
        netCode += item + m_return;
    }
}


void CCompileMgr::initProjectImportCode()
{
    m_projectImportCode.clear();

    if (m_isTest)
    {
        m_projectImportCode.push_back("import argparse");
        m_projectImportCode.push_back("");
    }

    m_projectImportCode.push_back("import os");
    m_projectImportCode.push_back("import importlib, sys");
    m_projectImportCode.push_back("Current_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))");
    m_projectImportCode.push_back("sys.path.append(Current_DIR)");
    m_projectImportCode.push_back("Base_DIR=os.path.abspath(os.path.join(os.getcwd(),\"..\"))");
    m_projectImportCode.push_back("sys.path.append(Base_DIR)");
    m_projectImportCode.push_back("");

    if (m_isTest)
    {
        m_projectImportCode.push_back("import torch");
        m_projectImportCode.push_back("import torchvision.models as models");
        m_projectImportCode.push_back("from torch.autograd import Variable");
        m_projectImportCode.push_back("");
    }
}

void CCompileMgr::initProjectRunCode()
{
    m_projectRunCode.clear();
    m_projectRunCode.push_back("def run():");
}

void CCompileMgr::GenProjectCodeInit(QString const &projectName, bool isTest)
{
    m_isTest = isTest;

    m_projectCode.clear();
    if (m_isTest)
    {
        m_projectCode.push_back("# Test code for " + projectName);
    }
    else
    {
        m_projectCode.push_back("# Train code for " + projectName);
    }
    m_projectCode.push_back("");

    initProjectImportCode();
    initProjectRunCode();
}

void CCompileMgr::GenProjectImportCode(QString const &itemDir, QString const &itemNetId, bool olnyParam)
{
    auto code = "from " + itemDir + "." + itemNetId + "_param import " + itemNetId + "_param";

    for (auto const &item : m_projectImportCode)
        if (item == code)
            return;

    m_projectImportCode.push_back(code);

    if (olnyParam)
        return;

    code = "from " + itemDir + "." + itemNetId + "_net import " + itemNetId + "_net";

    for (auto const &item : m_projectImportCode)
        if (item == code)
            return;

    m_projectImportCode.push_back(code);
}

QString CCompileMgr::indents(int indent) const
{
    return m_ind1;

    QString blanks;

    for (auto i = 0; i < indent; i++)
        blanks += m_ind1;

    return blanks;
}

QString CCompileMgr::notes(int indent) const
{
    QString note = "#";

    for (auto i = 0; i < indent; i++)
    {
        if (i > 0)
            note += "-";

        note += QString::number(i);
    }

    return note;
}

QString CCompileMgr::genRvalue(const int *data, int size) const
{
    QString rvalue = "[";
    for (auto i = 0; i < size; i++)
    {
        if (i > 0)
            rvalue += ", ";

        rvalue += QString::number(data[i]);
    }
    rvalue += "]";

    return rvalue;
}

QString CCompileMgr::genRvalue(const double *data, int size, int prec) const
{
    QString rvalue = "[";
    for (auto i = 0; i < size; i++)
    {
        if (i > 0)
            rvalue += ", ";

        rvalue += QString::number(data[i], 'g', prec);
    }
    rvalue += "]";

    return rvalue;
}

void CCompileMgr::addParamFieldCode(int indent, QString const &param, QString const &field, QString const &rvalue)
{
    auto lvalue = param + "." + field;
    auto code = lvalue + " = " + rvalue;
    m_projectRunCode.push_back(indents(indent) + code);
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
void CCompileMgr::genInitConvParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetConvParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = QString::number(aiParams.inChannels);
    addParamFieldCode(indent, param, "in_channels", rvalue);

    rvalue = QString::number(aiParams.outChannels);
    addParamFieldCode(indent, param, "out_channels", rvalue);

    rvalue = QString::number(aiParams.group);
    addParamFieldCode(indent, param, "group", rvalue);

    rvalue = genRvalue(aiParams.kernelSize, aiParams.dim);
    addParamFieldCode(indent, param, "kernel_size", rvalue);

    rvalue = genRvalue(aiParams.stride, aiParams.dim);
    addParamFieldCode(indent, param, "stride", rvalue);

    rvalue = genRvalue(aiParams.dilation, aiParams.dim);
    addParamFieldCode(indent, param, "dilation", rvalue);

    rvalue = genRvalue(aiParams.padding, aiParams.dim);
    addParamFieldCode(indent, param, "padding", rvalue);

    rvalue = "\'" + QString(aiParams.paddingMode) + "\'";
    addParamFieldCode(indent, param, "padding_mode", rvalue);

    rvalue = QString::number(aiParams.requiresGrad);
    addParamFieldCode(indent, param, "requires_grad", rvalue);
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_dilation": [1, 1, 1],
    "05_padding": [0, 0, 0],
    "06_requiresGrad": 1
}*/
void CCompileMgr::genInitMaxPoolParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetMaxPoolParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = genRvalue(aiParams.kernelSize, aiParams.dim);
    addParamFieldCode(indent, param, "kernel_size", rvalue);

    rvalue = genRvalue(aiParams.stride, aiParams.dim);
    addParamFieldCode(indent, param, "stride", rvalue);

    rvalue = genRvalue(aiParams.dilation, aiParams.dim);
    addParamFieldCode(indent, param, "dilation", rvalue);

    rvalue = genRvalue(aiParams.padding, aiParams.dim);
    addParamFieldCode(indent, param, "padding", rvalue);

    rvalue = QString::number(aiParams.requiresGrad);
    addParamFieldCode(indent, param, "requires_grad", rvalue);
}

/*{
    "01_dim": 2,
    "02_kernelSize": [3, 3, 3],
    "03_stride": [1, 1, 1],
    "04_padding": [0, 0, 0],
    "05_requiresGrad": 1
}*/
void CCompileMgr::genInitAvgPoolParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetAvgPoolParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = genRvalue(aiParams.kernelSize, aiParams.dim);
    addParamFieldCode(indent, param, "kernel_size", rvalue);

    rvalue = genRvalue(aiParams.stride, aiParams.dim);
    addParamFieldCode(indent, param, "stride", rvalue);

    rvalue = genRvalue(aiParams.padding, aiParams.dim);
    addParamFieldCode(indent, param, "padding", rvalue);

    rvalue = QString::number(aiParams.requiresGrad);
    addParamFieldCode(indent, param, "requires_grad", rvalue);
}

/*{
    "01_dim": 2,
    "02_outputSize": [32, 32, 32]
}*/
void CCompileMgr::genInitAdaptiveAvgPoolParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetAdaptiveAvgPoolParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = genRvalue(aiParams.outputSize, aiParams.dim);
    addParamFieldCode(indent, param, "output_size", rvalue);
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
void CCompileMgr::genInitConvTransposeParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetConvTransposeParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = QString::number(aiParams.inChannels);
    addParamFieldCode(indent, param, "in_channels", rvalue);

    rvalue = QString::number(aiParams.outChannels);
    addParamFieldCode(indent, param, "out_channels", rvalue);

    rvalue = QString::number(aiParams.group);
    addParamFieldCode(indent, param, "group", rvalue);

    rvalue = genRvalue(aiParams.kernelSize, aiParams.dim);
    addParamFieldCode(indent, param, "kernel_size", rvalue);

    rvalue = genRvalue(aiParams.stride, aiParams.dim);
    addParamFieldCode(indent, param, "stride", rvalue);

    rvalue = genRvalue(aiParams.dilation, aiParams.dim);
    addParamFieldCode(indent, param, "dilation", rvalue);

    rvalue = genRvalue(aiParams.padding, aiParams.dim);
    addParamFieldCode(indent, param, "padding", rvalue);

    rvalue = genRvalue(aiParams.outputPadding, aiParams.dim);
    addParamFieldCode(indent, param, "output_padding", rvalue);

    rvalue = QString::number(aiParams.requiresGrad);
    addParamFieldCode(indent, param, "requires_grad", rvalue);
}

/*{
    "01_dim": 2,
    "02_scaleFactor": [2.0, 2.0, 2.0],
    "03_mode": "nearest"
}*/
void CCompileMgr::genInitUpSampleParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetUpSampleParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = genRvalue(aiParams.scaleFactor, aiParams.dim);
    addParamFieldCode(indent, param, "scale_factor", rvalue);

    rvalue = "\'" + QString(aiParams.mode) + "\'";
    addParamFieldCode(indent, param, "mode", rvalue);
}

/*{
    "01_inFeatures": 64,
    "02_outFeatures": 64,
    "03_bias": true,
    "04_requiresGrad": 1
}*/
void CCompileMgr::genInitLinearParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetLinearParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.inFeatures);
    addParamFieldCode(indent, param, "in_channels", rvalue);

    rvalue = QString::number(aiParams.outFeatures);
    addParamFieldCode(indent, param, "out_channels", rvalue);

    rvalue = (aiParams.bias ? "True" : "False");
    addParamFieldCode(indent, param, "bias", rvalue);

    rvalue = QString::number(aiParams.requiresGrad);
    addParamFieldCode(indent, param, "requires_grad", rvalue);
}

/*{
    "01_dim": 2,
    "02_prob": 0.5
}*/
void CCompileMgr::genInitDropOutParamsmCode(int indent, QString const &param, QJsonObject const &express)
{
    NetDropOutParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = QString::number(aiParams.prob);
    addParamFieldCode(indent, param, "p", rvalue);
}

/*{
    "01_prob": 0.5
}*/
void CCompileMgr::genInitDropPathParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetDropPathParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.prob);
    addParamFieldCode(indent, param, "drop_prob", rvalue);
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CCompileMgr::genInitBatchNormParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetBatchNormParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = QString::number(aiParams.numFeatures);
    addParamFieldCode(indent, param, "num_features", rvalue);
}

/*{
    "01_numGroups": 2,
    "02_numChannels": 64
}*/
void CCompileMgr::genInitGroupNormParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetGroupNormParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.numGroups);
    addParamFieldCode(indent, param, "num_groups", rvalue);

    rvalue = QString::number(aiParams.numChannels);
    addParamFieldCode(indent, param, "num_channels", rvalue);
}

/*{
    "01_dim": 2,
    "02_numFeatures": 64
}*/
void CCompileMgr::genInitInstanceNormParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetInstanceNormParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);

    rvalue = QString::number(aiParams.numFeatures);
    addParamFieldCode(indent, param, "num_features", rvalue);
}

/*{
    "01_normalizedShape": [0, 1, 3, 2]
}*/
void CCompileMgr::genInitLayerNormParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetLayerNormParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = genRvalue(aiParams.normalizedShape, 4);
    addParamFieldCode(indent, param, "normalized_shape", rvalue);
}

/*{
    "01_function": "SiLU"
}*/
void CCompileMgr::genInitActiveParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetActiveParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = "\'" + QString(aiParams.function) + "\'";
    addParamFieldCode(indent, param, "function", rvalue);
}

/*{
    "01_softMaxDim": 2
}*/
void CCompileMgr::genInitSoftMaxParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetSoftMaxParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.softMaxDim);
    addParamFieldCode(indent, param, "softmaxdim", rvalue);
}

/*{
    "01_startDim": 1,
    "02_endDim": -1
}*/
void CCompileMgr::genInitFlattenParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetFlattenParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.startDim);
    addParamFieldCode(indent, param, "start_dim", rvalue);

    rvalue = QString::number(aiParams.endDim);
    addParamFieldCode(indent, param, "end_dim", rvalue);
}

/*{
    "01_groups": 2
}*/
void CCompileMgr::genInitChannelShuffleParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetChannelShuffleParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.groups);
    addParamFieldCode(indent, param, "groups", rvalue);
}

/*{
    "01_numberClass": 2,
    "02_anchorSize": [[210, 210], [300, 400], [300, 400]],
    "03_imageSize": 416
}*/
void CCompileMgr::genInittDetectLayer2DParamsmCode(int indent, QString const &param, QJsonObject const &express)
{
    NetDetectLayer2DParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.numberClass);
    addParamFieldCode(indent, param, "number_class", rvalue);

    rvalue = genRvalue<3, 2>(aiParams.anchorSize);
    addParamFieldCode(indent, param, "anchor_size", rvalue);

    rvalue = QString::number(aiParams.imageSize);
    addParamFieldCode(indent, param, "image_size", rvalue);
}

/*{
    "01_catDim": 0
}*/
void CCompileMgr::genInitCatParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetCatParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.catDim);
    addParamFieldCode(indent, param, "catdim", rvalue);
}


// 计算单元: calunit
/*{

}*/
void CCompileMgr::genInitScaledParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}

/*{

}*/
void CCompileMgr::genInitAddParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}

/*{
    "01_input": 0
}*/
void CCompileMgr::genInitConstParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetConstParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.input);
    addParamFieldCode(indent, param, "input", rvalue);
}

/*{
    "01_swapDims": [0, 1, 3, 2]
}*/
void CCompileMgr::genInitPermuteParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetPermuteParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = genRvalue(aiParams.swapDims, 4);
    addParamFieldCode(indent, param, "swapdims", rvalue);
}

/*{
    "01_changedims": 0
}*/
void CCompileMgr::genInitViewParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetViewParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.changeDims);
    addParamFieldCode(indent, param, "changedims", rvalue);
}

/*{
    "01_splitsize": 2
    "02_dim": 0
}*/
void CCompileMgr::genInitSplitParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetSplitParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.splitSize);
    addParamFieldCode(indent, param, "splitsize", rvalue);

    rvalue = QString::number(aiParams.dim);
    addParamFieldCode(indent, param, "dim", rvalue);
}

/*{

}*/
void CCompileMgr::genInitIdentityParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}


// 运行单元: rununit
/*{

}*/
void CCompileMgr::genInitShowResultParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}

/*{
    "01_function": "CrossEntropyLoss"
    "02_p": 1.0
}*/
void CCompileMgr::genInitBaseLossParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetBaseLossParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = "\'" + QString(aiParams.function) + "\'";
    addParamFieldCode(indent, param, "function", rvalue);

    rvalue = QString::number(aiParams.p);
    addParamFieldCode(indent, param, "p", rvalue);
}

/*{

}*/
void CCompileMgr::genInitDiceLossParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}

/*{

}*/
void CCompileMgr::genInitDetectYoloLossParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}

/*{
    "01_lr": 1.0,
    "02_rho": 0.9,
    "03_weightDecay": 0.0,
    "04_eps": 1.00E-06
}*/
void CCompileMgr::genInitOptAdaDeltaParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetOptAdaDeltaParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.lr);
    addParamFieldCode(indent, param, "lr", rvalue);

    rvalue = QString::number(aiParams.rho);
    addParamFieldCode(indent, param, "rho", rvalue);

    rvalue = QString::number(aiParams.weightDecay);
    addParamFieldCode(indent, param, "weight_decay", rvalue);

    rvalue = QString::number(aiParams.eps, 'g', 9);
    addParamFieldCode(indent, param, "eps", rvalue);
}

/*{
    "01_lr": 0.01,
    "02_lrDecay": 0.0,
    "03_weightDecay": 0.0,
    "04_initialAccumulatorValue": 0.0,
    "05_eps": 1.00E-06
}*/
void CCompileMgr::genInitOptAdaGradParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetOptAdaGradParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.lr);
    addParamFieldCode(indent, param, "lr", rvalue);

    rvalue = QString::number(aiParams.lrDecay);
    addParamFieldCode(indent, param, "lr_decay", rvalue);

    rvalue = QString::number(aiParams.weightDecay);
    addParamFieldCode(indent, param, "weight_decay", rvalue);

    rvalue = QString::number(aiParams.initialAccumulatorValue);
    addParamFieldCode(indent, param, "initial_accumulator_value", rvalue);

    rvalue = QString::number(aiParams.eps, 'g', 9);
    addParamFieldCode(indent, param, "eps", rvalue);
}

/*{
    "01_lr": 0.01,
    "02_eps": 1.00E-06,
    "03_weightDecay": 0.0
}*/
void CCompileMgr::genInitOptAdamParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetOptAdamParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.lr);
    addParamFieldCode(indent, param, "lr", rvalue);

    rvalue = QString::number(aiParams.eps, 'g', 9);
    addParamFieldCode(indent, param, "eps", rvalue);

    rvalue = QString::number(aiParams.weightDecay);
    addParamFieldCode(indent, param, "weight_decay", rvalue);
}

/*{

}*/
void CCompileMgr::genInitOptASGDParamsCode(int indent, QString const &param, QJsonObject const &express)
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
void CCompileMgr::genInitOptRMSpropParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetOptRMSpropParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.lr);
    addParamFieldCode(indent, param, "lr", rvalue);

    rvalue = QString::number(aiParams.alpha);
    addParamFieldCode(indent, param, "alpha", rvalue);

    rvalue = QString::number(aiParams.eps, 'g', 9);
    addParamFieldCode(indent, param, "eps", rvalue);

    rvalue = QString::number(aiParams.weightDecay);
    addParamFieldCode(indent, param, "weight_decay", rvalue);

    rvalue = QString::number(aiParams.momentum);
    addParamFieldCode(indent, param, "momentum", rvalue);
}

/*{
    "01_lr": 0.01,
    "02_momentum": 0.0,
    "03_dampening": 0.0,
    "04_weightDecay": 0.0
}*/
void CCompileMgr::genInitOptSGDParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetOptSGDParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.lr);
    addParamFieldCode(indent, param, "lr", rvalue);

    rvalue = QString::number(aiParams.momentum);
    addParamFieldCode(indent, param, "momentum", rvalue);

    rvalue = QString::number(aiParams.dampening);
    addParamFieldCode(indent, param, "dampening", rvalue);

    rvalue = QString::number(aiParams.weightDecay);
    addParamFieldCode(indent, param, "weight_decay", rvalue);
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
void CCompileMgr::genInitTrain2DTypeCode(int indent, QString const &param, QJsonObject const &express)
{
    NetTrain2DParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = "\'" + QString(aiParams.savePthPath) + "\'";
    addParamFieldCode(indent, param, "save_pth_path", rvalue);

    rvalue = "\'" + QString(aiParams.preweightsPath) + "\'";
    addParamFieldCode(indent, param, "preweights_path", rvalue);

    rvalue = "\'" + QString(aiParams.logDir) + "\'";
    addParamFieldCode(indent, param, "log_dir", rvalue);

    rvalue = QString::number(aiParams.epochNum);
    addParamFieldCode(indent, param, "epoch_num", rvalue);

    rvalue = QString::number(aiParams.batchSize);
    addParamFieldCode(indent, param, "batch_size", rvalue);

    rvalue = (aiParams.isCuda ? "True" : "False");
    addParamFieldCode(indent, param, "cuda", rvalue);

    rvalue = genRvalue(aiParams.classNums, 4);
    addParamFieldCode(indent, param, "class_nums", rvalue);

    rvalue = "\'" + QString(aiParams.criteriaFunction) + "\'";
    addParamFieldCode(indent, param, "criteria_function", rvalue);

    rvalue = (aiParams.isTrainDataCriteria ? "True" : "False");
    addParamFieldCode(indent, param, "is_train_data_criteria", rvalue);

    rvalue = (aiParams.showLossFigure ? "True" : "False");
    addParamFieldCode(indent, param, "show_loss_figure", rvalue);

    rvalue = (aiParams.showCriteriaFigure ? "True" : "False");
    addParamFieldCode(indent, param, "show_criteria_figure", rvalue);

    rvalue = QString::number(aiParams.epochSaveModel);
    addParamFieldCode(indent, param, "epoch_save_model", rvalue);
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
void CCompileMgr::genInitDataLoader2DTypeCode(int indent, QString const &param, QJsonObject const &express)
{
    NetDataLoader2DParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.inputW);
    addParamFieldCode(indent, param, "input_w", rvalue);

    rvalue = QString::number(aiParams.inputH);
    addParamFieldCode(indent, param, "input_h", rvalue);

    rvalue = "\'" + QString(aiParams.dataJsonPath) + "\'";
    addParamFieldCode(indent, param, "data_json_path", rvalue);

    rvalue = "\'" + QString(aiParams.dataLabelCodeJsonPath) + "\'";
    addParamFieldCode(indent, param, "data_label_code_json_path", rvalue);

    rvalue = (aiParams.isRgb ? "True" : "False");
    addParamFieldCode(indent, param, "rgb", rvalue);

    rvalue = QString::number(aiParams.batchSize);
    addParamFieldCode(indent, param, "batch_size", rvalue);

    rvalue = QString::number(aiParams.numWorkers);
    addParamFieldCode(indent, param, "num_workers", rvalue);

    rvalue = (aiParams.shuffle ? "True" : "False");
    addParamFieldCode(indent, param, "shuffle", rvalue);
}

/*{
    "01_inputDim": 0
}*/
void CCompileMgr::genInitDataInputTestParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    NetDataInputTestParams aiParams;
    CAttrViewMgr::ins().decode(express, aiParams);

    auto rvalue = QString::number(aiParams.inputDim);
    addParamFieldCode(indent, param, "input_dim", rvalue);
}

/*{

}*/
void CCompileMgr::genInitDataOutputParamsCode(int indent, QString const &param, QJsonObject const &express)
{
    // no-op
}


void CCompileMgr::genInitStdParamCode(int indent, QString const &netId, int sn, QJsonObject const &express)
{
    auto param = "param_" + netId.toLower() + "_" + QString::number(sn);

    // 网络单元: netunit
    if (netId == "conv")
    {
        genInitConvParamsCode(indent, param, express);
    }
    else if (netId == "maxpool")
    {
        genInitMaxPoolParamsCode(indent, param, express);
    }
    else if (netId == "avgpool")
    {
        genInitAvgPoolParamsCode(indent, param, express);
    }
    else if (netId == "adaptiveavgpool")
    {
        genInitAdaptiveAvgPoolParamsCode(indent, param, express);
    }
    else if (netId == "convtranspose")
    {
        genInitConvTransposeParamsCode(indent, param, express);
    }
    else if (netId == "upsample")
    {
        genInitUpSampleParamsCode(indent, param, express);
    }
    else if (netId == "linear")
    {
        genInitLinearParamsCode(indent, param, express);
    }
    else if (netId == "dropout")
    {
        genInitDropOutParamsmCode(indent, param, express);
    }
    else if (netId == "droppath")
    {
        genInitDropPathParamsCode(indent, param, express);
    }
    else if (netId == "batchnorm")
    {
        genInitBatchNormParamsCode(indent, param, express);
    }
    else if (netId == "groupnorm")
    {
        genInitGroupNormParamsCode(indent, param, express);
    }
    else if (netId == "instancenorm")
    {
        genInitInstanceNormParamsCode(indent, param, express);
    }
    else if (netId == "layernorm")
    {
        genInitLayerNormParamsCode(indent, param, express);
    }
    else if (netId == "active")
    {
        genInitActiveParamsCode(indent, param, express);
    }
    else if (netId == "softmax")
    {
        genInitSoftMaxParamsCode(indent, param, express);
    }
    else if (netId == "flatten")
    {
        genInitFlattenParamsCode(indent, param, express);
    }
    else if (netId == "channelshuffle")
    {
        genInitChannelShuffleParamsCode(indent, param, express);
    }
    else if (netId == "detectlayer2d")
    {
        genInittDetectLayer2DParamsmCode(indent, param, express);
    }
    else if (netId == "cat")
    {
        genInitCatParamsCode(indent, param, express);
    }

    // 计算单元: calunit
    else if (netId == "scaled")
    {
        genInitScaledParamsCode(indent, param, express);
    }
    else if (netId == "add")
    {
        genInitAddParamsCode(indent, param, express);
    }
    else if (netId == "const")
    {
        genInitConstParamsCode(indent, param, express);
    }
    else if (netId == "permute")
    {
        genInitPermuteParamsCode(indent, param, express);
    }
    else if (netId == "view")
    {
        genInitViewParamsCode(indent, param, express);
    }
    else if (netId == "split")
    {
        genInitSplitParamsCode(indent, param, express);
    }
    else if (netId == "identity")
    {
        genInitIdentityParamsCode(indent, param, express);
    }

    // 运行单元: rununit
    else if (netId == "showresult")
    {
        genInitShowResultParamsCode(indent, param, express);
    }
    else if (netId == "loss")
    {
        genInitBaseLossParamsCode(indent, param, express);
    }
    else if (netId == "diceloss")
    {
        genInitDiceLossParamsCode(indent, param, express);
    }
    else if (netId == "detectyololoss")
    {
        genInitDetectYoloLossParamsCode(indent, param, express);
    }
    else if (netId == "optadadelta")
    {
        genInitOptAdaDeltaParamsCode(indent, param, express);
    }
    else if (netId == "optadagrad")
    {
        genInitOptAdaGradParamsCode(indent, param, express);
    }
    else if (netId == "optadam")
    {
        genInitOptAdamParamsCode(indent, param, express);
    }
    else if (netId == "optasgd")
    {
        genInitOptASGDParamsCode(indent, param, express);
    }
    else if (netId == "optrmsprop")
    {
        genInitOptRMSpropParamsCode(indent, param, express);
    }
    else if (netId == "optsgd")
    {
        genInitOptSGDParamsCode(indent, param, express);
    }
    else if (netId == "train2d")
    {
        genInitTrain2DTypeCode(indent, param, express);
    }
    else if (netId == "dataloader2d")
    {
        genInitDataLoader2DTypeCode(indent, param, express);
    }
    else if (netId == "datainputtest")
    {
        genInitDataInputTestParamsCode(indent, param, express);
    }
    else if (netId == "dataoutput")
    {
        genInitDataOutputParamsCode(indent, param, express);
    }
    else
    {
        // error
    }
}

void CCompileMgr::genInitParamCode(int indent, QJsonObject const &aiParams)
{
    if (!aiParams.contains("01_net") || !aiParams.value("01_net").isString())
        return;
    auto netId = aiParams.value("01_net").toString();

    if (!aiParams.contains("02_sn") || !aiParams.value("02_sn").isDouble())
        return;
    auto sn = aiParams.value("02_sn").toInt();

    if (!aiParams.contains("03_express") || !aiParams.value("03_express").isObject())
        return;
    auto express = aiParams.value("03_express").toObject();

    if (aiParams.contains("04_logic"))
    {
        if (!express.contains("01_cycle") || !express.value("01_cycle").isDouble())
            return;
        auto cycle = express.value("01_cycle").toInt();

        if (!express.contains("02_reqGrad") || !express.value("02_reqGrad").isDouble())
            return;
        auto requiresGrad = express.value("02_reqGrad").toInt();

        // def-init
        QString dir;
        emit getNetDir(netId, dir);
        GenProjectImportCode(dir, netId, true);

        m_projectRunCode.push_back("");
        m_projectRunCode.push_back(indents(indent) + notes(indent));
        auto lvalue0 = "param_" + netId.toLower() + "_" + QString::number(sn);
        auto rvalue = netId + "_param()";
        auto code = lvalue0 + " = " + rvalue;
        m_projectRunCode.push_back(indents(indent) + code);

        auto lvalue = lvalue0 + ".requires_grad";
        rvalue = QString::number(requiresGrad);
        code = lvalue + " = " + rvalue;
        m_projectRunCode.push_back(indents(indent) + code);

        lvalue = lvalue0 + ".repetnum";
        rvalue = QString::number(cycle);
        code = lvalue + " = " + rvalue;
        m_projectRunCode.push_back(indents(indent) + code);

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

            auto jsObjItem = valRef.toObject();
            genInitParamCode(indent + 1, jsObjItem);

            if (!jsObjItem.contains("01_net") || !jsObjItem.value("01_net").isString())
                continue;
            auto itemNetId = jsObjItem.value("01_net").toString();

            if (!jsObjItem.contains("02_sn") || !jsObjItem.value("02_sn").isDouble())
                continue;
            auto itemSn = jsObjItem.value("02_sn").toInt();

            lvalue = lvalue0 + "." + itemNetId.toLower() + "_" + QString::number(itemSn);
            rvalue = "param_" + itemNetId.toLower() + "_" + QString::number(itemSn);
            code = lvalue + " = " + rvalue;
            m_projectRunCode.push_back(indents(indent) + code);

            if (index + 1 == aiParamItems.size())
            {
                m_projectRunCode.push_back("");
                m_projectRunCode.push_back(indents(indent > 1 ? --indent : indent) + notes(indent));
            }
        }
    }
    else
    {
        if (indent > 1)
            indent--;

        // def-init
        QString dir;
        emit getNetDir(netId, dir);
        GenProjectImportCode(dir, netId, true);

        m_projectRunCode.push_back("");
        m_projectRunCode.push_back(indents(indent) + notes(indent));
        auto lvalue = "param_" + netId.toLower() + "_" + QString::number(sn);
        auto rvalue = netId + "_param()";
        auto code = lvalue + " = " + rvalue;
        m_projectRunCode.push_back(indents(indent) + code);

        genInitStdParamCode(indent, netId, sn, express);
    }
}

void CCompileMgr::GenProjectRunInitParamCode(QString const &netId, int sn, QJsonObject const &aiParams)
{
    genInitParamCode(1, aiParams);

    auto rvalue = "param_" + netId.toLower() + "_" + QString::number(sn);
    auto lvalue = "xtxoxpxixnxrxuxnx_" + rvalue;
    auto code = lvalue + " = " + rvalue;
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::GenProjectRunInitNetCode(QString const &netId, int sn)
{
    if (m_isTest)
    {

    }
    else
    {
        auto idsn = netId.toLower() + "_" + QString::number(sn);
        auto param = "xtxoxpxixnxrxuxnx_param_" + idsn;
        auto rvalue = netId + "_net(" + param + ")";
        auto lvalue = "xtxoxpxixnxrxuxnx_net_" + idsn;
        auto code = lvalue + " = " + rvalue;
        m_projectRunCode.push_back(m_ind1 + code);
    }
}

void CCompileMgr::GenProjectRunCallDataloaderCode(QString const &netId, int sn)
{
    auto idsn = netId.toLower() + "_" + QString::number(sn);
    auto lvalue = "xtxoxpxixnxrxuxnx_net_" + idsn;
    auto code = lvalue + " = " + lvalue + "()";
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::GenProjectRunDefineLossFunctionsCode()
{
    auto code = "xtxoxpxixnxrxuxnx_loss_functions = []";
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::GenProjectRunAddLossFunctionCode(QString const &netId, int sn)
{
    auto idsn = netId.toLower() + "_" + QString::number(sn);
    auto function = "xtxoxpxixnxrxuxnx_net_" + idsn;
    auto code = "xtxoxpxixnxrxuxnx_loss_functions.append(" + function + ")";
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::GenProjectRunInitLossInputCode(LossInfo const &info)
{
    m_projectRunCode.push_back("");

    auto rvalue = genRvalue(info.netInputs.data(), (int)info.netInputs.size());
    auto lvalue = QString("xtxoxpxixnxrxuxnx_loss_net_input");
    auto code = lvalue + " = " + rvalue;
    m_projectRunCode.push_back(m_ind1 + code);

    rvalue = genRvalue(info.dataInputs.data(), (int)info.dataInputs.size());
    lvalue = QString("xtxoxpxixnxrxuxnx_loss_data_input");
    code = lvalue + " = " + rvalue;
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::GenProjectRunTrainFunctionCode(std::pair<QString, int> const &dataLoaderInfo, std::pair<QString, int> const &unetInfo, std::pair<QString, int> const &optInfo, std::pair<QString, int> const &trainInfo)
{
    auto idsn = dataLoaderInfo.first.toLower() + "_" + QString::number(dataLoaderInfo.second);
    auto dataLoader = "xtxoxpxixnxrxuxnx_net_" + idsn;

    idsn = unetInfo.first.toLower() + "_" + QString::number(unetInfo.second);
    auto unet = "xtxoxpxixnxrxuxnx_net_" + idsn;

    idsn = optInfo.first.toLower() + "_" + QString::number(optInfo.second);
    auto optimizer = "xtxoxpxixnxrxuxnx_net_" + idsn;

    idsn = trainInfo.first.toLower() + "_" + QString::number(trainInfo.second);
    auto train = "xtxoxpxixnxrxuxnx_net_" + idsn;

    auto lossFunctions = QString("xtxoxpxixnxrxuxnx_loss_functions");
    auto lossNetInput = QString("xtxoxpxixnxrxuxnx_loss_net_input");
    auto lossDataInput = QString("xtxoxpxixnxrxuxnx_loss_data_input");

    auto code = train + "(";
    code += dataLoader + ", ";
    code += "None, ";
    code += lossFunctions + ", ";
    code += lossNetInput + ", ";
    code += lossDataInput + ", ";
    code += optimizer + ", ";
    code += unet + ")";
    m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::AddProjectRunCode(QString const &code, int n)
{
    for (auto i = 0; i < n; i++)
        m_projectRunCode.push_back(m_ind1 + code);
}

void CCompileMgr::finishProjectImportCode()
{
    m_projectImportCode.push_back("");

    if (m_isTest)
    {
        m_projectImportCode.push_back("import cv2");
    }
    else
    {
        m_projectImportCode.push_back("import torch");
    }

    m_projectImportCode.push_back("");
}

void CCompileMgr::finishProjectRunCode()
{
    m_projectRunCode.push_back("");
    m_projectRunCode.push_back("if __name__ == \"__main__\":");
    m_projectRunCode.push_back(m_ind1 + "run()");
    m_projectRunCode.push_back("");
}

void CCompileMgr::projectSplice()
{
    m_projectCode.insert(m_projectCode.end(), m_projectImportCode.begin(), m_projectImportCode.end());
    m_projectCode.insert(m_projectCode.end(), m_projectRunCode.begin(), m_projectRunCode.end());
}

void CCompileMgr::GenProjectCodeFinish()
{
    finishProjectImportCode();
    finishProjectRunCode();

    projectSplice();
}

void CCompileMgr::GetProjectCode(QString &projectCode) const
{
    for (auto const &item : m_projectCode)
    {
        projectCode += item + m_return;
    }
}
