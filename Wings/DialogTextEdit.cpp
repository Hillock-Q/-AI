#include "DialogTextEdit.h"
#include "ui_DialogTextEdit.h"
#include <QMouseEvent>
#include <QDebug>

constexpr int ITEM_WIDTH = 100;
constexpr int ITEM_HEIGHT = 62;
constexpr int MAX_STRLENGTH = 10;
constexpr int ADJUSR_WIDTH = 5;

CDialogTextEdit::CDialogTextEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogTextEdit)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    ui->lineEdit->installEventFilter(this);
}

CDialogTextEdit::~CDialogTextEdit()
{
    delete ui;
}

void CDialogTextEdit::SetText(QString const &text)
{
    adjustWidth(text);
    ui->lineEdit->setText(text);
}

void CDialogTextEdit::adjustWidth(QString const &text)
{
    auto rc = QRect(0, 0, ITEM_WIDTH, ITEM_HEIGHT);
    if ((int)text.size() > MAX_STRLENGTH)
    {
        auto dw = ADJUSR_WIDTH * ((int)text.size() - MAX_STRLENGTH);
        rc.adjust(-dw, 0, +dw, 0);
    }
    this->setGeometry(rc);
}

void CDialogTextEdit::showEvent(QShowEvent *)
{
    ui->lineEdit->setFocus();
    m_nRunTimerID = this->startTimer(3000);
}

void CDialogTextEdit::timerEvent(QTimerEvent *event)
{
    if (m_nRunTimerID == event->timerId())
    {
        this->killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;

        this->close();
    }
}

bool CDialogTextEdit::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lineEdit && (event->type() == QEvent::Enter || event->type() == QEvent::HoverEnter))
    {
        this->killTimer(m_nRunTimerID);
        m_nRunTimerID = 0;
    }
    else if (object == ui->lineEdit && (event->type() == QEvent::Leave || event->type() == QEvent::HoverLeave))
    {
        this->close();
    }

    return QDialog::eventFilter(object, event);
}

void CDialogTextEdit::on_lineEdit_textChanged(const QString &text)
{
    emit textChangedSignal(text);
}
