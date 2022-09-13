//---------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for de ESPotifyEcho controller
//
//  Version 1.1 (06-09-2022)
//
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------
include <{path-to-Your-YAPP_Box}/YAPP_Box/library/YAPPgenerator_v15.scad>

musicCube = "./STL/cube_5x5.stl";
tray      = "./STL/musicCubeTray.stl";
spareBase = "./STL/spareCubeBase_x2.stl";

/*
see https://polyd.com/en/conversione-step-to-stl-online
*/

myPcb = "./STL/ESPotifyEcho_v01_PCB.stl";


// Note: length/lengte refers to X axis, 
//       width/breedte to Y, 
//       height/hoogte to Z

/*
      padding-back|<------pcb length --->|<padding-front
                            RIGHT
        0    X-as ---> 
        +----------------------------------------+   ---
        |                                        |    ^
        |                                        |   padding-right 
        |                                        |    v
        |    -5,y +----------------------+       |   ---              
 B    Y |         | 0,y              x,y |       |     ^              F
 A    - |         |                      |       |     |              R
 C    a |         |                      |       |     | pcb width    O
 K    s |         |                      |       |     |              N
        |         | 0,0              x,0 |       |     v              T
      ^ |   -5,0  +----------------------+       |   ---
      | |                                        |    padding-left
      0 +----------------------------------------+   ---
        0    X-as --->
                          LEFT
*/

printBaseShell      = false;
printLidShell       = true;
showStlHolder       = false;
showStlPCB          = false;
showStlCube         = false;
showStlSpareBase    = false;

// Edit these parameters for your own board dimensions
wallThickness       = 2.0;
basePlaneThickness  = 2.0;
lidPlaneThickness   = 2.0;

//------------------- 35 mm --
baseWallHeight      = 10;
lidWallHeight       = 15;

// Total height of box = basePlaneThickness + lidPlaneThickness 
//                     + baseWallHeight + lidWallHeight
pcbLength           = 70.5;
pcbWidth            = 99.5;
pcbThickness        = 1.6;
                            
// padding between pcb and inside wall
paddingFront        = 90;
paddingBack         = 2;
paddingRight        = 20;
paddingLeft         = 20;

// ridge where base and lid off box can overlap
// Make sure this isn't less than lidWallHeight
ridgeHeight         = 3.5;
ridgeSlack          = 0.1;
roundRadius         = 2.0;

// How much the PCB needs to be raised from the base
// to leave room for solderings and whatnot
standoffHeight      = 13.5;
pinDiameter         = 3.9;  // insert M2.5
pinHoleSlack        = 0.2;
standoffDiameter    = 7.0;

//-- D E B U G -------------------
showSideBySide      = false;
hideLidWalls        = false;
onLidGap            = 0;
shiftLid            = 10;
colorLid            = "yellow";
hideBaseWalls       = false;
colorBase           = "white";
showPCB             = false;
showMarkers         = false;
inspectX            = 0;  // 0=none, >0 from front, <0 from back
inspectY            = 0;  // 0=none, >0 from left, <0 from right


if (showStlPCB)
{
    rotate([180,0,90])
    {    //      l/r , f/b,  u/d 
      translate([-55,  109.2, ((onLidGap+17)*-1)]) 
      {
        color("white") import(myPcb);
      }
    }
}

if (showStlHolder)
{
  offSet = 0;
  
          // f/b  , l/r   ,  u/d
  translate([91, 71.5, 45+(onLidGap/3)])
  {
  rotate([0,74,0])
  {
    rotate([0,0,90])
    {         //  l/r  , u/d   ,  f/b
   //   translate([71.5, (onLidGap+15.5)+offSet, 100+(onLidGap/3)])
        color("darkgray") import(tray);
    }
  }
}
} 

if (showStlSpareBase)
{
  offSet = 0;
        //  f/b, l/r,  u/d
  translate([40.5, 104, (21-offSet)])
    color("darkgray") import(spareBase);
 
         //  f/b, l/r,  u/d
  translate([40.5, 39, (21-offSet)])
  {
    rotate([0,0,180])
    {
      color("darkgray") import(spareBase);
    }
  }

} 

if (showStlCube)
{
    rotate([0,-17,0])
    {    //      f/b , l/r,  u/d 
      translate([129,  70, (onLidGap+17)]) 
      {
        color("white") import(musicCube);
      }
    }
  //-- Left Cube
  {    //      f/b , l/r,  u/d 
    translate([40,  39, (onLidGap+50)]) 
    {
      color("white") import(musicCube);
    }
  }

  //-- Right Cube
  {    //      f/b , l/r,  u/d 
    translate([40, 104, (onLidGap+50)]) 
    {
      color("white") import(musicCube);
    }
  }
}

//-- pcb_standoffs  -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = { yappBoth | yappLidOnly | yappBaseOnly }
// (3) = { yappHole, YappPin }
pcbStands = [
              [pcbLength-4, 4, yappLidOnly, yappHole]
             ,[pcbLength-4, pcbWidth-4, yappLidOnly, yappHole]
            ];     

//-- Lid plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLid =    [
               // [115, pcbWidth/2, 58, 57, 0, yappRectangle, yappCenter]  // cubeTray
                  [120, pcbWidth/2, 58, 62, 0, yappRectangle, yappCenter]  // cubeTray
                 ,[9.2, -10,   54, 54, 0, yappRectangle]  // spare Left cubeTray
                 ,[9.2,  55,   54, 54, 0, yappRectangle]  // spare Right cubeTray
                 
                 ,[74,  -9, 10, 2, 0, yappRectangle, yappCenter]  // GREEN
                 ,[83,  -9, 10, 2, 0, yappRectangle, yappCenter]  // SHUFFLE
                 ,[96,  +5, 10, 2, 0, yappRectangle, yappCenter]  // BL0
                 
                 ,[74, 110, 10, 2, 0, yappRectangle, yappCenter]  // RED
                 ,[96,  94, 10, 2, 0, yappRectangle, yappCenter]  // BL1

               ];

//-- base plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBase = [
                  [35,  -4, 20, 2, 65, yappRectangle]
                 ,[52,  -2, 40, 2, 65, yappRectangle]
                 ,[70,   0, 60, 2, 65, yappRectangle]
                 ,[70,  10, 60, 2, 65, yappRectangle]
                 ,[70,  20, 60, 2, 65, yappRectangle]
                 ,[70,  30, 60, 2, 65, yappRectangle]
                 ,[70,  40, 60, 2, 65, yappRectangle]
                 ,[70,  50, 60, 2, 65, yappRectangle]
                 ,[70,  60, 60, 2, 65, yappRectangle]
                 ,[70,  70, 60, 2, 65, yappRectangle]
                 ,[70,  80, 40, 2, 65, yappRectangle]
                 ,[70,  90, 14, 2, 65, yappRectangle]
             ];

//-- front plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsFront =  [
                ];

//-- back plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBack =   [
                  [28, -6, 16, 15, 0, yappCircle]    // power jack
                 ,[46, -8.5, 13, 10, 0, yappRectangle]  // USB connector
                ];
                
//-- left plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLeft =   [
                ];

//-- right plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsRight =  [
                 // [(pcbLength-20),6,12,12,0,yappCircle, yappCenter]
                ];

//-- connectors -- origen = box[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectors   =  [
                  [shellLength-10, shellWidth-10, 2.7, 3.9, 9]
                 ,[shellLength-10, 10, 2.7, 3.9, 9]

                ];
                
//-- connectorsPCB -- origin = pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectorsPCB   = [
                    [4, 4, 2.7, 3.9, 10]
                   //,[4, -26, 2.7, 4.1, 7]
                   ,[4, pcbWidth-4, 2.7, 3.9, 10]
                  ];

//-- base mounts -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = screwDiameter
// (2) = width
// (3) = height
// (4..7) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (5) = { yappCenter }
baseMounts   = [
               ];

//-- snap Joins -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = width
// (2..5) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (n) = { yappSymmetric }
snapJoins   = [
                [35, 5, yappLeft, yappRight, yappSymmetric]
               ,[shellWidth/2, 5, yappFront]
               ,[20, 5, yappBack, yappSymmetric]
              ];
               
//-- origin of labels is box [0,0,0]
// (0) = posx
// (1) = posy/z
// (2) = orientation
// (3) = depth
// (4) = plane {lid | base | left | right | front | back }
// (5) = font
// (6) = size
// (7) = "label text"
labelsPlane =  [
                [130,  15, 90, 1, "lid", "Liberation Mono:style=bold",20, "-" ] 
               ,[130, 110, 90, 1, "lid", "Liberation Mono:style=bold",20, "+" ] 
               ];

//===========================================================
module lidHookInside()
{
  module spareCube(yPos)
  {
    translate([40.2,yPos,-4.5])
    color("red")
    difference()
    {
      cube([56,56,7], true);
      cube([54,54,9], true);
    }
  } //  spareCube()
  
  module lidStrudBL()
  {
    translate([5.5,5.5,-6]) rotate([0,0,45]) color("orange") cube([10,2,10], true);

    
  } //  lidStrudBL()
  
  module lidStrudBR()
  {
    translate([5.5,138.3,-6]) rotate([0,0,-45]) color("orange") cube([10,2,10], true);
    
  } //  lidStrudBR()
  
  module lidStrudFR()
  {
    translate([161.5,138.5,-6]) rotate([0,0,45]) color("orange") cube([8,2,10], true);
    
  } //  lidStrudFR()
  
  module lidStrudFL()
  {
    translate([161.5,5.0,-6]) rotate([0,0,-45]) color("orange") cube([8,2,10], true);
    
  } //  lidStrudFR()
  
  module touchContactLeft()
  {
    translate([130,19,-5.5])
    {
      color("red")
      difference()
      {
        cube([40, 25, 10], true);
        translate([-1,1,0]) cube([40, 25, 13], true);
      }
      //translate([-10,20,2]) cube([25,1,3], true);
    }
    
  } //  touchContact()

  module touchContactRight()
  {
    translate([130,124,-5.5])
    {
      color("red")
      difference()
      {
        cube([40, 25, 10], true);
        translate([-1,-1,0]) cube([40, 25, 13], true);
      }
      //translate([-10,-20,2]) cube([25,1,3], true);
    }
    
  } //  touchContact()

  module prism(l, w, h)
  {
       polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
               );
  } //  prism()

  module trayWall(yPos)
  {
    translate([92,yPos,-11])
    {
      color("blue")  cube([64,2,10.5]);;
    }
    
  } //  trayWall()
  
  touchContactLeft();
  touchContactRight();
  
  trayWall(40.7);
  trayWall(100.5);
  
  lidStrudBL();
  lidStrudBR();
  lidStrudFR();
  lidStrudFL();
    
  spareCube(38.5);
  //spareCube(101.5);
  spareCube(104);
  
} // lidHookInside(dummy)
  
//===========================================================
module lidHookOutside()
{
  module ledStripe(posX, posY)
  {
    //--org--translate([posX, posY, -2])
    translate([posX, posY, -2])
    {
      difference()
      {
        translate([0,0,-0.52]) cube([8, 15, 5], true);
        translate([0,0,-4.0]) cube([2, 10, 10], true);
        translate([0,0,-2]) color("blue") cube([5, 20, 3], true);
      }
    }
  }

  ledStripe(78,130);    // RED
  ledStripe(100,115);   // BL1

  ledStripe(78,15);     // GREEN
  ledStripe(87,15);     // SHUFFLE
  ledStripe(100,28);    // BL0
  
} // lidHookOutside(dummy)

//===========================================================
module baseHookInside()
{
  module baseStrudBL()
  {
    translate([5.5,5.5,6]) rotate([0,0,45]) color("orange") cube([10,2,10], true);
    
  } //  baseStrudBL()
  
  module baseStrudBR()
  {
    translate([5.5,138.0,6]) rotate([0,0,-45]) color("orange") cube([10,2,10], true);
    
  } //  baseStrudBR()
  
  module baseStrudFR()
  {
    translate([161.5,138.5,6]) rotate([0,0,45]) color("orange") cube([8,2,10], true);
    
  } //  baseStrudFR()
  
  module baseStrudFL()
  {
    translate([161.5,5.0,6]) rotate([0,0,-45]) color("orange") cube([8,2,10], true);
    
  } //  baseStrudFR()

  module mainStrud()
  {
    translate([(shellLength/2)+8,shellWidth/2,3]) color("orange") cube([2, shellWidth-2.2,4], true);
    
  } //  baseStrudFR()
  
  baseStrudBL();
  baseStrudBR();
  baseStrudFR();
  baseStrudFL();
  mainStrud();

  
} // baseHookInside(dummy)

//===========================================================
module baseHookOutside()
{
  //echo("baseHookOutside(original) ..");
  
} // baseHookOutside(dummy)


//---- This is where the magic happens ----
YAPPgenerate();
