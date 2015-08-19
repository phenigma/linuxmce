import QtQuick 2.0
import "../components"
import "../."
import org.linuxmce.grids 1.0

Item {
    id: devicePopup
    anchors.margins: 30

    Rectangle {
        anchors.fill: parent
        color: Style.appcolor_background_list
        opacity: .65
    }

    property bool showCallToDevices: true
    property int selectedDevice: 0
    signal selected()

    StyledButton {
        id: closeBt
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        label: 'Close'
        onActivated: telecomDevices.visible = false
    }

    GridView {
        id: deviceGrid
        anchors.top: closeBt.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: specialDevice.top
        cellWidth: Style.appButtonWidth
        model: manager.getDataGridModel("phoneDevices", DataGrids.Devices_Of_Category, '89,90,91,92')
        delegate: StyledButton {
            label: description
            onActivated: {
                devicePopup.selectedDevice = value
                selected()
            }
        }
    }

    StyledButton {
        // Button for special device: either "broadcast to all MDs" or "Call from here" depending on the showCallToDevices property
        id: specialDevice
        anchors.topMargin: 10
        anchors.bottom: devicePopup.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        label: devicePopup.showCallToDevices ? qsTr('Broadcast to media directors') : qsTr('Call from this device')
        onActivated: {
            devicePopup.selectedDevice = -1
            selected()
        }
    }
}
