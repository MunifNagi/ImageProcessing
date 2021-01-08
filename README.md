# ImageProcessing
## Professor George Wolberg 

**This is a Qt project, in order to run this you have to install Qt**
Compiling CS470.skel under Visual Studio 2017
1) Download Visual studio community 2017 and select "Desktop development with C++" and install

2) Download Qt5.11.2 for Windows from here:

https://download.qt.io/archive/qt/5.11/5.11.2/qt-opensource-windows-x86-5.11.2.exe

3) During Qt installation, Select Qt5.11.2 for msvc2017_64.

It will install it in C:\Qt\Qt5.11.2\5.11.2\msvc2017_64

4) Add C:\Qt\Qt5.11.2\5.11.2\msvc2017_64\bin to your path and make sure in shell command line: "qmake -version" point to Qt5.11.2.

5) Replace CS470.pro with the attached CS470.pro.

6) Delete debug, release, CS470.pro.user, .qmake.stash and all previous CS470.skel.vcxproj.*

7) open a powershell or terminal in that folder and type:
qmake -tp vc CS470.pro

8) Double click on CS470.skel.vcxproj. It will load the project in VS 2017.

9) You should be able to compile with any error.

Compiling under Mac OS (unreponsive GUI bug fixed)
1) Download Qt5.11.2 from here:
https://download.qt.io/archive/qt/5.11/5.11.2/qt-opensource-mac-x64-5.11.2.dmg

2) Install it (you don't need to check source codes or other modules).

3) By default it installs in /User/"your user name"

4) add the following line to .bash_profile:

export PATH=~/Qt5.11.2/5.11.2/clang_64/bin:$PATH

5) Replace MainWindow.cpp by MainWindow.cpp in CS470.skel folder.

6) Open a terminal in CS470.skel folder.

7) Delete debug, Release, .qmake.stash files and folders

8) Type qmake -version to make sure your qmake pointing to Qt5.11.2

9) Type qmake -spec macx-clang CS470.pro

10) Type make -j4

if you want to work in Xcode, type the following:
qmake -spec macx-xcode CS470.pro




Step 1. Install [Qt](https://www.qt.io/download/)

Step 2. Navigate to one of the folders (that's `my_opengl_version`, `qip_1st_semester`, and `qip_2nd_semester`. Each one is different project)

Step 3. Running following commands one by one(assume we're in `qip_2nd_semester`).


```bash
qmake qip.pro
```
    
This command will generate a `Makefile`, note that in folder 'my_opengl_version', we use hw.pro instead.

Next

```bash
make
```
    
This command will complie the program and produce excutable.
    
Lastly

```bash
./qip
```
    
to run your excutable.
