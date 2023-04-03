#ifndef WGINTTABLEEDITOR_H
#define WGINTTABLEEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgIntTableEditor;
}

class WgIntTableEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgIntTableEditor(QWidget *parent = nullptr);
    ~WgIntTableEditor();

    void Init(QString const &name, QStringList const &rows, int rIndex, QStringList const &cols, int cIndex, int min, int max, int val, int step);

signals:
    void signalRowChanged(QString const &name, int rIndex, int &cIndex, int &val);
    void signalColChanged(QString const &name, int rIndex, int cIndex, int &val);
    void signalValueChanged(QString const &name, int rIndex, int cIndex, int val);


private slots:
    void on_comboBoxRow_currentIndexChanged(int rIndex);
    void on_comboBoxCol_currentIndexChanged(int cIndex);
    void on_spinBoxValue_valueChanged(int val);

private:
    Ui::WgIntTableEditor*           ui;

    std::atomic_bool                m_notify = {true};
};

#endif // WGINTTABLEEDITOR_H
