#include "RunMgr.h"

CRunMgr& CRunMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CRunMgr());
	return *kp.get();
}

CRunMgr::CRunMgr()
{

}

CRunMgr::~CRunMgr()
{

}
