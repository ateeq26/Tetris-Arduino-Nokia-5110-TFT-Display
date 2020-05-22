#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>

#include "Blocks.h"
#include "pitches.h"

Adafruit_PCD8544 disp= Adafruit_PCD8544(8,7,6,5,4);

const byte clk=2;
const byte aclk=3;
const byte lr=A0;
const byte buz=17;
const byte sel=11;
const byte pause=12;
const byte drop=10;
const byte led=9;
byte cSel,lSel,cBack,lBack,cDrop,lDrop,set=0;
byte ch,page=0;
byte fr,bk,go=0;
byte level,sf;
byte cPosX,cPosY;
byte g=0,s=0,freeze=0;
byte cBoard [12][18];
byte ledB=2,soSt=0,stoT=1,gi,hi;
byte nType=random(0,7);
byte nRot=random(0,4);
const int gmm[]={NOTE_F7,NOTE_E7,NOTE_DS7,NOTE_C7};
const int hsm[]={NOTE_C7,NOTE_D7,NOTE_E7,NOTE_F7};
volatile byte cType;
volatile int cRot,fRot;
volatile byte cClk,cAclk,lClk,lAclk;
Blocks *fBlock;
unsigned long dM=0,cT=0,kT=0,gmT,hmT;
int Speed=1000,Speed1;
int lrRead,xr;
int score=0;

enum {ON,OFF};

void InitBoard()
{
  for(int i=0;i<18;i++)
  {
    for(int j=0;j<12;j++)
      cBoard[j][i]=OFF;
  }
}

void posI()
{
  cClk=digitalRead(clk);
  if(cClk!=lClk && cClk==0)
  {
    cRot=(cRot+1)%4;
    fRot=cRot;
    if(soSt==1)
    tone(buz,NOTE_E7,67);
  }
  lClk=cClk;
}
  
void posD()
{
  cAclk=digitalRead(aclk);
    if(cAclk!=lAclk && cAclk==0)
  {
    fRot=(fRot-1)%4;
    cRot=fRot*-1;
//    Serial.println(cRot);
    if(soSt==1)
    tone(buz,NOTE_D7,67);
  }
  lAclk=cAclk;
}


void  DrawBlock (byte pX, byte pY, byte type, byte rot)
{
  byte i,j,c,d;
  pX=6+3*pX;
  pY=3*pY;
  for(i=(fBlock->getLimit(type,rot,0)),c=0;i<(fBlock->getLimit(type,rot,1));i++,c++)
  {
    for(j=(fBlock->getLimit(type,rot,2)),d=0;j<(fBlock->getLimit(type,rot,3));j++,d++)
    {
      if(fBlock -> getBlock(type,rot,j,i)!=0)
      {
        disp.drawRect((pX+c*3),(pY+30+d*3),3,3,BLACK);
            
       }
    }    
  }
  disp.display();
}

void  DrawNBlock (byte pX, byte pY, byte type, byte rot)
{
  byte i,j;
  
   for(i=(fBlock->getLimit(type,rot,0));i<(fBlock->getLimit(type,rot,1));i++)
  {
    for(j=(fBlock->getLimit(type,rot,2));j<(fBlock->getLimit(type,rot,3));j++)
    {
      if(fBlock -> getBlock(type,rot,j,i)!=0)
      {
        disp.drawRect((pX+i*3),(pY+j*3),3,3,BLACK);
           
       }
    }    
  }
  disp.display(); 
}

void DrawBoard()
{
  byte i,j,c=0,d=0;
  for(i=6;i<42;i+=3)
  {
    d=0;
    for(j=30;j<84;j+=3)
    {
      if(cBoard[c][d]==ON)
      {
     //   Serial.print(cBoard[c][d]);
       // Serial.print(" ");
        disp.drawRect(i,j,3,3,BLACK);
        
      }
     d++;
    }
  //  Serial.println();
    c++;
  }
  disp.display();
}

void StoreBlock(byte type,byte rot,byte pX,byte pY)
{
  for (byte i1 = pX, i2 = (fBlock->getLimit(type,rot,0)); i1 < pX + (fBlock->getLimit(type,rot,1)) ; i1++, i2++)
   {
    for (byte j1 = pY, j2 = (fBlock->getLimit(type,rot,2)); j1 < pY + (fBlock->getLimit(type,rot,3)); j1++, j2++)
      {  
  // Store only the blocks of the piece that are not holes
      if (fBlock->getBlock(type,rot, j2, i2) != 0)   
      cBoard[i1][j1]=ON;
}
}
stoT=1;
}
 
void delLine (int pY)
{
  // Moves all the upper lines one row down
  for (int j = pY; j > 0; j--)
  {
    for (int i = 0; i < 12; i++)
    {
      cBoard[i][j] = cBoard[i][j-1];
    }
  } 
}

void delComLines ()
{
  for (int j = 0; j < 18; j++)
  {
    int i = 0;
    while (i < 12)
    {
      if (cBoard[i][j] != ON) break;
      i++;
    }
    if (i == 12) 
    {
    delLine(j);
    score++;
    }
  }
}

void isGameover()
{ 
  byte i;
  for(i=0;i<12;i++)
  {
    if(cBoard[i][0]==ON)
    {
    go=1;
    break;
    }
  }  
}

void gameOver()
{ 
  /*if(u==0)
   {
    if(millis()-mT>247)  
    {
      mT=millis();
      tone(buz,gmm[gi++],225);
    }
   }
   else
   {
    u++;
   }
    if(i==4)
     {
      u=1;
     }*/
  disp.clearDisplay();
  disp.setTextSize(1);
  disp.fillRect(6,3,36,24,BLACK);
  disp.setTextColor(WHITE,BLACK);
  disp.setCursor(6,7);
  disp.print(" GAME ");
  disp.setTextSize(1);
  disp.setTextColor(WHITE,BLACK);
  disp.setCursor(6,16);
  disp.print(" OVER ");  
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(9,40);
  disp.print("SCORE:");  
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(9,54);
  disp.print(score);  
  disp.display();
  if(score>EEPROM.read(50))
    {
      set=1;
    }
    else
    {
      set=0;
    }
    HiScore();
}

void levelChange()
{
  disp.clearDisplay();
   disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(6,20);
  disp.print("Select");
  disp.setCursor(6,30);
  disp.print("Level:");
  lrRead=analogRead(lr);
  if(lrRead<157)
  lrRead=157;
  level=map(lrRead,1023,157,7,1);
  Speed=2233/level;
   disp.setTextSize(1);
  disp.setCursor(24,40);
  disp.print(level);
  disp.display();
}

void HiscoreShow()
{
  disp.clearDisplay();
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(4,0);
  disp.print("HiScore");
   disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(0,12);
  disp.print("1..");  
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(21,12);
  disp.print(EEPROM.read(10));    
  disp.setCursor(0,24);
  disp.print("2..");  
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(21,24);
  disp.print(EEPROM.read(20));  
  disp.setCursor(0,36);
  disp.print("3..");  
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(21,36);
  disp.print(EEPROM.read(30));  
  disp.setCursor(0,48);
  disp.print("4..");  
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(21,48);
  disp.print(EEPROM.read(40));  
  disp.setCursor(0,60);
  disp.print("5..");  
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(21,60);
  disp.print(EEPROM.read(50));
  disp.display();
}

void HiScore()
{
  /*if(u==0)
   {
    if(millis()-hmT>247)  
    {
      hmT=millis();
      tone(buz,hmm[hi++],225);
    }
   }
   else
   {
    u++;
   }
    if(hi==4)
     {
      u=1;
     }
  */
  if(set==1)
  {
  disp.clearDisplay();
  disp.setTextSize(1);
  disp.setTextColor(WHITE,BLACK);  
  disp.setCursor(4,0);
  disp.print("HiScore");
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(9,40);
  disp.print("SCORE:");  
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(9,54);
  disp.print(score);  
  disp.display();
  delay(1500);
  for(int i=10;i<51;i+=10)
    {
      Serial.print(score);
      Serial.print(" ");
      Serial.println(EEPROM.read(i));      
      if(score>EEPROM.read(i))
      {
        sf=i;     
        break;
    }
    }
    for(int j=50;j>=sf;j-=10)
    {
      EEPROM.write(j,EEPROM.read(j-10));
    }
    EEPROM.write(sf,score);
    set=0;
  }
}


void Tetris(byte ch)
{
  disp.setTextSize(2);
  disp.setTextColor(BLACK);
  disp.setCursor(0,0);
  disp.print("TET-");
  disp.setCursor(0,16);
  disp.print("-RiS");  
  disp.display();
   if(ch==1){
  disp.setTextSize(1);
  disp.setTextColor(WHITE ,BLACK);  
  disp.setCursor(12,40);
  disp.print(" Play");
  disp.setTextSize(1); 
   disp.setTextColor(BLACK,WHITE);  
   disp.setCursor(4,55);
  disp.print("Setting");
  disp.setTextSize(1);
   disp.setTextColor(BLACK,WHITE);  
   disp.setCursor(0,70);
  disp.print(" HiScore");
  disp.display();
}
   else if(ch==2)
   {
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(12,40);
  disp.print(" Play");
  disp.setTextSize(1); 
   disp.setTextColor(WHITE ,BLACK);  
   disp.setCursor(4,55);
  disp.print("Setting");
  disp.setTextSize(1);
   disp.setTextColor(BLACK,WHITE);  
   disp.setCursor(0,70);
  disp.print(" HiScore");
  disp.display();
}
   else if(ch==3)
   {
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(12,40);
  disp.print(" Play");
  disp.setTextSize(1); 
   disp.setTextColor(BLACK,WHITE);  
   disp.setCursor(4,55);
  disp.print("Setting");
  disp.setTextSize(1);
   disp.setTextColor(WHITE ,BLACK);  
   disp.setCursor(0,70);
  disp.print(" HiScore");
  disp.display();
}
}

void Setting(byte ch)
{  
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(0,0);
  disp.print("Settings");
  disp.display();
  if(ch==4)
  {
  disp.setTextSize(1);
  disp.setTextColor(WHITE ,BLACK);  
  disp.setCursor(9,40);
  disp.print(" Level");
  disp.setTextSize(1); 
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(0,55);
  disp.print(" Light");
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(38,55);
  disp.print(ledB); 
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(0,70);
  disp.print("Sound");
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(30,70);
  if(soSt==1)
  disp.print("ON");
  else
  disp.print("OFF");  
  disp.display();
  }
  else if(ch==5)
  {
  cSel=digitalRead(sel);
  if(cSel!=lSel && cSel==0)
    ledB=(ledB+1)%6;
  lSel=cSel;
  analogWrite(led,ledB*51);
  disp.setTextSize(1);
  disp.setTextColor(BLACK ,WHITE);  
  disp.setCursor(9,40);
  disp.print(" Level");
  disp.setTextSize(1); 
  disp.setTextColor(WHITE,BLACK);  
  disp.setCursor(0,55);
  disp.print(" Light");
  disp.setTextColor(WHITE,BLACK);  
  disp.setCursor(38,55);
  disp.print(ledB); 
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(0,70);
  disp.print("Sound");
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(30,70);
  if(soSt==1)
  disp.print("ON");
  else
  disp.print("OFF");  
  disp.display();  
  }
  else if(ch==6)
  {
  cSel=digitalRead(sel);
  if(cSel!=lSel && cSel==0)
    soSt=!soSt;
  lSel=cSel;
  disp.setTextSize(1);
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(9,40);
  disp.print(" Level");
  disp.setTextSize(1); 
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(0,55);
  disp.print(" Light");
  disp.setTextColor(BLACK,WHITE);  
  disp.setCursor(38,55);
  disp.print(ledB); 
  disp.setTextSize(1);
  disp.setTextColor(WHITE,BLACK);  
  disp.setCursor(0,70);
  disp.print("Sound");
  disp.setTextSize(1);
  disp.setTextColor(WHITE,BLACK);  
  disp.setCursor(30,70);
  if(soSt==1)
  disp.print("ON");
  else
  disp.print("OFF");  
  disp.display();
  }
  
}
void playGame(int freeze)
{
byte i,i2,f,j,j2,m;
  isGameover();
    f=0;
  if(go==0)
  {
   if(freeze==0)
   {
    cDrop=digitalRead(drop);
    if(cDrop==0)
    {
      Speed1=Speed/4;
    }
    else
    {
      Speed1=Speed;
    }
   if(millis()-cT>=Speed1)
   {
    cT=millis();
    if(g<=0)
    {
  disp.clearDisplay();        
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(0,0);
  disp.print("SCO");
  disp.display();
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(24,0);
  disp.print("NEXT");
  disp.display();
  disp.setTextSize(1);
  disp.setTextColor(BLACK);
  disp.setCursor(0,11);
  disp.print(score);
  disp.display();
  disp.drawRect(20,0,2,25,BLACK);
  disp.fillRect(14,16,6,7,WHITE);
  disp.fillRect(0,25,47,4,BLACK);
    disp.fillRect(0,30,6,54,BLACK);
    disp.fillRect(42,30,6,54,BLACK);
    disp.display();
    cType=nType;
    cRot=nRot;
    nType=random(0,7);
    nRot=random(0,4);
    DrawNBlock(30,11,nType,nRot);
    }
    for(i=(fBlock->getLimit(cType,cRot,0)),i2=0;i<(fBlock->getLimit(cType,cRot,1));i++,i2++)
    {
    for(j=(fBlock->getLimit(cType,cRot,2)),j2=0;j<(fBlock->getLimit(cType,cRot,3));j++,j2++)
    {
      if(fBlock -> getBlock(cType,cRot,j,i)!=0 && cBoard[cPosX+i2][g+j2]==ON)
      {
        f=1;
        stoT=0;
        goto here;
      }
    }
    }
    here:
    i=(fBlock->getLimit(cType,cRot,3)-fBlock->getLimit(cType,cRot,2))-1;
    if(((cType==0 && cRot==0)||(cType==0 && cRot==2)) && g==17)
    {
        f=1;
        cPosY=g;
    }
    else
    {
      if(17-g<i)
        f=1;
    }
    if(f==1)
    {
    StoreBlock(cType,cRot,cPosX,cPosY);
    if(stoT==1)
    g=0;
    else
    go=1;
    }
    else
    {
    cPosY=g;
    g++;
    }    
    if(g==18)
    g=0;
    delComLines();
    }
    if(millis()-dM>213)
    {
    dM=millis();
    lrRead=analogRead(lr);  
    if(lrRead<157)
    lrRead=157; 
    xr=map(lrRead,157,1023,0,11);
    if((11-xr)<(fBlock->getRight(cType,cRot)))
    xr=11-fBlock->getRight(cType,cRot);
 /*   if(xr < lBoardLimit(cType,cRot,xr,cPosY))
      xr=lBoardLimit(cType,cRot,xr,cPosY);*/    
    cPosX=xr;
    disp.fillRect(6,30,36,54,WHITE);       
    DrawBoard();
    DrawBlock(cPosX,cPosY,cType,cRot); 
    }
  }
  if(freeze==1)
  {
  /*disp.setTextSize(1);
  disp.setTextColor(WHITE,BLACK);
  disp.setCursor(14,16);
  disp.print("P");*/
    m++;
  }
}
if(go==1)
{
  //Serial.print("Gameover ");
  gameOver();
  delay(2500);
  HiscoreShow();
  InitBoard();
  page=0;
  go=0;
  /*
  while(page!=0)
  {
    HighScore();
  }*/
}
}



void setup() {
  // put your setup code here, to run once:
disp.begin();
disp.setRotation(1);
disp.setContrast(23);
disp.clearDisplay();
disp.fillScreen(WHITE);
pinMode(clk,INPUT);
pinMode(aclk,INPUT);
pinMode(lr,INPUT);
pinMode(sel,INPUT);
pinMode(pause,INPUT);
pinMode(drop,INPUT);
pinMode(buz,OUTPUT);
attachInterrupt(digitalPinToInterrupt(clk),posI,CHANGE);
attachInterrupt(digitalPinToInterrupt(aclk),posD,CHANGE);
//Wire.begin();
analogWrite(led,102);
//Serial.begin(9600);
InitBoard();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(go);
 if(page==0)
  {
  disp.clearDisplay();
  lrRead=analogRead(lr);
  if(lrRead<157)
  lrRead=157;
  if(lrRead>913)
  lrRead=913;
  //Serial.print(lrRead);
  //Serial.print(" ");
  ch=map(lrRead,157,913,3,1);
  cSel=digitalRead(sel);
  if(cSel!=lSel && cSel==0)
    page=ch;
  lSel=cSel;
  }
  else if(page==2)
  {
  disp.clearDisplay();
  lrRead=analogRead(lr);
  if(lrRead<157)
  lrRead=157;
  if(lrRead>913)
  lrRead=913;
  //Serial.print(lrRead);
  //Serial.print(" ");
  ch=map(lrRead,157,913,6,4);  
  if(ch==4)
  {
  cSel=digitalRead(sel);
  if(cSel!=lSel && cSel==0)
    page=ch;
  lSel=cSel;
  }
  cBack=digitalRead(pause);
    if(cBack!=lBack && cBack==0)
         page=0;
    lBack=cBack;
  }
  else 
  {
    cBack=digitalRead(pause);
    if(cBack!=lBack && cBack==0)
    {
      if(page==3)
         page=0;
      else if(page==4)
         page=2; 
      else
         freeze=!freeze;
    }
    lBack=cBack;
  }
  if(millis()-kT>497)
  {
    kT=millis();
    if(page==0)
      Tetris(ch);
    else if(page==2)
      Setting(ch);
    else if(page==3)
      HiscoreShow();
    else if(page==4)
      levelChange();
   }
   if(page==1)
      playGame(freeze); 
      
}
