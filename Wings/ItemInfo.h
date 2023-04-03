#ifndef CITEMINFO_H
#define CITEMINFO_H

#include <vector>
#include <QPointF>

using std::pair;
using std::vector;

enum ROLE
{
    e_src,
    e_dst,
    e_input,
    e_output,
};

using ChannelPairs_t = vector<pair<int, int>>;

struct PointInfo
{
    int                 sn;
    int                 owner;
    int                 peer;
    int                 arrow;
    int                 channel;
    ROLE                role;
    QPointF             pos;
    ChannelPairs_t      channels; // for input/output, pair<outChannel, inChannel>
};

using Points_t = vector<PointInfo>;

class CItemInfo
{
public:
    explicit CItemInfo(int sn);
    ~CItemInfo();

    int GetSn() const;
    void SetSn(int sn);

    int GetIn() const;
    void SetIn(int in);

    int GetOut() const;
    void SetOut(int out);

    bool AsInput() const;
    void AsInput(bool as);

    bool AsOutput() const;
    void AsOutput(bool as);

    bool GetInputPoint(PointInfo &point) const;
    void SetInputPoint(PointInfo const &point);

    bool GetOutputPoint(PointInfo &point) const;
    void SetOutputPoint(PointInfo const &point);

    Points_t const& GetPoints() const;
    void SetPoints(Points_t const &points);

    void AddPoint(PointInfo const &pInfo);
    void DelPoint(int sn);
    void UpdatePos(int sn, QPointF const &pos);

protected:
    int                         m_sn;

    int                         m_in = {0};
    int                         m_out = {0};

    bool                        m_asInput = {false};
    bool                        m_asOutput = {false};

    Points_t                    m_points;
};

#endif // CITEMINFO_H
