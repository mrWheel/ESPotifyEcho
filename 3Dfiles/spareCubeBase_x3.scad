/*
*   ESPotifyEcho spareCubeBase
*   22-08-2022
*/

$fn=60;

module cubeBase()
{
  difference()
  {
    cube([59,59,3], true);
    translate([0,0,0.7])      cube([57,57,3], true);
    translate([27.2,17.5,-3]) color("red") cube([9,10,10]);
    translate([-36.2,17.5,-3])  color("red") cube([9,10,10]);
  }
} //  cubeBase()


module tubeExt1()
{
  difference()
  {
    cylinder(h=10, d=11.5, center=true);
    cylinder(h=11, d=9.5, center=true);
  }
} //  tubeExt1()

module tubeExt2()
{
  difference()
  {
    cylinder(h=8, d=11.5, center=true);
    cylinder(h=9, d=9.5, center=true);
    translate([7,0,0]) cube([8,2.5,9], true);
  }
} //  tubeExt2()


//cubeBase();

//tubeExt1();
tubeExt2();