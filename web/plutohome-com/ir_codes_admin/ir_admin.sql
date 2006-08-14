-- need to apply this sql to work copy of local database for ID admin
DROP TABLE IF EXISTS `ir_admin_batch_history`;
CREATE TABLE IF NOT EXISTS `ir_admin_batch_history` (
  `PK_ir_admin_batch_history` int(11) NOT NULL auto_increment,
  `psc_batch` int(11) NOT NULL,
  `repository` varchar(10), 
  PRIMARY KEY  (`PK_ir_admin_batch_history`)
) TYPE=MyISAM;

DROP TABLE IF EXISTS `ir_admin_group_history`;
CREATE TABLE IF NOT EXISTS `ir_admin_group_history` (
  `PK_ir_admin_group_history` int(11) NOT NULL auto_increment,
  `FK_InfraredGroup` int(11) NOT NULL,
  PRIMARY KEY  (`PK_ir_admin_batch_history`)
) TYPE=MyISAM;
