/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * FCKeditorConfigurations.java: FCKeditor configurations container.
 *
 * Authors:
 *   Simone Chiaretta (simone@piyosailing.com)
 *   
 */
 
package com.fredck.FCKeditor;

import java.util.*;

/**
 * Contains the advanced configuration settings.<br>
 * Adding element to this collection you can override the settings specified in the config.js file.
 *
 * @author Simone Chiaretta (simone@piyosailing.com)
 */
public class FCKeditorConfigurations extends HashMap{
	
	/**
     * Initialize the configuration collection
     */
	public FCKeditorConfigurations() {
		super();
	}

	/**
     * Generate the url parameter sequence used to pass this configuration to the editor.
     *
     *
     *@return urlencoded sequence of configuration param
     */	
	public String getUrlParams() {
		StringBuffer osParams = new StringBuffer();
		
		for(Iterator i=this.entrySet().iterator();i.hasNext();) {
			Map.Entry entry = (Map.Entry) i.next();
			osParams.append("&"+entry.getKey()+"="+entry.getValue());
		}
		return osParams.toString();
	}	
	
}
