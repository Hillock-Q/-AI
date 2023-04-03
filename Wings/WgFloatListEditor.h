#ifndef WGFLOATLISTEDITOR_H
#define WGFLOATLISTEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgFloatListEditor;
}

class WgFloatListEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgFloatListEditor(QWidget *parent = nullptr);
    ~WgFloatListEditor();

    void Init(QString const &name, QStringList const &cols, int index, double min, double max, double val, double step, int prec);
    void Reset(QStringList const &cols);

signals:
    void signalIndexChanged(QString const &name, int index, double &val);
    void signalValueChanged(QString const &name, int index, double val);


private slots:
    void on_comboBoxIndex_currentIndexChanged(int index);
    void on_doubleSpinBoxValue_valueChanged(double val);

private:
    Ui::WgFloatListEditor*          ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGFLOATLISTEDITOR_H
