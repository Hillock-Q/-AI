#ifndef INUSECHECK_H
#define INUSECHECK_H

#include <QWidget>

namespace Ui {
class CInUseCheck;
}

class CInUseCheck : public QWidget
{
    Q_OBJECT

public:
    explicit CInUseCheck(QWidget *parent = nullptr);
    ~CInUseCheck();

    bool isChecked() const;
    void SetChecked(bool checked);

signals:
    void checkSignal(void const *self, bool checked);


private slots:
    void on_radioButton_clicked(bool checked);

private:
    Ui::CInUseCheck *ui;
};

#endif // INUSECHECK_H
