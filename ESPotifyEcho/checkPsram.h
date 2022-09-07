#pragma once

class psramCheck 
{
public:
      psramCheck();
      int startPsram();
      int endPsram();

private:
      int psramAtStart;
      int psramAtEnd;
      int psramDiff;
};

//-------------------------------------------------------------------
psramCheck::psramCheck() 
{
  this->psramAtStart  = 0;
  this->psramAtEnd    = 0;
  this->psramDiff     = 0;
  
} // psramCheck()

//-------------------------------------------------------------------
int psramCheck::startPsram() 
{
  this->psramAtStart  = ESP.getFreePsram();
  return this->psramAtStart;
  
} // startPsram()

//-------------------------------------------------------------------
int psramCheck::endPsram() 
{
  this->psramAtEnd    = ESP.getFreePsram();
  this->psramDiff = this->psramAtEnd - this->psramAtStart;
  if (this->psramDiff != 0)
  {
    Serial.printf("PSRAM: Error (%d-%d)=>[%d]\r\n", this->psramAtEnd
                                                  , this-> psramAtStart
                                                  , this-> psramDiff);
    return this->psramDiff;
  }
  return 0;
  
} // endPsram()
