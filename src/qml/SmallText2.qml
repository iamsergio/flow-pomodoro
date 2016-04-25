import QtQuick 2.0

Text {
    renderType: _controller.textRenderType
    font.pixelSize: _style.smallTextSize - 1
    color: _style.smallTextColor
    wrapMode: Text.WordWrap
}
