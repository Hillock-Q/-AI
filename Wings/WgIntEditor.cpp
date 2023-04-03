#include "WgIntEditor.h"
#include "ui_WgIntEditor.h"

WgIntEditor::WgIntEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgIntEditor)
{
    ui->setupUi(this);
}

WgIntEditor::~WgIntEditor()
{
    delete ui;
}

void WgIntEditor::Init(QString const &name, int min, int max, int val, int step)
{
    ui->labName->setText(name);

    m_notify = false;
    ui->spinBoxValue->setRange(min, max);
    ui->spinBoxValue->setSingleStep(step);
    ui->spinBoxValue->setValue(val);
    ui->spinBoxValue->setFocus();
    m_notify = true;
}

void WgIntEditor::SetMax(int max)
{
    ui->spinBoxValue->setMaximum(max);
}

void WgIntEditor::on_spinBoxValue_valueChanged(int val)
{
    if (m_notify)
        emit signalValueChanged(ui->labName->text(), val);
}
