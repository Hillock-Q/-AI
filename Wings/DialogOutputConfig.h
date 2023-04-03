#ifndef DIALOGOUTPUTCONFIG_H
#define DIALOGOUTPUTCONFIG_H

#include <QDialog>
#include <vector>

using std::pair;
using std::vector;
using ChannelPairs_t = vector<pair<int, int>>;

namespace Ui {
class CDialogOutputConfig;
}

class CDelegate;
class QTableWidgetItem;

class CDialogOutputConfig : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogOutputConfig(QWidget *parent = nullptr);
    ~CDialogOutputConfig();

    void Config(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt);

signals:
    void addOutputChannelSignal(int outChannel, int inChannel);
    void delOutputChannelSignal(int outChannel, int inChannel);
    void modifyOutputChannelSignal(int oldOutChannel, int inChannel, int newOutChannel);
    void swapOutputInChannelSignal(int inChannel1, int inChannel2);

protected:
    void mousePressEvent(QMouseEvent*event) override;
    void mouseMoveEvent(QMouseEvent*event) override;
    void mouseReleaseEvent(QMouseEvent*event) override;

private slots:
    void on_btnClose_clicked();
    void on_btnConfirm_clicked();

    void tableHoverIndexChangedSlot(const QModelIndex &index);
    void tableSelectedIndexChangedSlot(const QModelIndex &index);

    void tableCellDoubleClicked(int row, int column);
    void tableItemChangedSlot(QTableWidgetItem *item);

    void checkSlot(void const *inuseChecker, bool checked);

private:
    int getInChannel(int outChannel, ChannelPairs_t const &channelPairs) const;
    void resetTable(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt);
    void updateTable(int outChannelCnt, ChannelPairs_t const &channelPairs, int inChannelCnt);
    int getRow(void const *inuseChecker);

private:
    Ui::CDialogOutputConfig*            ui;
    bool                                m_move = {false};
    QPoint                              m_dPos;

    CDelegate*                          m_pDelegate = {nullptr};

    QString                             m_oldVal;
    bool                                m_cellClicked = {false};
    int                                 m_cellRow = {-1};
    int                                 m_cellCol = {-1};
    int                                 m_inChannelCnt = {0};
    vector<void*>                       m_inuseCheckers;
};

#endif // DIALOGOUTPUTCONFIG_H
