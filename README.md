# Yet Another PSARC

Cross platform update of Matthieu Milan's [PSARC PS3 extractor for Linux](https://www.ferb.fr/ps3/PSARC/).  The initial commit includes minimal changes but a new command line argument was added.  *-h* tells psarc to print header information to std out without formatting the file size, using colons as field separators, and newlines to delineate entries (id:size:path). This is useful for piping information to another program that would build a tree view of an archive(s) contents.  A complete list of changes can be found in [CHANGELOG.md](CHANGELOG.md).

## Getting Started

Linux: To compile, navigate to the project directory: and execute *make* (*debug* and *clean* are also acceptable targets).

Windows:  Assuming you have MinGW and zlib headers installed, you can compile by navigating to the project directory and executing *mingw32-make.exe* (*debug* and *clean* are also acceptable targets).

Execute psarc without arguments for use instructions.

### How to Setup MinGW

For a verbose and illustrated guide visit [codebind](http://www.codebind.com/cprogramming/install-mingw-windows-10-gcc/).  For those who prefer brevity, here is the tl;dr:

Download and Install 
* Download the MinGW setup [here](https://osdn.net/projects/mingw/downloads/68260/mingw-get-setup.exe).
* Launch the setup and click on *Install*.
* Accept the default options and click *Continue* twice.
* Once the package manager is up click on *Basic Setup* in the left pane
* In the right pane, right click *mingw32-base-bin* and click *Mark for Installation*
* In the right pane, right click *mingw32-gcc-g++-bin* and click *Mark for Installation*
* In the left pane, click All Packages -> MinGW Libraries -> MinGW Standard Libraries
* In the right pane scroll down until you find *mingw32-libz-dev* and right click -> Mark for Installation
* From the top menu, click Installation -> Apply Changes
* In the dialogue box that pops up click *Apply* and then *Close* once it completes, then close the installation manager

Add C:\MinGW\bin to your path
* Click on the Windows start menu and type *env*, then click on "Edit the system environment variables" from the list above.
* Near the bottom of the dialogue box that pops up, click the *Environment Variables* button.
* In the top list, click *Path* and then click the *Edit* directly below that box (middle of dialogue box).
* On the right, click *New*
* Assuming you didn't change the default install location, type in *C:\MinGW\bin* and click *OK*
* Click *OK* two more times to close out the previous dialogues.

If you ever need to update or change installed packages run the MinGW shortcut on your desktop or in the start menu.  You can also launch mingw32-get from the command line or Powershell.

Congratulations, you can now open a command prompt or Powershell and compile.

## Special Notes

For those interested, I have made copies of my VS Code json files available in the *json* folder.  To use these settings copy the *.vscode* folder appropriate for your OS to the project base directory.

## Deployment

As a Windows user, if you have MinGW installed it is not necessary to statically link libraries and the LDFLAGS variables can be commented out of the Makefile (# is used for comments).  This will reduce your executable size from over 400kb to around 92kb.

To comment out static linking change this:
```
LDFLAGS = -static-libstdc++ -static-libgcc -static
```
To This:
```
#LDFLAGS = -static-libstdc++ -static-libgcc -static
```

## Built With

* [Visual Studio Code](https://github.com/Microsoft/vscode) - IDE used for both Windows and Linux
* [MinGW](http://mingw.org/) - Windows development environment

## Authors

* **Paul Hedrick** - *Rework* - [github](https://github.com/paul-hedrick) & [website](https://phedrick.dev) - not currently active

## Acknowledgments

* Matthieu Milan for the original source
