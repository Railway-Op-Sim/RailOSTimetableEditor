# Compiling and deploying Qt applications for Windows

- Download and install the open-source version of Qt for Windows from https://www.qt.io/download-qt-installer.
It takes a LONG time, even on a fast computer, so be patient or do something else while you are waiting.

- Create a desktop shortcut for QtCreator.exe - the file is located here:
<br>`<user chosen Qt directory>\Qt5.14.2\Tools\QtCreator\bin\qtcreator.exe` 
<br>Qt5.14.2 was the directory of the current version when this note was written but may have been updated for later downloads - select the up-to-date version.

- Download the required Qt project from GitHub.

- Fire up QtCreator, click 'Open' and navigate to and select the Qt project file ending in `.pro`.  
If this is successful it will show 'Configure Project' with a number of boxes ticked.  

- Untick all except 'Desktop Qt 5.14.2 MinGW 32 Bit' (or later version than 5.14.2 if one was downloaded).  
Click 'Configure project'.  
If successful the project's files will be shown in the left hand area of the screen.

- From the 'Build' menu at the top select 'Build for run configuration "project name" '.  This normally takes several seconds so wait until it finishes (optionally you can specify if this is either a `debug` build or `release` build).

- After this there should be a new directory named something like:<br>
 `build-ROSTTBGen-Desktop_Qt_5_14_2_MinGW_32_bit-Debug`<br>where 'ROSTTBGen' is the project's name and may be different.  It will be located at the same level as the directory that contained the project file selected above, note in the case of a release build the suffix will be `Release`.

- Inside this directory there is a directory named `debug`/`release`, and inside that an executable file (extension `.exe`) for Windows.

- Copy the executable into a new directory ready for release.

- Note this executable will not work without the required `dll` files and plugins. Firstly you should copy all `dll` files from <br>`<user chosen Qt directory>\Qt<version>\<version>\mingw73_32\bin` into the same folder.<br> Then copy all directories within the <br>`<user chosen Qt directory>\Qt<version>\<version>\mingw<ming_version>\plugins`<br> folder to the same location as the executable.

- Rename your Qt installation folder to `QtInstall` temporarily and try running the executable. If it runs successfully try deleting the `dlls` within the executable folder. Those which can be deleted are not required in the final release.

