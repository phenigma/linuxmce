/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * FCKeditor.java: FCKeditor control class.
 *
 * Authors:
 *   Simone Chiaretta (simone@piyosailing.com)
 *   Hao Jiang (edwardrf@yahoo.com)
 *   
 */
 
 
package com.fredck.FCKeditor;

import javax.servlet.http.HttpServletRequest;

/**
 * The main class of the class lib.<br>
 * It's the container for all properties and the class that generate the output based on browser capabilities and configurations passed by the developer.
 *
 * @author Simone Chiaretta (simone@piyosailing.com)
 * @author Hao Jiang (edwardrf@yahoo.com)
 */
public class FCKeditor {
	
	private FCKeditorConfigurations oConfig;
	private String instanceName;
	private String value = "";
	private String basePath;
	private String toolbarSet = "Default";
	private String width = "100%";
	private String height = "200";
	private Boolean canUpload = null;
	private Boolean canBrowse = null;
	
	HttpServletRequest request;
	
    /**
     * Get the unique name of the editor
     *
     * @return name
     */
	public String getInstanceName() {
		return instanceName;
	}

    /**
     * Set the unique name of the editor
     *
     * @param value name
     */	
	public void setInstanceName(String value) {
		instanceName=value;
	}

    /**
     * Get the initial value to be edited.<br>
     * In HTML code
     *
     * @return value
     */	
	public String getValue() {
		return value;
	}
	
    /**
     * Set the initial value to be edited.<br>
     * In HTML code
     *
     * @param value value
     */		
	public void setValue(String value) {
		this.value=value;
	}

    /**
     * Get the dir where the FCKeditor files reside on the server
     *
     * @return path
     */		
	public String getBasePath() {
		return basePath;
	}
	
    /**
     * Set the dir where the FCKeditor files reside on the server
     *
     * @param value path
     */		
	public void setBasePath(String value) {
		basePath=request.getContextPath()+value;
	}

    /**
     * Get the name of the toolbar to display
     *
     * @return toolbar name
     */	
	public String getToolbarSet() {
		return toolbarSet;
	}

    /**
     * Set the name of the toolbar to display
     *
     * @param value toolbar name
     */		
	public void setToolbarSet(String value) {
		toolbarSet=value;
	}

    /**
     * Get the width of the textarea
     *
     * @return width
     */	
	public String getWidth() {
		return width;
	}

    /**
     * Set the width of the textarea
     *
     * @param value width
     */		
	public void setWidth(String value) {
		width=value;
	}

    /**
     * Get the height of the textarea
     *
     * @return height
     */	
	public String getHeight() {
		return height;
	}

    /**
     * Set the height of the textarea
     *
     * @param value height
     */		
	public void setHeight(String value) {
		height=value;
	}

    /**
     * Get the capability to upload files or images from inside the editor
     *
     * @return true/false
     */	
	public boolean getCanUpload() {
		return canUpload.booleanValue();
	}

    /**
     * Set the capability to upload files or images from inside the editor
     *
     * @param value true/false
     */		
	public void setCanUpload(boolean value) {
		canUpload=Boolean.valueOf(value);
	}

    /**
     * Get the capability to browse files or images from inside the editor
     *
     * @return true/false
     */	
	public boolean getCanBrowse() {
		return canBrowse.booleanValue();
	}

    /**
     * Set the capability to browse files or images from inside the editor
     *
     * @param value true/false
     */		
	public void setCanBrowse(boolean value) {
		canBrowse=Boolean.valueOf(value);
	}

    /**
     * Get the advanced configuation set.<br>
     * Adding element to this collection you can override the settings specified in the config.js file.
     *
     * @return configuration collection
     */		
	public FCKeditorConfigurations getConfig() {
		return oConfig;
	}

    /**
     * Set the advanced configuation set.
     *
     * @param value configuration collection
     */		
	public void setConfig(FCKeditorConfigurations value) {
		oConfig=value;
	}

    /**
     * Initialize the object setting all value to the default ones.
     * <p>
     * <ul>
     * <li>width: 100%</li>
     * <li>height: 200</li>
     * <li>toolbar name: Default</li>
     * <li>basePath: context root + "/FCKeditor/"</li>
     * </ul>
     * </p>
     *
     * @param req request object
     */	
	public FCKeditor(HttpServletRequest req){
		request=req;
		basePath = request.getContextPath() + "/FCKeditor/";
		oConfig = new FCKeditorConfigurations() ;
	}

    /**
     * Initialize the object setting the unique name and then all value to the default ones.
     * <p>
     * <ul>
     * <li>width: 100%</li>
     * <li>height: 200</li>
     * <li>toolbar name: Default</li>
     * <li>basePath: context root + "/FCKeditor/"</li>
     * </ul>
     * </p>
     *
     * @param req request object
     * @param parInstanceName unique name
     */		
	public FCKeditor(HttpServletRequest req, String parInstanceName){
		request=req;
		basePath = request.getContextPath() + "/FCKeditor/";
		instanceName=parInstanceName;
		oConfig = new FCKeditorConfigurations() ;
	}

    /**
     * Initialize the object setting all basic configurations.<br>
     *
     * @param req request object
     * @param parInstanceName unique name
     * @param parWidth width
     * @param parHeight height
     * @param parToolbarSet toolbarSet name
     * @param parValue initial value
     */		
	public FCKeditor(HttpServletRequest req, String parInstanceName, String parWidth, String parHeight, String parToolbarSet, String parValue){
		request=req;
		basePath = request.getContextPath() + "/FCKeditor/";
		instanceName=parInstanceName;
		width=parWidth;
		height=parHeight;
		toolbarSet=parToolbarSet;
		value=parValue;
		oConfig = new FCKeditorConfigurations() ;
	}
	
	
	private boolean isCompatible() {
		String userAgent=request.getHeader("user-agent");
		if(userAgent==null)
			return false;
		if ((userAgent.indexOf("MSIE") >= 0) && (userAgent.indexOf("Windows") >= 0) && (userAgent.indexOf("Opera") == -1)) {
			if(retrieveBrowserVersion(userAgent)>=5)
				return true;
			else
				return false;
		}
		return false;
	}
	
	private int retrieveBrowserVersion(String userAgent) {
		int initIndex=userAgent.indexOf("MSIE");
		return Integer.parseInt(userAgent.substring(initIndex+5,initIndex+6));
	}
	
	private String HTMLEncode(String txt) {
		txt=txt.replaceAll("&","&amp;");
		txt=txt.replaceAll("<","&lt;");
		txt=txt.replaceAll(">","&gt;");
		txt=txt.replaceAll("\"","&quot;");
		txt=txt.replaceAll("'","&#146;");
		return txt;
	}
	

    /**
     * Generate the HTML Code for the editor.
     * <br>
     * Evalute the browser capabilities and generate the editor if IE 5.0 or greater, or a simple textarea otherwise. 
     *
     * @return html code
     */	
	public String create() {
		String strEditor=null;
		
		if(isCompatible()) {
			StringBuffer sLink=new StringBuffer(basePath + "fckeditor.html?FieldName=" + instanceName);

			if (!toolbarSet.equals("Default") && !toolbarSet.equals(""))
				sLink.append("&Toolbar=" + toolbarSet);

			if (canUpload != null) 
				sLink.append("&Upload=" + canUpload.toString().toLowerCase());

			if (canBrowse != null) 
				sLink.append("&Browse=" + canBrowse.toString().toLowerCase());
			
			sLink.append(oConfig.getUrlParams());
			
			value=HTMLEncode(value);
			
			strEditor="<IFRAME src=\""+sLink+"\" width=\""+width+"\" height=\""+height+"\" frameborder=\"no\" scrolling=\"no\"></IFRAME>\r\n";
			strEditor += "<INPUT type=\"hidden\" name=\""+instanceName+"\" value=\""+value+"\">";
		}
		else{
			strEditor="<TEXTAREA name=\""+instanceName+"\" rows=\"4\" cols=\"40\" style=\"WIDTH: "+width+"; HEIGHT: "+height+"\" wrap=\"virtual\">"+value+"</TEXTAREA>";
		}
		return strEditor;
	}
	
	
}
