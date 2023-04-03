#include "DialogNew.h"
#include "ui_DialogNew.h"
#include <QMouseEvent>
#include <QRegExp>

CDialogNew::CDialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogNew)
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

CDialogNew::~CDialogNew()
{
    delete ui;
}

void CDialogNew::init()
{
    //QRegExp regExp("[A-Za-z0-9]+$");
    //ui->lineEditCatalog->setValidator(new QRegExpValidator(regExp, this));

    ui->lineEditProjectName->clear();
    ui->lineEditProjectName->setFocus();
    ui->lineEditProjectName->setToolTip("");

    m_catalogUsable = true;
    m_projectNameUsable = false;
    ui->btnConfirm->setEnabled(m_catalogUsable && m_projectNameUsable);
}

void CDialogNew::SetCatalog(QStringList const &catalog)
{
    ui->comboBoxCatalog->clear();
    ui->comboBoxCatalog->addItems(catalog);
    //ui->comboBoxCatalog->setCurrentIndex(0);
}

QString CDialogNew::GetCatalog() const
{
    if (ui->btnNew->text() == tr("New"))
    {
        return ui->comboBoxCatalog->currentText().trimmed();
    }
    else
    {
        return ui->lineEditCatalog->text().trimmed();
    }
}
QString CDialogNew::GetProjectName() const
{
    return ui->lineEditProjectName->text().trimmed();
}

void CDialogNew::showEvent(QShowEvent *)
{
    init();
}

void CDialogNew::mousePressEvent(QMouseEvent * event)
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

void CDialogNew::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_move)
        {
            move(event->globalPos() - m_dPos);
        }
    }
}

void CDialogNew::mouseReleaseEvent(QMouseEvent *)
{
    m_move = false;
}

void CDialogNew::on_btnClose_clicked()
{
    close();
}

void CDialogNew::on_btnConfirm_clicked()
{
    emit newConfirmSignal();
    close();
}

void CDialogNew::on_btnCancel_clicked()
{
    close();
}

void CDialogNew::on_btnNew_clicked()
{
    if (ui->btnNew->text() == tr("New"))
    {
        ui->comboBoxCatalog->hide();
        ui->lineEditCatalog->show();
        ui->lineEditCatalog->setFocus();
        ui->btnNew->setText(tr("Existing"));

        m_catalogUsable = isCatalogUsable(ui->lineEditCatalog->text().trimmed());
    }
    else
    {
        ui->lineEditCatalog->hide();
        ui->comboBoxCatalog->show();
        ui->btnNew->setText(tr("New"));

        m_catalogUsable = true;
    }

    bool usable;
    emit projectNameChangeSignal(GetCatalog(), GetProjectName(), usable);
    handleProjectNameUsable(usable);
}

void CDialogNew::on_lineEditCatalog_textChanged(const QString &catalog)
{
    m_catalogUsable = isCatalogUsable(catalog.trimmed());

    bool usable;
    emit projectNameChangeSignal(catalog.trimmed(), GetProjectName(), usable);
    handleProjectNameUsable(usable);
}

void CDialogNew::on_lineEditProjectName_textChanged(const QString &projectName)
{
    if (projectName.isEmpty())
    {
        m_projectNameUsable = false;
        ui->btnConfirm->setEnabled(m_catalogUsable && m_projectNameUsable);
    }
    else
    {
        bool usable;
        emit projectNameChangeSignal(GetCatalog(), projectName.trimmed(), usable);
        handleProjectNameUsable(usable);
    }
}

void CDialogNew::handleProjectNameUsable(bool usable)
{
    m_projectNameUsable = usable;
    ui->btnConfirm->setEnabled(m_catalogUsable && m_projectNameUsable);

    if (usable)
    {
        ui->lineEditProjectName->setToolTip("");
    }
    else
    {
        ui->lineEditProjectName->setToolTip("Name occupied!");
    }
}

bool CDialogNew::isCatalogUsable(QString const &catalog) const
{
    auto usable = catalog.indexOf('/') == -1;

    if (usable)
    {
        ui->lineEditCatalog->setToolTip("");
    }
    else
    {
        ui->lineEditCatalog->setToolTip("Catalog unusable!");
    }

    return usable;
}
