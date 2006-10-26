CREATE TABLE Headends(
 PK_Headends int(11) NOT NULL auto_increment,
 headend_id char(7),
 community_name varchar(28),
 county_name varchar(25),
 county_size char(1),
 st_county_code char(5),
 state_served char(2),
 zip_code char(5),
 dma_code int(3),
 dma_name varchar(70),
 mso_code char(5),
 dma_rank char(4),
 headend_name varchar(42),
 headend_location varchar(28),
 mso_name varchar(42),
 time_zone_code int(1),
 PRIMARY KEY(PK_Headends),
 UNIQUE (headend_id,community_name,zip_code)
);

CREATE TABLE Lineups(
 FK_Headends char(7),
 FK_Stations int(12),
 device char(1),
 tms_chan char(5),
 service_tier int(1),
 effective_date char(8),
 expiration_date char(8)
);

CREATE TABLE Stations(
 PK_Stations int(12),
 station_time_zone varchar(30),
 station_name varchar(40),
 station_call_sign varchar(10),
 station_affil varchar(25),
 station_city varchar(20),
 station_state varchar(15),
 station_zip_code varchar(12),
 station_country varchar(15),
 dma_name varchar(70),
 dma_num int(10),
 fcc_channel_num int(8),
 PRIMARY KEY(PK_Stations)
);

CREATE TABLE UserLineups(
 PK_UserLineups int(11) not NULL auto_increment,
 FK_Headends int(11) not NULL,
 FK_Stations int(12) not NULL,
 sourceid int(10) not NULL,
 name varchar(20) not NULL,
 device char(1),
 tms_chan char(5),
 service_tier int(1),
 effective_date char(8),
 expiration_date char(8),
 PRIMARY KEY (PK_UserLineups),
 UNIQUE (FK_Headends,FK_Stations,sourceid)
);
