import QtQuick 1.1
Item{
    id:controlPanel
    height: parent.height
    width: parent.width
    clip:true

    anchors.left: parent.right
    anchors.top: parent.top
    state:"unloaded"


    Rectangle{
        id:fil
        anchors.fill: parent
        color: appstyle.button_system_color
        opacity:.65
    }

    Loader{
        id:slideOut
        anchors{
            left:optionContainer.right
            right:optionContainer.right
            top:parent.top
            bottom:parent.bottom
        }
        source: ""

        onStatusChanged:{
            if(slideOut.source ===""){
                slideOut.anchors.right = undefined
                slideOut.anchors.right = optionContainer.right
            } else if (slideOut.source !=="" && slideOut.status===Component.Ready){
                slideOut.anchors.right =undefined
                slideOut.anchors.right = parent.right
            }
        }

        Behavior on anchors{
            AnchorAnimation{
                duration: 500
                easing.type: Easing.InCubic
            }
        }
    }

    Rectangle{
        id:optionContainer
        gradient:appstyle.buttonGradient
        width: scaleX(15)

        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            leftMargin: 10
        }

        ListView{
            id:optionView
            anchors.fill: parent
            model:availbleOptions
            delegate: StyledButton{
                buttonText: name
                textSize: 14
                anchors{
                    left:parent.left
                    right:parent.right
                }
                height:scaleY(20)
                onActivated: slideOut.source = control
            }
        }
    }


    ListModel{
        id:availbleOptions

        ListElement{
            name:"Zoom & Aspect"
            control:"Zoom.qml"
        }
        ListElement{
            name:"Bookmarks"
            control:"Bookmarks.qml"
        }
        ListElement{
            name:"Resend\nCommand"
            control:"Resend.qml"
        }

        ListElement{
            name:"Manage\nPlaylist"
            control:"PlaylistEditor.qml"
        }

        ListElement{
            name:"Thumbnail"
            control:"Thumbnail.qml"
        }
        ListElement{
            name:"Jog"
            control:"Jog.qml"
        }
    }

    states: [
        State {
            name: "unloaded"
            PropertyChanges {
                target: fil
                opacity:0

            }
        },
        State {
            name: "loaded"
            PropertyChanges {
                target: fil
                opacity:.65
            }
        }
    ]
}
