import QtQuick 2.2
import "../."
Item {
    id:control_popup
    anchors.fill: parent
    visible: activeIndex!==-1
    property alias controlTitle:label.text
    property variant controlData
    property int activeIndex:-1
    property variant currentControlItem:sendCommandBox.model.get(activeIndex)

    function setCommandDelegate(comp, index){
        activeIndex = index;
        if(activeIndex==-1){
            visible=false;
            controlTarget.source=undefined
            return;
        }

        visible=true
        controlTarget.sourceComponent=comp
    }

    function setParam(val){
        sendCommandBox.commandCache.set(model.index, {"value":val})
    }



    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: Style.appPanel_opacity
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            controlTarget.source=""
            control_popup.visible=false
        }
    }
    Rectangle{
        id:header
        height: Style.appNavigation_panelHeight
        width: parent.width
        color:Style.appcolor_navigationBackgroundColor
    }
    StyledText{
        id:label
        anchors.centerIn: header
        text:currentControlItem.command_name
    }

    Loader{
        id:controlTarget
        property variant cmdName : currentControlItem.command_name
        property variant cmdNumber :currentControlItem.command_number
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:header.bottom
        }
    }
}

