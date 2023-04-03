#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <vector>

using std::pair;
using std::vector;
using ChannelPairs_t = vector<pair<int, int>>;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsItem;

class CSystemTray;

class CDialogNew;
class CDialogGen;
class CDialogAbout;
class CDialogMessageInfo;
class CDialogTextEdit;
class CDialogParamEdit;
class CDialogNumberEdit;
class CDialogInputConfig;
class CDialogOutputConfig;
class CDialogAiParamsView;

class CMenuText;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    enum EPosType
    {
        eInside = 0x00,
        eTop = 0x01,
        eBottom = 0x02,
        eLeft = 0x04,
        eRight = 0x08,
        eTopLeft = 0x01 | 0x04,
        eTopRight = 0x01 | 0x08,
        eBottomLeft = 0x02 | 0x04,
        eBottomRight = 0x02 | 0x08,
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void mousePressEvent(QMouseEvent*event) override;
    void mouseMoveEvent(QMouseEvent*event) override;
    void mouseReleaseEvent(QMouseEvent*event) override;

private slots:
    void on_btnMin_clicked();
    void on_btnMaxOrNorm_clicked();
    void on_btnClose_clicked();
    void on_winChangeSlot(bool isMaximized);
    void on_appQuitSlot();//

    void on_tbtnNew_clicked();
    void on_tbtnEdit_clicked();//
    void on_tbtnClear_clicked();
    void on_tbtnCompile_clicked();
    void on_tbtnGen_clicked();
    void on_tbtnRun_clicked();//
    void on_tbtnTop_clicked();
    void on_tbtnAbout_clicked();

    void on_tbtnDesign_clicked();
    void on_tbtnProject_clicked();

    void textCopyAvailableSlot(bool bAvailable);
    void showOutputBrowserMenuSlot(const QPoint &);
    void copyTextSlot();
    void clearTextSlot();
    void selectAllTextSlot();

    void resizeSlot();

    void showInfoSlot(QString const &info);
    void showMsgDialogSlot(QString const &msg);

    void useNetSlot(bool std, QString const &catalogName, QString const &netId);
    void addNetSlot(bool std, QString const &catalogName, QString const &netId, QColor const &color);

    void SelectedProjectSlot(QString const &filePath);

    void netChangedSlot();
    void editNetNameSlot(QString const &netId, QPoint const &pos);
    void editTreeParamSlot(int sn, QString const &netId, QString const &netName, QString const &field, QString const &value, vector<QString> const &params);
    void editChannelSlot(int min, int max, int channel, QPoint const &pos);
    void inputConfigSlot(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt);
    void outputConfigSlot(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt);

    void enableUpBtnSlot(bool enabled);

    void netNameChangeSlot(QString const &netId);
    void genConfirmSlot();

    void projectNameChangeSlot(QString const &catalog, QString const &name, bool &usable);
    void newConfirmSlot();

    void clearAttrSlot();
    void showAttrSlot(bool show, int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams);

private:
    void init();
    void initBefore();
    void initMainWindow();
    void initSystemTray();
    void initDirFileMgr();
    void initAttrViewMgr();
    void initNetViewMgr();
    void initListViewMgr();
    void initCompileMgr();
    void initDesignMgr();
    void initProjectMgr();
    void initOutput();
    void initDialog();
    void initAfter();

    void updateCursorShape();
    void updateFrameGeometry(QPoint const &pos);

    void disableAllTextAction();

    void clearAll();

    void loadWorkFile();
    void loadProjectFile(QString const &filePath);

    void updateAiParamsView();

    bool compileCheck(bool showTip);

private:
    Ui::MainWindow*                     ui;

    bool                                m_move = {false};
    int                                 m_dis = {4};
    EPosType                            m_posType = {eInside};
    QPoint                              m_dPos;

    QString                             m_menuQss;

    CSystemTray*                        m_pSystemTray = {nullptr};

    CDialogAiParamsView*                m_pDlgAiParamsView = {nullptr};
    CDialogInputConfig*                 m_pDlgInputConfig = {nullptr};
    CDialogOutputConfig*                m_pDlgOutputConfig = {nullptr};
    CDialogTextEdit*                    m_pDlgTextEdit = {nullptr};
    CDialogParamEdit*                   m_pDlgParamEdit = {nullptr};
    CDialogNumberEdit*                  m_pDlgNumberEdit = {nullptr};
    CDialogMessageInfo*                 m_pDlgMessageInfo = {nullptr};
    CDialogGen*                         m_pDlgGen = {nullptr};
    CDialogNew*                         m_pDlgNew = {nullptr};
    CDialogAbout*                       m_pDlgAbout = {nullptr};

    CMenuText*                          m_pMenuText = {nullptr};

    int                                 m_nRunTimerID = {0};
    int                                 m_nUpdateTimerID = {0};

    bool                                m_bDesign = {true};
    bool                                m_isTest = {false};
};
#endif // MAINWINDOW_H
