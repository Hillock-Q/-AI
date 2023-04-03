#include "DialogGen.h"
#include "ui_DialogGen.h"
#include <QMouseEvent>
#include <QColorDialog>

CDialogGen::CDialogGen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogGen),
    m_r {
          /*0,*/   0,   0,  85,  85,  85,
        170, 170, 170, 255, 255, 255,
          0,   0,   0,  85,  85,  85,
        170, 170, 170, 255, 255, 255,
          0,   0,   0,  85,  85,  85,
        170, 170, 170, 255, 255, 255,
          0,   0,   0,  85,  85,  85,
        170, 170, 170, 255, 255, /*255*/
        },
    m_g {
          /*0,*/   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,
         85,  85,  85,  85,  85,  85,
         85,  85,  85,  85,  85,  85,
        170, 170, 170, 170, 170, 170,
        170, 170, 170, 170, 170, 170,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, /*255*/
        },
    m_b {
          /*0,*/ 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, 255,
          0, 127, 255,   0, 127, /*255*/
        }
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    ui->lineEditCatalog->hide();
    ui->comboBoxCatalog->show();

    init();
}

CDialogGen::~CDialogGen()
{
    delete ui;
}

void CDialogGen::SetCatalog(QStringList const &catalog)
{
    ui->comboBoxCatalog->clear();
    ui->comboBoxCatalog->addItems(catalog);
    //ui->comboBoxCatalog->setCurrentIndex(0);
}

void CDialogGen::SetNetNameUsable(bool usable)
{
    m_netNameUsable = usable;
    ui->btnConfirm->setEnabled(m_catalogUsable && m_netNameUsable);

    if (usable)
    {
        ui->lineEditNetName->setToolTip("");
    }
    else
    {
        ui->lineEditNetName->setToolTip("Name occupied!");
    }
}

QString CDialogGen::GetCatalog() const
{
    if (ui->btnNew->text() == tr("New"))
    {
        return ui->comboBoxCatalog->currentText();
    }
    else
    {
        return ui->lineEditCatalog->text();
    }
}

QString CDialogGen::GetNetName() const
{
    return ui->lineEditNetName->text();
}

QString CDialogGen::GetShape() const
{
    return ui->comboBoxShape->currentText();
}

QString CDialogGen::GetOutline() const
{
    return ui->lineEditOutline->text();
}

QString CDialogGen::GetSpecify() const
{
    return ui->textEditSpecify->toPlainText();
}

QColor const& CDialogGen::GetColor() const
{
    return m_color;
}

void CDialogGen::init()
{
    ui->textEditSpecify->clear();
    ui->lineEditOutline->clear();
    ui->lineEditNetName->clear();
    ui->lineEditNetName->setFocus();

    m_netNameUsable = false;
    ui->btnConfirm->setEnabled(m_catalogUsable && m_netNameUsable);

    m_color = genColor();
    updateColorBtn();
}

QColor CDialogGen::genColor()
{
    static int colorIndex = 0;

    auto color = QColor(m_r[colorIndex % RGB_CNT], m_g[colorIndex % RGB_CNT], m_b[colorIndex % RGB_CNT]);
    colorIndex++;

    return color;
}

void CDialogGen::updateColorBtn()
{
    QString bgColor = "rgb(" + QString::number(m_color.red()) + ", ";
            bgColor += QString::number(m_color.green()) + ", ";
            bgColor += QString::number(m_color.blue()) + ")";

    QString style = "QPushButton {";
                style += "background-color: " + bgColor + ";";
                style += "border:2px solid rgb(184, 199, 219);";
                style += "border-radius:5px;";
            style += "}";

    ui->btnColor->setStyleSheet(style);
}

void CDialogGen::showEvent(QShowEvent *)
{
    init();
}

void CDialogGen::mousePressEvent(QMouseEvent * event)
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

void CDialogGen::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogGen::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogGen::on_btnClose_clicked()
{
    close();
}

void CDialogGen::on_btnConfirm_clicked()
{
    emit genConfirmSignal();
    close();
}

void CDialogGen::on_btnCancel_clicked()
{
    close();
}

void CDialogGen::on_btnNew_clicked()
{
    if (ui->btnNew->text() == tr("New"))
    {
        ui->comboBoxCatalog->hide();
        ui->lineEditCatalog->show();
        ui->lineEditCatalog->setFocus();
        ui->btnNew->setText(tr("Existing"));

        m_catalogUsable = ui->lineEditCatalog->text().size();
    }
    else
    {
        ui->lineEditCatalog->hide();
        ui->comboBoxCatalog->show();
        ui->btnNew->setText(tr("New"));

        m_catalogUsable = true;
    }

    ui->btnConfirm->setEnabled(m_catalogUsable && m_netNameUsable);
}

void CDialogGen::on_btnColor_clicked()
{
    m_color = QColorDialog::getColor(
                m_color,
                this,
                tr("Color Picker"),
                QColorDialog::ShowAlphaChannel);

    updateColorBtn();
}

void CDialogGen::on_lineEditCatalog_textChanged(const QString &catalog)
{
    m_catalogUsable = catalog.size();
    ui->btnConfirm->setEnabled(m_catalogUsable && m_netNameUsable);
}

void CDialogGen::on_lineEditNetName_textChanged(const QString &netName)
{
    if (netName.isEmpty())
    {
        m_netNameUsable = false;
        ui->btnConfirm->setEnabled(m_catalogUsable && m_netNameUsable);
    }
    else
    {
        emit netNameChangeSignal(netName);
    }
}
