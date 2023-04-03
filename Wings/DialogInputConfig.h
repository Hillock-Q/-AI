#ifndef DIALOGINPUTCONFIG_H
#define DIALOGINPUTCONFIG_H

#include <QDialog>
#include <vector>

using std::pair;
using std::vector;
using ChannelPairs_t = vector<pair<int, int>>;

namespace Ui {
class CDialogInputConfig;
}

class CDelegate;
class QTableWidgetItem;

class CDialogInputConfig : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogInputConfig(QWidget *parent = nullptr);
    ~CDialogInputConfig();

    void Config(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt);

signals:
    void addInputChannelSignal(int outChannel, int inChannel);
    void delInputChannelSignal(int outChannel, int inChannel);
    void modifyInputChannelSignal(int oldOutChannel, int inChannel, int newOutChannel);
    void swapInputInChannelSignal(int inChannel1, int inChannel2);

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
    int getOutChannel(int inChannel, ChannelPairs_t const &channelPairs) const;
    void resetTable(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt);
    void updateTable(int outChannelCnt, vector<int> const &outChannelShareCnt, ChannelPairs_t const &channelPairs, vector<int> const &inChannels, int inChannelCnt);
    int getRow(void const *inuseChecker);

private:
    Ui::CDialogInputConfig*      ui;
    bool                                m_move = {false};
    QPoint                              m_dPos;

    CDelegate*                          m_pDelegate = {nullptr};

    QString                             m_oldVal;
    bool                                m_cellClicked = {false};
    int                                 m_cellRow = {-1};
    int                                 m_cellCol = {-1};
    int                                 m_inChannelCnt = {0};
    int                                 m_outChannelCnt = {0};
    vector<int>                         m_outChannelShareCnt;
    vector<void*>                       m_inuseCheckers;
};

#endif // DIALOGINPUTCONFIG_H
