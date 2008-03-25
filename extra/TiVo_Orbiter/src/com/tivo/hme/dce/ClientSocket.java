package com.tivo.hme.dce;

import java.io.PrintWriter;
import java.net.Socket;

/*
 * ClientSocket class : handles socket communication
 */
public class ClientSocket 
{
	protected String m_sHost;
	protected int m_nPort = 0;
	protected int m_nDevice = 0;

	protected Socket m_socket = null;
	protected PrintWriter m_out = null;

	protected static String MESSAGE_HEADER = "MESSAGE";

	public ClientSocket(String sHost, int nPort, int nDevice) 
	{
		m_sHost = sHost;
		m_nPort = nPort;
		m_nDevice = nDevice;
	}

	public boolean Connect() 
	{
		try 
		{
			m_socket = new Socket(m_sHost, m_nPort);

			if (IsConnected())
			{
				m_out = new PrintWriter(m_socket.getOutputStream(), true);
				return true;
			}
		} 
		catch (Exception e) 
		{
			System.out.println("Failed to connect. Error: " + e.getMessage());
		}

		return false;
	}

	public boolean Disconnect() 
	{
		try 
		{
			if(null != m_out)
				m_out.close();
			
			if(null != m_socket)
			{
				m_socket.close();
				m_socket = null;
			}
			
			return true;
		} 
		catch (Exception e) 
		{
			System.out.println("Failed to disconnect. Error: " + e.getMessage());
		}

		return false;
	}
	
	public boolean IsConnected()
	{
		if(null == m_socket || !m_socket.isConnected())
		{
			//reconnect
			Connect();
		}
		
		return null != m_socket && m_socket.isConnected();
	}

	public boolean SendMessage(Message message) 
	{
		try 
		{
			if (IsConnected()) 
			{
				m_socket.getOutputStream().write(message.Data());
				return true;
			}
		} 
		catch (Exception e) 
		{
			System.out.println("Failed to write string. Error: " + e.getMessage());
			Disconnect();
		}

		return false;
	}

	public boolean SendString(String str) 
	{
		try 
		{
			if (IsConnected()) 
			{
				m_out.println(str);
				return true;
			}
		} 
		catch (Exception e) 
		{
			System.out.println("Failed to write string. Error: " + e.getMessage());
			Disconnect();
		}

		return false;
	}

	public Message ReceivedMessage()
	{
		String sHeader = ReceivedString().trim();
		System.out.println("Expected message, received : " + sHeader);
		
		// MESSAGE number
		if(!sHeader.startsWith(MESSAGE_HEADER + " "))
		{
			Disconnect();
			return null;
		}
		
		String sSize = sHeader.substring(MESSAGE_HEADER.length() + 1);
		Integer nSize = Integer.parseInt(sSize);
		BinaryData data = new BinaryData();
		
		byte[] buff = new byte[1];

		try
		{
			while(nSize > 0)
			{
				int c = m_socket.getInputStream().read();
				
				if(c == -1)
				{
					System.out.println("Connection closed!");
					Disconnect();
					return null;
				}			
				
				buff[0] = (byte)c;
				nSize--;
				
				data.Write(buff);
			}
		}
		catch (Exception e) 
		{
			System.out.println("Failed to receive string. Error: " + e.getMessage());
			Disconnect();
		}
		
		Message message = new Message();
		message.Read(data.Data());
		return message;
	}

	public String ReceivedString() 
	{
		try 
		{
			if (IsConnected()) 
			{
				String str = new String();
				int c = -1;
				
				do
				{
					c = m_socket.getInputStream().read();
					
					if(c == -1)
					{
						System.out.println("Connection closed!");
						Disconnect();
						return "";
					}
					
					if(c != '\n' && c != '\0')
						str += (char)c;
				}
				while(c != '\n' && c != '\0');
				
				return str;
			}
		} 
		catch (Exception e) 
		{
			System.out.println("Failed to receive string. Error: " + e.getMessage());
			Disconnect();
		}

		return "";
	}	
}
