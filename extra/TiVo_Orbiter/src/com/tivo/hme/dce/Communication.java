package com.tivo.hme.dce;

public class Communication implements MessageReceiver
{
	private EventHandler m_pEventHandler = null;
	private CommandHandler m_pCommandHandler = null;
	private CommandHandlerAsync m_pCommandHandlerAsync = null;
	private MessageProcessor m_pMessageProcessor = null;
	
	public Communication(MessageProcessor pMessageProcessor, String sHost, int nPort, int nDevice)
	{
		m_pMessageProcessor = pMessageProcessor;
		m_pEventHandler = new EventHandler(sHost, nPort, nDevice);
		m_pCommandHandler = new CommandHandler(sHost, nPort, nDevice);
	}
	
	public boolean Connect()
	{
		boolean bConnected = m_pEventHandler.Connect() && m_pCommandHandler.Connect();
		
		if(bConnected)
		{
			m_pCommandHandlerAsync = new CommandHandlerAsync(this);
			new Thread(m_pCommandHandlerAsync).start();
	     }		
	     
	     return bConnected;
	}

	public void Disconnect()
	{
		m_pEventHandler.Disconnect();
		m_pCommandHandler.Disconnect();
	}	
	
	public boolean IsConnected()
	{
		return m_pEventHandler.IsConnected() && m_pCommandHandler.IsConnected();
	}
	
	public EventHandler EventHandler()
	{
		return m_pEventHandler;
	}
	
	public void WaitForMessages()
	{
		while(true)
		{
			if(IsConnected())
			{
				Message message = m_pCommandHandler.ReceivedMessage();
				if(null != message)
					m_pMessageProcessor.ProcessMessage(message);
			}
			else
			{
				try
				{
					Thread.sleep(10000);
				}
				catch (Exception e) 
				{
					System.out.println("Failed to sleep. Error: " + e.getMessage());
				}
			}
		}
	}
}
