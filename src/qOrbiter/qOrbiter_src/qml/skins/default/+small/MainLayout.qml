import QtQuick 2.2
import QtGraphicalEffects 1.0
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot

    Connections{
        target: qmlRoot
        onScreenSaverActivated:{
            uiOn=false
            pageLoader.toggleContent(false)
        }
        onResetTimeout:{
            pageLoader.toggleContent(true)
        }
    }
    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    property bool uiOn:true
    function raiseNavigation(raise){
        uiOn=raise;
    }

    QmlPictureFrame {
        id: qmlPictureFrame
        MouseArea{
            anchors.fill: parent
            onClicked: uiOn=!uiOn
        }
    }

    MediaInterface{
        id:mediaPlayer
        anchors.centerIn: parent
        height:parent.height
        width:parent.width
    }


    PageLoader {
        id: pageLoader

        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:footer.top
        }
    }

    DefaultHeader {
        id: header

        Component{
            id:roomSelector
            RoomSelector{

            }
        }

        FocusRow{
            anchors.fill: parent
            StyledButton{
                buttonText: qsTr("Home")
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                onActivated: manager.currentScreen="Screen_1.qml"
            }

        }

    }

    /*Item{
        id:mediaNotification
        anchors{
            top:header.bottom
            left: parent.left
            right:parent.right
        }
        Rectangle{
            anchors.fill: parent
            color:Style.appbutton_confirm_color
            opacity: Style.appPanel_opacity
        }

        MouseArea{
            anchors.fill: parent
            onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
        }

        height: dcenowplaying.b_mediaPlaying && manager.currentScreen=="Screen_1.qml" ? Style.appMenuPanel_height : 0

        StyledText{
            id:generaltitle
            text: dcenowplaying.qs_mainTitle
            height:parent.height
            width:parent.width /2
            anchors.left: parent.left
            opacity: mediaNotification.height == Style.appMenuPanel_height ? 1 : 0
            Behavior on opacity {
                PropertyAnimation{
                    duration: 350
                }
            }
        }
        StyledText {
            id: updating_time
            text:dcenowplaying.b_mediaPlaying ? dceTimecode.qsCurrentTime + " of " + dceTimecode.qsTotalTime :""
            fontSize:Style.appFontSize_header
            color: "white"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible:manager.currentScreen=="Screen_1.qml"
        }
    }*/

    Footer {
        id: footer

        ScrollRow{
            anchors.fill: parent
            contentHeight:row.height
            contentWidth:row.width
            Row{
                id:row
                width: children.length*Style.appButtonWidth
                height:footer.height
                StyledButton{
                    buttonText: roomList.currentRoom
                    anchors{
                        top:parent.top
                        bottom:parent.bottom
                        margins: 5
                    }
                    onActivated: qmlRoot.createPopup(roomSelector)
                }

                StyledButton{
                    buttonText: qsTr("Sleeping Menu")
                    anchors{
                        top:parent.top
                        bottom:parent.bottom
                        margins: 5
                    }
                    onActivated:{
                        manager.currentScreen="Screen_29.qml"
                    }
                }
                StyledButton{
                    anchors{
                        top:parent.top
                        bottom:parent.bottom
                        margins: 5
                    }
                    buttonText: qsTr("Advanced Menu")
                    onActivated: manager.currentScreen = "Screen_44.qml"
                }
                StyledButton{
                    anchors{
                        top:parent.top
                        bottom:parent.bottom
                        margins: 5
                    }
                    buttonText: qsTr("Power")
                }
            }


        }
    }

}
