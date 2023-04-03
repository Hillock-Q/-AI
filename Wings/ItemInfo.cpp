#include "ItemInfo.h"

CItemInfo::CItemInfo(int sn)
    : m_sn(sn)
{

}

CItemInfo::~CItemInfo()
{

}

int CItemInfo::GetSn() const
{
    return m_sn;
}

void CItemInfo::SetSn(int sn)
{
    m_sn = sn;
}

int CItemInfo::GetIn() const
{
    return m_in;
}

void CItemInfo::SetIn(int in)
{
    m_in = in;
}

int CItemInfo::GetOut() const
{
    return m_out;
}

void CItemInfo::SetOut(int out)
{
    m_out = out;
}

bool CItemInfo::AsInput() const
{
    return m_asInput;
}

void CItemInfo::AsInput(bool as)
{
    m_asInput = as;
}

bool CItemInfo::AsOutput() const
{
    return m_asOutput;
}

void CItemInfo::AsOutput(bool as)
{
    m_asOutput = as;
}

bool CItemInfo::GetInputPoint(PointInfo &point) const
{
    for (auto const &item : m_points)
        if (item.role == e_input)
        {
            point = item;
            return true;
        }

    return false;
}

void CItemInfo::SetInputPoint(PointInfo const &point)
{
    for (auto &item : m_points)
        if (item.role == e_input)
        {
            item = point;
            return;
        }
}

bool CItemInfo::GetOutputPoint(PointInfo &point) const
{
    for (auto const &item : m_points)
        if (item.role == e_output)
        {
            point = item;
            return true;
        }

    return false;
}

void CItemInfo::SetOutputPoint(PointInfo const &point)
{
    for (auto &item : m_points)
        if (item.role == e_output)
        {
            item = point;
            return;
        }
}

Points_t const& CItemInfo::GetPoints() const
{
    return m_points;
}

void CItemInfo::SetPoints(Points_t const &points)
{
    m_points = points;
}

void CItemInfo::AddPoint(PointInfo const &pInfo)
{
    m_points.push_back(pInfo);
}

void CItemInfo::DelPoint(int sn)
{
    for (auto itor = m_points.begin(); itor != m_points.end(); ++itor)
        if (itor->sn == sn)
        {
            m_points.erase(itor);
            break;
        }
}

void CItemInfo::UpdatePos(int sn, QPointF const &pos)
{
    for (auto &item : m_points)
        if (item.sn == sn)
            item.pos = pos;
}
