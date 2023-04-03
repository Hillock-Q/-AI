#ifndef COMPILEMGR_H
#define COMPILEMGR_H

#include <QObject>
#include <vector>
#include <QString>

using std::pair;
using std::vector;
using ChannelPairs_t = vector<pair<int, int>>;

class QJsonObject;

struct ForwardInfo
{
    QString             netId;
    int                 sn;
    int                 index;
    int                 inCnt;
    int                 inputCnt;
    vector<int>         inChannels;
    ChannelPairs_t      inputs;

    vector<int>         outCnts;
    vector<int>         outIndexs;
    vector<int>         outChannels;
};

struct FinishInfo
{
    int                 outputCnt;
    ChannelPairs_t      outputs;
    vector<int>         outCnts;
    vector<int>         outIndexs;
};

struct LossInfo
{
    vector<int>         netInputs;
    vector<int>         dataInputs;
};

class CCompileMgr: public QObject
{
    Q_OBJECT

public:
    static CCompileMgr& ins();

    void GenCodeInit(QString const &catalog, QString const &netId, QString const &outline, QString const &specify);

    void GenParamImportCode(QString const &itemDir, QString const &itemNetId);
    void GenParamInitCode(int index, QString const &itemNetId, int itemSn);

    void GenNetImportCode(QString const &itemDir, QString const &itemNetId);
    void GenNetInitCode(int index, QString const &netId, QString const &itemNetId, int itemSn);
    void GenNetForwardCode(ForwardInfo const &info);

    void GenCodeFinish(FinishInfo const &info);

    void GetCode(QString &paramCode, QString &netCode) const;


    void GenProjectCodeInit(QString const &projectName, bool isTest);

    void GenProjectImportCode(QString const &itemDir, QString const &itemNetId, bool olnyParam);
    void GenProjectRunInitParamCode(QString const &netId, int sn, QJsonObject const &aiParams);
    void GenProjectRunInitNetCode(QString const &netId, int sn);
    void GenProjectRunCallDataloaderCode(QString const &netId, int sn);
    void GenProjectRunDefineLossFunctionsCode();
    void GenProjectRunAddLossFunctionCode(QString const &netId, int sn);
    void GenProjectRunInitLossInputCode(LossInfo const &info);
    void GenProjectRunTrainFunctionCode(std::pair<QString, int> const &dataLoaderInfo, std::pair<QString, int> const &unetInfo, std::pair<QString, int> const &optInfo, std::pair<QString, int> const &trainInfo);
    void AddProjectRunCode(QString const &code, int n = 1);

    void GenProjectCodeFinish();

    void GetProjectCode(QString &projectCode) const;

signals:
    void showInfoSignal(QString const &info) const;
    void getNetDir(QString const &itemNetId, QString &itemDir);

private:
    CCompileMgr();
    ~CCompileMgr();

    CCompileMgr(CCompileMgr &&rhs) = delete;
    CCompileMgr(CCompileMgr const &rhs) = delete;

    CCompileMgr& operator=(CCompileMgr &&rhs) = delete;
    CCompileMgr& operator=(CCompileMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CCompileMgr *pt = 0) : p(pt){}
        CCompileMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CCompileMgr *p;
    };

    void initParamImportCode();
    void initParamClassInitCode(QString const &netId);
    void initNetImportCode();
    void initNetClassInitCode(QString const &netId);
    void initNetRequiresGradCode();
    void initNetForwardCode();

    void finishParamImportCode();
    void finishParamInitCode();
    void finishNetImportCode();
    void finishNetInitCode();
    void finishNetRequiresGradCode();
    void finishNetForwardCode(FinishInfo const &info);

    void splice();


    void initProjectImportCode();
    void initProjectRunCode();

    QString indents(int indent) const;
    QString notes(int indent) const;
    QString genRvalue(const int *data, int size) const;

    template<size_t R, size_t C>
    QString genRvalue(const int (&data)[R][C])
    {
        QString rvalue = "[";
        for (auto row = 0; row < R; row++)
        {
            if (row > 0)
                rvalue += ", ";

            rvalue += "[";
            for (auto col = 0; col < C; col++)
            {
                if (col > 0)
                    rvalue += ", ";

                rvalue += QString::number(data[row][col]);
            }
            rvalue += "]";
        }
        rvalue += "]";

        return rvalue;
    }

    QString genRvalue(const double *data, int size, int prec = 6) const;
    void addParamFieldCode(int indent, QString const &param, QString const &field, QString const &rvalue);

    // 网络单元: netunit
    void genInitConvParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitMaxPoolParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitAvgPoolParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitAdaptiveAvgPoolParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitConvTransposeParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitUpSampleParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitLinearParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDropOutParamsmCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDropPathParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitBatchNormParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitGroupNormParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitInstanceNormParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitLayerNormParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitActiveParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitSoftMaxParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitFlattenParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitChannelShuffleParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInittDetectLayer2DParamsmCode(int indent, QString const &param, QJsonObject const &express);
    void genInitCatParamsCode(int indent, QString const &param, QJsonObject const &express);

    // 计算单元: calunit
    void genInitScaledParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitAddParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitConstParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitPermuteParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitViewParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitSplitParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitIdentityParamsCode(int indent, QString const &param, QJsonObject const &express);

    // 运行单元: rununit
    void genInitShowResultParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitBaseLossParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDiceLossParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDetectYoloLossParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptAdaDeltaParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptAdaGradParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptAdamParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptASGDParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptRMSpropParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitOptSGDParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitTrain2DTypeCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDataLoader2DTypeCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDataInputTestParamsCode(int indent, QString const &param, QJsonObject const &express);
    void genInitDataOutputParamsCode(int indent, QString const &param, QJsonObject const &express);

    void genInitStdParamCode(int indent, QString const &netId, int sn, QJsonObject const &express);
    void genInitParamCode(int indent, QJsonObject const &aiParams);

    void finishProjectImportCode();
    void finishProjectRunCode();

    void projectSplice();

private:
    vector<QString>                     m_paramCode;
    vector<QString>                     m_paramImportCode;
    vector<QString>                     m_paramClassCode;

    vector<QString>                     m_netCode;
    vector<QString>                     m_netImportCode;
    vector<QString>                     m_netInitCode;
    vector<QString>                     m_netRequiresGradCode;
    vector<QString>                     m_netForwardCode;

    bool                                m_isTest;
    vector<QString>                     m_projectCode;
    vector<QString>                     m_projectImportCode;
    vector<QString>                     m_projectRunCode;

    QString                             m_ind1 = {"    "};
    QString                             m_ind2 = m_ind1 + m_ind1;
    QString                             m_ind3 = m_ind2 + m_ind1;
    QString                             m_ind4 = m_ind3 + m_ind1;
    QString                             m_return = {"\n"};
};

#endif // COMPILEMGR_H
