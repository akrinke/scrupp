This folder contains all the files that are needed to build 
Scrupp for Windows using Microsoft Visual C++ 6.

Open scrupp.dsw, select the active Configuration (Debug or 
Release) and compile everything. The temporary files will 
be created in the Debug/Release folder. The final 
executable scrupp.exe will be created in the root folder 
(..\..; the one which contains main.lua).

In order to run the executable you need to download the 
runtime libraries (DLLs). These are contained in the 
archive named scrupp-x.x-win32.zip available at 
Sourceforge.net. Copy them to the same folder as 
scrupp.exe.