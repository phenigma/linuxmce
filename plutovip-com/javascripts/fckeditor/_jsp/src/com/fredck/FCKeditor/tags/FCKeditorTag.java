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
 * FCKeditorTag.java: FCKeditor tag library.
 *
 * Authors:
 *   Simone Chiaretta (simone@piyosailing.com)
 *   Hao Jiang (edwardrf@yahoo.com)
 *   
 */
 
package com.fredck.FCKeditor.tags;

import com.fredck.FCKeditor.FCKeditor;

import java.io.IOException;

import javax.servlet.http.HttpServletRequest;

import javax.servlet.jsp.PageContext;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.JspTagException;
import javax.servlet.jsp.tagext.BodyContent;
import javax.servlet.jsp.tagext.BodyTagSupport;

/**
 * Custom Tag class to access the {@linkplain com.fredck.FCKeditor.FCKeditor contaniner}.<br>
 *<p>
 * <b>Simple usage</b>:
 * <pre>
 *	&lt;FCK:editor
 *		id="EditorAccessibility"
 *		width="80%"
 *		height="120"
 *		toolbarSet="Accessibility"
 *		canUpload="false"
 *		canBrowse="false"
 *	"&gt;This is another test. &lt;BR"&gt;&lt;B"&gt;The "Second" row.&lt;/B"&gt;&lt;/FCK:editor"&gt;
 * </pre>
 *
 *<p>In this example we set all the attribute for the fckedit tag.
 *
 *<p>
 *  <b>Advanced usage of the tag</b>:
 *  <pre>
 *	&lt;FCK:editor id="EditorDefault" basePath="/FCKeditor/"&gt;
 *		&lt;FCK:config name="StyleNames" value=";Style 1;Style 2; Style 3" /&gt;
 *		&lt;FCK:config name="ToolbarFontNames" value=";Arial;Courier New;Times New Roman;Verdana" /&gt;
 *		This is some &lt;B&gt;sample text&lt;/B&gt;.
 *	&lt;/FCK:editor&gt;
*  </pre>
 *<p>In this example we set the id and the basePath of the editor (since it is /FCKeditor/
 * we could have omitted it because it's already the default value).<br>
 * Then we used the inner tag &lt;FCK:config&gt; to set some advanced configuration settings.
 *
			
 * @author Simone Chiaretta (simone@piyosailing.com)
 * @author Hao Jiang (edwardrf@yahoo.com)
 */
public class FCKeditorTag extends BodyTagSupport  {
	
	private String id;
	private String value = "";
	private String basePath = null;
	private String toolbarSet = null;
	private String width = null;
	private String height = null;
	private String canUpload = null;
	private String canBrowse = null;


   /**
     * The underlying FCKeditor object
     *
     */		
	protected FCKeditor fcked = null;

    /**
     * Get the unique id of the editor
     *
     * @return id
     */	
	public String getId() {
		return id;
	}

    /**
     * Set the unique id of the editor
     *
     * @param value name
     */	
	public void setId(String value) {
		id=value;
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
		basePath=value;
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
	public String getCanUpload() {
		return canUpload;
	}

    /**
     * Set the capability to upload files or images from inside the editor
     *
     * @param value true/false
     */		
	public void setCanUpload(String value) {
		canUpload=value;
	}

    /**
     * Get the capability to browse files or images from inside the editor
     *
     * @return true/false
     */		
	public String getCanBrowse() {
		return canBrowse;
	}

    /**
     * Set the capability to browse files or images from inside the editor
     *
     * @param value true/false
     */		
	public void setCanBrowse(String value) {
		canBrowse=value;
	}

	/**
	 * Initialize the FCKeditor container and set attributes
	 *
	 * @return EVAL_BODY_BUFFERED
	 * @throws JspException if canUpload or canBrowse are not of boolean value
	 */
	public int doStartTag() throws JspException {
		fcked=new FCKeditor((HttpServletRequest)pageContext.getRequest(),id);
		if(toolbarSet!=null)
			fcked.setToolbarSet(toolbarSet);
		if(basePath!=null)
			fcked.setBasePath(basePath);
		if(width!=null)
			fcked.setWidth(width);
		if(height!=null)
			fcked.setHeight(height);
		if(canUpload!=null)
			if(canUpload.equals("true"))
				fcked.setCanUpload(true);
			else if (canUpload.equals("false"))
				fcked.setCanUpload(false);
			else
				throw new JspTagException("Error evaluating FCKeditor Tag: canUpload must be a boolean value");
		if(canBrowse!=null)
			if(canBrowse.equals("true"))
				fcked.setCanBrowse(true);
			else if (canBrowse.equals("false"))
				fcked.setCanBrowse(false);
			else
				throw new JspTagException("Error evaluating FCKeditor Tag: canBrowse must be a boolean value");
					
		 return EVAL_BODY_BUFFERED;
	}

	/**
	 * Retrive initial value to be edited and writes the HTML code in the page
	 *
	 * @return SKIP_BODY
	 * @throws JspException if an error occurs whie writing to the out buffer
	 */	
	public int doAfterBody()  throws JspException {
			BodyContent body = getBodyContent();
			JspWriter writer = body.getEnclosingWriter();
			String bodyString = body.getString();
			fcked.setValue(bodyString);
			try {
				writer.println(fcked.create());
			}catch(IOException ioe) {
				throw new JspException("Error: IOException while writing to the user");
			}
			
			return SKIP_BODY;
	}

}