# qtroad 
= rapid-object-analyzing-diagramm, implemented with QT Creator

This program converts c/cpp-code to UML Structure- and Classcharts.
Content of Edit is converted to graphic on Graphics View.
Multiple c/cpp-files can be loaded to the Edit and the displayed 
content on the Graphics View can be printed directly or saved as PDF.

Hints:
*there currently is no auto-width -> you need to recompile with new value in the construcor of structureChartDrawer, to adapt the width
*variables are currently not detected automatically. If you want to add them to the list under the headline you have to use the syntax "$$$<type> <name>/$$$", e.g.: "$$$int x/$$$" adds "x: GZ" under the headline

Authors:
* Lukas Lihotzki
* Christian Autenrieht
* Marvin Baral
