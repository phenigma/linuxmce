import QtQuick 2.2

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:manager.isProfile ? scaleY(3.5) :scaleY(2.5)
    property double largeFontSize: manager.isProfile ? scaleY(3) : scaleY(5)
     property double cellFontSize: manager.isProfile ? scaleY(2.8) : scaleY(3.2)
    horizontalAlignment: Text.AlignHCenter
    id:labelelement
    text: textLabel
    font.pixelSize: fontSize
    font.bold: isBold
    font.weight: isBold ? Font.Bold : Font.Light
    font.family: appFont.name
    color:skinstyle.tile_color
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
