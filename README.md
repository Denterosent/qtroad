# qtroad 
= rapid-object-analyzing-diagram, implemented with Qt Creator

This program converts c/cpp-code to UML Structure- and Classcharts.
The content in the textfield at the left side is converted to a graphic on the Graphics View on the right side.
Multiple c/cpp-files can be loaded to the textfield and then displayed on the Graphics View.
The content of the Graphics View can be printed directly or saved as PDF.

Hints:
* there currently is no auto-width -> you need to recompile with new value in the construcor of structureChartDrawer, to adapt the width
* variables are currently not detected automatically. If you want to add them to the list under the headline you have to use the syntax "$$$<type> <name>/$$$", e.g.: "$$$int x/$$$" adds "x: GZ" under the headline

Authors:
* Lukas Lihotzki
* Christian Autenrieth
* Marvin Baral
