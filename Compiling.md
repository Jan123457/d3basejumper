**Compiling Guide**

**Programs Required**

MS Visual Studio 2003

Source Code which can be downloaded here http://d3basejumper.googlecode.com/svn/trunk/d3basejumper-read-only

**Installation**

Install MS Visual Studio 2003.

Download Source Code to a folder of your choice.

I use Tortoise SVN which can be downloaded at

http://tortoisesvn.net/downloads

**Debugging**

The sourcecode has been set up to run in debug mode. The only change required at the moment is to set the path to the gameplay-d.exe and the path to the /game working directory.

To do this <right click> the gameplay project within the solution explorer and select >properties. Select the >debugging in the left hand window and under these files in the right hand pane.

All necessary #includes and libraries are already included within the sourcecode.

**Release**

Currently we have not set up the paths to the #includes or libraries etc for the release build. This will be done shortly but for now where they are required you can simply copy the paths from the debug build.

**Issues**

There is an issue when running the code in debug. Enter a location and attempt to jump and the code will crash.