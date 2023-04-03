#ifndef WGTEXTEDITOR_H
#define WGTEXTEDITOR_H

#include <QWidget>
#include <atomic>

namespace Ui {
class WgTextEditor;
}

class WgTextEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WgTextEditor(QWidget *parent = nullptr);
    ~WgTextEditor();

    void Init(QString const &name, QString const &text);

signals:
    void signalTextChanged(QString const &name, QString const &text);


private slots:
    void on_lineEdit_textChanged(const QString &text);

private:
    Ui::WgTextEditor*               ui;

    std::atomic_bool                m_notify = {false};
};

#endif // WGTEXTEDITOR_H
