# phpMyAdmin SQL Dump
# version 2.5.7
# http://www.phpmyadmin.net
#
# Host: localhost
# Generation Time: Sep 08, 2004 at 12:18 PM
# Server version: 4.0.16
# PHP Version: 4.3.8
# 
# Database : `pluto`
# 

# --------------------------------------------------------

#
# Table structure for table `FAQ`
#
# Creation: Sep 08, 2004 at 11:08 AM
# Last update: Sep 08, 2004 at 11:46 AM
#

DROP TABLE IF EXISTS `FAQ`;
CREATE TABLE `FAQ` (
  `faqID` int(10) unsigned NOT NULL auto_increment,
  `faqFAQCategID` int(11) NOT NULL default '0',
  `faqQuestion` text NOT NULL,
  `faqAnswer` text NOT NULL,
  `fakeField` char(1) NOT NULL default '0',
  `markedDeleted` char(1) NOT NULL default '0',
  PRIMARY KEY  (`faqID`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;

#
# Dumping data for table `FAQ`
#

INSERT INTO `FAQ` (`faqID`, `faqFAQCategID`, `faqQuestion`, `faqAnswer`, `fakeField`, `markedDeleted`) VALUES (1, 1, 'one question', 'and one answer', '1', '0');

# --------------------------------------------------------

#
# Table structure for table `FAQCategories`
#
# Creation: Sep 08, 2004 at 11:06 AM
# Last update: Sep 08, 2004 at 11:18 AM
#

DROP TABLE IF EXISTS `FAQCategories`;
CREATE TABLE `FAQCategories` (
  `faqCategID` smallint(5) unsigned NOT NULL auto_increment,
  `faqCategName` varchar(255) NOT NULL default '',
  `fakeField` char(1) NOT NULL default '0',
  `markedDeleted` char(1) NOT NULL default '0',
  PRIMARY KEY  (`faqCategID`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;

#
# Dumping data for table `FAQCategories`
#

INSERT INTO `FAQCategories` (`faqCategID`, `faqCategName`, `fakeField`, `markedDeleted`) VALUES (1, 'Categ 1', '1', '0');

# --------------------------------------------------------

#
# Table structure for table `News`
#
# Creation: Sep 08, 2004 at 10:12 AM
# Last update: Sep 08, 2004 at 11:06 AM
#

DROP TABLE IF EXISTS `News`;
CREATE TABLE `News` (
  `newsID` int(10) unsigned NOT NULL auto_increment,
  `newsNewsCategID` int(11) NOT NULL default '0',
  `newsTitle` varchar(255) NOT NULL default '',
  `newsContent` text NOT NULL,
  `fakeField` char(1) NOT NULL default '0',
  `markedDeleted` char(1) NOT NULL default '0',
  PRIMARY KEY  (`newsID`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

#
# Dumping data for table `News`
#

INSERT INTO `News` (`newsID`, `newsNewsCategID`, `newsTitle`, `newsContent`, `fakeField`, `markedDeleted`) VALUES (1, 1, 'first news', '<IMG height=56 alt="" src="http://localhost/pluto_home/userimages/icon_account.gif" width=65 border=0><IMG height=16 alt=test src="http://localhost/pluto_home/userimages/button_reset.png" width=16 border=0>&nbsp;the best <STRONG>news</STRONG> in town', '1', '0'),
(2, 3, 'test title', 'uraaa', '1', '0');

# --------------------------------------------------------

#
# Table structure for table `NewsCategories`
#
# Creation: Sep 08, 2004 at 09:08 AM
# Last update: Sep 08, 2004 at 09:43 AM
#

DROP TABLE IF EXISTS `NewsCategories`;
CREATE TABLE `NewsCategories` (
  `ncID` smallint(5) unsigned NOT NULL auto_increment,
  `ncName` varchar(255) NOT NULL default '',
  `fakeField` char(1) NOT NULL default '0',
  `markedDeleted` char(1) NOT NULL default '0',
  PRIMARY KEY  (`ncID`)
) TYPE=MyISAM AUTO_INCREMENT=4 ;

#
# Dumping data for table `NewsCategories`
#

INSERT INTO `NewsCategories` (`ncID`, `ncName`, `fakeField`, `markedDeleted`) VALUES (1, 'General', '1', '0'),
(2, 'General', '1', '1'),
(3, 'Not general', '1', '0');

# --------------------------------------------------------

#
# Table structure for table `Users`
#
# Creation: Sep 05, 2004 at 08:24 PM
# Last update: Sep 08, 2004 at 08:41 AM
#

DROP TABLE IF EXISTS `Users`;
CREATE TABLE `Users` (
  `PKID_Users` int(11) NOT NULL auto_increment,
  `UserName` varchar(25) NOT NULL default '',
  `Password` varchar(32) NOT NULL default '',
  `HasMailbox` tinyint(4) NOT NULL default '0',
  `InternalMailbox` tinyint(4) NOT NULL default '0',
  `AccessGeneralMailbox` tinyint(4) NOT NULL default '0',
  `Extension` int(11) default NULL,
  `FirstName` varchar(20) default NULL,
  `LastName` varchar(30) default NULL,
  `Nickname` varchar(15) default NULL,
  `ExtensionRingTimeout` int(11) NOT NULL default '0',
  `VoicemailEmail` varchar(50) default NULL,
  `isPowerUser` tinyint(4) NOT NULL default '0',
  `isTopLevelUserName` tinyint(4) NOT NULL default '0',
  `VoicemailName` varchar(25) default NULL,
  `FKID_Installation_Main` int(11) NOT NULL default '0',
  `UsesEPG` tinyint(4) NOT NULL default '0',
  `plutohomeAdminLevel` smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  (`PKID_Users`)
) TYPE=MyISAM AUTO_INCREMENT=420 ;

#
# COMMENTS FOR TABLE `Users`:
#   `plutohomeAdminLevel`
#       `1-normal user,3-admin level(news edit)`
#

#
# Dumping data for table `Users`
#

INSERT INTO `Users` (`PKID_Users`, `UserName`, `Password`, `HasMailbox`, `InternalMailbox`, `AccessGeneralMailbox`, `Extension`, `FirstName`, `LastName`, `Nickname`, `ExtensionRingTimeout`, `VoicemailEmail`, `isPowerUser`, `isTopLevelUserName`, `VoicemailName`, `FKID_Installation_Main`, `UsesEPG`, `plutohomeAdminLevel`) VALUES (8, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 0, 100, 'Race', 'Vanderdecken', NULL, 20, 'race@1control.com', 1, 1, NULL, 6, 1, 3),
(17, 'User', '123', 0, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 4, 0, 3),
(18, 'User', '123', 0, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 7, 0, 3),
(19, 'User', '123', 0, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 9, 0, 3),
(20, 'jjhuff', '123', 1, 0, 1, 113, 'Justin', 'Huff', NULL, 20, 'jjhuff@1control.com', 1, 1, NULL, 9, 1, 3),
(22, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 122, 'Matt', 'Lawson', NULL, 20, 'matt@plutohome.com', 1, 1, NULL, 3, 0, 3),
(23, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, 123, 'Dan', 'Dennedy', NULL, 20, 'dan@plutohome.com', 1, 1, NULL, 3, 0, 3),
(24, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, 129, 'Joe', 'Ellis', NULL, 20, 'joe@plutohome.com', 1, 1, NULL, 3, 0, 3),
(25, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, 115, 'Sean', 'Dotson', NULL, 20, 'sean@plutohome.com', 1, 1, NULL, 3, 0, 3),
(26, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 127, 'Jim', 'Blake', NULL, 20, 'jim@plutohome.com', 1, 1, NULL, 3, 0, 3),
(27, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, 117, 'Josh', 'Donlan', NULL, 20, 'josh@plutohome.com', 1, 1, NULL, 3, 0, 3),
(28, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, 119, 'Jose', 'Gandullia', NULL, 20, 'jose@plutohome.com', 1, 1, NULL, 3, 0, 3),
(29, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 110, 'Aaron', 'Baalbergen', NULL, 20, 'aaron@plutohome.com', 1, 1, NULL, 3, 0, 3),
(30, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, 126, 'Race', 'Vanderdecken', 'Racer', 20, 'race@1control.com', 1, 0, NULL, 3, 0, 3),
(32, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 114, 'Indi', NULL, NULL, 20, 'indi@plutohome.com', 1, 1, NULL, 3, 0, 3),
(33, 'dragos', 'd18928fabad59ca8760b9b81dae6a71c', 1, 0, 1, 128, 'Dragos', 'Grosu', NULL, 20, 'tony@plutohome.com', 1, 1, NULL, 3, 0, 1),
(34, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 100, 'Matt', NULL, NULL, 20, 'matt@test_server.plutohome.com', 1, 0, NULL, 11, 0, 3),
(35, 'jimblake', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 100, 'Jim', 'Blake', 'Jim', 20, NULL, 1, 0, NULL, 12, 0, 3),
(36, 'mary', 'b8e7be5dfa2ce0714d21dcfc7d72382c', 1, 0, 1, 101, 'Mary', 'Blake', 'Mary', 20, NULL, 0, 0, NULL, 12, 0, 3),
(37, 'johnny', 'f4eb27cea7255cea4d1ffabf593372e8', 1, 0, 0, 102, 'Johnny', 'Blake', 'Johnny', 20, NULL, 0, 0, NULL, 12, 0, 3),
(38, 'carrie', '97b9308f6a1f6a8524a91450dca473fa', 1, 0, 0, 103, 'Carrie', 'Blake', 'Carrie', 20, NULL, 0, 0, NULL, 12, 0, 3),
(39, 'johndoe', '527bd5b5d689e2c32ae974c6229ff785', 0, 0, 1, NULL, 'John Doe', NULL, NULL, 20, NULL, 1, 0, NULL, 13, 0, 3),
(77, 'miguel', '9eb0c9605dc81a68731f61b3e0838937', 1, 1, 1, NULL, 'Miguel', NULL, NULL, 10, 'miguel@xine.plutohome.com', 1, 0, NULL, 19, 0, 3),
(78, 'kermit', 'eea342a57c9313484e2b73715a5cbdb1', 1, 0, 0, 101, 'Kermit', 'LeFrogg', 'Kermy', 10, 'kermit@test_server.plutohome.com', 0, 0, NULL, 11, 0, 3),
(137, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 101, NULL, NULL, NULL, 20, 'matt@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(138, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, 102, NULL, NULL, NULL, 20, 'dan@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(139, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, 103, NULL, NULL, NULL, 20, 'joe@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(140, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, 104, NULL, NULL, NULL, 20, 'sean@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(141, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 105, NULL, NULL, NULL, 20, 'jim@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(142, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, 106, NULL, NULL, NULL, 20, 'josh@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(143, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, 107, NULL, NULL, NULL, 20, 'jose@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(144, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 108, NULL, NULL, NULL, 20, 'aaron@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(145, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, 109, NULL, NULL, NULL, 20, 'race@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(147, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 110, NULL, NULL, NULL, 20, 'indi@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(148, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, 111, NULL, NULL, NULL, 20, 'tony@i24clones3.plutohome.com', 1, 0, NULL, 24, 0, 3),
(149, 'jimblake', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 100, 'jim', 'blake', NULL, 20, 'jimblake@i25clones12.plutohome.com', 1, 0, NULL, 25, 0, 3),
(150, 'mary', 'b8e7be5dfa2ce0714d21dcfc7d72382c', 1, 0, 1, 101, 'mary', 'blake', NULL, 20, 'mary@i25clones12.plutohome.com', 0, 0, NULL, 25, 0, 3),
(151, 'johnny', 'f4eb27cea7255cea4d1ffabf593372e8', 1, 0, 0, 102, 'johnny', 'blake', NULL, 20, 'johnny@i25clones12.plutohome.com', 0, 0, NULL, 25, 0, 3),
(152, 'carrie', '97b9308f6a1f6a8524a91450dca473fa', 1, 0, 0, 103, 'carrie', 'blake', NULL, 20, 'carrie@i25clones12.plutohome.com', 0, 0, NULL, 25, 0, 3),
(263, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, 155, 'Race Test', 'Dummy Account', 'Race2', 10, 'race2@office.plutohome.com', 0, 0, NULL, 3, 0, 3),
(264, 'custserv', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 0, 102, 'Customer', 'Service', NULL, 10, 'custserv@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(265, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 0, 129, 'Joe', 'Ellis', NULL, 10, 'joe@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(266, 'jimblake', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 37, 0, 3),
(267, 'mary', 'b8e7be5dfa2ce0714d21dcfc7d72382c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 37, 0, 3),
(268, 'johnny', 'f4eb27cea7255cea4d1ffabf593372e8', 1, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 37, 0, 3),
(269, 'carrie', '97b9308f6a1f6a8524a91450dca473fa', 1, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 37, 0, 3),
(270, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 101, 'Jim', 'Blake', NULL, 4, 'jim@corpapart.plutohome.com', 1, 0, NULL, 38, 0, 3),
(271, 'guest', '084e0343a0486ff05530df6c705c8bb4', 1, 0, 0, 102, 'Guest', 'Guest', NULL, 4, 'guest@corpapart.plutohome.com', 0, 0, NULL, 38, 0, 3),
(272, 'jimblake', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 39, 0, 3),
(273, 'mary', 'b8e7be5dfa2ce0714d21dcfc7d72382c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 39, 0, 3),
(274, 'johnny', 'f4eb27cea7255cea4d1ffabf593372e8', 1, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 39, 0, 3),
(275, 'carrie', '97b9308f6a1f6a8524a91450dca473fa', 1, 0, 0, NULL, NULL, NULL, NULL, 20, NULL, 0, 0, NULL, 39, 0, 3),
(281, 'rob', 'd8a976d3b65e10fe72aa7368a2aa5a7a', 1, 0, 1, 101, 'Rob', 'Raper', 'djrobx', 20, 'djrobx@djrobx.com', 1, 1, NULL, 2, 0, 3),
(282, 'danc', '89a349d3a892c13faf5cd4a1a55e2b10', 1, 0, 1, 102, 'Dan', 'Conklin', 'tvrdan', 20, 'dan@djrobx.com', 1, 0, NULL, 2, 0, 3),
(283, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(284, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(285, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(286, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(287, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(288, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(289, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(290, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(291, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(292, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(293, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 40, 0, 3),
(294, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 0, 0, NULL, 40, 0, 3),
(295, 'sales', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 0, 101, 'Sales', 'Department', NULL, 10, 'sales@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(296, 'jose', '662eaa47199461d01a623884080934ab', 1, 0, 0, 119, 'Jose', 'Gandullia', NULL, 10, 'jose@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(297, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 110, 'Aaron', 'Baalbergen', NULL, 10, 'aaron@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(298, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(299, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(300, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(301, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(302, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(303, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(304, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(305, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(306, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(307, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(308, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 41, 0, 3),
(309, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 0, 0, NULL, 41, 0, 3),
(310, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(311, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(312, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(313, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(314, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(315, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(316, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(317, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(318, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(319, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(320, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 42, 0, 3),
(321, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 0, 0, NULL, 42, 0, 3),
(322, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 43, 0, 3),
(323, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(324, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(325, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(326, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(327, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(328, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(329, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(330, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(331, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(332, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(333, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, NULL, NULL, NULL, NULL, 20, NULL, 1, 0, NULL, 44, 0, 3),
(334, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 0, 0, NULL, 44, 0, 3),
(335, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 112, NULL, NULL, NULL, 20, 'matt@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(336, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 1, 101, NULL, NULL, NULL, 20, 'dan@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(337, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, 102, NULL, NULL, NULL, 20, 'joe@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(338, 'sean', '9b938710211168f2902f9ed4357cd05c', 1, 0, 1, 103, NULL, NULL, NULL, 20, 'sean@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(339, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 1, 104, NULL, NULL, NULL, 20, 'jim@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(340, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, 105, NULL, NULL, NULL, 20, 'josh@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(341, 'jose', '4c6f952c74631d6ed3f6e6c03f536997', 1, 0, 1, 106, NULL, NULL, NULL, 20, 'jose@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(342, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 107, NULL, NULL, NULL, 20, 'aaron@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(343, 'race', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, 108, NULL, NULL, NULL, 20, 'race@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(344, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 109, NULL, NULL, NULL, 20, 'indi@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(345, 'tony', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 1, 110, NULL, NULL, NULL, 20, 'tony@i45clones3.plutohome.com', 1, 0, NULL, 45, 0, 3),
(346, 'race2', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 1, 111, NULL, NULL, NULL, 10, 'race2@i45clones3.plutohome.com', 0, 0, NULL, 45, 0, 3),
(347, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, 101, 'Joe', 'Ellis', 'Joe', 10, 'joe@hammond.plutohome.com', 1, 0, NULL, 46, 0, 3),
(348, 'erin', '5f5be3890fa875bfe8fa797b4ba6a397', 1, 0, 1, 102, 'Erin', 'Ellis', 'Erin', 10, 'erin@hammond.plutohome.com', 1, 0, NULL, 46, 0, 3),
(349, 'thomas', '83e4a96aed96436c621b9809e258b309', 1, 0, 0, 104, 'Thomas', 'Martinez', NULL, 10, 'thomas@office.plutohome.com', 1, 0, NULL, 3, 0, 3),
(350, 'geoffrey', 'ea62ceda2458916567afa1afa2307ca1', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 1, 0, NULL, 47, 0, 3),
(351, 'jayne', '4eb8a566e4386589a98c4a13ccd12878', 1, 0, 1, NULL, NULL, NULL, NULL, 10, NULL, 1, 0, NULL, 47, 0, 3),
(352, 'tofer', '0c9898be5dbf8094194615e5177daee1', 1, 0, 1, 101, 'tofer', NULL, 'Tofer', 10, 'aaron@ehexpo.plutohome.com', 1, 0, NULL, 48, 0, 3),
(353, 'paul', '6c63212ab48e8401eaf6b59b95d816a9', 1, 0, 1, 102, 'paul', NULL, 'paul', 10, 'mark@ehexpo.plutohome.com', 1, 0, NULL, 48, 0, 3),
(354, 'guest', '084e0343a0486ff05530df6c705c8bb4', 1, 0, 1, 103, 'guest', NULL, NULL, 10, 'dave@ehexpo.plutohome.com', 1, 0, NULL, 48, 0, 3),
(356, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 114, 'Indi', 'Sukhra', NULL, 10, 'indi@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(357, 'tech', 'ddc5f5e86d2f85e1b1ff763aff13ce0a', 1, 0, 0, 103, 'Tech', 'Support', NULL, 10, 'tech@dsouza.plutohome.com', 0, 0, NULL, 36, 0, 3),
(358, 'account', '2e2a7a2e57d1578bf33a559b20c463f6', 1, 0, 0, 104, 'Accounting', 'Department', NULL, 10, 'account@dsouza.plutohome.com', 0, 0, NULL, 36, 0, 3),
(359, 'jim', '5e027396789a18c37aeda616e3d7991b', 1, 0, 0, 127, 'Jim', NULL, NULL, 10, 'jim@dsouza.plutohome.com', 0, 0, NULL, 36, 0, 3),
(360, 'rob', 'd8a976d3b65e10fe72aa7368a2aa5a7a', 1, 0, 1, 101, 'Rob', 'Raper', 'djrobx', 10, 'rob@gandullia.plutohome.com', 1, 0, NULL, 49, 0, 3),
(361, 'dan', '89a349d3a892c13faf5cd4a1a55e2b10', 1, 0, 1, 102, 'Dan', 'Conklin', 'tvrdan', 10, 'dan@gandullia.plutohome.com', 1, 0, NULL, 49, 0, 3),
(362, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 101, 'Matt', 'Lawson', NULL, 10, 'matt@lawson.plutohome.com', 1, 0, NULL, 50, 0, 3),
(363, 'karin', '97468f1980416a4376b44e701d25f24b', 1, 0, 1, 102, 'Karin', 'Lawson', NULL, 10, 'karin@lawson.plutohome.com', 1, 0, NULL, 50, 0, 3),
(364, 'joshua', 'cb16cefd41d8c728b35361f776711ead', 1, 0, 1, 101, 'Joshua', 'Donlan', NULL, 10, 'joshua@donlan.plutohome.com', 1, 0, NULL, 51, 0, 3),
(365, 'kristen', 'f52b06185d58a5eb571ec260092f099f', 1, 0, 1, 102, 'Kristen', 'Harding', NULL, 10, 'kristen@donlan.plutohome.com', 0, 0, NULL, 51, 0, 3),
(366, 'rob', 'dba40a322f0337eaceae33edaf11338b', 1, 0, 1, 101, 'Rob', 'Roland', 'Rob', 10, 'rob@roland.plutohome.com', 1, 0, NULL, 52, 0, 3),
(367, 'noel', '510b457037446c35192b1be9a203d990', 1, 0, 1, 102, 'Noel', 'Gibson', 'Noel', 10, 'noel@roland.plutohome.com', 0, 0, NULL, 52, 0, 3),
(368, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 101, 'aaron', NULL, NULL, 10, 'aaron@test-11-29.plutohome.com', 1, 0, NULL, 53, 0, 3),
(369, 'jose', '662eaa47199461d01a623884080934ab', 1, 0, 1, 102, 'jose', NULL, NULL, 10, 'jose@test-11-29.plutohome.com', 1, 0, NULL, 53, 0, 3),
(370, 'josh', 'f94adcc3ddda04a8f34928d862f404b4', 1, 0, 1, 103, 'josh', NULL, NULL, 10, 'josh@test-11-29.plutohome.com', 1, 0, NULL, 53, 0, 3),
(371, 'joe', '8ff32489f92f33416694be8fdc2d4c22', 1, 0, 1, 101, 'joe', 'ellis', NULL, 10, 'joe@ellis.plutohome.com', 1, 0, NULL, 54, 0, 3),
(372, 'erin', '5f5be3890fa875bfe8fa797b4ba6a397', 1, 0, 1, 102, 'erin', 'ellis', NULL, 10, 'erin@ellis.plutohome.com', 1, 0, NULL, 54, 0, 3),
(373, 'jerry', '30035607ee5bb378c71ab434a6d05410', 1, 0, 1, 101, 'jerry', 'ellis', NULL, 10, 'jerry@jellis.plutohome.com', 1, 0, NULL, 55, 0, 3),
(374, 'rose', 'fcdc7b4207660a1372d0cd5491ad856e', 1, 0, 1, 102, 'rose', 'ellis', NULL, 10, 'rose@jellis.plutohome.com', 1, 0, NULL, 55, 0, 3),
(375, 'justin', '53dd9c6005f3cdfc5a69c5c07388016d', 1, 0, 1, 103, 'justin', 'ellis', NULL, 10, 'justin@jellis.plutohome.com', 0, 0, NULL, 55, 0, 3),
(376, 'jesse', 'a1361cb85be840d6a2d762c68e4910e2', 1, 0, 1, 104, 'jesse', 'ellis', NULL, 10, 'jesse@jellis.plutohome.com', 0, 0, NULL, 55, 0, 3),
(377, 'aaron', '449a36b6689d841d7d27f31b4b7cc73a', 1, 0, 1, 101, 'aaron', NULL, NULL, 10, 'aaron@demo1202.plutohome.com', 1, 0, NULL, 56, 0, 3),
(378, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 102, 'indi', NULL, NULL, 10, 'indi@demo1202.plutohome.com', 1, 0, NULL, 56, 0, 3),
(379, 'dan', '9180b4da3f0c7e80975fad685f7f134e', 1, 0, 0, 103, 'dan', NULL, NULL, 10, 'dan@demo1202.plutohome.com', 1, 0, NULL, 56, 0, 3),
(380, 'tofer', '0c9898be5dbf8094194615e5177daee1', 1, 1, 1, 101, 'Tofer', 'Williams', 'Tofer', 10, 'tofer@williams.plutohome.com', 1, 0, NULL, 57, 0, 3),
(381, 'stephen', '7ff36797539130f77445f48da5d4a126', 1, 0, 1, 101, 'Stephen', NULL, NULL, 10, 'stephen@lewis.plutohome.com', 1, 0, NULL, 58, 0, 3),
(382, 'corinne', '092e602c7aa7926ba2ebfdf4362a715a', 1, 0, 1, 102, 'Corinne', NULL, NULL, 10, 'corinne@lewis.plutohome.com', 1, 0, NULL, 58, 0, 3),
(383, 'aaron', '8e646e99629e5c3541ac8ef230c19bce', 1, 0, 1, 101, 'aaron', NULL, NULL, 10, 'aaron@testsystem.plutohome.com', 1, 0, NULL, 59, 0, 3),
(384, 'indi', 'ac5e72357115f00c0bc6ac83bb203127', 1, 0, 1, 102, 'indi', NULL, NULL, 10, 'indi@testsystem.plutohome.com', 1, 0, NULL, 59, 0, 3),
(389, 'john', '527bd5b5d689e2c32ae974c6229ff785', 1, 0, 1, 101, 'John', NULL, NULL, 10, 'john@demo60.plutohome.com', 1, 0, NULL, 60, 0, 3),
(390, 'lenee', 'ac575e3eecf0fa410518c2d3a2e7209f', 1, 0, 1, 102, 'Lenee', NULL, NULL, 10, 'lenee@demo60.plutohome.com', 1, 0, NULL, 60, 0, 3),
(391, 'paul', '59f5b947ea11d2b0bb64dcc47770c6f4', 1, 1, 1, 102, 'Paul', 'Tanber', 'Paul', 10, 'paul@williams.plutohome.com', 1, 0, NULL, 57, 0, 3),
(392, 'kneupper', '63b04a371849694ef3864687adcb410a', 1, 0, 1, 101, 'Karl-Dieter', 'Kneupper', 'kdk', 10, 'kneupper@system61.plutohome.com', 1, 0, NULL, 61, 0, 3),
(394, 'colby', 'ea82410c7a9991816b5eeeebe195e20a', 1, 1, 1, 104, 'Mark', 'Colbert', 'Colby', 10, 'colby@williams.plutohome.com', 1, 0, NULL, 57, 0, 3),
(395, 'rob', 'd8a976d3b65e10fe72aa7368a2aa5a7a', 0, 0, 1, 101, 'rob', NULL, 'djrobx', 10, 'rob@valenciatest.plutohome.com', 1, 0, NULL, 62, 0, 3),
(396, 'corina', 'd41d8cd98f00b204e9800998ecf8427e', 1, 0, 1, 102, 'Corina', 'Kneupper', NULL, 10, 'corina@kneupper.plutohome.com', 0, 0, NULL, 61, 0, 3),
(397, 'kneupper', '63b04a371849694ef3864687adcb410a', 0, 0, 1, 101, 'Karl', 'Dieter', NULL, 10, 'joe@hh24.plutohome.com', 1, 0, NULL, 63, 0, 3),
(399, 'carina', 'e68b262dfb5013ae634b182f5d390db8', 0, 0, 1, 102, 'Carina', NULL, NULL, 10, 'carina@kneupper_de.plutohome.com', 1, 0, NULL, 63, 0, 3),
(400, 'michelle', '2345f10bb948c5665ef91f6773b3e455', 1, 0, 1, 102, 'Rommy', NULL, 'Michelle', 10, 'michelle@burgener.plutohome.com', 1, 0, NULL, 64, 0, 3),
(401, 'siggi', '90e3cca5bbc231c1c1d6ffd01d04190b', 0, 0, 1, 101, 'siggi', NULL, NULL, 10, 'siggi@siggi.plutohome.com', 1, 0, NULL, 65, 0, 3),
(402, 'guest', '0fb9cbecb7b8881511c69c39db643e8c', 0, 0, 0, 105, 'Adien', 'Achenbach', 'Adien', 10, 'guest@williams.plutohome.com', 0, 0, NULL, 57, 0, 3),
(403, 'aaron', '8e646e99629e5c3541ac8ef230c19bce', 1, 0, 1, 101, 'aaron', NULL, NULL, 10, 'aaron@pbhomeshow.plutohome.com', 1, 0, NULL, 66, 0, 3),
(404, 'matt', 'ce86d7d02a229acfaca4b63f01a1171b', 1, 0, 1, 102, 'matt', NULL, NULL, 10, 'matt@pbhomeshow.plutohome.com', 1, 0, NULL, 66, 0, 3),
(405, 'greg', '2ad4004f90593092c9363f9d8b836242', 0, 0, 1, 101, 'greg', NULL, NULL, 10, 'greg@bouchet.plutohome.com', 1, 0, NULL, 67, 0, 3),
(406, 'pedro', 'c6cc8094c2dc07b700ffcc36d64e2138', 1, 0, 0, 103, 'Robin', NULL, 'Pedro', 10, 'pedro@burgener.plutohome.com', 0, 0, NULL, 64, 0, 3),
(407, 'marco', 'f5888d0bb58d611107e11f7cbc41c97a', 1, 0, 1, 101, 'Bernhard', NULL, 'Marco', 10, 'marco@burgener.plutohome.com', 1, 0, NULL, 64, 0, 3),
(408, 'leon', 'c88a54c2e6b8153abf3bceb69b25c5a2', 1, 0, 1, 101, 'leon', NULL, NULL, 10, 'leon@bo-marbella.plutohome.com', 1, 0, NULL, 68, 0, 3),
(409, 'maite', 'c88a54c2e6b8153abf3bceb69b25c5a2', 1, 0, 1, 102, 'maite', NULL, NULL, 10, 'maite@bo-marbella.plutohome.com', 1, 0, NULL, 68, 0, 3),
(410, 'rick', '891f490e5d7bdb06d90d56f8d7db405f', 1, 0, 0, 103, 'rick', NULL, NULL, 10, 'rick@testsystem.plutohome.com', 1, 0, NULL, 59, 0, 3),
(411, 'rick', '891f490e5d7bdb06d90d56f8d7db405f', 1, 0, 0, 187, 'Rick', NULL, NULL, 10, 'rick@dsouza.plutohome.com', 0, 0, NULL, 36, 0, 3),
(412, 'djrob', '8781f0ee1c20078075ed319ceeb36853', 0, 0, 0, 130, 'Rob', 'Raper', NULL, 10, 'djrob@dsouza.plutohome.com', 1, 0, NULL, 36, 0, 3),
(413, 'robroland', '5fc6ceb538065afa1812d8e4378c3ea2', 0, 0, 0, 131, 'Rob', 'Roland', NULL, 10, 'robroland@dsouza.plutohome.com', 0, 0, NULL, 36, 0, 3),
(414, 'john', '527bd5b5d689e2c32ae974c6229ff785', 1, 0, 1, 101, 'john', NULL, NULL, 10, 'john@microspace-hybrid.plutohome.com', 1, 0, NULL, 69, 0, 3),
(415, 'felix', '9d836f2c72db315720d469015f5b7753', 1, 0, 1, 101, 'Felix', 'Kunz', NULL, 10, 'felix@digitallogic.plutohome.com', 1, 0, NULL, 70, 0, 3),
(416, 'arnold', '9d836f2c72db315720d469015f5b7753', 1, 0, 1, 102, 'Arnold', 'Stotzer', NULL, 10, 'arnold@digitallogic.plutohome.com', 1, 0, NULL, 70, 0, 3),
(417, 'jan', '9d836f2c72db315720d469015f5b7753', 1, 0, 1, 103, 'Jan', 'Schluep', NULL, 10, 'jan@digitallogic.plutohome.com', 1, 0, NULL, 70, 0, 3),
(418, 'dan', '89a349d3a892c13faf5cd4a1a55e2b10', 0, 0, 1, 102, 'Dan', NULL, 'tvrdan', 10, 'dan@valenciatest.plutohome.com', 1, 0, NULL, 62, 0, 3),
(419, 'rick', '891f490e5d7bdb06d90d56f8d7db405f', 1, 0, 1, 101, 'rick', NULL, NULL, 10, 'rick@rlewis.plutohome.com', 1, 0, NULL, 71, 0, 3);
