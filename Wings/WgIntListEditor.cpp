#include "WgIntListEditor.h"
#include "ui_WgIntListEditor.h"

WgIntListEditor::WgIntListEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgIntListEditor)
{
    ui->setupUi(this);
}

WgIntListEditor::~WgIntListEditor()
{
    delete ui;
}

void WgIntListEditor::Init(QString const &name, QStringList const &cols, int index, int min, int max, int val, int step)
{
    ui->labName->setText(name);

    m_notify = false;

    ui->comboBoxIndex->clear();
    ui->comboBoxIndex->addItems(cols);
    ui->comboBoxIndex->setCurrentIndex(index);

    ui->spinBoxValue->setRange(min, max);
    ui->spinBoxValue->setSingleStep(step);
    ui->spinBoxValue->setValue(val);
    ui->spinBoxValue->setFocus();

    m_notify = true;
}

void WgIntListEditor::Reset(QStringList const &cols)
{
    auto index = ui->comboBoxIndex->currentIndex();
    if (index >= cols.size())
        index = cols.size() - 1;

    m_notify = false;
    ui->comboBoxIndex->clear();
    ui->comboBoxIndex->addItems(cols);
    ui->comboBoxIndex->setCurrentIndex(index);
    m_notify = true;

    int val = 0;
    emit signalIndexChanged(ui->labName->text(), index, val);

    m_notify = false;
    ui->spinBoxValue->setValue(val);
    m_notify = true;
}

void WgIntListEditor::on_comboBoxIndex_currentIndexChanged(int index)
{
    if (m_notify)
    {
        int val = 0;
        emit signalIndexChanged(ui->labName->text(), index, val);

        m_notify = false;
        ui->spinBoxValue->setValue(val);
        m_notify = true;
    }
}

void WgIntListEditor::on_spinBoxValue_valueChanged(int val)
{
    if (m_notify)
        emit signalValueChanged(ui->labName->text(), ui->comboBoxIndex->currentIndex(), val);
}
