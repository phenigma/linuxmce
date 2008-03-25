package com.tivo.hme.dce;

/*
 * class CommandHandler : receives messages from other devices
 */
public class CommandHandler extends ClientSocket
{
	CommandHandler(String sHost, int nPort, int nDevice)
	{
		super(sHost, nPort, nDevice);
	}
	
	public boolean Connect() 
	{
		if(super.Connect())
		{
			// Send EVENT my_device_id '\n'
			SendString("COMMAND " + (new Integer(m_nDevice)).toString());

			// Receive the response
			String sResponse = ReceivedString();
			System.out.println("Router responded with " + sResponse);

			if (0 == sResponse.compareTo("OK"))
				return true;
			
			System.out.println("Handshake has failed for CommandHandler. Disconnecting...");
			
			Disconnect();
		}
		
		return false;
	}
}
