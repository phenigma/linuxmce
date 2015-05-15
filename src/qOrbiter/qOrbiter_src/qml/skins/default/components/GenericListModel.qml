import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
import org.linuxmce.grids 1.0
Item{
    id:genericListContainer
    width: Style.listViewWidth_medium

    property alias listView:view
    property alias model:view.model
    property alias label:itemlabel.text
    property alias delegate:view.delegate
    property int modelSpacing: 1
    property int modelCount:view.count
    property int dataGrid:-1
    property string dataGridLabel:""
    property string dataGridOptions:""
    property bool extended:true

    onEnabledChanged: if(!enabled)state="retracted"

    function toggleView(){
        extended=!extended
    }

    function refresh(){
        if(dataGrid==-1 || dataGridLabel===""){
            console.log("Invalid model id, cant reset")
            return;
        }

        manager.clearDataGrid(dataGridLabel);
        view.model=manager.getDataGridModel(dataGridLabel, dataGrid, dataGridOptions)
    }

    Component.onCompleted: {

        if(dataGrid==-1 || dataGridLabel==="")
            return;

        console.log("getting model "+dataGrid)
        view.model=manager.getDataGridModel(dataGridLabel, dataGrid, dataGridOptions)
    }
    onVisibleChanged: if(!visible) manager.clearDataGrid(dataGrid)

    Rectangle{
        id:hdr
        anchors{
            left: parent.left
            right:parent.right
            top:parent.top
        }
        height: Style.appNavigation_panelHeight
        color:enabled ? Style.appcolor_background_medium :" grey"
        opacity: Style.appList_opacity

        Behavior on color {
            ColorAnimation {
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }
        }

        Behavior on opacity {
            PropertyAnimation {
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }
        }

    }
    StyledText{
        id:itemlabel
        anchors.centerIn: hdr
        text:genericListContainer.label
        fontSize: Style.appFontSize_title
        color:"white"
    }

    Image {
        id: toggleSw
        source: "../images/vertical_arrow.png"
        rotation: genericListContainer.state==="extended" ? 0 : -90
        height: hdr.height*.75
        width: height
        fillMode: Image.PreserveAspectFit
        anchors{
            right:hdr.right
            verticalCenter: hdr.verticalCenter
        }

        MouseArea{
            anchors.fill: parent
            onClicked: toggleView()
        }
    }

    ListView{
        id:view
        clip:true

        visible: genericListContainer.visible
        opacity: genericListContainer.opacity
        spacing: modelSpacing
        anchors{
            left:parent.left
            top:hdr.bottom

            right:parent.right
        }
    }

    states: [
        State {
            name: "extended"
            when:genericListContainer.extended
            AnchorChanges{
                target:view
                anchors.bottom: parent.bottom
            }
        },
        State {
            name: "retracted"
            when:!genericListContainer.extended
            AnchorChanges{
                target:view
                anchors.bottom: hdr.bottom
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }

            PropertyAnimation{
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }
        }
    ]


}
