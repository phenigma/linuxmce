#!/bin/bash

OUT='insert into `phpbb2`.`phpbb_categories`(cat_id,cat_title) select PK_Package,Description FROM `pluto_main`.`Package` LEFT JOIN `phpbb2`.`phpbb_categories` ON PK_Package=cat_id WHERE HomePage IS NOT NULL AND cat_id is NULL';
echo $OUT | mysql -u root -pmoscow70bogata pluto_main

OUT='insert into `phpbb2`.`phpbb_forums`(forum_id,cat_id,forum_name,forum_desc) select PK_Package,PK_Package,"General",Comments FROM `pluto_main`.`Package` LEFT JOIN `phpbb2`.`phpbb_forums` ON PK_Package=cat_id WHERE HomePage IS NOT NULL AND cat_id is NULL';
echo $OUT | mysql -u root -pmoscow70bogata pluto_main

OUT='insert into `mantis`.`mantis_project_table`(id,name) select PK_Package,Package.Description FROM `pluto_main`.`Package` LEFT JOIN `mantis`.`mantis_project_table` ON PK_Package=id WHERE HomePage IS NOT NULL AND id is NULL';
echo $OUT | mysql -u root -pmoscow70bogata pluto_main
