//Author: Younes Bouab
//Date:   05-02-02
//Title:  Menu Configuration: Config.js
//
//Copyright: Younes Bouab 2001 
//Technical Support: bouaby@SUPEReDITION.com
//
/////////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2002 Younes Bouab (www.SUPEReDITION.com) Version 1.0
//
//Eperience the DHTML Menu - Get it at www.SUPEReDITION.com
//
//This script can be used freely as long as all copyright messages are intact.
//
//Menu HomePage: http://www.superedition.com/Main.asp?Page=Tutorials&query=Javascript
//////////////////////////////////////////////////////////////////////////////////////
//Menu Configuration File
/////////////////////////////////////////////////
/////You can change the value of a variable 
/////below or turn it off by making it equal "" 
/////to suit your needs, but you should not
/////delete any variable.
///////////////////////////////////////////////
/**********************************************/
//Menu Type: Do Not Change! 
/**********************************************/
MENU_TYPE=1; //1: Horizental
             //2: Vertical

Sort=0;   //Sort: When set to 1, the 
          //menu items are sorted according 
          //to the index value. This feature
          //can be used when a server side
          //language (asp. jsp. php,...)
          //is used to generate the menu items 
          //from a database and they are not always 
          //in order.  



/**********************************************/
//Menu Starting point
/**********************************************/
if (document.all)
	TOP=-8;
else
	TOP=-6;
LEFT=0;


/**********************************************/
//Menu item Dimension
/**********************************************/
WIDTH=132;
HEIGHT=30;


/**********************************************/
//Layers Alignment
/**********************************************/
HALIGN="LEFT";
VALIGN="MIDDLE";


/**********************************************/
//Global Menu Settings for all
/**********************************************/
//Main Menu Items
LayerColor="#DDE4EF";
LayerRollColor="#EFF2F9";
FONT="verdana";
FONTSIZE="2";
FONTSTYLE="B" // "": Normal, "B": Bold, "I": Italic
FONTCOLOR="#4E6CA6";
ROLL_FONTCOLOR="#000000";
START_CHAR=""; //Starting Character


/**********************************************/
//Main Parent Settings: Optional 
// leave empty "", if you would like to use 
// the Global Menu Settings above
/**********************************************/
Main_Parent_LayerColor="";
Main_Parent_LayerRollColor=""; 
Main_Parent_FONT="verdana";
Main_Parent_FONTSTYLE="";
Main_Parent_FONTSIZE="";
Main_Parent_FONTCOLOR="";
Main_Parent_ROLL_FONTCOLOR="";
Main_Parent_START_CHAR="&nbsp;&nbsp;";


/**********************************************/
//Layer Border Properties
/**********************************************/
LayerBorderSize="1";
LayerBorderStyle="solid";
LayerBorderColor="#4E6CA6";


/**********************************************/
//Menu Children Offsets
/**********************************************/
TOP_OFFSET=0;
LEFT_OFFSET=0;

var serverPath='http://10.0.0.150/support/';