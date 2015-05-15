// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import QtGraphicalEffects 1.0
import "."
import org.linuxmce.screeninfo 1.0
Item {
    anchors.fill: parent
    id:splashPage

    signal setupStart(string x, string y)
    signal splashLoaded()

    Component{
        id:options
        OptionMenu{

        }
    }

    Connections{
        target: window
        onShowList:{

            if(existing_orbiters.count === 0){
                screenChange("NewOrbiterSetup.qml")
            }
            else{
                existing_orbiters.state="showing"
                newOrbiterButton.state="showing"
                connectionBox.state="hidden"
            }
        }
        onPageChanged:screenchange(qmlPage)
    }

    Item{
        id:topContainer
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        height:parent.height *.08

        Rectangle{
            id:welcome_container
            color:theme.appcolor_background
            anchors.fill: parent
            opacity: .15
        }


        Text {
            id: ui_name
            text: screenInfo.primaryScreen.stringDeviceSize+"UI"
            font.pixelSize: theme.appFontSize_description
            font.weight: Font.Light
            anchors.right: optionIcon.left
            anchors.verticalCenter: optionIcon.verticalCenter
            color:"white"
        }
        Image{
            id:optionIcon
            source:"images/options.png"
            visible: true
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked:{options.createObject(splashPage) }
            }
        }

    }

    //    StatusRow {
    //        id: statusRow
    //    }

    Text {
        id: welcome
        text: qsTr(" Welcome to LinuxMCE")
        font.weight: Font.Light
        font.pixelSize: theme.appFontSize_header
        anchors{
           top:topContainer.top
            horizontalCenter: parent.horizontalCenter
        }
        color:"white"
    }
    Text {
        id: welcome_select
        text: qsTr("Please Choose an orbiter")
        font.pixelSize: theme.appFontSize_description
        font.weight: Font.Light
        anchors{
            bottom:topContainer.bottom
            horizontalCenter: welcome.horizontalCenter
        }
        color:"white"
        visible:orbiterList.count() !==0
    }



    Rectangle {
        width: parent.width
        height: scaleY(20)
        opacity: 1
        color: "transparent"

    }
    ConnectionBox {
        id: connectionBox
    }



    ExistingOrbiters {
        id:existing_orbiters
        anchors.top: welcome_select.bottom
        anchors.bottom: newOrbiterButton.top
    }

    NewOrbiterButton {
        id:newOrbiterButton
    }

}




