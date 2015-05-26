// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import org.linuxmce.floorplans 1.0
import "../."
Item {
    id:sprite_root
    width: itemW
    height: itemH

    Component.onCompleted: {
        console.log("assignment of initial state")
        var d =  floorplan_devices.getDeviceData(deviceNum);
        deviceNum=d.deviceNumber;
        deviceStatus=d.deviceStatus;
        deviceState=d.deviceState;
        deviceName=d.deviceName;
        deviceLevel=d.deviceLevel;
        alertStatus = d.alertStatus;
        selected=d.selected;
    }


    property color activeColor: "grey"
    property color inactivecolor: "green"
    property double devicelevel:0.0
    property string state: "unknown"
    property string deviceName:""// floorplan_devices.getText(Number(deviceNum))
    property string deviceNum:""
    property string deviceStatus:""
    property string deviceState:""
    property int deviceLevel:0
    property string alertStatus:""
    property string deviceType: ""
    property string imgUrl: ""
    property bool selected:false
    property int itemH:Style.scaleY(5)
    property int itemW: Style.scaleY(5)
    property double iconScale: 1.5

    Connections{
        target: floorplan_devices

        onDeviceChanged:{
          //  console.log("Item "+deviceNum+" handling deviceChanged( )"+device)
            if(device==Number(deviceNum)){
              console.log("many welps!")
                var d =  floorplan_devices.getDeviceData(deviceNum);

                deviceNum=d.deviceNumber;
                deviceStatus=d.deviceStatus;
                deviceState=d.deviceState;
                deviceName=d.deviceName;
                deviceLevel=d.deviceLevel;
                alertStatus = d.alertStatus;
                selected=d.selected;
            }
        }

        onChangePage:{
            sprite_root.destroy()
        }
        onSelectedDevicesChanged: {
            //   selected = floorplan_devices.getDeviceSelection(deviceNum)
        }
    }

    scale: iconScale
    function updateFpItem(){
        floorplan_devices.setDeviceSelection(deviceNum)
    }

    Image {
        id: fpDevice_image
        source:deviceType==="52" ? "" : "../images/floorplan/"+deviceType+".png"
        sourceSize:Qt.size(Style.scaleY(6), Style.scaleY(6))
        anchors.centerIn: parent
        anchors.fill: parent
        cache: false
    }

    function getColor(deviceNum) {
        var t = floorplan_devices.getText(deviceNum)
        t = t.substring(0, t.indexOf("|"))
        if (t === 'ON')
            return "#00FFFF"
        else if (t === 'OFF')
            return "#000000"
    }

    /*    ColorOverlay {
        anchors.fill: fpDevice_image
        source: bug
//        visible: getColor(deviceNum) != 0
        color: getColor(deviceNum)
    }
*/
    StyledText {
        id: deviceNumLabel
        anchors.horizontalCenter: fpDevice_image.horizontalCenter
        text:deviceName  +"\n"+deviceStatus + "::"+deviceLevel
        anchors.top: fpDevice_image.bottom
        font.pixelSize: Style.fontSize_small
        visible: true
        color:"black"
    }

    Rectangle{
        id:sprite
        height: parent.height - 1
        width: parent.width - 1
        color: "transparent"
        opacity: 0
        border.color: "red"
        border.width:selected ? 2 : 0
        Component.onCompleted: PropertyAnimation {target:sprite ; property:"opacity"; to: 1; duration:1500 }
        scale: iconScale
    }


    MouseArea{
        anchors.fill: sprite_root
        onClicked: {
            //  currentDevice.selected=!sprite_root.selected
            updateFpItem()
        }
    }
}
