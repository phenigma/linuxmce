package com.tivo.hme.orbiter.UI;

import com.tivo.hme.bananas.BApplication;
import com.tivo.hme.orbiter.PlutoProxy;

public class OrbiterScreen extends BasicScreen
{
	private PlutoProxy m_proxy = null;
	
	private final int BUTTON_Up_Arrow_CONST = 1;
	private final int BUTTON_Down_Arrow_CONST = 2;
	private final int BUTTON_Left_Arrow_CONST = 3;
	private final int BUTTON_Right_Arrow_CONST = 4;
	private final int BUTTON_Enter_CONST = 5;
	
	public OrbiterScreen(BApplication app, PlutoProxy proxy)
	{
		super(app);
		
		m_proxy = proxy;
	}
	
    /**
     * Listen for the left key.
     */
    public boolean handleKeyPress(int code, long rawcode) 
    {
    	//intercept keys and router them to ProxyOrbiter
    	
    	int nPlutoKey = 0;
    	
		switch (code) 
		{
			case KEY_LEFT:
				nPlutoKey = BUTTON_Left_Arrow_CONST;
				break;
				
			case KEY_RIGHT:
				nPlutoKey = BUTTON_Right_Arrow_CONST;
				break;

			case KEY_UP:
				nPlutoKey = BUTTON_Up_Arrow_CONST;
				break;

			case KEY_DOWN:
				nPlutoKey = BUTTON_Down_Arrow_CONST;
				break;
				
			case KEY_SELECT:
				nPlutoKey = BUTTON_Enter_CONST;
				break;
		} 
		
		if(nPlutoKey > 0)
			m_proxy.SendKey(nPlutoKey);
    	
        return super.handleKeyPress(code, rawcode);
    }
    
    /**
     * Screen title.
     */
    public String toString() 
    {
        return "";
    }	
}
