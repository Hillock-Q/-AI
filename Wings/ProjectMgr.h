#ifndef PROJECTMGR_H
#define PROJECTMGR_H

#include <QObject>

class CProjectMgr: public QObject
{
    Q_OBJECT

public:
    static CProjectMgr& ins();

    void Init();


signals:
    void showInfoSignal(QString const &info);
    void showMsgDialogSignal(QString const &msg);


private slots:


private:
    CProjectMgr();
    ~CProjectMgr();

    CProjectMgr(CProjectMgr &&rhs) = delete;
    CProjectMgr(CProjectMgr const &rhs) = delete;

    CProjectMgr& operator=(CProjectMgr &&rhs) = delete;
    CProjectMgr& operator=(CProjectMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CProjectMgr *pt = 0) : p(pt){}
        CProjectMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CProjectMgr *p;
    };


private:

};

#endif // PROJECTMGR_H
