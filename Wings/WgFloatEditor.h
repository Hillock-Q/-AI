#ifndef WGFLOATTEDITOR_H
#define WGFLOATTEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgFloatEditor;
}

class WgFloatEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgFloatEditor(QWidget *parent = nullptr);
    ~WgFloatEditor();

    void Init(QString const &name, double min, double max, double val, double step, int prec);

signals:
    void signalValueChanged(QString const &name, double val);


private slots:
    void on_doubleSpinBoxValue_valueChanged(double val);

private:
    Ui::WgFloatEditor*              ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGFLOATTEDITOR_H
