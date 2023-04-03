#ifndef RUNMGR_H
#define RUNMGR_H

class CRunMgr
{
public:
    static CRunMgr& ins();


private:
    CRunMgr();
    ~CRunMgr();

    CRunMgr(CRunMgr &&rhs) = delete;
    CRunMgr(CRunMgr const &rhs) = delete;

    CRunMgr& operator=(CRunMgr &&rhs) = delete;
    CRunMgr& operator=(CRunMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CRunMgr *pt = 0) : p(pt){}
        CRunMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CRunMgr *p;
    };


private:

};

#endif // RUNMGR_H
