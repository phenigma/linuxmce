<?php

session_start('Pluto-admin');

require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/masterusers.inc.php');
require('include/template.class.inc.php');

$section = @$_REQUEST['section'];

switch ($section) {	
	case 'login':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');		
	    include_once('operations/login.php');
	    login($output,$dbADO);
	break;
	case 'userHome':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/userHome.php');
	    userHome($output,$dbADO);
	break;
	case 'deviceTemplates':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=58');
	    include_once('operations/deviceTemplates.php');
	    deviceTemplates($output,$dbADO);
	break;
	case 'editAVDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/editAVDevice.php');
	    editAVDevice($output,$dbADO);
	break;
	case 'addManufacturer':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/manufacturers/addManufacturer.php');
	    addManufacturer($output,$dbADO);
	break;
	case 'editMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=59');
	    include_once('operations/editMasterDevice.php');
	    editMasterDevice($output,$dbADO);
	break;
	case 'addObjectToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/objects/addObjectToMasterDevice.php');
	    addObjectToMasterDevice($output,$dbADO);
	break;
	case 'deleteObjectFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/objects/deleteObjectFromMasterDevice.php');
	    deleteObjectFromMasterDevice($output,$dbADO);
	break;
	case 'addControlledViaToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/addControlledViaToMasterDevice.php');
	    addControlledViaToMasterDevice($output,$dbADO);
	break;	
	case 'editControlledViaToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/editControlledViaToMasterDevice.php');
	    editControlledViaToMasterDevice($output,$dbADO);
	break;	
	case 'deleteControlledViaFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deleteControlledViaFromMasterDevice.php');
	    deleteControlledViaFromMasterDevice($output,$dbADO);
	break;
	case 'deletePipeFromDeviceTemplateControlledVia':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deletePipeFromDeviceTemplateControlledVia.php');
	    deletePipeFromDeviceTemplateControlledVia($output,$dbADO);
	break;
	case 'deletePipeFromDeviceTemplateControlledViaCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deletePipeFromDeviceTemplateControlledViaCategory.php');
	    deletePipeFromDeviceTemplateControlledViaCategory($output,$dbADO);
	break;	
	case 'addControlledViaCategoryToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/addControlledViaCategoryToMasterDevice.php');
	    addControlledViaCategoryToMasterDevice($output,$dbADO);
	break;	
	case 'editControlledViaCategoryToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/editControlledViaCategoryToMasterDevice.php');
	    editControlledViaCategoryToMasterDevice($output,$dbADO);
	break;	
	case 'deleteControlledViaCategoryFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deleteControlledViaCategoryFromMasterDevice.php');
	    deleteControlledViaCategoryFromMasterDevice($output,$dbADO);
	break;
	case 'addPackageToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addPackageToMasterDevice.php');
	    addPackageToMasterDevice($output,$dbADO);
	break;	
	case 'addPackage':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addPackage.php');
	    addPackage($output,$dbADO);
	break;	
	case 'packagesList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=63');
	    include_once('operations/packages/packagesList.php');
	    packagesList($output,$dbADO);
	break;	

	case 'confirmPackages':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/confirmPackages.php');
	    confirmPackages($output,$dbADO);
	break;	
	case 'confirmDevicePackages':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/confirmDevicePackages.php');
	    confirmDevicePackages($output,$dbADO);
	break;	
	case 'addDistro':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDistro.php');
	    addDistro($output,$dbADO);
	break;	
	case 'deletePackageDistro':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/deletePackageDistro.php');
	    deletePackageDistro($output,$dbADO);
	break;
	case 'addRepositorySource':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addRepositorySource.php');
	    addRepositorySource($output,$dbADO);
	break;	
	case 'editRepositorySource':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editRepositorySource.php');
	    editRepositorySource($output,$dbADO);
	break;	
	case 'addDirectory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDirectory.php');
	    addDirectory($output,$dbADO);
	break;	
	case 'addDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDirectoryFile.php');
	    addDirectoryFile($output,$dbADO);
	break;	
	case 'editDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editDirectoryFile.php');
	    editDirectoryFile($output,$dbADO);
	break;
	case 'deleteDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/deleteDirectoryFile.php');
	    deleteDirectoryFile($output,$dbADO);
	break;
		
	case 'editPackageToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editPackageToMasterDevice.php');
	    editPackageToMasterDevice($output,$dbADO);
	break;	
	case 'deletePackageFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/deletePackageFromMasterDevice.php');
	    deletePackageFromMasterDevice($output,$dbADO);
	break;	
	case 'addParameter':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/addParameter.php');
	    addParameter($output,$dbADO);
	break;	
	case 'editDeviceData':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/editDeviceData.php');
	    editDeviceData($output,$dbADO);
	break;	
	case 'addParameterToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/addParameterToMasterDevice.php');
	    addParameterToMasterDevice($output,$dbADO);
	break;	
	case 'deleteParameterFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/deleteParameterFromMasterDevice.php');
	    deleteParameterFromMasterDevice($output,$dbADO);
	break;	
	//event list
	case 'deleteEventFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/deleteEventFromMasterDevice.php');
	    deleteEventFromMasterDevice($output,$dbADO);
	break;	
	case 'createEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEvent.php');
	    createEvent($output,$dbADO);
	break;	
	case 'createEventCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEventCategory.php');
	    createEventCategory($output,$dbADO);
	break;
	case 'editEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/editEvent.php');
	    editEvent($output,$dbADO);
	break;	
	case 'editEventParameterFromEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/editEventParameterFromEvent.php');
	    editEventParameterFromEvent($output,$dbADO);
	break;
	case 'deleteEventParameterFromEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/deleteEventParameterFromEvent.php');
	    deleteEventParameterFromEvent($output,$dbADO);
	break;
	case 'createEventParameter':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEventParameter.php');
	    createEventParameter($output,$dbADO);
	break;	
	//commands
	case 'addCommandGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/addCommandGroup.php');
	    addCommandGroup($output,$dbADO);	    
	break;	
	case 'editCommandGroupFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editCommandGroupFromMasterDevice.php');
	    editCommandGroupFromMasterDevice($output,$dbADO);	    
	break;	
	case 'addCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/addCommand.php');
	    addCommand($output,$dbADO);	    
	break;
	case 'editCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editCommand.php');
	    editCommand($output,$dbADO);	    
	break;
	case 'deleteParameterFromCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/deleteParameterFromCommand.php');
	    deleteParameterFromCommand($output,$dbADO);	    
	break;	
	case 'deleteCommandFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/deleteCommandFromMasterDevice.php');
	    deleteCommandFromMasterDevice($output,$dbADO);	    
	break;	
	case 'createParameterForCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/createParameterForCommand.php');
	    createParameterForCommand($output,$dbADO);	    
	break;	
	case 'editParameterFromCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editParameterFromCommand.php');
	    editParameterFromCommand($output,$dbADO);	    
	break;	
	//device params
	
	case 'editDeviceParams':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/editDeviceParams.php');
	    editDeviceParams($output,$dbADO);	    
	break;	
		
	case 'myDevices'://used by myDevicesFrames
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/myDevices.php');
	    myDevices($output,$dbADO);	    
	break;	
	case 'addMyDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/addMyDevice.php');
	    addMyDevice($output,$dbADO);	    
	break;
	case 'deleteMyDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteMyDevice.php');
	    deleteMyDevice($output,$dbADO);	    
	break;
	case 'devices':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/devices.php');
	    devices($output,$dbADO);
	break;
	case 'devicesAV':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/devicesAV.php');
	    devicesAV($output,$dbADO);
	break;
		
	//deviceCategory
	case 'editDeviceCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceCategory/editDeviceCategory.php');
	    editDeviceCategory($output,$dbADO);	    
	break;
	case 'deleteDeviceCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceCategory/deleteDeviceCategory.php');
	    deleteDeviceCategory($output,$dbADO);	    
	break;
	case 'deleteDeviceRelatedFromDeviceParams':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDeviceRelatedFromDeviceParams.php');
	    deleteDeviceRelatedFromDeviceParams($output,$dbADO);	    
	break;
	case 'deleteDevicePipeFromDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDevicePipeFromDevice.php');
	    deleteDevicePipeFromDevice($output,$dbADO);	    
	break;
	case 'deleteDeviceFromDeviceGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDeviceFromDeviceGroup.php');
	    deleteDeviceFromDeviceGroup($output,$dbADO);	    
	break;	
	case 'createDeviceGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/createDeviceGroup.php');
	    createDeviceGroup($output,$dbADO);	    
	break;
	case 'leftMenu':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/leftMenu.php');
	    leftMenu($output,$dbADO);	    
	break;
	//rooms
	case 'rooms':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=119');
	    include_once('operations/rooms/rooms.php');
	    rooms($output,$dbADO);	    
	break;	
	case 'deleteRoomFromInstallation':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/rooms/deleteRoomFromInstallation.php');
	    deleteRoomFromInstallation($output,$dbADO);	    
	break;	
	case 'entertainArea':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/entertainArea/entertainArea.php');
	    entertainArea($output,$dbADO);	    
	break;
	case 'deleteRoomFromEntertainArea':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/entertainArea/deleteRoomFromEntertainArea.php');
	    deleteRoomFromEntertainArea($output,$dbADO);	    
	break;	 
	//users_settings
	case 'installationSettings':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=116');
	    include_once('operations/users_settings/installationSettings.php');
	    installationSettings($output,$dbADO);	    
	break;
	case 'createInstallation':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/users_settings/createInstallation.php');
	    createInstallation($output,$dbADO);	    
	break;
	case 'users':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=118');
	    include_once('operations/users_settings/users.php');
	    users($output,$dbADO);	    
	break;
	case 'createUser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/createUser.php');
	    createUser($output,$dbADO);	    
	break;
	case 'removeUserFromInstallation':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/removeUserFromInstallation.php');
	    removeUserFromInstallation($output,$dbADO);	    
	break;
	case 'userChangePassword':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/userChangePassword.php');
	    userChangePassword($output,$dbADO);	    
	break;
	//orbiters
	case 'orbiters':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/orbiters/orbiters.php');
	    orbiters($output,$dbADO);	    
	break;
	case 'wizardOrbiters':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/orbiters/wizardOrbiters.php');
	    wizardOrbiters($output,$dbADO);	    
	break;
	
	//categories
	case 'manageCategories':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/manageCategories.php');
	    manageCategories($output,$dbADO);
	break;
	case 'deleteCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/deleteCategory.php');
	    deleteCategory($output,$dbADO);	    
	break;
	case 'editCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/editCategory.php');
	    editCategory($output,$dbADO);	    
	break;
	//my scenarios
	case 'myScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/myScenarios.php');
	    myScenarios($output,$dbADO);
	break;
	case 'mediaScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=124');
	    include_once('operations/myScenarios/mediaScenarios.php');
	    mediaScenarios($output,$dbADO);
	break;
	
	case 'editCommandGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/editCommandGroup.php');
	    editCommandGroup($output,$dbADO);
	break;
	case 'lightingScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=122');
	    include_once('operations/myScenarios/lightingScenarios.php');
	    lightingScenarios($output,$dbADO);
	break;
	case 'climateScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/support/index.php?section=document&docID=123');
	    include_once('operations/myScenarios/climateScenarios.php');
	    climateScenarios($output,$dbADO);
	break;	
	case 'wizardScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/wizardScenarios.php');
	    wizardScenarios($output,$dbADO);
	break;
	case 'deleteCommandFromCommandGroup_Command':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/deleteCommandFromCommandGroup_Command.php');
	    deleteCommandFromCommandGroup_Command($output,$dbADO);
	break;
	case 'addCommandGroupAsScenario':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/addCommandGroupAsScenario.php');
	    addCommandGroupAsScenario($output,$dbADO);	    
	break;	
	// infrared section
	case 'addInfraredGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/addInfraredGroup.php');
	    addInfraredGroup($output,$dbADO);	    
	break;	
	case 'editInfraredGroupFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/editInfraredGroupFromMasterDevice.php');
	    editInfraredGroupFromMasterDevice($output,$dbADO);	    
	break;	
	case 'deleteInfraredCommandFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/deleteInfraredCommandFromMasterDevice.php');
	    deleteInfraredCommandFromMasterDevice($output,$dbADO);	    
	break;
	case 'bootSequence':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/bootSequence.php');
	    bootSequence($output,$dbADO);	    
	break;
	//plugin space
	case 'customPage':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		$page=cleanString($_POST['page'],50);
		switch ($page) {
			case 'testXX':
				include_once('operations/pluginPages/pageXX.php');
	    		pageXX($output,$dbADO);	    				
			break;
			
			case 'testXX1':
				include_once('operations/pluginPages/pageXX1.php');
	    		pageXX1($output,$dbADO);	    				
			break;
		
			default:
			break;
		}
	break;
	case 'mediaBrowser';
	    include_once('operations/mediaBrowser/mediaBrowserFrameset.php');
	    mediaBrowserFrameset();	    
	break;		
	case 'mediaBrowserFrameset';
		$output = new Template($dbADO);	
		$output->setTemplateFileType('homeWithFrames');
		$output->setLeftFrameSrc('index.php?section=leftMediaBrowser');
		$output->setRightFrameSrc('index.php?section=mainMediaBrowser');
		$output->setBody('');		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	break;
	case 'leftMediaBrowser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/leftMediaBrowser.php');
	    leftMediaBrowser($output,$dbADO);	    
	break;
	case 'mainMediaBrowser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mainMediaBrowser.php');
	    mainMediaBrowser($output,$mediadbADO);	    
	break;

	case 'mediaFilesSync';
	    include_once('operations/mediaBrowser/mediaFilesSync.php');
	    mediaFilesSync();	    
	break;		
	case 'mediaFilesSyncFrameset';
		$output = new Template($dbADO);	
		$output->setTemplateFileType('homeWithFrames');
		$output->setLeftFrameSrc('index.php?section=leftMediaFilesSync');
		$output->setRightFrameSrc('index.php?section=mainMediaFilesSync');
		$output->setBody('');		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	break;
	case 'leftMediaFilesSync':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/leftMediaFilesSync.php');
	    leftMediaFilesSync($output,$mediadbADO);	    
	break;
	case 'mainMediaFilesSync':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mainMediaFilesSync.php');
	    mainMediaFilesSync($output,$mediadbADO);	    
	break;
	
	case 'floorplanWizard':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/floorplan/floorplanWizard.php');
	    floorplanWizard($output,$dbADO);
	break;
	case 'uploadFloorplan':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/floorplan/uploadFloorplan.php');
	    uploadFloorplan($output,$dbADO);
	break;
	case 'eventsHandler':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/eventsHandler.php');
	    eventsHandler($output,$dbADO);
	break;
	case 'editEventHandler':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/editEventHandler.php');
	    editEventHandler($output,$dbADO);
	break;

	case 'securitySettings':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/securitySettings.php');
	    securitySettings($output,$dbADO);
	break;

	case '';
		$output = new Template($dbADO);	
		
		$output->setTemplateFileType('homeWithFrames');
		
		$output->setLeftFrameSrc('index.php?section=leftMenu&rightSection=login');
		$output->setRightFrameSrc('index.php?section=login');
		
		$output->setBody('');		
		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	default:			
		$output = new Template($dbADO);	
		
		$output->setTemplateFileType('home');
		
		$output->setBody('Not a valid section');		
		
		
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
  		
	break;
}
?>