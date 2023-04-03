#include "WgBtnCtrl.h"
#include "ui_WgBtnCtrl.h"
#include "BtnCtrl.h"

WgBtnCtrl::WgBtnCtrl(QString const &netId, QColor const &color, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgBtnCtrl)
{
    ui->setupUi(this);
    ui->tBtn->SetNetId(netId);
    ui->tBtn->SetColor(color);

    connect(ui->tBtn, &BtnCtrl::signalUseNet, this, &WgBtnCtrl::signalUseNet);
    connect(ui->tBtn, &BtnCtrl::signalAddNet, this, &WgBtnCtrl::signalAddNet);
}

WgBtnCtrl::~WgBtnCtrl()
{
    delete ui;
}

void WgBtnCtrl::SetNetId(QString const &id)
{
    ui->tBtn->SetNetId(id);
}

QString const& WgBtnCtrl::GetNetId() const
{
    return ui->tBtn->GetNetId();
}

void WgBtnCtrl::SetColor(QColor const &color)
{
    ui->tBtn->SetColor(color);
}

QColor const& WgBtnCtrl::GetColor() const
{
    return ui->tBtn->GetColor();
}
