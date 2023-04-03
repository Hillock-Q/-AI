#include "WgTextEditor.h"
#include "ui_WgTextEditor.h"

WgTextEditor::WgTextEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgTextEditor)
{
    ui->setupUi(this);
}

WgTextEditor::~WgTextEditor()
{
    delete ui;
}

void WgTextEditor::Init(QString const &name, QString const &text)
{
    ui->labName->setText(name);

    m_notify = false;
    ui->lineEdit->setText(text);
    ui->lineEdit->setFocus();
    m_notify = true;
}

void WgTextEditor::on_lineEdit_textChanged(const QString &text)
{
    if (m_notify)
        emit signalTextChanged(ui->labName->text(), text);
}
