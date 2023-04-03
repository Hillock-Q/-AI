#ifndef WGCTRLLIST_H
#define WGCTRLLIST_H

#include <QWidget>

namespace Ui {
class WgCtrlList;
}

class WgCtrlList : public QWidget
{
    Q_OBJECT

public:
    explicit WgCtrlList(QString const &catalogName, QWidget *parent = nullptr);
    ~WgCtrlList();

    QString const& GetCatalogName() const;

    void AddNet(QString const &netId, QColor const &color);
    void DelNet(QString const &netId);
    void Update();

signals:
    void signalUpdate(QString const &catalogName);
    void signalUseNet(QString const &catalogName, QString const &netId);
    void signalAddNet(QString const &catalogName, QString const &netId, QColor const &color);


private slots:
    void on_tbtnCatalog_clicked(bool checked);

    void useNetSlot(QString const &netId);
    void addNetSlot(QString const &netId, QColor const &color);

private:
    void resizeHeight();

private:
    Ui::WgCtrlList*                 ui;
    QString                         m_catalogName;
};

#endif // WGCTRLLIST_H
