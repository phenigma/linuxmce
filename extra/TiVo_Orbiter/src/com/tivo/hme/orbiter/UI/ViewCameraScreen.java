/*  Java Orbiter
 *
 *  Copyright (C) 2007-2008 Pluto, Inc., a Delaware Corporation
 *
 *  http://www.plutohome.com
 *
 *  Phone: +1 (877) 758-8648
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

package com.tivo.hme.orbiter.UI;

import com.tivo.hme.bananas.*;
import com.tivo.hme.sdk.View; 
import com.tivo.hme.sdk.Resource; 

import com.tivo.hme.orbiter.*;

public class ViewCameraScreen extends BasicScreen
{
	View m_root; 
	Boolean m_active = false;
	ViewCameraEngine m_viewcameraengine = null;
	PlutoProxy m_proxy = null;
	Orbiter m_orbiter = null;
	
	private final int SCREEN_Main_CONST = 1;
	private final int BUTTON_Left_Arrow_CONST = 3;
	
    /**
     * Constructor 
     */
    public ViewCameraScreen(BApplication app, PlutoProxy proxy)
    {
        super(app);

        m_orbiter = (Orbiter)app;
        m_proxy = proxy;
        m_root = app.getRoot();
    }
    
    public void GetFrames()
    {
 	   while (m_active) 
	   { 
		  try 
		  { 
			  Resource sr = createImage("http://www.hotelunirea.ro:8053/record/current.jpg");
			  
		      // set the view to the new resource 
			  m_root.setResource(sr);
			  
			  System.out.println("Displaying new frame...");
		 
		      // sleep between refreshes 
		      Thread.sleep(1000); 
		      
		      // remove the old resource to free the memory 
		      sr.flush(); 
		      sr.remove(); 		      
		  }  
		  catch (Exception ex) 
		  { 
		      // if we're unable to update the image, just pass till 
			  // next time. 
		  } 
	   }     	
    }
  
    public void Run()
    {
    	m_active = true;
    	m_viewcameraengine = new ViewCameraEngine(this);
    	m_viewcameraengine.run();
    }
    
    public void Stop()
    {
    	m_active = false;
    }
    
    public Boolean IsActive()
    {
    	return m_active;
    }
    
    /**
     * Title of the screen
     */
    public String toString() 
    {
        return "View Camera";
    }
    
    /**
     * Listen for the left key.
     */
    public boolean handleKeyPress(int code, long rawcode) 
    {
    	//intercept any keys
    	Stop();
    	
    	m_orbiter.TivoGotoScreen((new Integer(SCREEN_Main_CONST)).toString());
    	m_proxy.SendKey(BUTTON_Left_Arrow_CONST);
    	
        return super.handleKeyPress(code, rawcode);
    }    
}
