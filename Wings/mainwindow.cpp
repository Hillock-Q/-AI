#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "SystemTray.h"
#include "GraphicsScene.h"

#include "DialogAiParamsView.h"
#include "DialogInputConfig.h"
#include "DialogOutputConfig.h"
#include "DialogTextEdit.h"
#include "DialogParamEdit.h"
#include "DialogNumberEdit.h"
#include "DialogMessageInfo.h"
#include "DialogNew.h"
#include "DialogGen.h"
#include "DialogAbout.h"

#include "MenuText.h"

#include "DirFileMgr.h"
#include "ListViewMgr.h"
#include "NetViewMgr.h"
#include "AttrViewMgr.h"
#include "DesignMgr.h"
#include "ProjectMgr.h"
#include "CompileMgr.h"
#include "RunMgr.h"

#define VERSION_INFO                    "Version: 1.23.01.01"
#define RUN_TIME_IN_SECONDS             (8 * (60/60.0) * 3600)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

MainWindow::~MainWindow()
{
    killTimer(m_nRunTimerID);

    delete m_pSystemTray;
    delete ui;
}

void MainWindow::init()
{
    initBefore();
    initMainWindow();
    initSystemTray();
    initDirFileMgr();
    initAttrViewMgr();
    initNetViewMgr();
    initListViewMgr();
    initCompileMgr();
    initDesignMgr();
    initProjectMgr();
    initOutput();
    initDialog();
    initAfter();
}

void MainWindow::initBefore()
{
    m_nRunTimerID = startTimer(RUN_TIME_IN_SECONDS * 1000);
}

void MainWindow::initMainWindow()
{
    m_menuQss = "QMenu{"
                "   background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0.999955, y2:0.983, stop:0 rgba(255, 255, 255, 255), stop:1 rgba(188, 220, 250, 255));"
                "   border: 2px solid rgb(171,189,212);"
                "   border-radius: 3px;"
                "}"
                "QMenu::item{"
                "   padding: 11px 32px;"
                "   color: rgb(35, 62, 95);"
                "}"
                "QMenu::item:!enabled{"
                "   color: rgb(192, 192, 192);"
                "}"
                "QMenu::item:selected{"
                "   background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0.999955, y2:0.983, stop:0 rgba(255, 201, 14, 255), stop:1 rgba(250, 128, 0, 255));"
                "}";

    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(300, 100);

    ui->wg2Menu->hide();

    ui->labContextTheme->clear();
    ui->btnMaxOrNorm->setText(tr("norm"));

    ui->tbtnEdit->setCheckable(true);
    ui->tbtnEdit->setChecked(true);

    ui->tbtnSave->setEnabled(false);

    ui->tbtnRun->setCheckable(true);
    ui->tbtnRun->setChecked(false);

    ui->tbtnTop->setEnabled(false);

    ui->tbtnDesign->setChecked(true);
    ui->tbtnDesign->setEnabled(false);
    ui->lineNavProject->hide();
    ui->wgProjects->hide();

    ui->tbtnNew->hide();
    ui->tbtnSave->hide();
    ui->lineSave->hide();

    ui->tbtnRun->hide();
    ui->lineRun->hide();

    ui->splitterLeftRight->setStretchFactor(0, 10);
    ui->splitterLeftRight->setStretchFactor(1, 90);

    ui->splitterDirectory->setStretchFactor(0, 40);
    ui->splitterDirectory->setStretchFactor(1, 40);
    ui->splitterDirectory->setStretchFactor(2, 24);

    ui->splitterUpDown->setStretchFactor(0, 80);
    ui->splitterUpDown->setStretchFactor(1, 20);

    ui->splitterMainAttr->setStretchFactor(0, 45);
    ui->splitterMainAttr->setStretchFactor(1, 45);
    ui->splitterMainAttr->setStretchFactor(1, 10);

    ui->splitterOutputStatus->setStretchFactor(0, 80);
    ui->splitterOutputStatus->setStretchFactor(1, 20);

    showMaximized();
}

void MainWindow::initSystemTray()
{
    m_pSystemTray = new CSystemTray(this);
    m_pSystemTray->GetQSystemTray()->contextMenu()->setStyleSheet(m_menuQss);

    connect(m_pSystemTray, SIGNAL(winChangeSignal(bool)), this, SLOT(on_winChangeSlot(bool)));
    connect(m_pSystemTray, SIGNAL(appQuitSignal()), this, SLOT(on_appQuitSlot()));

    m_pSystemTray->GetQSystemTray()->show();
}

void MainWindow::initDirFileMgr()
{
    CDirFileMgr::ins().Init();

    connect(&CDirFileMgr::ins(), SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(&CDirFileMgr::ins(), SIGNAL(showMsgDialogSignal(QString const &)), this, SLOT(showMsgDialogSlot(QString const &)));
}

void MainWindow::initAttrViewMgr()
{
    CAttrViewMgr::ins().Init(ui->listwgAttr);

    connect(&CAttrViewMgr::ins(), SIGNAL(showInfoSignal(QString const&)), this, SLOT(showInfoSlot(QString const&)));
    connect(&CAttrViewMgr::ins(), SIGNAL(checkNetInfoSignal(QString const&, bool&)), &CNetViewMgr::ins(), SLOT(checkNetInfoSlot(QString const&, bool&)));
    connect(&CAttrViewMgr::ins(), SIGNAL(paramsChangedSignal(int, QString const&, QString const&, QJsonObject const&)), &CNetViewMgr::ins(), SLOT(paramsChangedSlot(int, QString const&, QString const&, QJsonObject const&)));
    connect(&CAttrViewMgr::ins(), SIGNAL(treeParamsChangedSignal(int, QString const&, QString const&, QJsonObject const&, vector<NetMark>)), &CNetViewMgr::ins(), SLOT(treeParamsChangedSlot(int, QString const&, QString const&, QJsonObject const&, vector<NetMark>)));
    connect(&CAttrViewMgr::ins(), SIGNAL(editTreeParamSignal(int, QString const&, QString const&, QString const&, QString const&, vector<QString> const&)), this, SLOT(editTreeParamSlot(int, QString const&, QString const&, QString const&, QString const&, vector<QString> const&)));
}

void MainWindow::initNetViewMgr()
{
    ui->editView->setScene(new CGraphicsScene(this));
    ui->showView->setScene(new CGraphicsScene(this));

    CNetViewMgr::ins().Init(ui->editView, ui->showView, m_menuQss, VERSION_INFO);

    connect(ui->editView, SIGNAL(signalResize()), this, SLOT(resizeSlot()));
    connect(ui->editView, SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(ui->showView, SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));

    connect(&CNetViewMgr::ins(), SIGNAL(netChangedSignal()), this, SLOT(netChangedSlot()));
    connect(&CNetViewMgr::ins(), SIGNAL(showInfoSignal(QString const&)), this, SLOT(showInfoSlot(QString const&)));
    connect(&CNetViewMgr::ins(), SIGNAL(editNetNameSignal(QString const&, QPoint const&)), this, SLOT(editNetNameSlot(QString const&, QPoint const&)));
    connect(&CNetViewMgr::ins(), SIGNAL(editChannelSignal(int, int, int, QPoint const&)), this, SLOT(editChannelSlot(int, int, int, QPoint const&)));
    connect(&CNetViewMgr::ins(), SIGNAL(inputConfigSignal(int, vector<int> const&, ChannelPairs_t const&, vector<int> const&, int)), this, SLOT(inputConfigSlot(int, vector<int> const&, ChannelPairs_t const&, vector<int> const&, int)));
    connect(&CNetViewMgr::ins(), SIGNAL(outputConfigSignal(int, ChannelPairs_t const&, int)), this, SLOT(outputConfigSlot(int, ChannelPairs_t const&, int)));
    connect(&CNetViewMgr::ins(), SIGNAL(enableUpBtnSignal(bool)), this, SLOT(enableUpBtnSlot(bool)));

    connect(&CNetViewMgr::ins(), SIGNAL(clearAttrSignal()), this, SLOT(clearAttrSlot()));
    connect(&CNetViewMgr::ins(), SIGNAL(showAttrSignal(bool, int, QString const&, QString const&, QJsonObject const&)), this, SLOT(showAttrSlot(bool, int, QString const&, QString const&, QJsonObject const&)));
    connect(&CNetViewMgr::ins(), SIGNAL(showAttrTreeSignal(QJsonObject const&)), &CAttrViewMgr::ins(), SLOT(showAttrTreeSlot(QJsonObject const&)));
    connect(&CNetViewMgr::ins(), SIGNAL(markNetSignal(int, QString const&, QString const&, vector<NetMark>)), &CAttrViewMgr::ins(), SLOT(markNetSlot(int, QString const&, QString const&, vector<NetMark>)));
}

void MainWindow::initListViewMgr()
{
    CListViewMgr::ins().Init(ui->listwgStandard, ui->listwgCustom, ui->listwgProjects, m_menuQss);
    CListViewMgr::ins().LoadListView();

    connect(&CListViewMgr::ins(), SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(&CListViewMgr::ins(), SIGNAL(signalUseNet(bool, QString const&, QString const&)), this, SLOT(useNetSlot(bool, QString const&, QString const&)));
    connect(&CListViewMgr::ins(), SIGNAL(signalAddNet(bool, QString const&, QString const&, QColor const&)), this, SLOT(addNetSlot(bool, QString const&, QString const&, QColor const&)));
    connect(&CListViewMgr::ins(), SIGNAL(signalDelNets(QStringList const&)), &CNetViewMgr::ins(), SLOT(delNetsSlot(QStringList const&)));
    connect(&CListViewMgr::ins(), SIGNAL(signalSelectedProject(QString const &)), this, SLOT(SelectedProjectSlot(QString const &)));
}

void MainWindow::initCompileMgr()
{
    CCompileMgr::ins();
    connect(&CCompileMgr::ins(), SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(&CCompileMgr::ins(), SIGNAL(getNetDir(QString const&, QString&)), &CNetViewMgr::ins(), SLOT(getNetDirSlot(QString const&, QString&)));
}

void MainWindow::initDesignMgr()
{
    CDesignMgr::ins().Init();

    connect(&CDesignMgr::ins(), SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(&CDesignMgr::ins(), SIGNAL(showMsgDialogSignal(QString const &)), this, SLOT(showMsgDialogSlot(QString const &)));
}

void MainWindow::initProjectMgr()
{
    CProjectMgr::ins().Init();

    connect(&CProjectMgr::ins(), SIGNAL(showInfoSignal(QString const &)), this, SLOT(showInfoSlot(QString const &)));
    connect(&CProjectMgr::ins(), SIGNAL(showMsgDialogSignal(QString const &)), this, SLOT(showMsgDialogSlot(QString const &)));
}

void MainWindow::initOutput()
{
    ui->outputBrowser->document()->setMaximumBlockCount(100000);
    ui->outputBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->outputBrowser, SIGNAL(copyAvailable(bool)), this, SLOT(textCopyAvailableSlot(bool)));
    connect(ui->outputBrowser, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showOutputBrowserMenuSlot(const QPoint &)));

    m_pMenuText = new CMenuText(this);
    m_pMenuText->setStyleSheet(m_menuQss);

    disableAllTextAction();

    connect(m_pMenuText, SIGNAL(copySignal()), this, SLOT(copyTextSlot()));
    connect(m_pMenuText, SIGNAL(clearSignal()), this, SLOT(clearTextSlot()));
    connect(m_pMenuText, SIGNAL(selectAllSignal()), this, SLOT(selectAllTextSlot()));
}

void MainWindow::initDialog()
{
    m_pDlgAbout = new CDialogAbout(this);

    m_pDlgGen = new CDialogGen(this);
    m_pDlgNew = new CDialogNew(this);

    m_pDlgMessageInfo = new CDialogMessageInfo(this);

    m_pDlgTextEdit = new CDialogTextEdit(this);
    m_pDlgParamEdit = new CDialogParamEdit(this);
    m_pDlgNumberEdit = new CDialogNumberEdit(this);

    m_pDlgInputConfig = new CDialogInputConfig(this);
    m_pDlgOutputConfig = new CDialogOutputConfig(this);

    m_pDlgAiParamsView = new CDialogAiParamsView(this);
    CAttrViewMgr::ins().SetAiParamsView(m_pDlgAiParamsView->GetAiParamsView());

    connect(m_pDlgGen, SIGNAL(genConfirmSignal()), this, SLOT(genConfirmSlot()));
    connect(m_pDlgGen, SIGNAL(netNameChangeSignal(QString const&)), this, SLOT(netNameChangeSlot(QString const&)));

    connect(m_pDlgNew, SIGNAL(newConfirmSignal()), this, SLOT(newConfirmSlot()));
    connect(m_pDlgNew, SIGNAL(projectNameChangeSignal(QString const&, QString const&, bool&)), this, SLOT(projectNameChangeSlot(QString const&, QString const&, bool&)));

    connect(m_pDlgNumberEdit, SIGNAL(numberChangedSignal(int)), &CNetViewMgr::ins(), SLOT(channelChangedSlot(int)));
    connect(m_pDlgTextEdit, SIGNAL(textChangedSignal(QString const&)), &CNetViewMgr::ins(), SLOT(netNameChangedSlot(QString const&)));
    connect(m_pDlgParamEdit, SIGNAL(paramsChangedSignal(int, QString const&, QString const&, NetAiParams const&)), &CAttrViewMgr::ins(), SLOT(treeParamsChangedSlot(int, QString const&, QString const&, NetAiParams const&)));
    connect(m_pDlgParamEdit, SIGNAL(showInfoSignal(QString const&)), this, SLOT(showInfoSlot(QString const&)));

    connect(m_pDlgInputConfig, SIGNAL(addInputChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(addInputChannelSlot(int, int)));
    connect(m_pDlgInputConfig, SIGNAL(delInputChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(delInputChannelSlot(int, int)));
    connect(m_pDlgInputConfig, SIGNAL(modifyInputChannelSignal(int, int, int)), &CNetViewMgr::ins(), SLOT(modifyInputChannelSlot(int, int, int)));
    connect(m_pDlgInputConfig, SIGNAL(swapInputInChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(swapInputInChannelSlot(int, int)));

    connect(m_pDlgOutputConfig, SIGNAL(addOutputChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(addOutputChannelSlot(int, int)));
    connect(m_pDlgOutputConfig, SIGNAL(delOutputChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(delOutputChannelSlot(int, int)));
    connect(m_pDlgOutputConfig, SIGNAL(modifyOutputChannelSignal(int, int, int)), &CNetViewMgr::ins(), SLOT(modifyOutputChannelSlot(int, int, int)));
    connect(m_pDlgOutputConfig, SIGNAL(swapOutputInChannelSignal(int, int)), &CNetViewMgr::ins(), SLOT(swapOutputInChannelSlot(int, int)));

    m_pDlgAbout->SetInfo(tr(VERSION_INFO));
}

void MainWindow::initAfter()
{
    updateAiParamsView();
    showInfoSlot(tr("Hello Wings!"));
    loadWorkFile();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_pSystemTray && m_pSystemTray->GetQSystemTray()->isVisible())
    {
        event->ignore();
        hide();
        m_pDlgAiParamsView->hide();
        m_pSystemTray->GetQSystemTray()->showMessage(tr("System Tray"), tr("Wings is still running..."));
    }
    else
    {
        event->accept();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nUpdateTimerID)
    {
        killTimer(m_nUpdateTimerID);
        m_nUpdateTimerID = 0;

        updateAiParamsView();
    }
    else if (event->timerId() == m_nRunTimerID)
    {
        killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;

        on_appQuitSlot();
        qApp->quit();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!isMaximized())
    {
        if (event->button() == Qt::LeftButton)
        {
            if (event->y() < ui->wg2Menu->geometry().bottom())
            {
                m_move = true;
                m_dPos = event->globalPos() - pos();
                updateAiParamsView();
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!isMaximized())
    {
        if (event->buttons() & Qt::LeftButton)
        {
            if (m_posType != eInside)
            {
                updateFrameGeometry(event->globalPos());
            }
            else if (m_move)
            {
                move(event->globalPos() - m_dPos);
                updateAiParamsView();
            }
        }
        else
        {
            updateCursorShape();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (!isMaximized())
    {
        m_move = false;

        if (m_posType != eInside)
        {
            updateCursorShape();
        };
    }
}

void MainWindow::updateCursorShape()
{
    auto rect = frameGeometry();
    auto dLeft = abs(cursor().pos().x() - rect.left());
    auto dRight = abs(cursor().pos().x() - rect.right());
    auto dTop = abs(cursor().pos().y() - rect.top());
    auto dBottom = abs(cursor().pos().y() - rect.bottom());

    if (dTop < m_dis)
    {
        if (dLeft < m_dis)
        {
            m_posType = eTopLeft;
            setCursor(Qt::SizeFDiagCursor);
        }
        else if (dRight < m_dis)
        {
            m_posType = eTopRight;
            setCursor(Qt::SizeBDiagCursor);
        }
        else
        {
            m_posType = eTop;
            setCursor(Qt::SizeVerCursor);
        }
    }
    else if (dBottom < m_dis)
    {
        if (dLeft < m_dis)
        {
            m_posType = eBottomLeft;
            setCursor(Qt::SizeBDiagCursor);
        }
        else if (dRight < m_dis)
        {
            m_posType = eBottomRight;
            setCursor(Qt::SizeFDiagCursor);
        }
        else
        {
            m_posType = eBottom;
            setCursor(Qt::SizeVerCursor);
        }
    }
    else if (dLeft < m_dis)
    {
        m_posType = eLeft;
        setCursor(Qt::SizeHorCursor);
    }
    else if (dRight < m_dis)
    {
        m_posType = eRight;
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        m_posType = eInside;
        setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::updateFrameGeometry(QPoint const &pos)
{
    auto rect = frameGeometry();
    auto top = rect.top();
    auto left = rect.left();
    auto right = rect.right();
    auto bottom = rect.bottom();

    if (m_posType & eTop)
    {
        if (height() == minimumHeight())
        {
            top = std::min(pos.y(), top);
        }
        else if (height() == maximumHeight())
        {
            top = std::max(pos.y(), top);
        }
        else
        {
            top = pos.y();
        }
    }
    else if (m_posType & eBottom)
    {
        if (height() == minimumHeight())
        {
            bottom = std::max(pos.y(), top);
        }
        else if (height() == maximumHeight())
        {
            bottom = std::min(pos.y(), top);
        }
        else
        {
            bottom = pos.y();
        }
    }

    if (m_posType & eLeft)
    {
        if (width() == minimumWidth())
        {
            left = std::min(pos.x(), left);
        }
        else if (width() == maximumWidth())
        {
            left = std::max(pos.x(), left);
        }
        else
        {
            left = pos.x();
        }
    }
    else if (m_posType & eRight)
    {
        if (width() == minimumWidth())
        {
            right = std::max(pos.x(), right);
        }
        else if (width() == maximumWidth())
        {
            right = std::min(pos.x(), right);
        }
        else
        {
            right = pos.x();
        }
    }

    setGeometry(QRect(QPoint(left, top), QPoint(right, bottom)));
}

void MainWindow::on_btnMin_clicked()
{
    showMinimized();
}

void MainWindow::on_btnMaxOrNorm_clicked()
{
    if (isMaximized())
    {
        ui->btnMaxOrNorm->setText(tr("max"));
        ui->btnMaxOrNorm->setToolTip(tr("Maximize"));
        showNormal();
        updateAiParamsView();
        m_nUpdateTimerID = startTimer(100);
    }
    else
    {
        ui->btnMaxOrNorm->setText(tr("norm"));
        ui->btnMaxOrNorm->setToolTip(tr("Normal"));
        showMaximized();
        updateAiParamsView();
        m_nUpdateTimerID = startTimer(100);
    }
}

void MainWindow::on_btnClose_clicked()
{
    close();
}

void MainWindow::on_winChangeSlot(bool isMaximized)
{
    if (isMaximized)
    {
        ui->btnMaxOrNorm->setText(tr("norm"));
        ui->btnMaxOrNorm->setToolTip(tr("Normal"));
    }
    else
    {
        ui->btnMaxOrNorm->setText(tr("max"));
        ui->btnMaxOrNorm->setToolTip(tr("Maximize"));
    }

    updateAiParamsView();
    m_nUpdateTimerID = startTimer(100);
}

void MainWindow::on_appQuitSlot()
{
    // do some thing before exit.
}

void MainWindow::on_tbtnNew_clicked()
{
    m_pDlgNew->SetCatalog(CDirFileMgr::ins().GetProjectsCatalogs());
    m_pDlgNew->show();
}

void MainWindow::on_tbtnEdit_clicked()
{

}

void MainWindow::on_tbtnClear_clicked()
{
    clearAll();
    showInfoSlot(tr("Clear all."));
}

void MainWindow::on_tbtnCompile_clicked()
{
    showInfoSlot(tr(""));

    if (compileCheck(false))
    {
        if (m_bDesign)
        {
            QString paramCode, netCode;
            CNetViewMgr::ins().GenCode("work", "temp", "", "", paramCode, netCode);
        }
        else
        {
            auto filePath = CListViewMgr::ins().GetSelectedProjectFilePath();
            auto fileDir = CListViewMgr::ins().GetSelectedProjectDir(filePath);
            auto projectName = CListViewMgr::ins().GetSelectedProjectName(filePath, false);
            auto projectCode = CNetViewMgr::ins().GenProjectCode(projectName, m_isTest);
            if (!CDirFileMgr::ins().SaveProjectCode(projectName, projectCode, fileDir))
            {
                showMsgDialogSlot(tr("Save code of project ") + projectName + tr(" error! @") + fileDir);
            }
        }
    }

    return;

    if (m_pDlgAiParamsView->isHidden())
    {
        updateAiParamsView();
        m_pDlgAiParamsView->show();
    }
    else
    {
        m_pDlgAiParamsView->hide();
    }
}

void MainWindow::on_tbtnGen_clicked()
{
    if (compileCheck(true))
    {
        m_pDlgGen->SetCatalog(CDirFileMgr::ins().GetCustomCatalogs());
        m_pDlgGen->show();
    }
}

void MainWindow::on_tbtnRun_clicked()
{
    if (ui->tbtnRun->isChecked())
    {
        showInfoSlot(tr("Wings start running..."));
    }
    else
    {
        showInfoSlot(tr("Wings stop running!"));
    }
}

void MainWindow::on_tbtnTop_clicked()
{
    ui->tbtnTop->setEnabled(CNetViewMgr::ins().RewindNet());
}

void MainWindow::on_tbtnAbout_clicked()
{
    m_pDlgAbout->exec();
}

void MainWindow::on_tbtnDesign_clicked()
{
    m_bDesign = true;

    ui->tbtnDesign->setEnabled(false);
    ui->tbtnProject->setEnabled(true);

    ui->lineNavProject->hide();
    ui->lineNavDesign->show();

    ui->tbtnNew->hide();
    //ui->tbtnSave->hide();
    ui->lineSave->hide();

    ui->tbtnRun->hide();
    ui->lineRun->hide();

    ui->tbtnEdit->show();
    ui->tbtnGen->show();
    ui->lineGen->show();

    ui->wgProjects->hide();

    CListViewMgr::ins().SetMode(m_bDesign);

    loadWorkFile();
}

void MainWindow::on_tbtnProject_clicked()
{
    m_bDesign = false;

    ui->tbtnProject->setEnabled(false);
    ui->tbtnDesign->setEnabled(true);

    ui->lineNavDesign->hide();
    ui->lineNavProject->show();

    ui->tbtnNew->show();
    //ui->tbtnSave->show();
    ui->lineSave->show();

    ui->tbtnRun->show();
    ui->lineRun->show();

    ui->tbtnEdit->hide();
    ui->tbtnGen->hide();
    ui->lineGen->hide();

    ui->wgProjects->show();

    CListViewMgr::ins().SetMode(m_bDesign);

    loadProjectFile(CListViewMgr::ins().GetSelectedProjectFilePath());
}

void MainWindow::textCopyAvailableSlot(bool bAvailable)
{
    m_pMenuText->EnableCopyAction(bAvailable);
}

void MainWindow::showOutputBrowserMenuSlot(const QPoint &)
{
    auto newPos = QCursor::pos();
    newPos.setX(newPos.x() - 20);
    newPos.setY(newPos.y() - 20);
    m_pMenuText->exec(newPos);
}

void MainWindow::disableAllTextAction()
{
    m_pMenuText->EnableCopyAction(false);
    m_pMenuText->EnableClearAction(false);
    m_pMenuText->EnableSelectAllAction(false);
}

void MainWindow::copyTextSlot()
{
    ui->outputBrowser->copy();
}

void MainWindow::clearTextSlot()
{
    ui->outputBrowser->clear();
    disableAllTextAction();
}

void MainWindow::selectAllTextSlot()
{
    ui->outputBrowser->selectAll();
}

void MainWindow::resizeSlot()
{
    updateAiParamsView();
    m_nUpdateTimerID = startTimer(100);
}

void MainWindow::showInfoSlot(QString const &info)
{
    auto datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->outputBrowser->append(datetime + ":    " + info);

    if (ui->outputBrowser->document()->maximumBlockCount() == 10000)
    {
        ui->outputBrowser->document()->setMaximumBlockCount(100000);
    }
    else if (ui->outputBrowser->document()->lineCount() == 100000)
    {
        ui->outputBrowser->document()->setMaximumBlockCount(10000);
    }

    m_pMenuText->EnableClearAction(true);
    m_pMenuText->EnableSelectAllAction(true);
}

void MainWindow::showMsgDialogSlot(QString const &msg)
{
    m_pDlgMessageInfo->SetInfo(msg);
    m_pDlgMessageInfo->exec();
}

void MainWindow::useNetSlot(bool std, QString const &catalogName, QString const &netId)
{
    showInfoSlot(tr("useNet: ") + QString(std ? tr("Standard-") : tr("Custom-")) + catalogName + "-" + netId + tr("."));
}

void MainWindow::addNetSlot(bool std, QString const &catalogName, QString const &netId, QColor const &color)
{
    Q_UNUSED(std);
    Q_UNUSED(catalogName);

    auto cursorPos = mapFromGlobal(cursor().pos());
    auto viewBase = mapFromGlobal(ui->editView->mapToGlobal(QPoint(0, 0)));
    if (cursorPos.x() > viewBase.x() &&
        cursorPos.x() < viewBase.x() + ui->editView->viewport()->width() &&
        cursorPos.y() > viewBase.y() &&
        cursorPos.y() < viewBase.y() + ui->editView->viewport()->height())
    {
        auto scenePos = ui->editView->mapToScene(QPoint(cursorPos.x() - viewBase.x(), cursorPos.y() - viewBase.y()));
        CNetViewMgr::ins().AddNet(netId, color, scenePos);
    }
}

void MainWindow::SelectedProjectSlot(QString const &filePath)
{
    loadProjectFile(filePath);
}

void MainWindow::netChangedSlot()
{
    auto netJsonObj = CNetViewMgr::ins().GetNet();

    if (m_bDesign)
    {
        if (!CDirFileMgr::ins().SaveWorkNet(netJsonObj))
        {
            showMsgDialogSlot(tr("Save work file Error!"));
        }
    }
    else
    {
        auto filePath = CListViewMgr::ins().GetSelectedProjectFilePath();
        if (!CDirFileMgr::ins().SaveProject(netJsonObj, filePath))
        {
            auto fileName = CListViewMgr::ins().GetSelectedProjectName(filePath);
            showMsgDialogSlot(tr("Save project ") + fileName + tr(" Error!"));
        }
    }
}

void MainWindow::editNetNameSlot(QString const &netId, QPoint const &pos)
{
    m_pDlgTextEdit->SetText(netId);
    m_pDlgTextEdit->move(pos.x() - m_pDlgTextEdit->width() / 2, pos.y() - m_pDlgTextEdit->height() / 2);
    m_pDlgTextEdit->exec();
}

void MainWindow::editTreeParamSlot(int sn, QString const &netId, QString const &netName, QString const &field, QString const &value, vector<QString> const &params)
{
    auto pos = QCursor::pos();
    QCursor::setPos(QPoint(pos.x() + m_pDlgParamEdit->width() / 2 - 60, pos.y() + 55));
    m_pDlgParamEdit->Show(sn, netId, netName, field, value, params);
    m_pDlgParamEdit->move(pos.x() - m_pDlgParamEdit->width() / 2, pos.y() - m_pDlgParamEdit->height() / 2 + 45);
    m_pDlgParamEdit->exec();
}

void MainWindow::editChannelSlot(int min, int max, int channel, QPoint const &pos)
{
    m_pDlgNumberEdit->SetNumber(min, max, channel);
    m_pDlgNumberEdit->move(pos.x() - m_pDlgNumberEdit->width() / 2, pos.y() - m_pDlgNumberEdit->height() / 2);
    m_pDlgNumberEdit->exec();
}

void MainWindow::inputConfigSlot(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt)
{
    m_pDlgInputConfig->Config(outChannelCnt, outChannelShareCnt, channelPairs, inChannels, inChannelCnt);
    m_pDlgInputConfig->exec();
}

void MainWindow::outputConfigSlot(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt)
{
    m_pDlgOutputConfig->Config(outChannelCnt, channelPairs, inChannelCnt);
    m_pDlgOutputConfig->exec();
}

void MainWindow::enableUpBtnSlot(bool enabled)
{
    ui->tbtnTop->setEnabled(enabled);
}

void MainWindow::netNameChangeSlot(QString const &netId)
{
    m_pDlgGen->SetNetNameUsable(!CNetViewMgr::ins().inUse(netId));
}

void MainWindow::genConfirmSlot()
{
    QString paramCode, netCode;
    auto netJsonObj = CNetViewMgr::ins().GenNetCode(
                m_pDlgGen->GetCatalog(),
                m_pDlgGen->GetNetName(),
                m_pDlgGen->GetOutline(),
                m_pDlgGen->GetSpecify(),
                m_pDlgGen->GetShape(),
                m_pDlgGen->GetColor(),
                paramCode,
                netCode);

    CDirFileMgr::ins().SaveCustomNet(
                m_pDlgGen->GetCatalog(),
                m_pDlgGen->GetNetName(),
                netJsonObj,
                paramCode,
                netCode);

    CListViewMgr::ins().AddCustomNet(
                m_pDlgGen->GetCatalog(),
                m_pDlgGen->GetNetName(),
                m_pDlgGen->GetColor());
}

void MainWindow::projectNameChangeSlot(QString const &catalog, QString const &name, bool &usable)
{
    usable = CDirFileMgr::ins().CheckProjectFilePathUsable(catalog, name);
}

void MainWindow::newConfirmSlot()
{
    auto catalog = m_pDlgNew->GetCatalog();
    auto name = m_pDlgNew->GetProjectName();

    auto filePath = CDirFileMgr::ins().MakeProjectFilePath(catalog, name);
    if (filePath.isEmpty())
        return showInfoSlot("New project failed!");

    QString msg = "New project: ";
    if (!catalog.isEmpty())
        msg += catalog + "/";
    msg += name + ".wgs";
    showInfoSlot(msg);

    QJsonObject netJsonObj;
    CDirFileMgr::ins().SaveProject(netJsonObj, filePath);
    CListViewMgr::ins().AddProject(filePath);
}

void MainWindow::clearAttrSlot()
{
    CAttrViewMgr::ins().clearAttr();

    m_pDlgAiParamsView->hide();
}

void MainWindow::showAttrSlot(bool show, int sn, QString const &netId, QString const &netName, QJsonObject const &expressParams)
{
    CAttrViewMgr::ins().showAttr(sn, netId, netName, expressParams);

    if (show)
    {
        updateAiParamsView();
        m_pDlgAiParamsView->show();
    }
}

void MainWindow::clearAll()
{
    QJsonObject netJsonObj;
    CNetViewMgr::ins().SetNet(netJsonObj);
    netChangedSlot();

    ui->listwgAttr->clear();
    ui->outputBrowser->clear();
}

void MainWindow::loadWorkFile()
{
    QJsonObject netJsonObj;
    if (CDirFileMgr::ins().LoadWorkNet(netJsonObj))
    {
        CNetViewMgr::ins().SetNet(netJsonObj);
        showInfoSlot(tr("Load work file ok."));
        ui->labContextTheme->setText("Work.net");
    }
}

void MainWindow::loadProjectFile(QString const &filePath)
{
    QJsonObject netJsonObj;
    CDirFileMgr::ins().LoadProject(netJsonObj, filePath);
    CNetViewMgr::ins().SetNet(netJsonObj);

    auto fileName = CListViewMgr::ins().GetSelectedProjectName(filePath);
    showInfoSlot(tr("Load project: ") + fileName + tr(" ok."));
    ui->labContextTheme->setText(fileName);
}

void MainWindow::updateAiParamsView()
{
    auto point = ui->editView->mapToGlobal(QPoint(0, 0));
    m_pDlgAiParamsView->setGeometry(point.x(), point.y(), ui->editView->width(), ui->editView->height());
}

bool MainWindow::compileCheck(bool showTip)
{
    bool ret = true;
    auto inChannels = CNetViewMgr::ins().GetInputChannelCnt();
    auto outChannels = CNetViewMgr::ins().GetOutputChannelCnt();

    if (m_bDesign)
    {
        if (inChannels < 1 || outChannels < 1)
        {
            ret = false;
            showInfoSlot(tr("Error: Design mode needs At least 1 input and 1 output!  (current: input = ") + QString::number(inChannels) + tr(", output = ") + QString::number(outChannels) + tr(")."));

            if (showTip)
                showMsgDialogSlot(tr("Please handle the error first!"));
        }
    }
    else
    {
        auto info = CNetViewMgr::ins().GetProjectInfo();
        if (info.testCnt == 0 && info.trainCnt == 0)
        {
            ret = false;
            showInfoSlot(tr("Error: Project mode needs net \"train2d\" or \"test2d\"!"));
        }
        else if (info.testCnt > 1 || info.trainCnt > 1 || (info.testCnt > 0 && info.trainCnt > 0))
        {
            ret = false;
            showInfoSlot(tr("Error: Project mode needs 1 of net \"train2d\" or \"test2d\" only!"));
        }
        else if (info.trainCnt == 1)
        {
            m_isTest = false;

            // for train
            if (info.dataloaderCnt != 1)
            {
                ret = false;
                showInfoSlot(tr("Error: train needs 1 \"dataloader2d\" net only!"));
            }

            if (info.unetCnt == 0)
            {
                ret = false;
                showInfoSlot(tr("Error: train needs self-define net!"));
            }

            if (info.lossCnt == 0)
            {
                ret = false;
                showInfoSlot(tr("Error: train needs \"loss\" net!"));
            }

            if (info.optCnt != 1)
            {
                ret = false;
                showInfoSlot(tr("Error: train needs 1 \"opt***\" net only!"));
            }

            if (info.dataloader2unet == 0)
            {
                ret = false;
                showInfoSlot(tr("Error:  dataloader2d needs inflow to self-define net!"));
            }

            if (!info.lossInputErrs.empty())
            {
                ret = false;
                showInfoSlot(tr("Error: ") + QString::number(info.lossInputErrs.size()) + tr(" loss's input needs inflow from 1 dataloader2d and 1 self-define net:"));
                for (auto const &NetName : info.lossInputErrs)
                {
                    showInfoSlot("    " + NetName);
                }
            }

            if (!info.lossOutputErrs.empty() || !info.optOutputErrs.empty())
            {
                ret = false;
                showInfoSlot(tr("Error: ") + QString::number(info.lossOutputErrs.size() + info.optOutputErrs.size()) + tr(" item(s) needs inflow to train2d:"));
                for (auto const &NetName : info.lossOutputErrs)
                {
                    showInfoSlot("    " + NetName);
                }
                for (auto const &NetName : info.optOutputErrs)
                {
                    showInfoSlot("    " + NetName);
                }
            }
        }
        else if (info.testCnt == 1)
        {
            // for test
            m_isTest = true;
        }
        else
        {
            ret = false;
            showInfoSlot(tr("Unknown error!"));
        }

        if (inChannels != 0 || outChannels != 0)
        {
            ret = false;
            showInfoSlot(tr("Error: Project mode no need input and output!  (current: input = ") + QString::number(inChannels) + tr(", output = ") + QString::number(outChannels) + tr(")."));
        }
    }

    auto missingNetIds = CNetViewMgr::ins().GetMissingNetIds();
    auto nullInputItems = CNetViewMgr::ins().GetNullInputItems();
    auto needFullInputItems = CNetViewMgr::ins().GetNeedFullInputItems();

    if (!missingNetIds.empty())
    {
        ret = false;
        showInfoSlot(tr("Error: ") + QString::number(missingNetIds.size()) + tr(" type of net is missing:"));
        for (auto const &netId : missingNetIds)
        {
            auto matchNetNames = CNetViewMgr::ins().GetMatchNetNames(netId);
            for (auto const &netName : matchNetNames)
            {
                showInfoSlot("    " + netId + " : " + netName);
            }
        }

        if (showTip)
            showMsgDialogSlot(tr("Please handle the error first!"));
    }

    if (!nullInputItems.empty())
    {
        ret = false;
        showInfoSlot(tr("Error: ") + QString::number(nullInputItems.size()) + tr(" item(s) input is null:"));
        for (auto const &NetName : nullInputItems)
        {
            showInfoSlot("    " + NetName);
        }

        if (showTip)
            showMsgDialogSlot(tr("Please handle the error first!"));
    }

    if (!needFullInputItems.empty())
    {
        ret = false;
        showInfoSlot(tr("Error: ") + QString::number(needFullInputItems.size()) + tr(" item(s) need full input:"));
        for (auto const &NetName : needFullInputItems)
        {
            showInfoSlot("    " + NetName);
        }

        if (showTip)
            showMsgDialogSlot(tr("Please handle the error first!"));
    }

    return ret;
}
