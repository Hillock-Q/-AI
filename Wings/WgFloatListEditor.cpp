#include "WgFloatListEditor.h"
#include "ui_WgFloatListEditor.h"

WgFloatListEditor::WgFloatListEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgFloatListEditor)
{
    ui->setupUi(this);
}

WgFloatListEditor::~WgFloatListEditor()
{
    delete ui;
}

void WgFloatListEditor::Init(QString const &name, QStringList const &cols, int index, double min, double max, double val, double step, int prec)
{
    ui->labName->setText(name);

    m_notify = false;

    ui->comboBoxIndex->clear();
    ui->comboBoxIndex->addItems(cols);
    ui->comboBoxIndex->setCurrentIndex(index);

    ui->doubleSpinBoxValue->setDecimals(prec);
    ui->doubleSpinBoxValue->setSingleStep(step);
    ui->doubleSpinBoxValue->setRange(min, max);
    ui->doubleSpinBoxValue->setValue(val);
    ui->doubleSpinBoxValue->setFocus();

    m_notify = true;
}

void WgFloatListEditor::Reset(QStringList const &cols)
{
    auto index = ui->comboBoxIndex->currentIndex();
    if (index >= cols.size())
        index = cols.size() - 1;

    m_notify = false;
    ui->comboBoxIndex->clear();
    ui->comboBoxIndex->addItems(cols);
    ui->comboBoxIndex->setCurrentIndex(index);
    m_notify = true;

    double val = 0;
    emit signalIndexChanged(ui->labName->text(), index, val);

    m_notify = false;
    ui->doubleSpinBoxValue->setValue(val);
    m_notify = true;
}

void WgFloatListEditor::on_comboBoxIndex_currentIndexChanged(int index)
{
    if (m_notify)
    {
        double val = 0;
        emit signalIndexChanged(ui->labName->text(), index, val);

        m_notify = false;
        ui->doubleSpinBoxValue->setValue(val);
        m_notify = true;
    }
}

void WgFloatListEditor::on_doubleSpinBoxValue_valueChanged(double val)
{
    if (m_notify)
        emit signalValueChanged(ui->labName->text(), ui->comboBoxIndex->currentIndex(), val);
}
