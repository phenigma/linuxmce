#ifndef __IINPUT_PROVIDER__
#define __IINPUT_PROVIDER__
//--------------------------------------------------------------------------------------------------------
class IInputProvider
{
public:

	IInputProvider(IInputProcessor *pInputProcessor) : m_pInputProcessor(pInputProcessor) 
	{
	}
	
	virtual ~IInputProvider() 
	{
	}	

	bool HandleInput(IInput *pInput)
	{
		if(NULL != m_pInputProcessor)
			return m_pInputProcessor->ProcessInput(pInput);

		return false;
	}

private:

	IInputProcessor *m_pInputProcessor;
};
//--------------------------------------------------------------------------------------------------------
#endif //__IINPUT_PROVIDER__
