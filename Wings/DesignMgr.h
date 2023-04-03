#ifndef DESIGNMGR_H
#define DESIGNMGR_H

#include <QObject>

class CDesignMgr: public QObject
{
    Q_OBJECT

public:
    static CDesignMgr& ins();

    void Init();


signals:
    void showInfoSignal(QString const &info);
    void showMsgDialogSignal(QString const &msg);


private slots:


private:
    CDesignMgr();
    ~CDesignMgr();

    CDesignMgr(CDesignMgr &&rhs) = delete;
    CDesignMgr(CDesignMgr const &rhs) = delete;

    CDesignMgr& operator=(CDesignMgr &&rhs) = delete;
    CDesignMgr& operator=(CDesignMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CDesignMgr *pt = 0) : p(pt){}
        CDesignMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CDesignMgr *p;
    };


private:

};

#endif // DESIGNMGR_H
