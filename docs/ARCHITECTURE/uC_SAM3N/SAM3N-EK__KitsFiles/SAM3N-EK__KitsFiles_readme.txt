SAM3N-EK Change Log
--------------------------------------------
date        boardREV    kitREV      SCHEMA      PCB         SAMrev.     TEST        Comment     
Jul-10      002         001         B           B           A           10          
Aug-10      A09-1143-1  A09-1142-1  B           B           A           10          Packaging change
Jan-10      A09-1143-2  A09-1142-2  B           B           A           10          QT600 USB bridge added in the kit

Kit Design & Manufacturing files - bundle contents description
--------------------------------------------------------------

SCHEMA archive:
         - The schematics in EDA tool format (note 1)
         - PDF printouts of these schematics

PCB archive:
         - The PCB design file in EDA tool format (note 2)
         - The GERBER files (PCB layers manufacturing plots)
         - The board assembly views, top and bottom, in PDF format

boardBOM archive:
         - The board Bill Of Material (BOM) in Excel format

TEST archive:
         - All the test package material (note 3) that is used on manufacturing line to test and 
           configure the board with the default demo.
         - A usage guide in PDF format.


---
note 1: Most of our board schematics are designed with Orcad. The file extension is .DSN
        A few have been made with Cadstar. In that case the file extension is .SCM
        
note 2: Most of our board PCBs are designed with Allegro. The file extension is .BRD
        A few have been made with Cadstar. In that case the file extension is .PCB

note 3: Composed of needed Windows software installations, binary files, TCL scripts, DOS scripts, etc.,
        in order to replay the last manufacturing step, which consists in testing and putting the board 
        into its default configuration state.

