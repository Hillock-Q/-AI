#ifndef WGSTRSELECTOR_H
#define WGSTRSELECTOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgStrSelector;
}

class WgStrSelector : public QWidget
{
    Q_OBJECT

public:
    explicit WgStrSelector(QWidget *parent = nullptr);
    ~WgStrSelector();

    void Init(QString const &name, QStringList const &rows, QString const &selected);

signals:
    void signalSelectedChanged(QString const &name, QString const &selected);


private slots:
    void on_comboBoxValue_currentIndexChanged(const QString &selected);

private:
    Ui::WgStrSelector*              ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGSTRSELECTOR_H
