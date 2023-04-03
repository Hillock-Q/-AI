#ifndef DIALOGTEXTEDIT_H
#define DIALOGTEXTEDIT_H

#include <QDialog>

namespace Ui {
class CDialogTextEdit;
}

class CDialogTextEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogTextEdit(QWidget *parent = nullptr);
    ~CDialogTextEdit();

    void SetText(QString const &text);

signals:
    void textChangedSignal(const QString &text);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
    virtual bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    void adjustWidth(QString const &text);

private:
    Ui::CDialogTextEdit*                ui;
    int                                 m_nRunTimerID = {0};
};

#endif // DIALOGTEXTEDIT_H
