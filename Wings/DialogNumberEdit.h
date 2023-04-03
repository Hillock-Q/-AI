#ifndef DIALOGNUMBEREDIT_H
#define DIALOGNUMBEREDIT_H

#include <QDialog>

namespace Ui {
class CDialogNumberEdit;
}

class CDialogNumberEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogNumberEdit(QWidget *parent = nullptr);
    ~CDialogNumberEdit();

    void SetNumber(int min, int max, int value);

signals:
    void numberChangedSignal(int val);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
    virtual bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::CDialogNumberEdit*              ui;
    int                                 m_nRunTimerID = {0};
};

#endif // DIALOGNUMBEREDIT_H
