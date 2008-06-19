package com.tivo.hme.orbiter.UI;

import com.tivo.hme.bananas.BApplication;
import com.tivo.hme.orbiter.Orbiter;
import com.tivo.hme.orbiter.PlutoProxy;
import com.tivo.hme.sdk.View;

public class SecurityPanelScreen extends BasicScreen 
{
	View m_root; 
	Boolean m_active = false;
	PlutoProxy m_proxy = null;
	Orbiter m_orbiter = null;
	
    public SecurityPanelScreen(BApplication app, PlutoProxy proxy)
    {
        super(app);

        m_orbiter = (Orbiter)app;
        m_proxy = proxy;
    }
    
    public void Run()
    {
    	m_active = true;    	
    }
    
    /**
     * Screen title.
     */
    public String toString() 
    {
        return "Security Panel";
    }	    
}
