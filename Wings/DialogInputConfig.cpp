#include "DialogInputConfig.h"
#include "ui_DialogInputConfig.h"
#include <QMouseEvent>
#include <QDebug>
#include "InUseCheck.h"
#include "Delegate.h"

CDialogInputConfig::CDialogInputConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogInputConfig)
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
    connect(ui->tableWidget, &QTableWidget::entered, this, &CDialogInputConfig::tableHoverIndexChangedSlot);
    connect(ui->tableWidget, &QTableWidget::clicked, this, &CDialogInputConfig::tableSelectedIndexChangedSlot);

    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableCellDoubleClicked(int, int)));
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(tableItemChangedSlot(QTableWidgetItem *)));

    ui->tableWidget->setColumnCount(3);

    QStringList header;
    header << tr("use") << tr("input channels") << tr("net channels");
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0, 60);
    ui->tableWidget->setColumnWidth(1, 250);
}

CDialogInputConfig::~CDialogInputConfig()
{
    delete ui;
}

void CDialogInputConfig::Config(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt)
{
    m_inChannelCnt = inChannelCnt;
    m_outChannelShareCnt = outChannelShareCnt;
    m_outChannelCnt = outChannelCnt;

    if (ui->tableWidget->rowCount() == (int)inChannels.size())
    {
        updateTable(outChannelCnt, outChannelShareCnt, channelPairs, inChannels, inChannelCnt);
    }
    else
    {
        resetTable(outChannelCnt, outChannelShareCnt, channelPairs, inChannels, inChannelCnt);
    }
}

int CDialogInputConfig::getOutChannel(int inChannel, ChannelPairs_t const &channelPairs) const
{
    for (auto channelPair : channelPairs)
        if (inChannel == channelPair.second)
            return channelPair.first;

    return -1;
}

void CDialogInputConfig::resetTable(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt)
{
    m_inuseCheckers.clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount((int)inChannels.size());

    for (auto index = 0u; index < inChannels.size(); index++)
    {
        // in use
        auto item0 = new CInUseCheck(ui->tableWidget);
        connect(item0, SIGNAL(checkSignal(void const*, bool)), this, SLOT(checkSlot(void const*, bool)));
        m_inuseCheckers.push_back(item0);

        // outChannel
        auto item1 = new QTableWidgetItem();

        // inChannel
        auto item2 = new QTableWidgetItem(QString::number(inChannels[index]));

        auto outChannel = getOutChannel(inChannels[index], channelPairs);
        if (outChannel != -1)
        {
            item0->SetChecked(true);

            item1->setText(QString::number(outChannel));
            item1->setFlags(item1->flags() | Qt::ItemIsEditable);
            if (outChannelShareCnt[outChannel] > 1)
            {
                item1->setToolTip("[0, " + QString::number(m_outChannelCnt) + "]");
            }
            else
            {
                item1->setToolTip("[0, " + QString::number(m_outChannelCnt - 1) + "]");
            }

            item2->setFlags(item2->flags() | Qt::ItemIsEditable);
            item2->setToolTip("[0, " + QString::number(inChannelCnt - 1) + "]");
        }
        else
        {
            item0->SetChecked(false);

            item1->setText(QString::number(outChannelCnt++));
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            item1->setToolTip("");

            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            item2->setToolTip("");
        }        

        ui->tableWidget->setCellWidget(index, 0, item0);
        ui->tableWidget->setItem(index, 1, item1);
        ui->tableWidget->setItem(index, 2, item2);
    }
}

void CDialogInputConfig::updateTable(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt)
{
    for (auto index = 0u; index < inChannels.size(); index++)
    {
        // in use
        auto item0 = dynamic_cast<CInUseCheck*>(ui->tableWidget->cellWidget(index, 0));
        if (item0 == nullptr)
            continue;

        // outChannel
        auto item1 = ui->tableWidget->item(index, 1);

        // inChannel
        auto item2 = ui->tableWidget->item(index, 2);
        item2->setText(QString::number(inChannels[index]));

        auto outChannel = getOutChannel(inChannels[index], channelPairs);
        if (outChannel != -1)
        {
            item0->SetChecked(true);

            item1->setText(QString::number(outChannel));
            item1->setFlags(item1->flags() | Qt::ItemIsEditable);
            if (outChannelShareCnt[outChannel] > 1)
            {
                item1->setToolTip("[0, " + QString::number(m_outChannelCnt) + "]");
            }
            else
            {
                item1->setToolTip("[0, " + QString::number(m_outChannelCnt - 1) + "]");
            }

            item2->setFlags(item2->flags() | Qt::ItemIsEditable);
            item2->setToolTip("[0, " + QString::number(inChannelCnt - 1) + "]");
        }
        else
        {
            item0->SetChecked(false);

            item1->setText(QString::number(outChannelCnt++));
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            item1->setToolTip("");

            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            item2->setToolTip("");
        }
    }
}

void CDialogInputConfig::tableHoverIndexChangedSlot(const QModelIndex &index)
{
    m_pDelegate->onHoverIndexChanged(index);
    ui->tableWidget->update();
}

void CDialogInputConfig::tableSelectedIndexChangedSlot(const QModelIndex &index)
{
    index.row();
    index.column();
    m_pDelegate->onSelectedIndexChanged(index);
    ui->tableWidget->update();
}

void CDialogInputConfig::tableCellDoubleClicked(int row, int column)
{
    if (column < 1 || column > 2)
        return;

    auto item = ui->tableWidget->item(row, column);
    m_oldVal = item->text();
    m_cellRow = row;
    m_cellCol = column;
    m_cellClicked = true;
}

void CDialogInputConfig::tableItemChangedSlot(QTableWidgetItem *item)
{
    if (m_cellClicked && m_cellRow == item->row() && m_cellCol == item->column())
    {
        m_cellClicked = false;

        if (m_cellCol == 1)
        {
            bool ok = false;
            auto oldOutChannel = (int)m_oldVal.toUInt();
            auto newOutChannel = (int)item->text().toUInt(&ok);
            if (ok && newOutChannel < ((oldOutChannel < (int)m_outChannelShareCnt.size()) && m_outChannelShareCnt[oldOutChannel] > 1 ? m_outChannelCnt + 1 : m_outChannelCnt ))
            {
                // in use
                auto item0 = dynamic_cast<CInUseCheck*>(ui->tableWidget->cellWidget(m_cellRow, 0));
                if (item0 != nullptr && item0->isChecked())
                {
                    auto inItem = ui->tableWidget->item(m_cellRow, m_cellCol + 1);
                    auto inChannel = (int)inItem->text().toUInt();
                    emit modifyInputChannelSignal(oldOutChannel, inChannel, newOutChannel);
                }
            }
            else
            {
                item->setText(m_oldVal);
            }
        }
        else
        {
            bool ok = false;
            auto newInChannel = (int)item->text().toUInt(&ok);
            if (ok && newInChannel < m_inChannelCnt)
            {
                // in use
                auto item0 = dynamic_cast<CInUseCheck*>(ui->tableWidget->cellWidget(m_cellRow, 0));
                if (item0 != nullptr && item0->isChecked())
                {
                    auto inChannel = (int)m_oldVal.toUInt();
                    emit swapInputInChannelSignal(inChannel, newInChannel);
                }
            }
            else
            {
                item->setText(m_oldVal);
            }
        }
    }
}

void CDialogInputConfig::checkSlot(void const *inuseChecker, bool checked)
{
    auto row = getRow(inuseChecker);

    auto outItem = ui->tableWidget->item(row, 1);
    auto outChannel = (int)outItem->text().toUInt();

    auto inItem = ui->tableWidget->item(row, 2);
    auto inChannel = (int)inItem->text().toUInt();

    if (checked)
    {
        emit addInputChannelSignal(outChannel, inChannel);
    }
    else
    {
        emit delInputChannelSignal(outChannel, inChannel);
    }
}

int CDialogInputConfig::getRow(void const *inuseChecker)
{
    for (auto i = 0u; i < m_inuseCheckers.size(); i++)
        if (m_inuseCheckers[i] == inuseChecker)
            return i;

    return -1;
}

void CDialogInputConfig::mousePressEvent(QMouseEvent * event)
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

void CDialogInputConfig::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogInputConfig::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogInputConfig::on_btnClose_clicked()
{
    close();
}

void CDialogInputConfig::on_btnConfirm_clicked()
{
    close();
}
