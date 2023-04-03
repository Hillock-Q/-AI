#include "DesignMgr.h"

CDesignMgr& CDesignMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CDesignMgr());
	return *kp.get();
}

CDesignMgr::CDesignMgr()
{

}

CDesignMgr::~CDesignMgr()
{

}

void CDesignMgr::Init()
{

}
