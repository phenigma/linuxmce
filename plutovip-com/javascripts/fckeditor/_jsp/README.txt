----------------------------------------
	FCKEditor JSP tag library
----------------------------------------


---------------
Contents:
---------------

1 - Preface
2 - Installation
3 - Usage
4 - Building
5 - Future plans
6 - Further information and support

--------------------
1 - Preface
--------------------

This is the JSP taglibrary for using FCKeditor inside a java server page without
the complexity of using a Java scriptlets or the javascript api.
I hope you find it usefull.



--------------------
2 - Installation
--------------------

Installing FCKeditor in a JSP environment is quite easy:
 - First install the FCKeditor as described in the _docs/installation.html file
 - Copy the files in the _jsp/dist directory in the WEB-INF dir of your web application
   (the jar file in WEB-INF/lib and the tld in WEB_INF/)
 - Now you can start using the _test/test.jsp and _test/test-tag.jsp to test the JSP integration



--------------------
3 - Usage
--------------------

Take a look at the test files and in the javadoc api provided in the distribution.
Anyway, for the lazy ones, here is a simple example

First put this taglib definiton at the top of the JSP page
<%@ taglib uri="/WEB-INF/FCKeditor.tld" prefix="FCK" %>

then the tag:

	<FCK:editor
		id="EditorAccessibility"	//Unique name of the editor
		width="80%"			//Width
		height="120"			//Height
		toolbarSet="Accessibility"	//Toolbar name
		canUpload="false"		//Override default canUpload capability
		canBrowse="false"		//Override default canBrowse capability
	>This is another test. <BR><B>The "Second" row.</B></FCK:editor>

A more advance example, that show how to use the inner tags to overrides any of the settings specified in
the config.js file

	<FCK:editor id="EditorDefault" basePath="/FCKeditor/">
		<FCK:config name="StyleNames" value=";Style 1;Style 2; Style 3" />
		<FCK:config name="ToolbarFontNames" value=";Arial;Courier New;Times New Roman;Verdana" />
		This is some <B>sample text</B>.
	</FCK:editor>


--------------------
4 - Building
--------------------

Inside the _jsp directory there are the source files for the tags (_jsp/src) and the ant build file (build.xml)

The ant build file provides the following tasks:
 - all: Clean build and dist directories, then compile
 - clean: Delete old build and dist directories
 - compile: Compile Java sources
 - dist: Create binary distribution
 - javadoc: Create Javadoc API documentation
 - release: Create Release Distribution

To start building the class you first need to modify the "catalina.home" property inside the build.xml,
then type "ant dist" to compile, create the javadoc API description and generate the jar library to be copied in the WEB-INF


--------------------
5 - Future plans
--------------------

 * Provide the ability to pass a Map of advanced configuration instead of using the inner tags
 * Add upload and browse capabilities inside the tag instead of using a completely different thing
 * Integrate with Struts (so that FCKeditor can retrive and pass informations from Form Beans)



--------------------
6 - Further informations and support
--------------------

For further informations refers to http://www.fredck.com/FCKEditor/+
For support specific with this JSP integration taglib send an email to simo@users.sourceforge.net