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

var startX=0;
var startY=0;

globalXFrom=-1;
globalYFrom=-1;
globalFromDevice=-1;
globalToDevice=-1;
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
		if(audioPipe[dev]['to']==devID){
			eval("audio_"+dev+".clear()");
		}
	}
	eval("video_"+devID+".clear()");
	for(dev in videoPipe){
		if(videoPipe[dev]['to']==devID){
			eval("video_"+dev+".clear()");
		}
	}
	eval("audioLive_"+devID+".clear()");
	for(dev in audioLivePipe){
		if(audioLivePipe[dev]['to']==devID){
			eval("audioLive_"+dev+".clear()");
		}
	}
	eval("videoLive_"+devID+".clear()");
	for(dev in videoLivePipe){
		if(videoLivePipe[dev]['to']==devID){
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

function bring_to_front(val)
{
	last_z_index++;
	eval("document.getElementById('"+val+"').style.zIndex=last_z_index;");
	for(i=0;i<layersArray.length;i++){
		eval("document.getElementById('head_"+layersArray[i]+"').style.background='red';");
	}
}

function setPipe(device,command,prefix,itemName)
{
	// a lot of things goes here
	// for now I only get coordinates of the object's anchor
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
		document.getElementById('toMessage').innerHTML=itemName;
		
	}else{
		globalXFrom=xContainer+xOffsetItem+30;
		globalYFrom=yContainer+yOffsetItem+10;
		if(globalFromDevice!=-1){
			setMessage('Invalid pipe from output to output',1);
		}		
		globalFromDevice=device;
		document.getElementById('fromMessage').innerHTML=itemName;
	}

	if(globalYTo!=-1 && globalXFrom!=-1){
		if(globalFromDevice==globalToDevice){
			setMessage('You selected input and output for the same device',1);
		}else{
			drawPipe(globalFromDevice,globalPipe,globalXFrom,globalYFrom,globalXTo,globalYTo);
			switch(globalPipe){
				case 'audio':
					audioPipe[globalFromDevice]=new Array();
					audioPipe[globalFromDevice]['to']=globalToDevice;
					audioPipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
					setMessage('Audio Pipe set',0);
				break;
				case 'video':
					videoPipe[globalFromDevice]=new Array();
					videoPipe[globalFromDevice]['to']=globalToDevice;
					videoPipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
					setMessage('Video Pipe set',0);
				break;
				case 'audioLive':
					audioLivePipe[globalFromDevice]=new Array();
					audioLivePipe[globalFromDevice]['to']=globalToDevice;
					audioLivePipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
					setMessage('Audio Live Pipe set',0);
				break;
				case 'videoLive':
					videoLivePipe[globalFromDevice]=new Array();
					videoLivePipe[globalFromDevice]['to']=globalToDevice;
					videoLivePipe[globalFromDevice]['coords']=globalXFrom+','+globalYFrom+','+globalXTo+','+globalYTo;
					setMessage('Video Live Pipe set',0);
				break;
			}
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