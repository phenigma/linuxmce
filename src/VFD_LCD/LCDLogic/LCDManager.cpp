#include "LCDManager.h"
#include "MenuHolder.h"
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
#define REFRESH_INTERVAL	400
#define IDLE_INTERVAL		100
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_RenderMutex("render"), 
	m_RerendererThreadID(0), m_ulRendererInterval(0), m_bQuit(false)
{
	m_display_state.m_sStatusMessage = "Welcome to Fiire 1.0!";

	m_RenderMutex.Init(NULL);

	//Testing
	SetVariable("{IP}", "192.168.89.2");
	SetVariable("{NETMASK}", "255.255.255.0");
	SetVariable("{GATEWAY}", "192.168.89.1");
	SetVariable("{DNS1}", "192.168.89.1");
	SetVariable("{DNS2}", "10.0.0.1");
}
//--------------------------------------------------------------------------------------------------------
LCDManager::~LCDManager()
{
	m_bQuit = true;

	if(m_RerendererThreadID)
		pthread_join(m_RerendererThreadID, 0);

	pthread_mutex_destroy(&m_RenderMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::AddRenderer(IRenderer *pRenderer)
{
	if(NULL != pRenderer)
	{
		PLUTO_SAFETY_LOCK(sm, m_RenderMutex);
		m_listRenderer.push_back(pRenderer);

		Render();
	}
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::ProcessInput(const Input &input)
{
	PLUTO_SAFETY_LOCK(sm, m_RenderMutex);

	if(NULL == m_pMenuHolder)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got not menu holder. The input will be ignored");
		return false;
	}

	//process the input
	switch(input.type)
	{
		case itKeyboardInput:
		{
			
			{
				switch(input.keyboardinput)
				{
					case kiUp :		
						m_pMenuHolder->MoveUp();
						break;
					case kiDown :	
						m_pMenuHolder->MoveDown();
						break;
					case kiLeft :	
						m_pMenuHolder->MoveLeft();
						break;
					case kiRight :	
						m_pMenuHolder->MoveRight();
						break;
					default :	
						break;
				}
			}
		}
		break;

		case itChangeStatus:
			m_display_state.m_sStatusMessage = input.status;
			m_display_state.m_sProgress = input.progress;
		break;

		default:
			break;
	}

	//decide which is current node
	//display on renderer
	//execute action for current node, if any

	Render();

	return true;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Render()
{
	CancelRerender();

	Prepare();

	for(list<IRenderer *>::iterator it = m_listRenderer.begin(), end = m_listRenderer.end(); it != end; ++it)
		(*it)->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Prepare()
{
	if(NULL == m_pMenuHolder || NULL == m_pMenuHolder->CurrentMenuItem())
	{
		m_display_state.m_sHeader = "Got no menu";
		return;
	}

	switch(m_pMenuHolder->CurrentMenuItem()->Type())
	{
		case itStatusItem:
		{
			m_display_state.m_sHeader = m_display_state.m_sStatusMessage;
		}
		break;

		case itListItem:
		{
			m_display_state.m_sHeader = ListItemDescription(m_pMenuHolder->CurrentMenuItem());
		}
		break;

		case itInputBox:
		{
			m_display_state.m_sHeader = m_pMenuHolder->CurrentMenuItem()->Description();
			m_display_state.m_sStatusMessage = m_pMenuHolder->CurrentMenuItem()->Status();
			ScheduleRerender(REFRESH_INTERVAL);
		}
		break;
	}

	//navigation
	m_display_state.m_bCanGoDown = m_pMenuHolder->CurrentMenuItem()->CanGoDown();
	m_display_state.m_bCanGoUp = m_pMenuHolder->CurrentMenuItem()->CanGoUp();
	m_display_state.m_bCanGoLeft = m_pMenuHolder->CurrentMenuItem()->CanGoLeft();
	m_display_state.m_bCanGoRight = m_pMenuHolder->CurrentMenuItem()->CanGoRight();

	//reset last state
	m_display_state.m_vectNextOptions.clear();
	m_display_state.m_vectPriorOptions.clear();

	//prev options
	int nIndex = ADIACENT_OPTIONS_NUMBER;
	MenuItem *pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0 && NULL != pCurrentItem->Parent())
	{
		MenuItem *pPrevItem = pCurrentItem->Parent()->PrevChild(pCurrentItem);
		if(NULL != pPrevItem)
		{
            m_display_state.m_vectPriorOptions.push_back(ListItemDescription(pPrevItem));
			pCurrentItem = pPrevItem;
		}
		else
			break;
	}

	//next options
	nIndex = ADIACENT_OPTIONS_NUMBER;
	pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0 && NULL != pCurrentItem->Parent())
	{
		MenuItem *pNextItem = pCurrentItem->Parent()->NextChild(pCurrentItem);
		if(NULL != pNextItem)
		{
            m_display_state.m_vectNextOptions.push_back(ListItemDescription(pNextItem));
			pCurrentItem = pNextItem;
		}
		else
			break;
	}
}
//--------------------------------------------------------------------------------------------------------
void *RerenderThread(void *p)
{
	LCDManager *pManager = reinterpret_cast<LCDManager *>(p);

	while(NULL != pManager && !pManager->Quit())
	{
		if(pManager->RerenderInterval())
		{
			pManager->Render(); 
			Sleep(pManager->RerenderInterval());
		}
		else
		{
            Sleep(IDLE_INTERVAL);
		}
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
unsigned long LCDManager::RerenderInterval()
{
	return m_ulRendererInterval;
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::Quit()
{
	return m_bQuit;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::CancelRerender()
{
	m_ulRendererInterval = 0;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::ScheduleRerender(unsigned long ulMiliseconds)
{
	m_ulRendererInterval = ulMiliseconds;

	if(!m_RerendererThreadID)
		pthread_create(&m_RerendererThreadID, NULL, RerenderThread, (void *)this);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::SetVariable(string sVar, string sValue)
{
	m_mapVariables[sVar] = sValue;
}
//--------------------------------------------------------------------------------------------------------
string LCDManager::ListItemDescription(MenuItem *pMenuItem)
{
	if(!pMenuItem->Value().empty())
		return pMenuItem->Description() + ": " + m_mapVariables[pMenuItem->Value()];

	return pMenuItem->Description();
}
//--------------------------------------------------------------------------------------------------------
