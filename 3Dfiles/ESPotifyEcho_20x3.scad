//---------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for de ESPotifyEcho controller
//
//  Version 1.1 (13-02-2022)
//
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------
include </Users/WillemA/Documents/openSCAD/YAPP_Box/library/YAPPgenerator_v15.scad>

musicCube = "./STL/cube_5x5.stl";
tray      = "./STL/MusicCubeTray.stl";

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

printBaseShell      = true;
printLidShell       = true;
showStlHolder       = true;
showStlPCB          = true;
showStlCube         = true;

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
paddingFront        = 80;
paddingBack         = 2;
paddingRight        = 50;
paddingLeft         = 50;

// ridge where base and lid off box can overlap
// Make sure this isn't less than lidWallHeight
ridgeHeight         = 3.5;
ridgeSlack          = 0.1;
roundRadius         = 1.0;

// How much the PCB needs to be raised from the base
// to leave room for solderings and whatnot
standoffHeight      = 13.5;
pinDiameter         = 3.9;  // insert M2.5
pinHoleSlack        = 0.2;
standoffDiameter    = 6.5;

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
  translate([0,0,0])
  {
    rotate([180,0,90])
    {    //      l/r , f/b,  u/d 
      translate([-25,  109.2, -17]) 
      {
        color("white") import(myPcb);
      }
    }
  }
}

if (showStlHolder)
{
  translate([0,0,0])
  {
    rotate([0,75,0])
    {
      rotate([0,0,90])
      {         //  l/r  , u/d   ,  f/b
        translate([102.0, 21.5, (95+onLidGap)])
          color("darkgray") import(tray);
      }
    }
  }
}

if (showStlCube)
{
  translate([0,0,0])
  {
    rotate([0,-15,0])
    {    //      f/b , l/r,  u/d 
      translate([124,  102, 22]) 
      {
        color("white") import(musicCube);
      }
    }
  }
  //-- Left Cube
  translate([0,0,0])
  {
    //rotate([0,-10,0])
    {    //      f/b , l/r,  u/d 
      translate([40,  33, 48]) 
      {
        color("white") import(musicCube);
      }
    }
  }
  //-- mid Cube
  translate([0,0,0])
  {
    //rotate([0,-10,0])
    {    //      f/b , l/r,  u/d 
      translate([40,  101, 48]) 
      {
        color("white") import(musicCube);
      }
    }
  }

  //-- Right Cube
  translate([0,0,0])
  {
    //rotate([0,-10,0])
    {    //      f/b , l/r,  u/d 
      translate([40, 168, 48]) 
      {
        color("white") import(musicCube);
      }
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
                  [110, pcbWidth/2, 58, 57, 0, yappRectangle, yappCenter]  // cubeTray
                 ,[9.2, -45,   54, 54, 0, yappRectangle]  // spare Left cubeTray
                 ,[9.2,  22.5, 54, 54, 0, yappRectangle]  // spare middle cubeTray
                 ,[9.2,  90,   54, 54, 0, yappRectangle]  // spare Right cubeTray
                 
                 ,[86,  -2.2, 10, 2, 0, yappRectangle, yappCenter]  // BL0
                 ,[83, -27,   10, 2, 0, yappRectangle, yappCenter]  // GREEN
                 ,[76, -27.0, 10, 2, 0, yappRectangle, yappCenter]  // SHUFFLE
                 
                 ,[86,  98.5, 10, 2, 0, yappRectangle, yappCenter]  // BL1
                 ,[76, 123.5, 10, 2, 0, yappRectangle, yappCenter]  // RED

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
                  [45, -34, 20, 2, 65, yappRectangle]
                 ,[62, -32, 40, 2, 65, yappRectangle]
                 ,[80, -30, 60, 2, 65, yappRectangle]
                 ,[80, -20, 60, 2, 65, yappRectangle]
                 ,[80, -10, 60, 2, 65, yappRectangle]
                 ,[80,   0, 60, 2, 65, yappRectangle]
              // ,[80,  10, 60, 2, 65, yappRectangle]
                 ,[80,  20, 60, 2, 65, yappRectangle]
                 ,[80,  30, 60, 2, 65, yappRectangle]
                 ,[80,  40, 60, 2, 65, yappRectangle]
                 ,[80,  50, 60, 2, 65, yappRectangle]
              // ,[80,  60, 60, 2, 65, yappRectangle]
                 ,[80,  70, 60, 2, 65, yappRectangle]
                 ,[80,  80, 60, 2, 65, yappRectangle]
                 ,[80,  90, 60, 2, 65, yappRectangle]
                 ,[80, 100, 60, 2, 65, yappRectangle]
                 ,[80, 110, 40, 2, 65, yappRectangle]
                 ,[80, 120, 20, 2, 65, yappRectangle]
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
                  [shellLength-10, shellWidth-10, 2.7, 3.9, 7]
                 ,[shellLength-10, 10, 2.7, 3.9, 7]

                ];
                
//-- connectorsPCB -- origin = pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectorsPCB   = [
                    [4, 4, 2.7, 3.9, 7]
                   //,[4, -26, 2.7, 4.1, 7]
                   ,[4, pcbWidth-4, 2.7, 3.9, 7]
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
snapJoins   =     [
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
                [125,  30, 90, 0.5, "lid", "Liberation Mono:style=bold",20, "-" ] 
               ,[130, 155, 90, 0.5, "lid", "Liberation Mono:style=bold",20, "+" ] 
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
  
  module cornerStrudBL()
  {
    translate([5.5,5.5,-6]) rotate([0,0,45]) color("orange") cube([10,2,10], true);
    
  } //  cornerStrudBL()
  
  module cornerStrudBR()
  {
    translate([5.5,198.0,-6]) rotate([0,0,-45]) color("orange") cube([10,2,10], true);
    
  } //  cornerStrudBR()
  
  module cornerStrudFR()
  {
    translate([151.5,198.5,-6]) rotate([0,0,45]) color("orange") cube([8,2,10], true);
    
  } //  cornerStrudFR()
  
  module cornerStrudFL()
  {
    translate([151.5,5.0,-6]) rotate([0,0,-45]) color("orange") cube([8,2,10], true);
    
  } //  cornerStrudFR()
  
  module touchContactLeft()
  {
    translate([125,40,-5.5])
    {
      color("red")
      difference()
      {
        cube([40, 40, 10], true);
        translate([-1,1,0]) cube([40, 40, 13], true);
      }
      translate([-10,20,2]) cube([25,1,3], true);
    }
    
  } //  touchContact()

  module touchContactRight()
  {
    translate([125,164,-5.5])
    {
      color("red")
      difference()
      {
        cube([40, 40, 10], true);
        translate([-1,-1,0]) cube([40, 40, 13], true);
      }
      translate([-10,-20,2]) cube([25,1,3], true);
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
    translate([149,yPos,-1])
    {
      color("blue")
      //translate([-10,-20,2]) 
      rotate([180,0,270]) prism(2,64,10.5);;
    }
    
  } //  trayWall()
  
  touchContactLeft();
  touchContactRight();
  
  trayWall(73);
  trayWall(133);
  
  cornerStrudBL();
  cornerStrudBR();
  cornerStrudFR();
  cornerStrudFL();
  
  //ledStripe(80,25);
  //ledStripe(90,25);
  //ledStripe(80,175);
  //ledStripe(90,50);
  //ledStripe(90,150);
  
  spareCube(33.5);
  spareCube(101.5);
  spareCube(169);
  
} // lidHookInside(dummy)
  
//===========================================================
module lidHookOutside()
{
  module ledStripe(posX, posY)
  {
    translate([posX, posY, -2])
    {
      difference()
      {
        translate([0,0,-0.52]) cube([8, 15, 5], true);
        translate([0,0,-3.5]) cube([2, 10, 10], true);
        translate([0,0,-2]) color("blue") cube([5, 20, 3], true);
      }
    }
  }

  ledStripe(80,25);   // SHUFFLE
  ledStripe(87,25);   // GREEN
  ledStripe(90,50);   // BL0
  ledStripe(90,150);  // BL1
  ledStripe(80,175);  // RED

  
} // lidHookOutside(dummy)

//===========================================================
module baseHookInside()
{
  module cornerStrudBL()
  {
    translate([5.5,5.5,6]) rotate([0,0,45]) color("orange") cube([10,2,10], true);
    
  } //  cornerStrudBL()
  
  module cornerStrudBR()
  {
    translate([5.5,198.0,6]) rotate([0,0,-45]) color("orange") cube([10,2,10], true);
    
  } //  cornerStrudBR()
  
  module cornerStrudFR()
  {
    translate([151.5,198.5,6]) rotate([0,0,45]) color("orange") cube([8,2,10], true);
    
  } //  cornerStrudFR()
  
  module cornerStrudFL()
  {
    translate([151.5,5.0,6]) rotate([0,0,-45]) color("orange") cube([8,2,10], true);
    
  } //  cornerStrudFR()

  module mainStrud()
  {
    translate([(shellLength/2)+8,shellWidth/2,3]) color("orange") cube([2, shellWidth-2.2,4], true);
    
  } //  cornerStrudFR()
  
  cornerStrudBL();
  cornerStrudBR();
  cornerStrudFR();
  cornerStrudFL();
  mainStrud();

  
} // baseHookInside(dummy)

//===========================================================
module baseHookOutside()
{
  //echo("baseHookOutside(original) ..");
  
} // baseHookOutside(dummy)


//---- This is where the magic happens ----
YAPPgenerate();
