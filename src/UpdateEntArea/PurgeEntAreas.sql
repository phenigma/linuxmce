DELETE CommandGroup_Command_CommandParameter FROM 
CommandGroup_Command_CommandParameter
JOIN CommandGroup_Command ON PK_CommandGroup_Command=FK_CommandGroup_Command
JOIN CommandGroup ON PK_CommandGroup=CommandGroup_Command.FK_CommandGroup
JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
JOIN EntertainArea ON FK_EntertainArea=pK_EntertainArea
LEFT JOIN Room ON FK_Room=PK_Room
WHERE ManuallyConfigureEA is null OR ManuallyConfigureEA=0;

DELETE CommandGroup_Command
FROM CommandGroup_Command 
JOIN CommandGroup ON PK_CommandGroup=CommandGroup_Command.FK_CommandGroup
JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
JOIN EntertainArea ON FK_EntertainArea=pK_EntertainArea
LEFT JOIN Room ON FK_Room=PK_Room
WHERE ManuallyConfigureEA is null OR ManuallyConfigureEA=0;

DELETE CommandGroup
FROM CommandGroup 
JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
JOIN EntertainArea ON FK_EntertainArea=pK_EntertainArea
LEFT JOIN Room ON FK_Room=PK_Room
WHERE ManuallyConfigureEA is null OR ManuallyConfigureEA=0;

DELETE CommandGroup_EntertainArea
FROM CommandGroup_EntertainArea 
JOIN EntertainArea ON FK_EntertainArea=pK_EntertainArea
LEFT JOIN Room ON FK_Room=PK_Room
WHERE ManuallyConfigureEA is null OR ManuallyConfigureEA=0;

DELETE EntertainArea
FROM EntertainArea 
LEFT JOIN Room ON FK_Room=PK_Room
WHERE ManuallyConfigureEA is null OR ManuallyConfigureEA=0;

DELETE CommandGroup_Command_CommandParameter 
FROM CommandGroup_Command_CommandParameter 
JOIN CommandGroup_Command ON PK_CommandGroup_Command=FK_CommandGroup_Command
JOIN CommandGroup ON PK_CommandGroup=CommandGroup_Command.FK_CommandGroup
LEFT JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
WHERE FK_Template IN (1,2,3,4,5,6,7,8,18) AND FK_EntertainArea IS NULL;

DELETE CommandGroup_Command
FROM CommandGroup_Command
JOIN CommandGroup ON PK_CommandGroup=CommandGroup_Command.FK_CommandGroup
LEFT JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
WHERE FK_Template IN (1,2,3,4,5,6,7,8,18) AND FK_EntertainArea IS NULL;

DELETE CommandGroup
FROM CommandGroup
LEFT JOIN CommandGroup_EntertainArea ON PK_CommandGroup=CommandGroup_EntertainArea.FK_CommandGroup
WHERE FK_Template IN (1,2,3,4,5,6,7,8,18) AND FK_EntertainArea IS NULL;


