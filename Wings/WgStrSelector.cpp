#include "WgStrSelector.h"
#include "ui_WgStrSelector.h"

WgStrSelector::WgStrSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgStrSelector)
{
    ui->setupUi(this);
}

WgStrSelector::~WgStrSelector()
{
    delete ui;
}

void WgStrSelector::Init(QString const &name, QStringList const &rows, QString const &selected)
{
    ui->labName->setText(name);

    m_notify = false;
    ui->comboBoxValue->clear();
    ui->comboBoxValue->addItems(rows);
    ui->comboBoxValue->setCurrentText(selected);
    ui->comboBoxValue->setFocus();
    m_notify = true;
}

void WgStrSelector::on_comboBoxValue_currentIndexChanged(const QString &selected)
{
    if (m_notify)
        emit signalSelectedChanged(ui->labName->text(), selected);
}
