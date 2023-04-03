#ifndef DIALOGGEN_H
#define DIALOGGEN_H

#include <QDialog>

namespace Ui {
class CDialogGen;
}

constexpr int RGB_CNT = 48 - 2;

class CDialogGen : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogGen(QWidget *parent = nullptr);
    ~CDialogGen();

    void SetCatalog(QStringList const &catalog);
    void SetNetNameUsable(bool usable);

    QString GetCatalog() const;
    QString GetNetName() const;
    QString GetShape() const;
    QString GetOutline() const;
    QString GetSpecify() const;
    QColor const& GetColor() const;

signals:
    void netNameChangeSignal(QString const &netId);
    void genConfirmSignal();


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
    void on_btnColor_clicked();

    void on_lineEditCatalog_textChanged(const QString &catalog);
    void on_lineEditNetName_textChanged(const QString &netName);

private:
    void init();
    QColor genColor();
    void updateColorBtn();

private:
    Ui::CDialogGen*                     ui;
    bool                                m_move = {false};
    QPoint                              m_dPos;

    QColor                              m_color;
    bool                                m_catalogUsable = {true};
    bool                                m_netNameUsable = {false};

    unsigned char                       m_r[RGB_CNT];
    unsigned char                       m_g[RGB_CNT];
    unsigned char                       m_b[RGB_CNT];
};

#endif // DIALOGGEN_H
