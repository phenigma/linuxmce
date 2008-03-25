package com.tivo.hme.dce;

public class CommandHandlerAsync extends Thread 
{
	Communication m_comm = null;
	
	public CommandHandlerAsync(Communication comm)
	{
		m_comm = comm;
	}
	
    public void run() 
    {
       	m_comm.WaitForMessages();
    }
}
