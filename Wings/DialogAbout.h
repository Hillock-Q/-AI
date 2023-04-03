#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

namespace Ui {
class CDialogAbout;
}

class CDialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogAbout(QWidget *parent = nullptr);
    ~CDialogAbout();
    void SetInfo(QString const &info);

signals:
    void confirmSignal(bool flag);

protected:
    void mousePressEvent(QMouseEvent*event) override;
    void mouseMoveEvent(QMouseEvent*event) override;
    void mouseReleaseEvent(QMouseEvent*event) override;

private slots:
    void on_btnClose_clicked();
    void on_btnConfirm_clicked();
    void on_btnCancel_clicked();

private:
    Ui::CDialogAbout*                   ui;
    bool                                m_move = {false};
    QPoint                              m_dPos;
};

#endif // DIALOGMESSAGEINFO_H
