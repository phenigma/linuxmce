#include "LCDManager.h"
#include "MenuHolder.h"
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_RenderMutex("render")
{
	m_display_state.m_sHeader = "Welcome to Fiire 1.0!";

	m_RenderMutex.Init(NULL);
}
//--------------------------------------------------------------------------------------------------------
LCDManager::~LCDManager()
{
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
	Prepare();

	for(list<IRenderer *>::iterator it = m_listRenderer.begin(), end = m_listRenderer.end(); it != end; ++it)
		(*it)->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Prepare()
{
	if(NULL != m_pMenuHolder && NULL != m_pMenuHolder->CurrentMenuItem())
		m_display_state.m_sHeader = m_pMenuHolder->CurrentMenuItem()->Description();

	m_display_state.m_bCanGoDown = m_pMenuHolder->CurrentMenuItem()->CanGoDown();
	m_display_state.m_bCanGoUp = m_pMenuHolder->CurrentMenuItem()->CanGoUp();
	m_display_state.m_bCanGoLeft = m_pMenuHolder->CurrentMenuItem()->CanGoLeft();
	m_display_state.m_bCanGoRight = m_pMenuHolder->CurrentMenuItem()->CanGoRight();

	m_display_state.m_vectNextOptions.clear();
	m_display_state.m_vectPriorOptions.clear();

	//prev options
	int nIndex = ADIACENT_OPTIONS_NUMBER;
	MenuItem *pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0)
	{
		MenuItem *pPrevItem = pCurrentItem->Parent()->PrevChild(pCurrentItem);
		if(NULL != pPrevItem)
		{
            m_display_state.m_vectPriorOptions.push_back(pPrevItem->Description());
			pCurrentItem = pPrevItem;
		}
		else
			break;
	}

	//next options
	nIndex = ADIACENT_OPTIONS_NUMBER;
	pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0)
	{
		MenuItem *pNextItem = pCurrentItem->Parent()->NextChild(pCurrentItem);
		if(NULL != pNextItem)
		{
            m_display_state.m_vectNextOptions.push_back(pNextItem->Description());
			pCurrentItem = pNextItem;
		}
		else
			break;
	}
}
//--------------------------------------------------------------------------------------------------------
