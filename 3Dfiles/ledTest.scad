
plateWidth = 15;
plateLength = 25;
standWidth  = 8;
standLength = 15;

module ledStripe(plateThickness, standHeight)
{
  difference()
  {
    translate([0, 0, 2]) 
            cube([standWidth, standLength, standHeight], true);
    translate([0,0,3]) 
            cube([2, 8, standHeight*2], true);
    translate([0,0,4]) color("blue") cube([5, 20, 3], true);
  }
}

module test(plateThickness, standHeight)
{
  cube([plateWidth,plateLength,plateThickness],true);
  //translate([-10,-10,plateThickness*-0.5]) ledStripe(10,10, plateThickness, standHeight);
  ledStripe(plateThickness, standHeight);
}

test(1.5, 5);
//translate([-30,0,0]) test(1.5, 5);
//translate([-10,0,0]) test(1.5, 6);
//translate([ 30,0,0]) test(2, 6);
//translate([ 10,0,0]) test(1.5, 7);
