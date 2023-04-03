#include "DialogOutputConfig.h"
#include "ui_DialogOutputConfig.h"
#include <QMouseEvent>
#include <QDebug>
#include "InUseCheck.h"
#include "Delegate.h"

CDialogOutputConfig::CDialogOutputConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogOutputConfig)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    ui->tableWidget->verticalHeader()->setFixedWidth(40);

    ui->tableWidget->horizontalHeader()->setFixedHeight(50);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_pDelegate = new CDelegate(this);
    ui->tableWidget->setItemDelegate(m_pDelegate);
    connect(ui->tableWidget, &QTableWidget::entered, this, &CDialogOutputConfig::tableHoverIndexChangedSlot);
    connect(ui->tableWidget, &QTableWidget::clicked, this, &CDialogOutputConfig::tableSelectedIndexChangedSlot);

    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableCellDoubleClicked(int, int)));
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(tableItemChangedSlot(QTableWidgetItem *)));

    ui->tableWidget->setColumnCount(3);

    QStringList header;
    header << tr("use") << tr("net channels") << tr("output channels");
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0, 60);
    ui->tableWidget->setColumnWidth(1, 250);
}

CDialogOutputConfig::~CDialogOutputConfig()
{
    delete ui;
}

void CDialogOutputConfig::Config(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt)
{
    m_inChannelCnt = inChannelCnt;
    if (ui->tableWidget->rowCount() == outChannelCnt)
    {
        updateTable(outChannelCnt, channelPairs, inChannelCnt);
    }
    else
    {
        resetTable(outChannelCnt, channelPairs, inChannelCnt);
    }
}

int CDialogOutputConfig::getInChannel(int outChannel, ChannelPairs_t const &channelPairs) const
{
    for (auto channelPair : channelPairs)
        if (outChannel == channelPair.first)
            return channelPair.second;

    return -1;
}

void CDialogOutputConfig::resetTable(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt)
{
    m_inuseCheckers.clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(outChannelCnt);

    for (auto index = 0; index < outChannelCnt; index++)
    {
        // in use
        auto item0 = new CInUseCheck(ui->tableWidget);
        connect(item0, SIGNAL(checkSignal(void const*, bool)), this, SLOT(checkSlot(void const*, bool)));
        m_inuseCheckers.push_back(item0);

        // outChannel
        auto item1 = new QTableWidgetItem(QString::number(index));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));

        // inChannel
        auto item2 = new QTableWidgetItem();
        auto inChannel = getInChannel(index, channelPairs);
        if (inChannel != -1)
        {
            item0->SetChecked(true);
            item2->setText(QString::number(inChannel));
            item2->setFlags(item2->flags() | Qt::ItemIsEditable);
            item2->setToolTip("[0, " + QString::number(m_inChannelCnt - 1) + "]");
        }
        else
        {
            item0->SetChecked(false);
            item2->setText(QString::number(inChannelCnt++));
            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            item2->setToolTip("");
        }

        ui->tableWidget->setCellWidget(index, 0, item0);
        ui->tableWidget->setItem(index, 1, item1);
        ui->tableWidget->setItem(index, 2, item2);
    }
}

void CDialogOutputConfig::updateTable(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt)
{
    for (auto index = 0; index < outChannelCnt; index++)
    {
        // in use
        auto item0 = dynamic_cast<CInUseCheck*>(ui->tableWidget->cellWidget(index, 0));
        if (item0 == nullptr)
            continue;

        // outChannel
        auto item1 = ui->tableWidget->item(index, 1);
        item1->setText(QString::number(index));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));

        // inChannel
        auto item2 = ui->tableWidget->item(index, 2);

        auto inChannel = getInChannel(index, channelPairs);
        if (inChannel != -1)
        {
            item0->SetChecked(true);

            item2->setText(QString::number(inChannel));
            item2->setFlags(item2->flags() | Qt::ItemIsEditable);
            item2->setToolTip("[0, " + QString::number(m_inChannelCnt - 1) + "]");
        }
        else
        {
            item0->SetChecked(false);

            item2->setText(QString::number(inChannelCnt++));
            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            item2->setToolTip("");
        }
    }
}

void CDialogOutputConfig::tableHoverIndexChangedSlot(const QModelIndex &index)
{
    m_pDelegate->onHoverIndexChanged(index);
    ui->tableWidget->update();
}

void CDialogOutputConfig::tableSelectedIndexChangedSlot(const QModelIndex &index)
{
    index.row();
    index.column();
    m_pDelegate->onSelectedIndexChanged(index);
    ui->tableWidget->update();
}

void CDialogOutputConfig::tableCellDoubleClicked(int row, int column)
{
    if (column != 2)
        return;

    auto item = ui->tableWidget->item(row, column);
    m_oldVal = item->text();
    m_cellRow = row;
    m_cellCol = column;
    m_cellClicked = true;
}

void CDialogOutputConfig::tableItemChangedSlot(QTableWidgetItem *item)
{
    if (m_cellClicked && m_cellRow == item->row() && m_cellCol == item->column())
    {
        m_cellClicked = false;

        bool ok = false;
        auto newInChannel = (int)item->text().toUInt(&ok);
        if (ok && newInChannel < m_inChannelCnt)
        {
            // in use
            auto item0 = dynamic_cast<CInUseCheck*>(ui->tableWidget->cellWidget(m_cellRow, 0));
            if (item0 != nullptr && item0->isChecked())
            {
                auto inChannel = (int)m_oldVal.toUInt();
                emit swapOutputInChannelSignal(inChannel, newInChannel);
            }
        }
        else
        {
            item->setText(m_oldVal);
        }
    }
}

void CDialogOutputConfig::checkSlot(void const *inuseChecker, bool checked)
{
    auto row = getRow(inuseChecker);

    auto outItem = ui->tableWidget->item(row, 1);
    auto outChannel = (int)outItem->text().toUInt();

    auto inItem = ui->tableWidget->item(row, 2);
    auto inChannel = (int)inItem->text().toUInt();

    if (checked)
    {
        emit addOutputChannelSignal(outChannel, inChannel);
    }
    else
    {
        emit delOutputChannelSignal(outChannel, inChannel);
    }
}

int CDialogOutputConfig::getRow(void const *inuseChecker)
{
    for (auto i = 0u; i < m_inuseCheckers.size(); i++)
        if (m_inuseCheckers[i] == inuseChecker)
            return i;

    return -1;
}

void CDialogOutputConfig::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (event->y() < ui->wg2Title->geometry().bottom())
        {
            m_move = true;
            m_dPos = event->globalPos() - pos();
        }
    }
}

void CDialogOutputConfig::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogOutputConfig::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogOutputConfig::on_btnClose_clicked()
{
    close();
}

void CDialogOutputConfig::on_btnConfirm_clicked()
{
    close();
}
