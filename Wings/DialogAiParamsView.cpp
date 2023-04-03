#include "DialogAiParamsView.h"
#include "ui_DialogAiParamsView.h"

CDialogAiParamsView::CDialogAiParamsView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogAiParamsView)
{
    ui->setupUi(this);

    setWindowOpacity(0.913);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_ShowModal, false);
    setWindowModality(Qt::NonModal);
    setModal(false);
}

CDialogAiParamsView::~CDialogAiParamsView()
{
    delete ui;
}

QTreeWidget* CDialogAiParamsView::GetAiParamsView() const
{
    return ui->treeWidget;
}
