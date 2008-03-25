package com.tivo.hme.dce;

public class BinaryData 
{
	public static int DATA_BLOCK = 4096;
	byte[] m_pData = null;
	int m_nPos = 0;
	
	public BinaryData()
	{
		m_pData = new byte[DATA_BLOCK];
	}
	
	public BinaryData(byte[] pData)
	{
		m_pData = pData;
	}
	
	public byte[] Data()
	{
		return m_pData;
	}
	
	public int Position()
	{
		return m_nPos;
	}
	
	private void IncreaseSize()
	{
		byte[] pNewData = new byte[m_pData.length + DATA_BLOCK];
		
		for(int i = 0; i < m_pData.length; ++i)
			pNewData[i] = m_pData[i];
		
		m_pData = pNewData;
	}
	
	public BinaryData Write(byte[] pSmallData)
	{
		if(m_nPos + pSmallData.length > m_pData.length)
			IncreaseSize();
		
		for(int i = 0; i < pSmallData.length; ++i)
			m_pData[m_nPos + i] = pSmallData[i];
		
		m_nPos += pSmallData.length;
		
		return this;
	}

	public BinaryData WriteFront(byte[] pSmallData)
	{
		int nNewLength = m_pData.length;
		if(m_nPos + pSmallData.length > m_pData.length)
			nNewLength = m_nPos + pSmallData.length;
		
		byte[] pNewData = new byte[nNewLength];
		
		for(int i = 0; i < pSmallData.length; ++i)
			pNewData[i] = pSmallData[i];
		
		for(int j = 0; j < m_nPos; ++j)
			pNewData[pSmallData.length + j] = m_pData[j];
		
		m_pData = pNewData;
		m_nPos += pSmallData.length;
	
		return this;
	}
	
	public byte[] Read(Integer nBytes)
	{
		byte[] pSmallData = new byte[nBytes];
		
		if(m_nPos + nBytes < m_pData.length)
		{
			for(int i = 0; i < nBytes; ++i)
				pSmallData[i] = m_pData[m_nPos + i];
			
			m_nPos += nBytes;
		}
		else
		{
			System.out.println("Cannot serialize object from position " + 
					(new Integer(m_nPos)).toString() + " bytes to read " +
					(new Integer(nBytes)).toString());
		}
		
		return pSmallData;
	}
	
	public String ReadString()
	{
		String sResult = new String();
		
		for(int i = m_nPos; i < m_pData.length; i++)
		{
			if(m_pData[i] == '\0')
			{
				m_nPos = i + 1;
				break;
			}
			else
				sResult += (char)m_pData[i];
		}
		
		return sResult;
	}
}
