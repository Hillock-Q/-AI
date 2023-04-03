#ifndef WGINTEDITOR_H
#define WGINTEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgIntEditor;
}

class WgIntEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgIntEditor(QWidget *parent = nullptr);
    ~WgIntEditor();

    void Init(QString const &name, int min, int max, int val, int step);
    void SetMax(int max);

signals:
    void signalValueChanged(QString const &name, int val);


private slots:
    void on_spinBoxValue_valueChanged(int val);

private:
    Ui::WgIntEditor*                ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGINTEDITOR_H
