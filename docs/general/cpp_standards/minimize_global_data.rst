Minimize Global and Shared Data
-------------------------------

Shared data complicates program logic and can cause unwanted
coupling between different parts of the program.

It also weakens unit testing because the history of the
program causes changes.

For icetray specifically, it causes problems when
running more than one `I3Tray`. People will complain,
so don't use global data.
