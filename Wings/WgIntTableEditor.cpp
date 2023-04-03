#include "WgIntTableEditor.h"
#include "ui_WgIntTableEditor.h"

WgIntTableEditor::WgIntTableEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgIntTableEditor)
{
    ui->setupUi(this);
}

WgIntTableEditor::~WgIntTableEditor()
{
    delete ui;
}

void WgIntTableEditor::Init(QString const &name, QStringList const &rows, int rIndex, QStringList const &cols, int cIndex, int min, int max, int val, int step)
{
    ui->labName->setText(name);

    m_notify = false;

    ui->comboBoxRow->clear();
    ui->comboBoxRow->addItems(rows);
    ui->comboBoxRow->setCurrentIndex(rIndex);

    ui->comboBoxCol->clear();
    ui->comboBoxCol->addItems(cols);
    ui->comboBoxCol->setCurrentIndex(cIndex);

    ui->spinBoxValue->setRange(min, max);
    ui->spinBoxValue->setSingleStep(step);
    ui->spinBoxValue->setValue(val);
    ui->spinBoxValue->setFocus();

    m_notify = true;
}

void WgIntTableEditor::on_comboBoxRow_currentIndexChanged(int rIndex)
{
    if (m_notify)
    {
        int cIndex = 0, val = 0;
        emit signalColChanged(ui->labName->text(), rIndex, cIndex, val);

        m_notify = false;
        ui->comboBoxCol->setCurrentIndex(cIndex);
        ui->spinBoxValue->setValue(val);
        m_notify = true;
    }
}

void WgIntTableEditor::on_comboBoxCol_currentIndexChanged(int cIndex)
{
    if (m_notify)
    {
        int val = 0;
        emit signalColChanged(ui->labName->text(), ui->comboBoxRow->currentIndex(), cIndex, val);

        m_notify = false;
        ui->spinBoxValue->setValue(val);
        m_notify = true;
    }
}

void WgIntTableEditor::on_spinBoxValue_valueChanged(int val)
{
    if (m_notify)
        emit signalValueChanged(ui->labName->text(), ui->comboBoxRow->currentIndex(), ui->comboBoxCol->currentIndex(), val);
}
