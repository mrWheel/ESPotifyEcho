/*
*   44x44 box-cube
*/

include <../YAPP_Box/library/roundedcubes.scad>

printMale   = true;
printFemale = false;

//----------------------------------------------------
cubeR   = 43;
cubeH   = 43;   // 44
cubeM   =  8;
cubeMx  =  6;
cubeF   = cubeR - (cubeM+0.5);
finger  = 10;
fingerD =  5;
offset  = 0;

//----------------------------------------------------
module male()
{
  roundedCube([cubeM, cubeR, (cubeH-cubeMx)], true, radius = 1.0, apply_to = "all");
  translate([0,0,(cubeH/2)-1])
  {
    color("blue")
    roundedCube([cubeM, cubeR-14, cubeMx+2], true, radius = 1.0, apply_to = "all");
  }
  translate([8,-10,0]) color("red") cube([finger,2,(cubeH-cubeMx)], true);
  translate([cubeM+3,-10,0]) cylinder(h=(cubeH-cubeMx), d=fingerD, center=true);
  
  translate([8,+10,0]) color("red")  cube([finger,2,(cubeH-cubeMx)],true);
  translate([cubeM+3,+10,0]) cylinder(h=(cubeH-cubeMx), d=fingerD, center=true);
  
} //  male()


//----------------------------------------------------
module female()
{
  difference()
  {
    roundedCube([cubeF,cubeR, cubeH], true, radius = 1.0, apply_to = "all");
    translate([-13,-10,0]) color("blue")   cube([finger,2.5,cubeR+1],true);
    translate([-12.3+(fingerD/2)-0.5,-10,0])  color("blue") cylinder(h=cubeR+1, d=5.5, center=true);
  
    translate([-13,+10,0]) color("blue") cube([10,2.5,cubeR+1],true);
    translate([-12.3+(fingerD/2)-0.5,+10,0]) cylinder(h=cubeR+1, d=5.5, center=true);
  }
} //  female()

//=======================================================================

if (printMale)
{
  translate([((cubeM/2))*-1,0,cubeMx/-2]) 
  {  
    color("red");  male();
  }
}

if (printFemale)
{
  translate([((cubeF/2)+offset)*+1,0,0]) 
  {
    color("green"); female();
  }
}
