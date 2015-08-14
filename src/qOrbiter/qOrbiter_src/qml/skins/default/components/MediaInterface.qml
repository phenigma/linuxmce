import QtQuick 2.2
import QtMultimedia 5.0
import AudioVisual 1.0
import "../."
Item{
    id:mediaPlayerRoot
    visible:active

    property bool dceConnected:manager.connectedState
    property bool mediaPlayerConnected:lmceData.connected
    property int mediaPlayerId:manager.mediaPlayerID
    property bool active:lmceData.mediaPlaying

    function prepareMedia(mediaUrl){
        processLabel.text="Please Wait, pre-processing media for mobile device."
        var url = mediaUrl+"&pre=1";
        console.log("Prepare url-->"+url );
        var request = new XMLHttpRequest();
        request.onreadystatechange=function(){
            if(request.readyState==XMLHttpRequest.DONE){
                console.log("prepare finished, updating data.");
                // processLabel.text=request.responseText;
                qmlPlayer.source = lmceData.pluginUrl;
            }

        }
        request.open("GET", url);
        request.send();
    }



    Rectangle{
        anchors.fill: parent
        color:"black"
        visible:manager.i_current_mediaType==5
        StyledText{
            id:processLabel
            anchors.centerIn: parent
            color:"white"
        }
    }

    function restart(){
        lmceData.reInit();
    }

    MediaManager{
        id:lmceData
        onMediaPlayingChanged: {
            if(!mediaPlaying && qmlPlayer.source!==""){
                qmlPlayer.stop()
                videoPlane.visible = false
            } else {
                videoPlane.visible=true
            }

        }

        /* TODO Add reconnect handler in cpp and here */
        /* TODO Change android url to streaming url, generic for other devices. */
        onPluginUrlUpdated:{
            console.log("New QML media Url ==>"+pluginUrl)
            if(pluginUrl.length > 4){
                console.log("URL ok!")
                prepareMedia(lmceData.pluginUrl)


            }
            else{
                qmlPlayer.source=""
                console.log("Url Malformed!")
                console.log("Url::"+pluginUrl)
            }
        }

        onMutedChanged: {
            if(qmlPlayer.muted)
                qmlPlayer.muted=false
            else
                qmlPlayer.muted=true
        }

        onPluginVolumeUp:{
            console.log("dceplayer::vol up")
            if(qmlPlayer.volume < 1.0){
                qmlPlayer.volume= qmlPlayer.volume+.10
            }


        }

        onPluginVolumeDown:{
            console.log("dceplayer::vol down")
            if(qmlPlayer.volume > 0.0){
                qmlPlayer.volume= qmlPlayer.volume-.10
            }
        }



        onPauseMedia:{
            console.log("dceplayer::pause")
            if(qmlPlayer.playbackState == MediaPlayer.PausedState)
                qmlPlayer.play()
            else
                qmlPlayer.pause()
        }

        onUpdatePluginSeek:{
            console.log("dceplayer::seek to"+pos)
            qmlPlayer.seek(pos*1000);
        }

        Component.onCompleted: {
            if(manager.mediaPlayerID!==-1){
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                console.log("onCompleted::initializing qml media player::"+manager.mediaPlayerID+"::"+manager.m_ipAddress)
            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                if(manager.mediaPlayerID!==-1){
                    console.log("onIdChanged::initializing qml media player::"+manager.mediaPlayerID)
                    lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                }
            }
            onConnectedStateChanged:{
                if(manager.connectedState ){
                    //lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                    mediaPlayerRoot.restart()
                }
            }
        }
    }

    MediaPlayer{
        id:qmlPlayer
        autoPlay: true
        autoLoad: true


        onPlaybackStateChanged: console.log(qmlPlayer.source)

        onErrorStringChanged: {

            if(qmlPlayer.error===MediaPlayer.NetworkError){
                console.log(errorString+"::NetworkError")
            } else if(qmlPlayer.error==MediaPlayer.ResourceError){
                console.log(errorString+"::ResourceError")
                lmceData.pluginNotifyEnd()
            }
        }

        onStopped: {

            if(qmlPlayer.errorString!==""){
                console.log("ended with errors");
                console.log(qmlPlayer.errorString)
                lmceData.pluginNotifyEnd(true)
            } else {
               // lmceData.pluginNotifyEnd(false)
            }
        }

        onMediaObjectChanged: {
            console.log(JSON.stringify(metaData, null, "\t"))
        }

        onDurationChanged: {
            if(!duration)
                return;

            console.log("Track duration "+duration)
            lmceData.setQmlTotalTime(Number(duration))
            //   lmceData.setTotalTime(duration)

            console.log("current mediaObeject MetaData")
            console.log(JSON.stringify(metaData, null, "\t"))
        }
        onErrorChanged: {
            console.log("QML Media player error::"+error)
            if(error===1){
              //  play()
            }
        }

        onPositionChanged: {
            if(!position)
                return;
            lmceData.processTimeCode(position);
        }
        onHasVideoChanged: {

            if(hasVideo){
                videoPlane.visible=true
            } else {
                videoPlane.visible=false
            }
        }
    }

    VideoOutput{
        id:videoPlane
        visible:qmlPlayer.hasVideo
        height: manager.appHeight
        width: manager.appWidth
        anchors.centerIn: parent
        source:qmlPlayer
    }

    /* Item{
        id:testData
        height: parent.height*.15
        property bool showing:false
        anchors{
            left:parent.left
            bottom:parent.bottom
            right:parent.right

        }
        Rectangle{
            anchors.fill: parent
            color:"red"
            opacity: .75
        }


        Flickable{
           anchors{
               left:parent.left
               bottom:parent.bottom
               right:parent.right
               top:parent.top
           }
            Row{
                anchors.fill: parent
                spacing: 5
                Column{
                    height: parent.height
                    width: parent.width*.20
                    StyledText{ text: qsTr("Album Artist:   %1").arg(qmlPlayer.metaData.albumArtist)}
                    StyledText{ text: qsTr("Album Title:    %1").arg(qmlPlayer.metaData.albumTitle) }
                    StyledText{ text: qsTr("Lead Performer: %1").arg(qmlPlayer.metaData.leadPerformer) }
                    StyledText{ text: qsTr("Conductor:      %1").arg(qmlPlayer.metaData.conductor) }
                    StyledText{ text: qsTr("Composer:       %1").arg(qmlPlayer.metaData.composer) }
                }

                Column{
                    height: parent.height
                    width: parent.width*.15
                    StyledText{ text: qsTr("Title:          %1").arg(qmlPlayer.metaData.title)}
                    StyledText{ text: qsTr("Sub-title:      %1").arg(qmlPlayer.metaData.subTitle) }
                    StyledText{ text: qsTr("Writer:         %1").arg(qmlPlayer.metaData.writer) }
                    StyledText{ text: qsTr("Description:    %1").arg(qmlPlayer.metaData.description) }
                    StyledText{ text: qsTr("Director:       %1").arg(qmlPlayer.metaData.director) }
                }

                Column{
                    height: parent.height
                    width: parent.width*.15
                    StyledText{ text: qsTr("Audio Codec:   %1").arg(qmlPlayer.metaData.audioCodec)}
                    StyledText{ text: qsTr("Audio Bitrate: %1").arg(qmlPlayer.metaData.audioBitRate) }
                    StyledText{ text: qsTr("Video Codec:   %1").arg(qmlPlayer.metaData.leadPerformer) }
                    StyledText{ text: qsTr("Video Bitrate: %1").arg(qmlPlayer.metaData.leadPerformer) }
                    StyledText{ text: qsTr("Framerate:     %1").arg(qmlPlayer.metaData.leadPerformer) }
                }

            }
        }



    }*/

    //    MouseArea{
    //        anchors.fill: parent
    //        onClicked: {testData.showing=!testData.showing; console.log("video clicked")}
    //    }
}
