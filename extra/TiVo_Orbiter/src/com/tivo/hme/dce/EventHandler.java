package com.tivo.hme.dce;

/*
 * class EventHandler : sends messages to other devices
 */
public class EventHandler extends ClientSocket
{
	EventHandler(String sHost, int nPort, int nDevice)
	{
		super(sHost, nPort, nDevice);
	}
	
	public boolean Connect() 
	{
		if(super.Connect())
		{
			// Send EVENT my_device_id '\n'
			SendString("EVENT " + (new Integer(m_nDevice)).toString());

			// Receive the response
			String sResponse = ReceivedString();
			System.out.println("Router responded with " + sResponse);

			String sToken = "";
			if (sResponse.length() > 2)
				sToken = sResponse.substring(0, 2);

			if (0 == sToken.compareTo("OK"))
				return true;
			
			System.out.println("Handshake has failed for EventHandler. Disconnecting...");
			
			Disconnect();
		}
		
		return false;
	}
}
