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
import javax.servlet.jsp.tagext.TagSupport;


/**
 * Custom Tag class to access the {@linkplain com.fredck.FCKeditor.FCKeditorConfigurations	 advanced settings}.<br>
 * Used to override the settings specified in the config.js file.
 * This tag is valid only inside an &lt;FCK:editor&gt;
 * <p>
 * <b>Simple usage</b>:
 * <pre>
 * &lt;FCK:config name="StyleNames" value=";Style 1;Style 2; Style 3" /&gt;
 * </pre>
 *
 * @author Simone Chiaretta (simone@piyosailing.com)
 */
public class FCKeditorConfigurationsTag extends TagSupport  {
	private String name = null;
	private String value = null;
	
    /**
     * Get the name of the setting
     *
     * @return name
     */		
	public String getName() {
		return name;
	}

    /**
     * Set the name of the setting
     *
     * @param value name
     */		
	public void setName(String value) {
		name=value;
	}

    /**
     * Get the value of the setting
     *
     * @return name
     */		
	public String getValue() {
		return value;
	}

    /**
     * Set the value of the setting
     *
     * @param value value
     */		
	public void setValue(String value) {
		this.value=value;
	}	
	
	/**
	 * Update the configuration settings adding the specified value
	 *
	 * @return SKIP_BODY
	 * @throws JspException if tag used outside the &lt;FCK:editor&gt; tag
	 */
	public int doStartTag () throws JspException {
		FCKeditorTag editorTag=(FCKeditorTag)findAncestorWithClass(this, FCKeditorTag.class);
		if(editorTag==null)
			throw new JspTagException("This tag cannot be used outside the <FCK:editor> tag");
		editorTag.fcked.getConfig().put(name,value);
		return SKIP_BODY;
	}	
}