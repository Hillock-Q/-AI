#ifndef DIALOGNEW_H
#define DIALOGNEW_H

#include <QDialog>

namespace Ui {
class CDialogNew;
}

class CDialogNew : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogNew(QWidget *parent = nullptr);
    ~CDialogNew();

    void SetCatalog(QStringList const &catalog);

    QString GetCatalog() const;
    QString GetProjectName() const;

signals:
    void projectNameChangeSignal(QString const &catalog, QString const &name, bool &usable);
    void newConfirmSignal();


protected:
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent*event) override;
    void mouseMoveEvent(QMouseEvent*event) override;
    void mouseReleaseEvent(QMouseEvent*event) override;

private slots:
    void on_btnClose_clicked();
    void on_btnConfirm_clicked();
    void on_btnCancel_clicked();
    void on_btnNew_clicked();

    void on_lineEditCatalog_textChanged(const QString &catalog);
    void on_lineEditProjectName_textChanged(const QString &projectName);

private:
    void init();
    void handleProjectNameUsable(bool usable);
    bool isCatalogUsable(QString const &catalog) const;

private:
    Ui::CDialogNew*                     ui;
    bool                                m_move = {false};
    QPoint                              m_dPos;

    bool                                m_catalogUsable = {true};
    bool                                m_projectNameUsable = {false};
};

#endif // DIALOGNEW_H
