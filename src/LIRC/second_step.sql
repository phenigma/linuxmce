USE LIRC_Remote_Controls;

UPDATE LIRC_Remote_Controls SET Manufacturer=REPLACE(Manufacturer,'_',' ');

INSERT INTO Pluto_main_devel.Manufacturer(Description)

SELECT DISTINCT Manufacturer from LIRC_Remote_Controls.LIRC_Remote_ControlsM LEFT JOIN Pluto_main_devel.Manufacturer ON Manu
WHERE PK_Manufacturer IS NULL;

INSERT INTO Pluto_main.DeviceTemplate(Description,FK_Manufacturer,FK_DeviceCategory,ImplementsDCE,CommandLine,FK_Package)
SELECT Remote,PK_Manufacturer,118,1,'LIRC_DCE',340 FROM
LIRC_Remote_Controls.LIRC_Remote_ControlsM
LEFT JOIN Pluto_main.Manufacturer ON Manufacturer=Manufacturer.Description
LEFT JOIN Pluto_main.DeviceTemplate ON Remote=DeviceTemplate.Description
WHERE PK_DeviceTemplate IS NULL;

UPDATE LIRC_Remote_Controls.LIRC_Remote_ControlsM
LEFT JOIN Pluto_main.Manufacturer ON Manufacturer=Manufacturer.Description
LEFT JOIN Pluto_main.DeviceTemplate ON Remote=DeviceTemplate.Description AND FK_Manufacturer=PK_Manufacturer
SET EK_DeviceTemplate=PK_DeviceTemplate;

USE pluto_main;

INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia(FK_DeviceTemplate,FK_DeviceCategory)
SELECT PK_DeviceTemplate,6 FROM DeviceTemplate
LEFT JOIN DeviceTemplate_DeviceCategory_ControlledVia ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE FK_DeviceTemplate IS NULL and DeviceTemplate.FK_DeviceCategory=118;

INSERT INTO Pluto_main.DeviceTemplate_DeviceData(FK_DeviceTemplate,FK_DeviceData,IK_DeviceData)
SELECT EK_DeviceTemplate,59,Configuration FROM
LIRC_Remote_Controls.LIRC_Remote_ControlsM
LEFT JOIN DeviceTemplate_DeviceData ON FK_DeviceTemplate=EK_DeviceTemplate AND FK_DeviceData=59
WHERE FK_DeviceTemplate IS NULL;
