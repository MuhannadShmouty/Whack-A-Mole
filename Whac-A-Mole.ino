// Whack-A-Mole game


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/////                                     TO-DO                                            /////
/////                                     UPDATE                                           /////
/////      Leds turn off for half a second after being pressed or after gameSpeed          /////
/////       In Setup, initalize random number to a number out of the range (0-4)           /////
/////         Make the voltages ranges Macros (readability and maintainability)            /////
/////     No need to change voltage to float (deal with the values 0-1023 directly)        /////
/////               Read average voltage from buttons for more precision                   /////
/////                          less magic numbers (use Macros)                             /////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Moles appear one at a time, hit only the correct ones to win.
 * The game speeds up each time you hit the right mole.
 * The score is displayed.
 * Hit the right mole 50 times to win.
 */

 
/*
  The circuit:
  * player 1 LED1 ==> player 1 Button 1
  * player 1 LED2 ==> player 1 Button 2
  * ....
  * player 2 LED1 ==> player 2 Button 1
  * player 2 LED2 ==> player 2 Button 2
  * .....
 */


#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>


/******     First LCD Pins     ******/
#define LCD1_RS 2
#define LCD1_EN 3
#define LCD1_D4 4
#define LCD1_D5 5
#define LCD1_D6 6
#define LCD1_D7 7


/******     Players buttons Pins     ******/
#define Player1_button A0
#define Player2_button A1

/******     LEDs Pins     ******/
const byte P1LEDs[] = {12, 11, 10, 9, 8};
const byte P2LEDs[] = {0, 1, A2, A3, 13};


LiquidCrystal lcd1(LCD1_RS, LCD1_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);
LiquidCrystal_I2C lcd2(0x27,16,2);

/******     Defining Player struct      ******/
typedef struct{
  bool buttonPressed = false;
  long gameSpeed = 3000, timePassed = millis();
  byte random_number, Score = 0;
  float ButtonVoltage = 0;
}Player;


byte winningScore = 50;

Player p1;
Player p2;


void setup() 
{
  for(int i = 0; i < 5; i++)
  {
    pinMode(P1LEDs[i], OUTPUT);
    pinMode(P2LEDs[i], OUTPUT);
    digitalWrite(P1LEDs[i], LOW);
    digitalWrite(P2LEDs[i], LOW);
  }

  lcd1.begin(16, 2);
  lcd2.begin(16, 2);
  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Score :");
  lcd2.setCursor(0, 0);
  lcd2.print("Score :");

  
  pinMode(Player1_button, INPUT);
  pinMode(Player2_button, INPUT);

  /***********    initializing the pseudo-random number generator    ***********/
  randomSeed(analogRead(A7));                       // A7 is a free analogue pin
  
}

void loop() 
{
  Game(&p1, P1LEDs, sizeof(P1LEDs) / sizeof(byte), Player1_button);
  Game(&p2, P2LEDs, sizeof(P2LEDs) / sizeof(byte), Player2_button);
  lcd1.setCursor(0, 1);
  lcd1.print(p1.Score);
  lcd1.print("   ");
  lcd1.print(p1.ButtonVoltage);
  lcd2.setCursor(0, 1);
  lcd2.print(p2.Score);
  lcd2.print("   ");
  lcd2.print(p2.ButtonVoltage);
  findWinner(&p1, &p2);
}

int FindButton(float voltage)
{
  
  /********* Find the button depending of the voltage produced *********/
  if(voltage >= 4.30)                           // range where voltage produced when the first button is pressed
  {
    return 0;
  }
  else if(voltage >= 2.0)                       // range where voltage produced when the second button is pressed
  {
    return 1;
  }
  else if(voltage >= 1.28)                      // range where voltage produced when the third button is pressed
  {
    return 2;
  }
  else if(voltage >= 0.74)                     // range where voltage produced when the fourth button is pressed
  {
    return 3;
  }
  else if(voltage >= 0.45)                     // range where voltage produced when the fifth button is pressed
  {
    return 4;
  }
  else
  {
    return -1;
  }
}

void GenerateNewNumber(Player * player,byte LEDs[])
{
  /***********    Generate random Number for first Player    ***********/
  digitalWrite(LEDs[player->random_number], LOW);
  byte temp = random(0, 5);
  if (temp == player->random_number)
  {
    temp = abs(temp - 1);
  }
  player->random_number = temp;
  digitalWrite(LEDs[player->random_number], HIGH);
}

void Game(Player * player, byte LEDs[], int sizeofArray, byte Player_button)
{
  /***********    Generate random Number every 3 seconds    ***********/
  
  if(millis() - player->timePassed >= player->gameSpeed)
    {
      GenerateNewNumber(player, LEDs, sizeofArray);
      player->timePassed = millis();
    }
    
  /***********    Get Voltage for the pressed button for each player    ***********/
  player->ButtonVoltage = analogRead(Player_button) * (5.0 / 1023.0);           // converting the analogue value to voltage
  
  /***********    Generate random Number if a button is pressed    ***********/
  if(player->ButtonVoltage >= 0.15)                         // 0.15 and not 0 to avoid the effect of noise
  {
    if(!player->buttonPressed)
    {
      /***********    Check if each player pressed the right button separately    ***********/
      if(FindButton(player->ButtonVoltage) == player->random_number)                         
      {
        player->Score++;
        //player->gameSpeed -= 80;                           // number of milliseconds subtracted each time a correct button is pressed
        if(player->gameSpeed <= 600)                       // max game speed 0.6 seconds
          player->gameSpeed = 600;
        GenerateNewNumber(player, LEDs, sizeofArray);
        player->timePassed = millis();
        player->buttonPressed = true;   
      }            
    }
  }
  else
  {
    player->buttonPressed = false;
  }
}

void findWinner(Player * player1, Player * player2)
{
  /***********    Declare a winner according to the score    ***********/
  
  if(player1->Score == winningScore && player2->Score != winningScore)
  {
    while(true)
    {
      lcd1.clear();
      lcd2.clear();
      delay(800);
      lcd1.setCursor(0, 0);
      lcd1.print("WINNER!!");
      lcd2.setCursor(0, 0);
      lcd2.print("LOSER!!");
      delay(1500);
    }
  }
  else if(player1->Score != winningScore && player2->Score == winningScore)
  {
    while(true)
    {  
      lcd1.clear();
      lcd2.clear();
      delay(800);
      lcd2.setCursor(0, 0);
      lcd2.print("WINNER!!");
      lcd1.setCursor(0, 0);
      lcd1.print("LOSER!!");
      delay(1500);
    }
  }
  else if(player1->Score == winningScore && player2->Score == winningScore)
  {
    while(true)
    {  
      lcd1.clear();
      lcd2.clear();
      delay(800);
      lcd2.setCursor(0, 0);
      lcd2.print("TIE!!");
      lcd1.setCursor(0, 0);
      lcd1.print("TIE!!");
      delay(1500);
    }
  }
}