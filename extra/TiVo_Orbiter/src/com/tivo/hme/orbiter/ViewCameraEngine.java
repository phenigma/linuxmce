package com.tivo.hme.orbiter;

import com.tivo.hme.orbiter.UI.ViewCameraScreen;

public class ViewCameraEngine extends Thread
{
	ViewCameraScreen m_viewcamerascreen = null;
	
	public ViewCameraEngine(ViewCameraScreen viewcamerascreen)
	{
		m_viewcamerascreen = viewcamerascreen;
	}
	
    public void run() 
    {
    	System.out.println("Starting view camera engine. Getting frames...");
    	
    	m_viewcamerascreen.GetFrames();
    	
    	System.out.println("Stopping view camera engine");
    }	
}
