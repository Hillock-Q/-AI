#include "WgCtrlList.h"
#include "ui_WgCtrlList.h"
#include "BtnCtrl.h"

constexpr int ROW_HEIGHT = 40;

WgCtrlList::WgCtrlList(QString const &catalogName, QWidget *parent)
    :
      QWidget(parent),
      ui(new Ui::WgCtrlList),
      m_catalogName(catalogName)
{
    ui->setupUi(this);
    ui->tbtnCatalog->setText(m_catalogName);
    resizeHeight();
}

WgCtrlList::~WgCtrlList()
{
    delete ui;
}

QString const& WgCtrlList::GetCatalogName() const
{
    return m_catalogName;
}

void WgCtrlList::AddNet(QString const &netId, QColor const &color)
{
    auto tbtn = new BtnCtrl(netId, color, this);
    tbtn->setFixedHeight(ROW_HEIGHT - 2);
    tbtn->show();

    connect(tbtn, SIGNAL(signalUseNet(QString const&)), this, SLOT(useNetSlot(QString const&)));
    connect(tbtn, SIGNAL(signalAddNet(QString const&, QColor const&)), this, SLOT(addNetSlot(QString const&, QColor const&)));

    auto item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ROW_HEIGHT));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, tbtn);

    resizeHeight();
}

void WgCtrlList::DelNet(QString const &netId)
{
    for (auto i = 0; i < ui->listWidget->count(); i++)
    {
        auto tbtn = dynamic_cast<BtnCtrl*>(ui->listWidget->itemWidget(ui->listWidget->item(i)));
        if (tbtn == nullptr || tbtn->GetNetId() != netId)
            continue;

        ui->listWidget->removeItemWidget(ui->listWidget->item(i));
        delete ui->listWidget->takeItem(i);
        resizeHeight();
        break;
    }
}

void WgCtrlList::Update()
{
    ui->tbtnCatalog->click();
    ui->tbtnCatalog->click();
}

void WgCtrlList::resizeHeight()
{
    ui->listWidget->setFixedHeight(ROW_HEIGHT * ui->listWidget->count());
    setFixedHeight(ui->tbtnCatalog->height() + ui->listWidget->height());
    update();
}

void WgCtrlList::on_tbtnCatalog_clicked(bool checked)
{
    if (checked)
    {
        ui->tbtnCatalog->setArrowType(Qt::DownArrow);
        ui->listWidget->show();
        setFixedHeight(ui->tbtnCatalog->height() + ui->listWidget->height());
    }
    else
    {
        ui->tbtnCatalog->setArrowType(Qt::RightArrow);
        ui->listWidget->hide();
        setFixedHeight(ui->tbtnCatalog->height());
    }

    emit signalUpdate(m_catalogName);
}

void WgCtrlList::useNetSlot(QString const &netId)
{
    emit signalUseNet(m_catalogName, netId);
}

void WgCtrlList::addNetSlot(QString const &netId, QColor const &color)
{
    emit signalAddNet(m_catalogName, netId, color);
}
