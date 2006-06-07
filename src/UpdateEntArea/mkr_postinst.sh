#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

#/usr/bin/mysql pluto_main < /usr/pluto/bin/PurgeEntAreas.sql

# We now set variable 1 for cameras, not variable 22
RunSQL "update CommandGroup_Command_CommandParameter 
join CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command 
join CommandGroup on FK_CommandGroup=PK_CommandGroup 
set IK_CommandParameter=1 
WHERE FK_Array=3 AND FK_Template in (15,16) AND FK_Command=27 and FK_CommandParameter=4 and IK_CommandParameter=22"

# We moved 'movies' into 'videos'
RunSQL "delete from CommandGroup where FK_Template=34 and TemplateParm1=0 and TemplateParm2=3"
RunSQL "update CommandGroup set Description='Video',psc_mod=psc_mod where FK_Template=34 and TemplateParm1=0 and TemplateParm2=5 and Description='Videos'"
RunSQL "update CommandGroup set Description='Audio',psc_mod=psc_mod where FK_Template=34 and TemplateParm1=0 and TemplateParm2=4 and Description='Music'"
