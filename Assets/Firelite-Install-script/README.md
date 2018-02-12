## Build the installer

* Build the application under Release configuration.
* Run the **windeployqt** tool and resolve the dependencies.
* Copy all the files and dependencies to the `packages\{package name}\data` directory.
* Navigate back to the root installer directory.
* Run the following command in order to create the installer.

> binarycreator --offline-only -c config/config.xml -p packages Firelite

