#include "ProjectMgr.h"

CProjectMgr& CProjectMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CProjectMgr());
	return *kp.get();
}

CProjectMgr::CProjectMgr()
{

}

CProjectMgr::~CProjectMgr()
{

}

void CProjectMgr::Init()
{

}
