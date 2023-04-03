#ifndef NETMARK_H
#define NETMARK_H

#include <QString>

struct NetMark
{
    int                 sn;
    QString             net;
    QString             name;

    NetMark(int _sn = -1, QString const &_net = "", QString const &_name = "")
        : sn(_sn), net(_net), name(_name)
    {}

    void Reset() {
        sn = -1;
        net.clear();
        name.clear();
    }

    bool operator==(NetMark const &rhs) const
    {
        return sn == rhs.sn && net == rhs.net;
    }

    bool operator!=(NetMark const &rhs) const
    {
        return !operator==(rhs);
    }
};

#endif // NETMARK_H
