#include "DialogNumberEdit.h"
#include "ui_DialogNumberEdit.h"
#include <QMouseEvent>

CDialogNumberEdit::CDialogNumberEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogNumberEdit)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    ui->spinBox->installEventFilter(this);
}

CDialogNumberEdit::~CDialogNumberEdit()
{
    delete ui;
}

void CDialogNumberEdit::SetNumber(int min, int max, int value)
{
    ui->spinBox->setRange(min, max);
    ui->spinBox->setValue(value);
    ui->spinBox->setToolTip("[0, " + QString::number(max) + "]");
}

void CDialogNumberEdit::showEvent(QShowEvent *)
{
    ui->spinBox->setFocus();
    m_nRunTimerID = this->startTimer(3000);
}

void CDialogNumberEdit::timerEvent(QTimerEvent *event)
{
    if (m_nRunTimerID == event->timerId())
    {
        this->killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;

        this->close();
    }
}

bool CDialogNumberEdit::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->spinBox && (event->type() == QEvent::Enter || event->type() == QEvent::HoverEnter))
    {
        this->killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;
    }
    else if (object == ui->spinBox && (event->type() == QEvent::Leave || event->type() == QEvent::HoverLeave))
    {
        this->close();
    }

    return QDialog::eventFilter(object, event);
}

void CDialogNumberEdit::on_spinBox_valueChanged(int val)
{
    emit numberChangedSignal(val);
}
