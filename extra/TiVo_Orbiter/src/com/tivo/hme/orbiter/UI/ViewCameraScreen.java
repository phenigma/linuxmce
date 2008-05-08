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
	
	View m_videofeed = null;
	
	private final int SCREEN_Main_CONST = 1;
	//private final int BUTTON_Left_Arrow_CONST = 3;
	
	BButton m_buttonRight = null;
	BButton m_buttonLeft = null;
	BButton m_buttonUp = null;
	BButton m_buttonDown = null;
	BButton m_buttonBack = null;	
    /**
     * Constructor 
     */
    public ViewCameraScreen(BApplication app, PlutoProxy proxy)
    {
        super(app);

        m_orbiter = (Orbiter)app;
        m_proxy = proxy;
        m_root = app.getRoot();

        m_videofeed = new View(m_root, SAFE_ACTION_H, SAFE_ACTION_V,
        		m_root.getWidth() - SAFE_ACTION_H * 2,
        		m_root.getHeight() - SAFE_ACTION_V * 4);
        
        m_buttonRight = new BButton(getNormal(), SAFE_TITLE_H+20, m_root.getHeight() - SAFE_ACTION_V * 3 + 10, 50, 30);
        m_buttonRight.setBarAndArrows(BAR_DEFAULT, BAR_DEFAULT, H_LEFT, H_RIGHT, null, null, false);
        m_buttonRight.setResource(createText("default-18.font", Color.white, "right"));
        setFocusDefault(m_buttonRight);
        
        m_buttonLeft = new BButton(getNormal(), SAFE_TITLE_H+80, m_root.getHeight() - SAFE_ACTION_V * 3 + 10, 50, 30);
        m_buttonLeft.setBarAndArrows(BAR_DEFAULT, BAR_DEFAULT, H_LEFT, H_RIGHT, null, null, false);
        m_buttonLeft.setResource(createText("default-18.font", Color.white, "left"));

        m_buttonUp = new BButton(getNormal(), SAFE_TITLE_H+140, m_root.getHeight() - SAFE_ACTION_V * 3 + 10, 50, 30);
        m_buttonUp.setBarAndArrows(BAR_DEFAULT, BAR_DEFAULT, H_LEFT, H_RIGHT, null, null, false);
        m_buttonUp.setResource(createText("default-18.font", Color.white, "up"));
 
        m_buttonDown = new BButton(getNormal(), SAFE_TITLE_H+200, m_root.getHeight() - SAFE_ACTION_V * 3 + 10, 50, 30);
        m_buttonDown.setBarAndArrows(BAR_DEFAULT, BAR_DEFAULT, H_LEFT, H_RIGHT, null, null, false);
        m_buttonDown.setResource(createText("default-18.font", Color.white, "down"));
        
        m_buttonBack = new BButton(getNormal(), SAFE_TITLE_H+260, m_root.getHeight() - SAFE_ACTION_V * 3 + 10, 50, 30);
        m_buttonBack.setBarAndArrows(BAR_DEFAULT, BAR_DEFAULT, H_LEFT, H_RIGHT, null, null, false);
        m_buttonBack.setResource(createText("default-18.font", Color.white, "back"));
    }
    
    public void GetFrames()
    {
 	   while (m_active) 
	   { 
		  try 
		  { 
			  Resource sr = m_videofeed.createImage("http://www.hotelunirea.ro:8053/record/current.jpg");
			  
		      // set the view to the new resource 
			  m_videofeed.setResource(sr);
			  
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
    	m_videofeed.setVisible(true);
    	m_active = true;
    	m_viewcameraengine = new ViewCameraEngine(this);
    	m_viewcameraengine.run();
    }
    
    public void Stop()
    {
    	m_videofeed.setVisible(false);
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
		if(code == KEY_SELECT)
		{
			if(m_buttonBack.hasFocus())
			{
		    	Stop();
		    	
		    	m_orbiter.TivoGotoScreen((new Integer(SCREEN_Main_CONST)).toString());
		    	m_proxy.GotoScreen(SCREEN_Main_CONST);				
			}
			else if(m_buttonRight.hasFocus())
			{
				System.out.println("Move camera right");
			}
			else if(m_buttonLeft.hasFocus())
			{
				System.out.println("Move camera left");
			}
			else if(m_buttonUp.hasFocus())
			{
				System.out.println("Move camera up");
			}
			else if(m_buttonDown.hasFocus())
			{
				System.out.println("Move camera down");
			}			
		}
    	
        return super.handleKeyPress(code, rawcode);
    }    
}
