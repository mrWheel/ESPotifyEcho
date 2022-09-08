/*
Tray for Music Cubes
*/
printBody     = false;
printLidLeft  = false;
printLidRight = true;
printCube     = false;
printRFIDpcb  = false;
cubeX         = 54;
wallThickness = 2;
$fn = 30;

musicCube = "./STL/cube_5x5.stl";

module printBodyPart()  // RED
{
  difference()
  {
    union()
    {
      //-- Floor
      translate([0,-28,(cubeX/2)+2]) color("red") cube([cubeX-2,4, cubeX+10], true);
      //-- back RFID 
      cube([cubeX-2,cubeX, (wallThickness*3)+0.5], true); 
      translate([0,-15,-4.3]) rotate([15.5,0,0]) color("gray") cube([cubeX-2,1,5], true);
      translate([0,-29,cubeX+6.5]) color("blue") cube([cubeX-2,8,2], true);
      
    }

    //-- back RFID open
    translate([0,-25,-2.5]) color("green") cube([cubeX+15,14, 3.5], true); 
    //-- RFID slot
    translate([0,-7,0]) color("blue") cube([cubeX+5,cubeX+10, 2.5], true); 
    //-- LED strip
    translate([-30, -30.5, 51])  color("orange") cube([60,2,4]);    
    //-- White LED 1   
    translate([12, -34.5, 52]) color("red") cube([5, 10, 2]);       
    //-- White LED 2
    translate([-15,-34.5, 52]) color("red") cube([5, 10, 2]);      
  }

} //  printBodyPart()


module printLidLeft()
{
  difference()
  {
    union()
    {
      //-- floor RFID
      translate([0,-2.5,0]) color("red") cube([2,cubeX+3.0, 6.5], true);  
      //-- sloped side Wall      
      translate([-1, -15.5, 3]) rotate([0,180,180]) prism(2,15.5,55);  
      //-- small side wall
      translate([0,-30,-29]) color("yellow") cube([2,9,cubeX+11], true);
    }
    translate([0,-38,-56.5]) rotate([-15.7,0,0]) color("brown") cube([3,10,16], true);
  }
  //-- slope connector
  translate([2.5,-25,-27.3]) rotate([-15.7,0,0]) color("black") cube([7,1,cubeX+17], true);
  
} //  printLidLeft()


module printLidRight()
{
  difference()
  {
    union()
    {
      //-- floor RFID
      translate([0,-2.5,0]) color("red") cube([2,cubeX+3.0, 6.5], true);  
      //----------x,   y,   z --side Wall      
      translate([-1, -15.5, 3]) rotate([0,180,180]) prism(2,15.5,55);  
      //-- small side wall
      translate([0,-30,-29]) color("yellow") cube([2,9,cubeX+11], true);
    }
    //-----    
    translate([0,-38,-56.5]) rotate([-15.7,0,0]) color("brown") cube([3,10,16], true);
  }
  //--------- x,  y,   z  -- slope connector
  translate([-2.5,-25,-27.3]) rotate([-15.7,0,0]) color("black") cube([7,1,cubeX+17], true);
 
  
} //  printLidRight()


module prism(l, w, h)
{
       polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
               );
} //  prism()

//=============== main ======================

if (printCube)
{
  translate([0,0,25+4]) 
  {
    color("gray") import(musicCube);
  }

//  translate([0,-7,0]) color("gray") cube([40, 60, 2.5], true);
}

if (printRFIDpcb)
{
  translate([0,-7,0]) color("gray") cube([40, 60, 2.5], true);
}

if (printBody)
{
  translate([0,0,0])
  {
    //color("red")
    printBodyPart();
  }
}

offset = 0.0;

if (printLidLeft)
{
  rotate([180,0,180])
  {
    translate([(27+offset), 1, 0])
    {
      //color("blue")
      printLidLeft();
    }
  } // rotate
}

if (printLidRight)
{
  rotate([180,0,180])
  {
    translate([(-27-offset), 1, 0])
    {
      //color("gray")
      printLidRight();
    }
  } // rotate
}
  





