package com.tivo.hme.scenarios;

public class CommandGroup 
{
	private int m_nCommandGroupID = 0;
	private int m_nArrayID = 0;
	private int m_nRoomID = 0;
	private String m_sName = new String();
	private String m_sRoomName = new String();
	
	public CommandGroup(int nCommandGroupID, int nArrayID, int nRoomID, String sName, String sRoomName)
	{
		m_nCommandGroupID = nCommandGroupID;
		m_nArrayID = nArrayID;
		m_nRoomID = nRoomID;
		m_sName = sName;
		m_sRoomName = sRoomName;
	}
	
	public int CommandGroupID()
	{
		return m_nCommandGroupID;
	}
	
	public int ArrayID()
	{
		return m_nArrayID;
	}
	
	public int RoomID()
	{
		return m_nRoomID;
	}
	
	public String Name()
	{
		return m_sName;
	}
	
	public String toString()
	{
		return m_sName + " / " + m_sRoomName;
	}
}
