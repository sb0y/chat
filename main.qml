import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import СontactListModel 1.0
import MessageListModel 1.0

ApplicationWindow {
    id: main_window
    visible: true
    width: 640
    height: 480
    title: qsTr("Stack")

    TextEdit {
        id: cliphelper
        visible: false
    }

    readonly property string rightbkcolor: "#f1f1f1"
    readonly property string panebkcolor: "#e0e0e0"
    readonly property string messagebkcolor: "#eeeeee"
    readonly property string selecteditembkcolor: "#4a90d9"
    readonly property string normalitembkcolor: "white"

    ColumnLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: splitter.left

        Rectangle {
            anchors.fill: parent
            color: rightbkcolor
        }

        ListView {
            id: messages
            Layout.fillWidth: true
            //Layout.fillHeight: true
            anchors.top: messagesToolbar.bottom
            anchors.bottom: prePane.top
            //Layout.margins: 5
            //displayMarginBeginning: 10
            //displayMarginEnd: 20
            //verticalLayoutDirection: ListView.BottomToTop
            spacing: 4
            model: message_model
            delegate: Row {
                readonly property bool sentByMe: from == contact_model.self_contact_id()
                readonly property bool sentByCurrent: from == message_model.current_contact

                anchors.right: sentByMe ? parent.right : undefined

                Rectangle {
                    width: 12
                    height: 25
                    color: rightbkcolor
                }

                Rectangle {
                    id: avatar
                    radius: 12
                    width: 24
                    height: 24
                    color: from_ab_background
                    visible: !(sentByMe || sentByCurrent)
                    Label {
                        anchors.centerIn: parent
                        text: from_ab
                        color: from_ab_color
                    }
                }

                Rectangle {
                    width: 2
                    height: 25
                    color: rightbkcolor
                }

                Rectangle {
                    property string text
                    readonly property int bodyTop: 12
                    readonly property int widthCorrect: sentByCurrent ? 25 : 0
                    radius: 5
                    height: bodyTop + (text_field.paintedHeight < 25 ? 25 : text_field.paintedHeight+4)
                    width: text_field.width < 120 ? 130 : text_field.width + 10 + (sentByMe && (sended || readed) ? 16 : 0)
                    color: sentByMe ? "white" : "#e3e3e3"
                    text: message
                    Label {
                        leftPadding: 3
                        y: 0
                        anchors.left:  parent.left
                        anchors.right: labelMsgTimeField.left
                        text: sentByMe ? "me" : from_name
                        color: "#909090"
                        font.bold: true
                        font.pointSize: 10
                        height: parent.bodyTop
                        maximumLineCount: 1
                        elide: Text.ElideRight
                    }

                    Label {
                        rightPadding: 3
                        id: labelMsgTimeField
                        y: 0
                        height: parent.bodyTop
                        anchors.right: parent.right
                        text: tm
                        font.pointSize: 10
                        color: "#909090"
                    }


                    TextEdit {
                        id: text_field
                        y: parent.bodyTop
                        anchors.left: parent.left
                        height: parent.height
                        width: dummy_text.paintedWidth > messages.width - 26 - (sentByMe && (sended || readed) ? 16 : 0)
                               ? messages.width - 26 - (sentByMe && (sended || readed) ? 16 : 0)
                               : dummy_text.paintedWidth < 30 ? 30 : dummy_text.paintedWidth
                        readOnly: true
                        text: parent.text
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        selectedTextColor: "white"
                        selectionColor: selecteditembkcolor
                        leftPadding: 4
                        rightPadding: 4
                        topPadding: 2
                        bottomPadding: 2

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            onClicked: {
                                contextMessageMenu.popup()
                            }
                        }

                        Menu {
                            id: contextMessageMenu
                            width: 120
                            padding: 4
                            MenuItem {
                                text: "Copy"
                                onClicked: { text_field.copy() }
                            }
                            background: Rectangle {
                                implicitWidth: 200
                                border.color: "#e3e3e3"
                            }
                        }
                    }

                    Text {
                        id: dummy_text
                        text: parent.text
                        leftPadding: 4
                        rightPadding: 4
                        topPadding: 2
                        bottomPadding: 2
                        visible: false
                    }
                    Rectangle {
                        id: sended_readed
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        height: 16
                        width: sentByMe && (sended || readed) ? 16 : 0
                        color: "transparent"
                        Image {
                            anchors.fill: parent
                            source: "images/checkg.png"
                            visible: sentByMe && sended && !readed
                        }
                        Image {
                            anchors.fill: parent
                            source: "images/check2g.png"
                            visible: sentByMe && readed
                        }
                    }
                }

                Rectangle {
                    width: 3
                    height: 25
                    color: rightbkcolor
                }
            }

            ScrollBar.vertical: ScrollBar {}

            onCountChanged: {
                currentIndex = count-1
            }

            Component.onCompleted: {
                main_window.title = contact_model.self_contact_id()
                if(message_model.rowCount() > 0)
                {
                    contacts.currentIndex = 0
                    message_model.current_contact = contact_model.data(
                            contact_model.index(contacts.currentIndex,0), contact_model.ID)
                }
            }
        }

        ToolBar {
            id: messagesToolbar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: messagesToolButton.implicitHeight

            ToolButton {
                id: messagesToolButton
                y: 1
                height: 38
                anchors.right: messagesToolButtonRightPadding.left
                text: "\u22EE"
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                background: Rectangle {
                    color: messagesToolButton.down ? "#c0c0c0" : "transparent"
                    radius: 19
                }
                onClicked: {
                    contextMessageToolbarMenu.popup()
                }
            }

            Menu {
                id: contextMessageToolbarMenu
                width: 120
                padding: 4
                MenuItem {
                    text: "Info"
                    onClicked: {
                        infoContactDialog.open()
                    }
                }
                background: Rectangle {
                    implicitWidth: 200
                    border.color: "#e3e3e3"
                }
            }

            Rectangle {
                id: messagesToolButtonRightPadding
                width: 3
                height: messagesToolbar.height
                anchors.right: parent.right
                color: "transparent"
            }

            TextArea {
                id: filterMessages
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: messagesToolButton.left
                placeholderText: qsTr("Find message")
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextfilterMessagesMenu.popup()
                    }
                }
                Menu {
                    id: contextfilterMessagesMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { filterMessages.copy() }
                    }
                    MenuItem {
                        text: "Paste"
                        onClicked: { filterMessages.paste() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
        }

        Rectangle {
            id: prePane
            Layout.fillWidth: true
            height: 3
            anchors.bottom: prePane2.top
            color: rightbkcolor
        }

        Rectangle {
            id: prePane2
            Layout.fillWidth: true
            height: 3
            anchors.bottom: pane.top
            color: "white"
        }

        Pane {
            id: pane
            Layout.fillWidth: true
            padding: 5
            anchors.bottom: parent.bottom
            background: Rectangle {
                color: rightbkcolor
            }

            RowLayout {
                width: parent.width

                TextArea {
                    id: messageField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Compose message")
                    wrapMode: TextArea.Wrap
                    selectByMouse: true
                    selectedTextColor: "white"
                    selectionColor: selecteditembkcolor
                    persistentSelection: true
                    inputMethodHints: Qt.ImhNoPredictiveText
                    enabled: contacts.currentIndex >= 0
                    property bool ctrlPressed: false
                    Keys.onPressed: {
                        if(event.key === Qt.Key_Control) ctrlPressed = true;
                        if(event.key === Qt.Key_Return) {
                            if(ctrlPressed)
                                messageField.append("")
                            else if(messageField.length > 2)
                                message_model.append(messageField.text.substring(0, messageField.text.length))
                            event.accepted = true;
                        }
                    }
                    Keys.onReleased: {
                        if(event.key === Qt.Key_Return && !ctrlPressed) messageField.text = ""
                        if(event.key === Qt.Key_Control) ctrlPressed = false;
                    }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: {
                            contextPaneMenu.popup()
                        }
                    }

                    Menu {
                        id: contextPaneMenu
                        width: 120
                        padding: 4
                        MenuItem {
                            text: "Copy"
                            onClicked: { messageField.copy() }
                        }
                        MenuItem {
                            text: "Paste"
                            onClicked: { messageField.paste() }
                        }
                         background: Rectangle {
                            implicitWidth: 200
                            border.color: "#e3e3e3"
                        }
                    }
                }

                /*ToolButton {
                    id: sendButton
                    text: "\u2658"
                    //font.pixelSize: Qt.application.font.pixelSize * 1.6
                    background: Rectangle {
                        color: "white"
                    }
                    onClicked: {
                        message_model.append(messageField.text)
                        messageField.text = ''
                    }
                }*/
            }
        }
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: splitter.left

        Rectangle {
            anchors.fill: contacts
            color: "white"
        }

        ListView {
            id: contacts
            Layout.fillWidth: true
            //Layout.fillHeight: true
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            Layout.margins: 0
            displayMarginBeginning: 0
            displayMarginEnd: 0
            verticalLayoutDirection: ListView.TopToBottom
            //spacing: 2
            model: contact_model
            currentIndex: -1
            delegate: Row {
                readonly property bool orderBy: index % 2 == 0
                readonly property bool selected: index == contacts.currentIndex

                Rectangle {
                    width: 2
                    height: 40
                    color: normalitembkcolor
                }

                Rectangle {
                    radius: 19
                    y:1
                    width: 38
                    height: 38
                    color: ab_background
                    Label {
                        anchors.centerIn: parent
                        text: ab
                        font.pointSize: 16
                        color: ab_color
                    }
                    Rectangle {
                        x: 29
                        y: 29
                        width: 8
                        height: 8
                        color: "#00c95f"
                        radius: 4
                        visible: joined
                        border.color: "white"
                        border.width: 1
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: contacts.currentIndex = index
                    }
                }

                Rectangle {
                    width: 2
                    height: 40
                    color: normalitembkcolor
                }

                Rectangle {
                    width: contacts.width - 44
                    height: 40
                    color: selected ? selecteditembkcolor : orderBy ? rightbkcolor : normalitembkcolor

                    Label {
                        leftPadding: 3
                        y: 0
                        anchors.left:  parent.left
                        anchors.right: labelTimeField.left
                        text: alias
                        font.bold: true
                        color: selected ? "white" : "black"
                        height: 20
                        maximumLineCount: 1
                        elide: Text.ElideRight
                    }

                    Label {
                        rightPadding: 3
                        id: labelTimeField
                        y: 0
                        anchors.right: parent.right
                        text: tm
                        font.pointSize: 10
                        color: selected ? "white" : "#707070"
                        height: 20
                        elide: Text.ElideLeft
                    }

                    Label {
                        leftPadding: 3
                        y: 24
                        anchors.left:  parent.left
                        anchors.right: unread_cnt > 0 ? newMessageCountField.left : parent.right
                        text: id
                        font.pointSize: 8
                        color: selected ? "white" : "#707070"
                        height: 16
                        maximumLineCount: 1
                        elide: Text.ElideRight
                    }

                    Label {
                        id: newMessageCountField
                        y: 18
                        anchors.right: rightPaddingMsgCount.left
                        padding: 3
                        text: unread_cnt
                        font.pointSize: 8
                        color: selected ? "#707070" : "white"
                        height: 20
                        width: 22
                        horizontalAlignment: Qt.AlignCenter
                        background: Rectangle {
                            radius: 10
                            color: selected ? "white" : "Gray" //selecteditembkcolor
                        }
                        visible: unread_cnt > 0
                    }

                    Rectangle {
                        id: rightPaddingMsgCount
                        y: 18
                        anchors.right: parent.right
                        width: 3
                        height: 20
                        color: selected ? selecteditembkcolor : orderBy ? rightbkcolor : normalitembkcolor
                        //radius: 3
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: {
                            contacts.currentIndex = index
                            message_model.current_contact = id
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: {
                            contacts.currentIndex = index
                            message_model.current_contact = id
                            contextContactMenu.popup()
                        }
                    }

                    Menu {
                        id: contextContactMenu
                        width: 120
                        padding: 4
                        MenuItem {
                            text: "Copy ID"
                            onClicked: {
                                cliphelper.text = id
                                cliphelper.selectAll()
                                cliphelper.copy()
                            }
                        }
                        MenuItem {
                            text: "Change name"
                            onClicked: {
                                changeNameContactDialog.open()
                            }
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: "Delete"
                            onClicked: {
                                deleteContactDialog.open()
                            }
                        }
                        background: Rectangle {
                            implicitWidth: 200
                            border.color: "#e3e3e3"
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        ToolBar {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: toolButton.implicitHeight

            ToolButton {
                id: toolButton
                y: 1
                x: 3
                height: 38
                text: "\u2630"
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                background: Rectangle {
                    color: toolButton.down ? "#c0c0c0" : "transparent"
                    radius: 19
                }
                onClicked: {
                    drawer.open()
                }
            }

            Rectangle {
                id: contactToolbarSeparator
                y: 0
                height: parent.height
                anchors.right: parent.right
                width: 2
                color: rightbkcolor
            }

            TextArea {
                id: filterContacts
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: toolButton.right
                anchors.right: contactToolbarSeparator.left
                placeholderText: qsTr("Filter contacts")
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextfilterContactsMenu.popup()
                    }
                }
                Menu {
                    id: contextfilterContactsMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { filterContacts.copy() }
                    }
                    MenuItem {
                        text: "Paste"
                        onClicked: { filterContacts.paste() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
        }
    }

    Rectangle {
        id: splitter
        x: x>0 ? x: parent.width/3
        y: 0
        height: parent.height
        width: 10
        color: "transparent"
        MouseArea {
            id: dragArea
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 20
            drag.maximumX: main_window.width - parent.width-20
        }
    }

    Drawer {
        id: drawer
        width: 150
        height: parent.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Add contact")
                width: parent.width
                onClicked: {
                    drawer.close()
                    addContactDialog.open()
                }
            }
        }
    }

    Dialog {
        id: addContactDialog

        x: (main_window.width - width) / 2
        y: (main_window.height - height) / 2

        ColumnLayout {
            anchors.fill: parent
            Label { text: "Name" }
            TextArea {
                id: nameField
                Layout.fillWidth: true
                Layout.minimumWidth: 300
                background: Rectangle { border.color: "#e3e3e3" }
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextnameFieldMenu.popup()
                    }
                }
                Menu {
                    id: contextnameFieldMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { nameField.copy() }
                    }
                    MenuItem {
                        text: "Paste"
                        onClicked: { nameField.paste() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
            Label { text: "ID" }
            TextArea {
                id: idField
                Layout.fillWidth: true
                Layout.minimumWidth: 300
                background: Rectangle { border.color: "#e3e3e3" }
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextidFieldMenu.popup()
                    }
                }
                Menu {
                    id: contextidFieldMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { idField.copy() }
                    }
                    MenuItem {
                        text: "Paste"
                        onClicked: { idField.paste() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
            RowLayout {
                Layout.topMargin: 5
                Layout.alignment: Qt.AlignRight
                Button {
                    text: "Cancel"
                    onClicked: {
                        addContactDialog.close()
                    }
                }
                Button {
                    text: "OK"
                    onClicked: {
                        addContactDialog.close()
                        contact_model.add(idField.text, nameField.text)
                    }
                }
            }
        }
        background: Rectangle { border.color: "#e3e3e3" }
    }

    Dialog {
        id: changeNameContactDialog

        x: (main_window.width - width) / 2
        y: (main_window.height - height) / 2

        ColumnLayout {
            anchors.fill: parent
            Label { text: "Name" }
            TextArea {
                id: nameField2
                Layout.fillWidth: true
                text: contact_model.data(contact_model.index(contacts.currentIndex,0), contact_model.ALIAS)
                background: Rectangle { border.color: "#e3e3e3" }
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextnameField2Menu.popup()
                    }
                }
                Menu {
                    id: contextnameField2Menu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { nameField2.copy() }
                    }
                    MenuItem {
                        text: "Paste"
                        onClicked: { nameField2.paste() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
            RowLayout {
                Layout.topMargin: 5
                Layout.alignment: Qt.AlignRight
                Button {
                    text: "Cancel"
                    onClicked: {
                        changeNameContactDialog.close()
                    }
                }
                Button {
                    text: "OK"
                    onClicked: {
                        changeNameContactDialog.close()
                        contact_model.changeAlias(contacts.currentIndex, nameField2.text)
                    }
                }
            }
        }
        background: Rectangle { border.color: "#e3e3e3" }
    }

    Dialog {
        id: deleteContactDialog

        x: (main_window.width - width) / 2
        y: (main_window.height - height) / 2

        ColumnLayout {
            Label {
                Layout.alignment: Qt.AlignCenter
                text: "You want to delete this?"
            }
            Label {
                text: contact_model.data(contact_model.index(contacts.currentIndex,0), contact_model.ALIAS)
                elide: Text.ElideRight
            }
            RowLayout {
                Layout.topMargin: 5
                Layout.alignment: Qt.AlignCenter
                Button {
                    text: "Cancel"
                    onClicked: {
                        deleteContactDialog.close()
                    }
                }
                Button {
                    text: "OK"
                    onClicked: {
                        deleteContactDialog.close()
                        contact_model.del(contact_model.data(contact_model.index(contacts.currentIndex,0), contact_model.ID))
                    }
                }
            }
        }
        background: Rectangle { border.color: "#e3e3e3" }
    }

    Dialog {
        id: infoContactDialog

        x: (main_window.width - width) / 2
        y: (main_window.height - height) / 2

        ColumnLayout {
            anchors.fill: parent
            Label { text: "Name" }
            TextArea {
                id: name3Field
                Layout.fillWidth: true
                text: contact_model.data(contact_model.index(contacts.currentIndex,0), contact_model.ALIAS)
                readOnly: true
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextname3FieldMenu.popup()
                    }
                }
                Menu {
                    id: contextname3FieldMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { name3Field.copy() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
            Label { text: "ID" }
            TextArea {
                id: id3Field
                Layout.fillWidth: true
                text: contact_model.data(contact_model.index(contacts.currentIndex,0), contact_model.ID)
                readOnly: true
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: selecteditembkcolor
                persistentSelection: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextid3FieldMenu.popup()
                    }
                }
                Menu {
                    id: contextid3FieldMenu
                    width: 120
                    padding: 4
                    MenuItem {
                        text: "Copy"
                        onClicked: { id3Field.copy() }
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        border.color: "#e3e3e3"
                    }
                }
            }
            RowLayout {
                Layout.topMargin: 5
                Layout.alignment: Qt.AlignCenter
                Button {
                    text: "OK"
                    onClicked: {
                        infoContactDialog.close()
                    }
                }
            }
        }
        background: Rectangle { border.color: "#e3e3e3" }
    }
}
