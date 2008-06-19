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
import java.awt.Color; 

import com.tivo.hme.orbiter.*;

public class ViewCameraScreen extends BasicScreen
{
	View m_root; 
	Boolean m_active = false;
	ViewCameraEngine m_viewcameraengine = null;
	PlutoProxy m_proxy = null;
	Orbiter m_orbiter = null;
	String m_sCameraURL = "";
	String m_sCameraID = "0";
	
	View m_videofeed = null;
	View m_textview = null;
	
	private final int SCREEN_Main_CONST = 1;
	
	private final int COMMAND_MOVE_UP_CONST = 200;
	private final int COMMAND_MOVE_DOWN_CONST = 201;
	private final int COMMAND_MOVE_LEFT_CONST = 202;
	private final int COMMAND_MOVE_RIGHT_CONST = 203;
	private final int COMMAND_ZOOM_IN_CONST = 684;
	private final int COMMAND_ZOOM_OUT_CONST = 685;
	
    /**
     * Constructor 
     */
    public ViewCameraScreen(BApplication app, PlutoProxy proxy, String sCameraURL, String sCameraID)
    {
        super(app);

        m_orbiter = (Orbiter)app;
        m_proxy = proxy;
        m_sCameraURL = sCameraURL;
        m_sCameraID = sCameraID;
        
        m_root = app.getRoot();

        m_videofeed = new View(m_root, SAFE_ACTION_H, SAFE_ACTION_V,
        		m_root.getWidth() - SAFE_ACTION_H * 2,
        		m_root.getHeight() - SAFE_ACTION_V * 4);
        
        m_textview = new View(m_root, SAFE_ACTION_H,
        		m_root.getHeight() - 3 * SAFE_ACTION_V, m_root.getWidth() - SAFE_ACTION_H, 3 * SAFE_ACTION_V - 1);
        m_textview.setVisible(false);
        m_textview.setResource(createText("default-18.font", Color.white,
        		"Use UP/DOWN/LEFT/RIGHT to move camera.\nUse THUMBUP/THUMBDOWN to zoomin/zoomout.\nPress SELECT to go back."
          ),
       RSRC_TEXT_WRAP);       
    }
    
    public void GetFrames()
    {
 	   while (m_active) 
	   { 
		  try 
		  { 
			  Resource sr = m_videofeed.createImage(m_sCameraURL);
			  
		      // set the view to the new resource 
			  m_videofeed.setResource(sr);
			  
			  System.out.println("Displaying new frame...");
		 
		      // sleep between refreshes 
		      Thread.sleep(200); 
		      
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
    	m_videofeed.setVisible(true);
    	m_textview.setVisible(true);
    	m_active = true;
    	m_viewcameraengine = new ViewCameraEngine(this);
    	m_viewcameraengine.run();
    }
    
    public void Stop()
    {
    	m_videofeed.setVisible(false);
    	m_textview.setVisible(false);
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
        return "";
    }
    
    /**
     * Listen for the left key.
     */
    public boolean handleKeyPress(int code, long rawcode) 
    {
    	switch(code)
    	{
    		case KEY_SELECT:
    	    	Stop();
    	    	m_orbiter.TivoGotoScreen((new Integer(SCREEN_Main_CONST)).toString());
    	    	m_proxy.GotoScreen(SCREEN_Main_CONST);	    			
    	    	break;
    		
    		case KEY_THUMBSUP:
    			System.out.println("Zoom in");
    			m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_ZOOM_IN_CONST);
    			break;
    			
    		case KEY_THUMBSDOWN:
    			System.out.println("Zoom out");
    			m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_ZOOM_OUT_CONST);
    			break;
    			
    		case KEY_LEFT:
    			System.out.println("Move camera left");
    			m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_MOVE_LEFT_CONST); 	
    			break;
    			
    		case KEY_RIGHT:
				System.out.println("Move camera right");
				m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_MOVE_RIGHT_CONST);     			
    			break;
    			
    		case KEY_UP:
				System.out.println("Move camera up");
				m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_MOVE_UP_CONST);    			
    			break;
    			
    		case KEY_DOWN:
				System.out.println("Move camera down");
				m_proxy.CameraAction(Integer.parseInt(m_sCameraID), COMMAND_MOVE_DOWN_CONST);    			
    			break;
    	}

        return super.handleKeyPress(code, rawcode);
    }    
}
