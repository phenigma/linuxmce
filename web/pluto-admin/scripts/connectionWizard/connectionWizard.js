var last_z_index=1;
var tmpx=0;
var tmpy=0;
var flag_drag=0;
var object_to_drag=null;

if (document.layers) { // Netscape
	document.captureEvents(Event.MOUSEMOVE);
	document.onmousemove = captureMousePosition;
} else if (document.all) { // Internet Explorer
	document.onmousemove = captureMousePosition;
} else if (document.getElementById) { // Netcsape 6
	document.onmousemove = captureMousePosition;
}

xMousePos = 0; // Horizontal position of the mouse on the screen
yMousePos = 0; // Vertical position of the mouse on the screen
pipesArray=new Array('audio','video','audioLive','videoLive');

var startX=0;
var startY=0;

globalXFrom=-1;
globalYFrom=-1;
globalFromDevice=-1;
globalToDevice=-1;
globalInput=-1;
globalOutput=-1;
globalPipe='audio';
globalXTo=-1;
globalYTo=-1

audioPipe=new Array();
videoPipe=new Array();
audioLivePipe=new Array();
videoLivePipe=new Array();

function captureMousePosition(e) {
	if (document.layers) {
		xMousePos = e.pageX;
		yMousePos = e.pageY;
	} else if (document.all) {
		xMousePos = window.event.x+document.body.scrollLeft;
		yMousePos = window.event.y+document.body.scrollTop;
	} else if (document.getElementById) {
		xMousePos = e.pageX;
		yMousePos = e.pageY;
	}
}


function show_layer(val)
{
	bring_to_front(val);
	last_z_index++;
	eval("document.getElementById('"+val+"').style.zIndex=last_z_index;");
	eval("document.getElementById('"+val+"').style.display='';");
}

function hide_layer(val)
{
	eval("document.getElementById('"+val+"').style.display='none';");
}


function assign_res(val)
{
	show_layer('layer_assign');
}

function start_drag(val)
{
	eval("tmpx=xMousePos-parseInt(document.getElementById('"+val+"').style.left)");
	eval("tmpy=yMousePos-parseInt(document.getElementById('"+val+"').style.top)");
	startX=xMousePos;
	startY=yMousePos;
	flag_drag=1;
	object_to_drag=val;
	devID=val.substring(7);
	
	eval("audio_"+devID+".clear()");
	for(dev in audioPipe){
		if(audioPipe[dev] && audioPipe[dev]['to']==devID){
			eval("audio_"+dev+".clear()");
		}
	}
	eval("video_"+devID+".clear()");
	for(dev in videoPipe){
		if(videoPipe[dev] && videoPipe[dev]['to']==devID){
			eval("video_"+dev+".clear()");
		}
	}
	eval("audioLive_"+devID+".clear()");
	for(dev in audioLivePipe){
		if(audioLivePipe[dev] && audioLivePipe[dev]['to']==devID){
			eval("audioLive_"+dev+".clear()");
		}
	}
	eval("videoLive_"+devID+".clear()");
	for(dev in videoLivePipe){
		if(videoLivePipe[dev] && videoLivePipe[dev]['to']==devID){
			eval("videoLive_"+dev+".clear()");
		}
	}
}

function drag_layer()
{
	if(flag_drag==1)
	{
		eval("document.getElementById('"+object_to_drag+"').style.left=xMousePos-tmpx");
		eval("document.getElementById('"+object_to_drag+"').style.top=yMousePos-tmpy");
	}
}

function end_drag(val)
{
	flag_drag=0;
	object_to_drag=null;
	redrawPipes(val);
}

function drawPipe(device,connectionType,fromX,fromY,toX,toY)
{
	pipeName=connectionType+'_'+device;
	switch (connectionType){
		case 'audio':
			pipeColor='red';
		break;
		case 'video':
			pipeColor='blue';
		break;
		case 'audioLive':
			pipeColor='green';
		break;
		case 'videoLive':
			pipeColor='magenta';
		break;
	}
	eval(pipeName+".setColor(\""+pipeColor+"\")"); 
  	eval(pipeName+".drawLine("+fromX+", "+fromY+", "+toX+","+toY+")"); 
	eval(pipeName+".paint()");
}

function clearPipe(pipeName)
{
	eval(pipeName+".clear()");
}

function bring_to_front(val)
{
	last_z_index++;
	eval("document.getElementById('"+val+"').style.zIndex=last_z_index;");
	for(i=0;i<layersArray.length;i++){
		eval("document.getElementById('head_"+layersArray[i]+"').style.background='red';");
	}
}

function setPipeVariables()
{
	switch(globalPipe){
		case 'audio':
			audioPipe[globalFromDevice]=new Array();
			audioPipe[globalFromDevice]['to']=globalToDevice;
			audioPipe[globalFromDevice]['input']=globalInput;
			audioPipe[globalFromDevice]['output']=globalOutput;
			audioPipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
			setMessage('Audio Pipe set',0);
		break;
		case 'video':
			videoPipe[globalFromDevice]=new Array();
			videoPipe[globalFromDevice]['to']=globalToDevice;
			videoPipe[globalFromDevice]['input']=globalInput;
			videoPipe[globalFromDevice]['output']=globalOutput;
			videoPipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
			setMessage('Video Pipe set',0);
		break;
		case 'audioLive':
			audioLivePipe[globalFromDevice]=new Array();
			audioLivePipe[globalFromDevice]['to']=globalToDevice;
			audioLivePipe[globalFromDevice]['input']=globalInput;
			audioLivePipe[globalFromDevice]['output']=globalOutput;
			audioLivePipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
			setMessage('Audio Live Pipe set',0);
		break;
		case 'videoLive':
			videoLivePipe[globalFromDevice]=new Array();
			videoLivePipe[globalFromDevice]['to']=globalToDevice;
			videoLivePipe[globalFromDevice]['input']=globalInput;
			videoLivePipe[globalFromDevice]['output']=globalOutput;
			videoLivePipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
			setMessage('Video Live Pipe set',0);
		break;
	}
}

function setPipe(device,command,prefix,itemName)
{
	eval("xContainer=parseInt(document.getElementById('device_"+device+"').style.left)");
	eval("yContainer=parseInt(document.getElementById('device_"+device+"').style.top)");
	
	eval("xOffsetItem=parseInt(document.getElementById('"+prefix+device+"_cmd_"+command+"').style.left)");
	eval("yOffsetItem=parseInt(document.getElementById('"+prefix+device+"_cmd_"+command+"').style.top)");

	if(prefix=='in_'){
		globalXTo=xContainer+xOffsetItem;
		globalYTo=yContainer+yOffsetItem+10;
		if(globalToDevice!=-1){
			setMessage('Invalid pipe from input to input',1);
		}
		globalToDevice=device;		
		globalInput=command;
		document.getElementById('toMessage').innerHTML=itemName;
		
	}else{
		globalXFrom=xContainer+xOffsetItem+30;
		globalYFrom=yContainer+yOffsetItem+10;
		if(globalFromDevice!=-1){
			setMessage('Invalid pipe from output to output',1);
		}		
		globalFromDevice=device;
		globalOutput=command;
		document.getElementById('fromMessage').innerHTML=itemName;
	}

	if(globalYTo!=-1 && globalXFrom!=-1){
		if(globalFromDevice==globalToDevice){
			setMessage('You selected input and output for the same device',1);
		}else{
			existAlready=eval("("+globalPipe+"Pipe["+globalFromDevice+"]!=null)?1:0");
			if(existAlready){
				if(eval("confirm('The "+globalPipe+" pipe for device "+globalToDevice+" exist. Do you want to delete it to create a new one?')")){
					eval("clearPipe('"+globalPipe+"_'+"+globalFromDevice+")");
				}
			}
			setPipeVariables();
			drawPipe(globalFromDevice,globalPipe,globalXFrom,globalYFrom,globalXTo,globalYTo);
			
			globalXFrom=-1;
			globalYFrom=-1;
			globalXTo=-1;
			globalYTo=-1
			globalFromDevice=-1;
			globalToDevice=-1;
		}
	}
}

function redrawPipes(val)
{
	deviceID=val.substring(7);
	redrawConnectors('audio',audioPipe,deviceID);
	redrawConnectors('video',videoPipe,deviceID);
	redrawConnectors('audioLive',audioLivePipe,deviceID);
	redrawConnectors('videoLive',videoLivePipe,deviceID);
}

function redrawConnectors(connectorType,pipeArray,deviceID)
{
	try{
		for(dev in pipeArray){
			if(dev==deviceID){
				// there are audio pipes who start from this device
				oldCoords = pipeArray[dev]['coords'].split(','); 
				newX=parseInt(oldCoords[0])+parseInt(xMousePos)-parseInt(startX);
				newY=parseInt(oldCoords[1])+parseInt(yMousePos)-parseInt(startY);
				drawPipe(deviceID,connectorType,newX,newY,oldCoords[2],oldCoords[3]);
				pipeArray[deviceID]['coords']=newX+','+newY+','+oldCoords[2]+','+oldCoords[3];
			}
			if( pipeArray[dev]['to']==deviceID){
				// redraw the pipes who end in this device
				oldCoords = pipeArray[dev]['coords'].split(','); 
				newX=parseInt(oldCoords[2])+parseInt(xMousePos)-parseInt(startX);
				newY=parseInt(oldCoords[3])+parseInt(yMousePos)-parseInt(startY);
				drawPipe(dev,connectorType,oldCoords[0],oldCoords[1],newX,newY);
				pipeArray[dev]['coords']=oldCoords[0]+','+oldCoords[1]+','+newX+','+newY;
			}
		}
	}catch(e){
		// do nothing
	}
}

// category 0 is normal message, 1 mean error
function setMessage(message,category)
{
	if(category==1){
		document.getElementById('generalMessage').innerHTML='<span class="err">'+message+'</span>';
	}else{
		document.getElementById('generalMessage').innerHTML=message;
	}
}

// set which pipe will be drawn
function setPipeType()
{
	for(i=0;i<document.forms[0].pipeType.length;i++){
		if(document.forms[0].pipeType[i].checked){
			globalPipe=document.forms[0].pipeType[i].value;
			break;
		}
	}
}

// save all positions and pipes
function savePositions()
{
	toSave='';
	for(key in layersArray){
		eval("xCoord=document.getElementById('device_"+layersArray[key]+"').style.left");
		eval("yCoord=document.getElementById('device_"+layersArray[key]+"').style.top");
		audioCoords=(audioPipe[layersArray[key]])?audioPipe[layersArray[key]]['to']+':'+audioPipe[layersArray[key]]['coords']+':'+audioPipe[layersArray[key]]['input']+':'+audioPipe[layersArray[key]]['output']:'none';
		videoCoords=(videoPipe[layersArray[key]])?videoPipe[layersArray[key]]['to']+':'+videoPipe[layersArray[key]]['coords']+':'+videoPipe[layersArray[key]]['input']+':'+videoPipe[layersArray[key]]['output']:'none';
		audioLiveCoords=(audioLivePipe[layersArray[key]])?audioLivePipe[layersArray[key]]['to']+':'+audioLivePipe[layersArray[key]]['coords']+':'+audioLivePipe[layersArray[key]]['input']+':'+audioLivePipe[layersArray[key]]['output']:'none';
		videoLiveCoords=(videoLivePipe[layersArray[key]])?videoLivePipe[layersArray[key]]['to']+':'+videoLivePipe[layersArray[key]]['coords']+':'+videoLivePipe[layersArray[key]]['input']+':'+videoLivePipe[layersArray[key]]['output']:'none';
		toSave+=';'+layersArray[key]+';'+xCoord+';'+yCoord+';'+audioCoords+';'+videoCoords+';'+audioLiveCoords+';'+videoLiveCoords;
	}
	document.forms[0].devicesCoords.value=toSave;
	document.connectionWizard.action.value='setCookie';
	document.connectionWizard.submit();
}

// display info toolbar with checkboxes to be able to delete existing pipes
function showInfoToolbar(device)
{
	eval("document.getElementById('deleteToolbar').style.left="+(xMousePos-10));
	eval("document.getElementById('deleteToolbar').style.top="+(yMousePos-10));
	for(i=0;i<pipesArray.length;i++){
		hasPipe=eval("("+pipesArray[i]+"Pipe["+device+"]!=null)?1:0");	
		if(hasPipe==1){
			eval("toDevice="+pipesArray[i]+"Pipe["+device+"]['to']");
			eval("deviceInput="+pipesArray[i]+"Pipe["+device+"]['input']");
			eval("deviceOutput="+pipesArray[i]+"Pipe["+device+"]['output']");
			eval("document.getElementById('del_"+pipesArray[i]+"_text').innerHTML=\'from "+deviceOutput+" to "+deviceInput+" on "+toDevice+"\'");
			eval("document.getElementById('del_"+pipesArray[i]+"').checked=true;");
			eval("document.getElementById('del_"+pipesArray[i]+"').value="+device);
		}else{
			eval("document.getElementById('del_"+pipesArray[i]+"_text').innerHTML=\'not used\'");
			eval("document.getElementById('del_"+pipesArray[i]+"').checked=false;");
			eval("document.getElementById('del_"+pipesArray[i]+"').value=1");
		}
	}
	document.getElementById('deleteToolbar').style.display='';	
}

function removePipe(val)
{
	if(val.value!=1){
		pipeToDel=val.id.substr(4);
		clearPipe(pipeToDel+'_'+val.value);
		eval(pipeToDel+"Pipe["+val.value+"]=null");
	}
}

// functions used to check if an element has other elements on it
function mouseLeaves (element, evt) {
	if (typeof evt.toElement != 'undefined' && typeof element.contains != 'undefined') {
		return !element.contains(evt.toElement);
	}
	else if (typeof evt.relatedTarget != 'undefined' && evt.relatedTarget) {
		return !contains(element, evt.relatedTarget);
	}
}

function contains (container, containee) {
  while (containee) {
    if (container == containee) {
      return true;
    }
    containee = containee.parentNode;
  }
  return false;
}