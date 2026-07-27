import QtQuick 2.15

Rectangle {

    id: root

    signal retryClicked()

    width: 1280
    height: 720

    color: "#251914"


    // =========================
    // Background Gradient
    // =========================

    Rectangle {

        anchors.fill: parent

        gradient: Gradient {

            GradientStop {
                position: 0.0
                color: "#624631"
            }

            GradientStop {
                position: 0.45
                color: "#3D2A20"
            }

            GradientStop {
                position: 1.0
                color: "#211611"
            }
        }
    }



    // =========================
    // Floating Light 1
    // =========================

    Rectangle {

        id: lightOne

        width: 500
        height: 500

        radius: width / 2

        color: "#D6A16E"

        opacity: 0.08

        x: -180
        y: -150


        SequentialAnimation on x {

            loops: Animation.Infinite

            NumberAnimation {

                from: -180
                to: -110

                duration: 9000
            }


            NumberAnimation {

                from: -110
                to: -180

                duration: 9000
            }
        }



        SequentialAnimation on opacity {

            loops: Animation.Infinite

            NumberAnimation {

                from: 0.05
                to: 0.12

                duration: 4000
            }


            NumberAnimation {

                from: 0.12
                to: 0.05

                duration: 4000
            }
        }
    }



    // =========================
    // Floating Light 2
    // =========================

    Rectangle {

        id: lightTwo


        width: 360
        height: 360


        radius: width / 2


        color: "#FFE3B8"


        opacity: 0.06


        anchors.right: parent.right
        anchors.bottom: parent.bottom


        anchors.rightMargin: -120
        anchors.bottomMargin: -100



        SequentialAnimation on opacity {

            loops: Animation.Infinite


            NumberAnimation {

                from: 0.03
                to: 0.08

                duration: 5000
            }


            NumberAnimation {

                from: 0.08
                to: 0.03

                duration: 5000
            }
        }
    }



    // =========================
    // Small floating particles
    // =========================


    Rectangle {

        width: 6
        height: 6

        radius: 3


        color: "#F8E6D2"

        opacity: 0.15


        x: 180
        y: 180



        SequentialAnimation on y {

            loops: Animation.Infinite


            NumberAnimation {

                from: 180
                to: 150

                duration: 3000
            }


            NumberAnimation {

                from: 150
                to: 180

                duration: 3000
            }
        }
    }



    Rectangle {

        width: 5
        height: 5

        radius: 3


        color: "#F8E6D2"

        opacity: 0.12


        x: 1050
        y: 500



        SequentialAnimation on y {

            loops: Animation.Infinite


            NumberAnimation {

                from: 500
                to: 470

                duration: 3500
            }


            NumberAnimation {

                from: 470
                to: 500

                duration: 3500
            }
        }
    }

    // =========================
        // Glass Card
        // =========================

        Rectangle {

            id: glassCard


            width: 540
            height: 560


            anchors.centerIn: parent


            radius: 32


            color: "#28FFF6EE"


            border.width: 1

            border.color: "#55FFFFFF"



            opacity: 0

            scale: 0.92



            Behavior on opacity {

                NumberAnimation {

                    duration: 700

                    easing.type: Easing.OutQuad
                }
            }



            Behavior on scale {

                NumberAnimation {

                    duration: 900

                    easing.type: Easing.OutBack
                }
            }



            Component.onCompleted: {

                opacity = 1

                scale = 1
            }



            // =====================
            // Top glass shine
            // =====================

            Rectangle {

                anchors.top: parent.top

                anchors.left: parent.left

                anchors.right: parent.right


                height: 2


                radius: 2


                color: "#BFFFFFFF"


                opacity: 0.55
            }



            // =====================
            // Inner glass layer
            // =====================

            Rectangle {


                anchors.fill: parent


                anchors.margins: 12


                radius: 25


                color: "#10FFFFFF"


                border.width: 1


                border.color: "#25FFFFFF"
            }



            // =====================
            // Bottom warm light
            // =====================

            Rectangle {


                width: parent.width * 0.55

                height: 2


                radius: 2


                anchors.bottom: parent.bottom

                anchors.horizontalCenter: parent.horizontalCenter



                color: "#D3A06E"


                opacity: 0.45
            }


            // =====================
            // soft moving reflection
            // =====================

            Rectangle {


                id: reflection


                width: 120

                height: parent.height


                color: "#20FFFFFF"


                opacity: 0.08


                rotation: 18


                x: -180



                SequentialAnimation on x {


                    loops: Animation.Infinite



                    NumberAnimation {

                        from: -180

                        to: 600

                        duration: 7000
                    }



                    PauseAnimation {

                        duration: 3000
                    }
                }
            }


            // =========================
                    // Content
                    // =========================


                    Column {

                        id: contentColumn


                        anchors.centerIn: parent


                        width: parent.width - 80


                        spacing: 18



                        // -------------------------
                        // Icon container
                        // -------------------------

                        Rectangle {

                            id: iconCircle


                            width: 130
                            height: 130


                            radius: 65


                            anchors.horizontalCenter: parent.horizontalCenter


                            color: "#18FFFFFF"


                            border.width: 1


                            border.color: "#55FFFFFF"



                            // Pulse animation

                            SequentialAnimation on scale {

                                loops: Animation.Infinite


                                NumberAnimation {

                                    from: 1

                                    to: 1.06

                                    duration: 1600

                                    easing.type: Easing.InOutQuad
                                }


                                NumberAnimation {

                                    from: 1.06

                                    to: 1

                                    duration: 1600

                                    easing.type: Easing.InOutQuad
                                }
                            }



                            Image {

                                id: libraryIcon


                                anchors.centerIn: parent


                                width: 85

                                height: 85


                                source: "qrc:/resources/library_disconnected.svg"


                                fillMode: Image.PreserveAspectFit


                                smooth: true
                            }
                        }



                        // -------------------------
                        // Title
                        // -------------------------

                        Text {


                            width: parent.width


                            text: "ارتباط با کتابخانه قطع شده است"



                            horizontalAlignment: Text.AlignHCenter


                            wrapMode: Text.WordWrap



                            color: "#FFF8EF"



                            font.family: "Vazirmatn"



                            font.pixelSize: 26


                            font.bold: true
                        }



                        // -------------------------
                        // Description
                        // -------------------------

                        Text {


                            width: parent.width


                            text:
                            "در حال تلاش برای برقراری مجدد ارتباط با سرور هستیم...\nلطفاً چند لحظه صبر کنید."



                            horizontalAlignment: Text.AlignHCenter



                            wrapMode: Text.WordWrap



                            color: "#DCC7B3"



                            font.family: "Vazirmatn"



                            font.pixelSize: 16


                            lineHeight: 1.3
                        }



                        // -------------------------
                        // Custom loading dots
                        // -------------------------

                        Row {


                            anchors.horizontalCenter: parent.horizontalCenter


                            spacing: 10



                            Repeater {


                                model: 3



                                Rectangle {


                                    width: 12

                                    height: 12


                                    radius: 6


                                    color: "#D7A477"



                                    opacity: 0.35



                                    SequentialAnimation on opacity {


                                        loops: Animation.Infinite



                                        PauseAnimation {


                                            duration: index * 250
                                        }



                                        NumberAnimation {


                                            from: 0.35

                                            to: 1


                                            duration: 600
                                        }



                                        NumberAnimation {


                                            from: 1

                                            to: 0.35


                                            duration: 600
                                        }
                                    }
                                }
                            }
                        }



                        Text {


                            width: parent.width


                            text: "در حال اتصال..."



                            horizontalAlignment: Text.AlignHCenter



                            color: "#BFA38A"



                            font.family: "Vazirmatn"



                            font.pixelSize: 14
                        }
                        // =========================
                                    // Retry Button
                                    // =========================


                                    Rectangle {

                                        id: retryButton


                                        width: 230

                                        height: 55


                                        radius: 18



                                        anchors.horizontalCenter: parent.horizontalCenter



                                        color: "#8B5E3C"



                                        border.width: 1


                                        border.color: "#B98B63"



                                        scale: 1



                                        Behavior on scale {

                                            NumberAnimation {

                                                duration: 120

                                                easing.type: Easing.OutQuad
                                            }
                                        }



                                        Behavior on color {

                                            ColorAnimation {

                                                duration: 200
                                            }
                                        }



                                        Text {


                                            anchors.centerIn: parent



                                            text: "تلاش مجدد"



                                            color: "#FFF6ED"



                                            font.family: "Vazirmatn"



                                            font.pixelSize: 17



                                            font.bold: true
                                        }



                                        MouseArea {


                                            anchors.fill: parent



                                            hoverEnabled: true



                                            onEntered: {


                                                retryButton.color = "#A8744B"


                                                retryButton.scale = 1.05

                                            }



                                            onExited: {


                                                retryButton.color = "#8B5E3C"


                                                retryButton.scale = 1

                                            }



                                            onPressed: {


                                                retryButton.scale = 0.96

                                            }



                                            onReleased: {


                                                retryButton.scale = 1.05

                                            }



                                            onClicked: {

                                                root.retryClicked()

                                            }
                                        }
                                    }
                    }

        }

}
