#ifndef DIALOGAIPARAMSVIEWT_H
#define DIALOGAIPARAMSVIEWT_H

#include <QDialog>

namespace Ui {
class CDialogAiParamsView;
}

class QTreeWidget;

class CDialogAiParamsView : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogAiParamsView(QWidget *parent = nullptr);
    ~CDialogAiParamsView();

    QTreeWidget* GetAiParamsView() const;


private:
    Ui::CDialogAiParamsView*              ui;
};

#endif // DIALOGAIPARAMSVIEWT_H
