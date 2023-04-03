#ifndef WGINTLISTEDITOR_H
#define WGINTLISTEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgIntListEditor;
}

class WgIntListEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgIntListEditor(QWidget *parent = nullptr);
    ~WgIntListEditor();

    void Init(QString const &name, QStringList const &cols, int index, int min, int max, int val, int step);
    void Reset(QStringList const &cols);

signals:
    void signalIndexChanged(QString const &name, int index, int &val);
    void signalValueChanged(QString const &name, int index, int val);


private slots:
    void on_comboBoxIndex_currentIndexChanged(int index);
    void on_spinBoxValue_valueChanged(int val);

private:
    Ui::WgIntListEditor*            ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGINTLISTEDITOR_H
