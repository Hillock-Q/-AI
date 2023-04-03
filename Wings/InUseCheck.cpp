#include "InUseCheck.h"
#include "ui_InUseCheck.h"

CInUseCheck::CInUseCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CInUseCheck)
{
    ui->setupUi(this);
    ui->radioButton->setCheckable(true);
    ui->radioButton->setChecked(false);
}

CInUseCheck::~CInUseCheck()
{
    delete ui;
}

bool CInUseCheck::isChecked() const
{
    return ui->radioButton->isCheckable() && ui->radioButton->isChecked();
}

void CInUseCheck::SetChecked(bool checked)
{
    ui->radioButton->setChecked(checked);
}

void CInUseCheck::on_radioButton_clicked(bool checked)
{
    emit checkSignal(this, checked);
}
