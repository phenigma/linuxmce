/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
// --------------------------------------------------------------------------
/*
	To build a multi-section application
	1. In makefile, set MULTIPLE_CODE_SECTIONS = TRUE
	2. Edit Sections.def to specify the number of code sections desired
	3. Modify this file to match the code sections
	   (Sections.def and this file are set up with 2 extra sections
	   by default.)
	4. Include this file in project sources.
	5. Annotate each function with a section define like so: 
		void DoWork() EXTRA_SECTION_ONE;
		void DrawForm() EXTRA_SECTION_TWO;
	   Notice that the annotations need to be visible at the function
	   definition, and everywhere the function is used.  Any function
	   without an annotation will go into the default code section.

	To effectively disable the multi-section annotations, 
	just define these section macros to expand to nothing.

	Be sure to follow the instructions and warnings given at:
	http://prc-tools.sourceforge.net/doc/prc-tools_3.html#SEC17

*/
// --------------------------------------------------------------------------

#ifndef _SECTIONS_H
#define _SECTIONS_H

#define EXTRA_SECTION_ONE  __attribute__ ((section ("code1")))
#define EXTRA_SECTION_TWO  __attribute__ ((section ("code2")))

#endif 
