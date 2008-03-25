package com.tivo.hme.dce;

public class Serialize
{
	public Serialize()
	{
	
	}
	
	static BinaryData WriteInteger(BinaryData pData, int nValue)
	{
		byte[] pSmallData = new byte[4];

		pSmallData[3] = (byte)((nValue & 0xff000000) >> 24);
		pSmallData[2] = (byte)((nValue & 0x00ff0000) >> 16);
		pSmallData[1] = (byte)((nValue & 0x0000ff00) >>  8);
		pSmallData[0] = (byte)((nValue & 0x000000ff)		 );
		
		//System.out.println("Integer " + (new Integer(nValue).toString()) + " : " + DumpByteArray(pSmallData));
		return pData.Write(pSmallData);
	}
	
	static BinaryData WriteString(BinaryData pData, String sValue)
	{
		//System.out.println("String '" + sValue + "' : " + DumpByteArray(sValue.getBytes()));

		//write the string
		pData.Write(sValue.getBytes());
		
		//write EOS
		byte[] pSmallData = new byte[1];
		pSmallData[0] = '\0';
		return pData.Write(pSmallData);
	}

	static BinaryData WriteChar(BinaryData pData, char sValue)
	{
		//System.out.println("Char '" + sValue + "' : " + DumpByteArray(String.valueOf(sValue).getBytes()));
		return pData.Write(String.valueOf(sValue).getBytes());
	}
	
	static BinaryData WriteStringFront(BinaryData pData, String sValue)
	{
		//System.out.println("String (front) '" + sValue + "' : " + DumpByteArray(sValue.getBytes()));
		return pData.WriteFront(sValue.getBytes());
	}
	
	static BinaryData WriteBlock(BinaryData pData, byte[] pValue)
	{
		System.out.println("Block: " + DumpByteArray(pValue));
		
		//write the length of the block
		WriteInteger(pData, pValue.length);
		
		//write the block
		return pData.Write(pValue);
	}
	
	static Integer ReadInteger(BinaryData pData)
	{
		byte[] pSmallData = pData.Read(4);
		
		Integer nValue = 
			((256 + pSmallData[3]) % 256 << 24) + 
			((256 + pSmallData[2]) % 256 << 16) + 
			((256 + pSmallData[1]) % 256 <<  8) + 
			((256 + pSmallData[0]) % 256	  ); 
		
		return nValue;
	}
	
	static char ReadChar(BinaryData pData)
	{
		byte[] pSmallData = pData.Read(1);
		return (char)pSmallData[0];		
	}
	
	static String ReadString(BinaryData pData)
	{
		return pData.ReadString();
	}
	
	static byte[] ReadBlock(BinaryData pData)
	{
		Integer len = ReadInteger(pData);
		byte[] pSmallData = pData.Read(len);
		return pSmallData;
	}
	
	static String DumpByteArray(byte[] array)
	{
		String str = new String();
		
		for(int i = 0; i < array.length; ++i)
			str += "0x" + Integer.toHexString((256 + array[i]) % 256) + ", ";
		
		return str;
	}
}
