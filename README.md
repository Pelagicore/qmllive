QmlLive
=======

A live coding environment for QML. Allows to reload your qml view when a file in your project changes. This can be done on the same device using the QmlLiveBench or on a remote device using the QmlLiveRuntime.

Sub Projects
============

- qmllivebench - the qml-live workbench. All inclusive
- qmllivert - a default qml-live runtime for easy use
- src - common source code shared by all projects

Desktop
-------

For desktop usage just compile livebench.

    $ qmake
    $ make
    $ ./bin/qmllivebench

The project files will automatically detect the qt version and enables
or disables some features based on that.

Usage
-----

We are using git-flow in this project. To get the very latest version, please use the develop branch.

    $ git checkout develop

To checkout the latest stable release you need to check out master.

    $ git checkout master

Running a remote session
------------------------

Configuring the target:

Make sure you have any required imports available. The qml files will be
provided by the remote client, so create a clean directory and start
qmlliveruntime from there, passing any required imports using -importpath.

Connecting with the client:

Start qmlliveremote on your host machine. Open the qml workspace and connect to
the target. Enable Publish changes and press Publish all to transfer the entire
workspace to the target. Click on the qml file you want to run, it should start
on the target. Edit using your favorite tools on the host and instantly get the
changes on the target when saving.

Contributions
-------------

* Dominik Holland - Thank you for shouldering most of the work and to ensure the IPC code is rock-solid. 
* Robert Griebl - Thank you for knowing so many details on Qt and helping with the internals.
* Juergen Bocklage-Ryannel - For providing the initial idea and believing into the product.

