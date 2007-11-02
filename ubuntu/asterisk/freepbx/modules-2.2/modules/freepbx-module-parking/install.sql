CREATE TABLE IF NOT EXISTS `parkinglot` (
  `id` VARCHAR( 20 ) NOT NULL default '1',
  `keyword` varchar(40) NOT NULL default '',
  `data` varchar(150) NOT NULL default '',
  PRIMARY KEY  (`id`,`keyword`)
) TYPE=MyISAM;
