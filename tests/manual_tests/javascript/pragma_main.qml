import QtQuick 2.0
import "pragmalib.js" as PragmaLib
import "lib.js" as Lib

Item {

    Component.onCompleted: {
        for (var i= 0; i< 10; i++) {
            print("Pragma Lib " + PragmaLib.counter++)
        }

        for (var i= 0; i< 10; i++) {
            print("Lib " + Lib.counter++)
        }
    }
}
