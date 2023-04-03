#include "WgFloatEditor.h"
#include "ui_WgFloatEditor.h"

WgFloatEditor::WgFloatEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgFloatEditor)
{
    ui->setupUi(this);
}

WgFloatEditor::~WgFloatEditor()
{
    delete ui;
}

void WgFloatEditor::Init(QString const &name, double min, double max, double val, double step, int prec)
{
    ui->labName->setText(name);

    m_notify = false;
    ui->doubleSpinBoxValue->setDecimals(prec);
    ui->doubleSpinBoxValue->setSingleStep(step);
    ui->doubleSpinBoxValue->setRange(min, max);
    ui->doubleSpinBoxValue->setValue(val);
    ui->doubleSpinBoxValue->setFocus();
    m_notify = true;
}

void WgFloatEditor::on_doubleSpinBoxValue_valueChanged(double val)
{
    if (m_notify)
        emit signalValueChanged(ui->labName->text(), val);
}
