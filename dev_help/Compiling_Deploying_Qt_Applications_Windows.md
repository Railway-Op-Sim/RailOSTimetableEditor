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

- From the 'Build' menu at the top select 'Build for run configuration "project name" '.  This normally takes several seconds so wait until it finishes.

- After this there should be a new directory named something like:<br>
 `build-ROSTTBGen-Desktop_Qt_5_14_2_MinGW_32_bit-Debug`<br>where 'ROSTTBGen' is the project's name and may be different.  It will be located at the same level as the directory that contained the project file selected above.

- Inside this directory there is a directory named `debug`, and inside that an executable file (extension `.exe`) for Windows, but it can't be run yet because it needs a number of `.dll` files.

- Use the following dlls (these are for Krizar's graphical timetable editor) from<br> 
`<user chosen Qt directory>\Qt5.14.2\5.14.2\mingw73_32\bin`.<br>There are others that have the same names in different directories but these are the ones required:<br>
  - `libgcc_s_dw2-1.dll`
  - `libstdc++-6.dll`
  - `libwinpthread-1.dll`
  - `Qt5Core.dll`
  - `Qt5Gui.dll`
  - `Qt5Widgets.dll`<br>

   and put them all in the same directory as the executable.

- Also needed is the `platforms` directory from<br> 
`<user chosen Qt directory>\Qt5.14.2\5.14.2\mingw73_32\plugins\`.
This directory must be copied and added to the directory containing the executable, keep the same name - 'platforms' - or the executable won't find it.

- Now try to run the executable, and if it doesn't run make a note of any `.dll` files that are mentioned in the warning boxes (only one is mentioned in each box if more than one are needed), locate them from<br>
`<user chosen Qt directory>\Qt5.14.2\5.14.2\mingw73_32\bin`<br>and copy them into the directory that contains the executable.

- Now it should run.



