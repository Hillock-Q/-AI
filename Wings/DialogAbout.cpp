#include "DialogAbout.h"
#include "ui_DialogAbout.h"
#include <QMouseEvent>

CDialogAbout::CDialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogAbout)
{
    ui->setupUi(this);

    ui->btnConfirm->hide();
    ui->btnCancel->hide();

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

CDialogAbout::~CDialogAbout()
{
    delete ui;
}

void CDialogAbout::SetInfo(QString const &info)
{
    ui->labText->setText(info);
}

void CDialogAbout::mousePressEvent(QMouseEvent * event)
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

void CDialogAbout::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogAbout::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogAbout::on_btnClose_clicked()
{
    close();
    emit confirmSignal(false);
}

void CDialogAbout::on_btnConfirm_clicked()
{
    close();
    emit confirmSignal(true);
}

void CDialogAbout::on_btnCancel_clicked()
{
    close();
    emit confirmSignal(false);
}
