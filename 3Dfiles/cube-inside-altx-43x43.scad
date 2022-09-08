/*
*   5x5 Box-Cube
*   10 stuks hartvormige doosjes plexiglas 5X5X5 cm transparant X confect
*   Merk: EMMEBI
*   https://amzn.eu/d/60zKORg
*   
*   This cube has a lid and is not square! 
*   There is a difference at the outside of about 2mm between the top (50x50) and 
*   the bottom (48.5x48.5)
*   Inside is about 42.5x42.5 (bottom) and 43.5x43.5 (top)
*/

include <../YAPP_Box/library/roundedCubes.scad>

rib = 43;

module inCube()
{
  difference()
  {
    roundedCube([rib, rib, rib], true, radius = 1.0, apply_to = "all");
    //-- room for hinges
    translate([(rib/2)-3.5, (rib/+2)-2,(rib/2)-3.5]) color("red") cube([7, 4, 7], true);
    translate([(rib/2)-3.5, (rib/-2)+2,(rib/2)-3.5]) color("red") cube([7, 4, 7], true);
  }
  
} //  inCube()

inCube();