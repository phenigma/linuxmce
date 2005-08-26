#include "DeviceData_Router.h"
#include "OH_Orbiter.h"

using namespace DCE;

bool OH_Orbiter::NeedVMC()
{
    m_pDeviceData_Router->m_pRow_Device->Reload();
    return string::npos != m_pDeviceData_Router->m_sState_get().find(NEED_VMC_TOKEN);
}

void OH_Orbiter::NeedVMC(bool bTrue)
{
    if(NeedVMC() != bTrue) //need to change state
        SetState(bTrue, NeedApp());
}

bool OH_Orbiter::NeedApp()
{
    m_pDeviceData_Router->m_pRow_Device->Reload();
    return string::npos != m_pDeviceData_Router->m_sState_get().find(NEED_APP_TOKEN);
}

void OH_Orbiter::NeedApp(bool bTrue)
{
    if(NeedApp() != bTrue) //need to change state
        SetState(NeedVMC(), bTrue);
}

void OH_Orbiter::SetState(bool bNeedVmc, bool bNeedApp)
{
    m_pDeviceData_Router->m_sState_set(
        (bNeedVmc ? NEED_VMC_TOKEN + string(" | ") : string()) +
        (bNeedApp ? NEED_APP_TOKEN + string(" | ") : string())
    );
}