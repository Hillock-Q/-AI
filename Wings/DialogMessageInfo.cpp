#include "DialogMessageInfo.h"
#include "ui_DialogMessageInfo.h"
#include <QMouseEvent>

CDialogMessageInfo::CDialogMessageInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogMessageInfo)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    ui->btnCancel->hide();
}

CDialogMessageInfo::~CDialogMessageInfo()
{
    delete ui;
}

void CDialogMessageInfo::SetInfo(QString const &info)
{
    ui->labText->setText(info);
}

void CDialogMessageInfo::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (event->y() < ui->wg2Title->geometry().bottom())
        {
            m_move = true;
            m_dPos = event->globalPos() - pos();
        }
    }
}

void CDialogMessageInfo::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogMessageInfo::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogMessageInfo::on_btnClose_clicked()
{
    close();
    emit confirmSignal(false);
}

void CDialogMessageInfo::on_btnConfirm_clicked()
{
    close();
    emit confirmSignal(true);
}

void CDialogMessageInfo::on_btnCancel_clicked()
{
    close();
    emit confirmSignal(false);
}
