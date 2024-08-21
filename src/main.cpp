#include "../lib/timerISR.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
//#include "EEPROM.h"   NOT USED , PERSONAL USE, NOT A BUILD UPON 
#include "avr/eeprom.h"
#include "../lib/spiAVR.h"
#include <time.h>
#include <stdbool.h>
#include "../lib/S7735.h"
#include "../lib/game.h"
#include "../lib/SimonSays.h"
#include "../lib/FlappyApple.h"
#include "../lib/Pet.h"
#include "../lib/serialATmega.h"
#define NUM_TASKS 8 //TODO: Change to the number of tasks being used

typedef struct _task{
	signed 	 char state; 		
	unsigned long period; 		
	unsigned long elapsedTime; 	
	int (*TickFct)(int); 		
} task;




pet Pet; //Global Pet
pet Pet_2;
//int pet1adress = 0;   PERSONAL USE ONLY
//int pet2adress = sizeof(Pet_2);
//Global Variables
bool display_food_switch = 0;
bool displaying_wanted = 1; //Use for pet idle 

bool Pet_1_Hatched  = 0;
bool Pet_2_Hatched = 0 ; 
bool in_pet_idle = 0 ;
bool PetSelected = 0 ; // 0 for Pet 1, 1 for pet 2

//Count of games
int basket_games_won = 0;
int simon_games_won = 0;
int ryuk_bird_won = 0 ;

bool music_on = 1;
bool caught = 0 ;
bool pause;

bool check_food = 0; //For PET -> check_food
bool basket_game_on = 0; //For Pet ->game 1
bool simon_on = 0 ; //For Pet -> game 2
bool ryuk_on = 0 ; //For Pet - game 3

bool good = 0;
bool bad = 0 ;
bool buzzing = 0;

const unsigned long GCD_PERIOD = 100; // GCD
const unsigned long BUTTON_period = 100;
const unsigned long BUZZER_period = 100;
//Pet Periods
const unsigned long PET_period = 100;
const unsigned long FOOD_period = 100;
const unsigned long CHECK_period = 600000 ;// 10 Minutes , can be set to 14,400,000 for a 24hour hatch period
//Game Periods
const unsigned long BASKET_game = 100;
const unsigned long SIMON_period = 100;
const unsigned long RYUK_period = 100;
task tasks[NUM_TASKS]; 


void TimerISR() {
	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
		if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
			tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
		}
		tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
	}
}




enum button_states{IDLE,BUTTON_PRESSED,BUTTON_RELEASE};
bool B1 = 0 ;
bool B2 = 0 ;
bool B3 = 0 ;
bool B4 = 0 ;
bool B5 = 0 ;
//Function: Controls Global Button Control
//Status: Functional
int button_tick(int state){
  switch (state)
  {
  case IDLE:
    if(ADC_read(3) > 60 || ADC_read(4) > 60 || ADC_read(5) > 60 || ADC_read(2) > 60 || ADC_read(1) > 60){
    
      state = BUTTON_PRESSED;
    }
    break;
  case BUTTON_PRESSED:
        if(ADC_read(3) > 60){
        B1 = 1 ;
      }else if(ADC_read(4)>60){
        B2 = 1;
      }else if( ADC_read(5) > 60){
        B3 = 1;
      }else if(ADC_read(2) > 60){
        B4 = 1;
      }else if(ADC_read(1) > 60){
        B5 = 1;
      }
      state = BUTTON_RELEASE;
    break;
  case BUTTON_RELEASE:
      B1 = B2 = B3 = B4 = B5 = 0 ;
     if(ADC_read(3) < 60 && ADC_read(4) < 60 && ADC_read(5) < 60 && ADC_read(2) < 60 && ADC_read(1) < 60){
      state = IDLE;
  
    }
    break;
  default:
    break;
  }

  return state;
}

//Controls Buzzer 
//TODO : Add Helper function for different events 
// - button
// -status
// -correct food option
// -incorrect food option
enum buzzer_states{B_IDLE, B_PRES,  B_release,B_Caught,B_GAME_1,B_PAUSE,B_GOOD,B_BAD};
enum events{Button,Theme,Pause,Caught,Bad};
double tones[] = {11452.835,9630.4988,7638.04,11452.835,9630.4988,7638.04,11452.835,9630.4988,7638.04,
                  9630.4988, 15288.00, 10203.1857, 9630.4988, 10203.1857, 15288.00, 10203.1857, 9630.4988, 10203.1857,
                  14429.9515, 9630.4988, 7638.04, 9630.4988, 14429.9515, 9630.4988, 7638.04, 9630.4988, 12855.527, 8579.6711,
                  14429.9515, 8579.6711, 11452.835, 8579.6711, 14429.9515, 8579.671
                  };
//Parameters : event
double caught_t[] = {8098.07,12133.9};
double bad_t[] = {6809.48,4050.33};
//Function: Plays specific tone based on event (can be sequence of tones)
int tone_step = 0;
int caught_step = 0;
int bad_step = 0 ;
void play_buzzer(events event){
  switch (event)
  {
    case Button:
     ICR1 = 2500;
     OCR1A = 1280;
      break;
    case Theme:
    ICR1 = tones[tone_step];
    OCR1A = (tones[tone_step])/4;
    if(tone_step == 33){
      tone_step = 0;
    }else{
    ++tone_step;
    }
    break;
    case Pause:
      ICR1 = 2500;
      OCR1A = 2500;
      //PORTD = SetBit(PORTD,6,0);
      break;
    case Caught:
    ICR1 = caught_t[caught_step];
    OCR1A = caught_t[caught_step]/2;
    ++caught_step;
    if(caught_step == 1){
      caught_step = 0;
    }
    break;
    case Bad:
      ICR1 = bad_t[bad_step];
      OCR1A = bad_t[bad_step]/2;
      break;
    default:
      break;
  }
 
}
int buzz_i = 0;
int caught_tick = 0;
int buzzer_tick(int state){
    switch (state)
    {
    case B_IDLE:
     if( (B1 || B2 || B3 ) && simon_on){
      state = B_PRES;
     }
     if(basket_game_on){
      state = B_GAME_1;
     }else if (pause)
     {
      state = B_PAUSE;
     }else if(good){
      state = B_GOOD;
      buzz_i = 0;
      buzzing = 1;
     }else if(bad){
      state = B_BAD;
      buzz_i = 0;
      buzzing = 1;
     }
     
      break;
    case B_GOOD:
      if(buzz_i > 6){
        state = B_IDLE;
        buzz_i = 0;
        good = 0;
        buzzing = 0;
      }
      break;
    case B_BAD:
       if(buzz_i > 6){
        state = B_IDLE;
        buzz_i = 0;
        bad = 0;
        buzzing = 0;
      }
      break;
    case B_PRES:
      state = B_release;
      break;
    case B_release:
      if(!B1 && !B2 && !B3){
        state = B_IDLE;
        //play_buzzer(Pause);
      }
      break;
    case B_GAME_1:
      if(!basket_game_on){
        state = B_IDLE;
      }
      if(caught){
        state = B_Caught;
        caught_tick = 0;
      }
      break;
    case B_Caught:
      if(caught_tick > 1){
        state = B_GAME_1;
      }
      break;
    case B_PAUSE:
      if(!pause){
        state = B_IDLE;
      }
      break;
    }
    switch (state)
    {
    case B_IDLE:
    
     if( (in_pet_idle && music_on) || ryuk_on){
     ++buzz_i;
     if(buzz_i > 5){
     play_buzzer(Theme);
     buzz_i = 0 ;
     }
     }else{
      play_buzzer(Pause);
     }
      break;
    case B_GOOD:
      play_buzzer(Caught);
      ++buzz_i;
      break;
    case B_BAD:
    play_buzzer(Bad);
      ++buzz_i;
      break;
    case B_PRES:
    play_buzzer(Button);
      
      break;
    case B_release:
    play_buzzer(Pause);
      break;
    case B_Caught:
      play_buzzer(Caught);
      ++caught_tick;
      break;
    case B_GAME_1:
      play_buzzer(Pause);
     break;
    case B_PAUSE:
      play_buzzer(Pause);
    default:
      
    
    break;
    }
    
    return state;
}

//Controls Pet Flow/Menu
enum pet_states{PET_INIT,PET_IDLE,PET_SELECT,PET_FOOD_SELECT,PET_GAME_SELECT,PET_FOOD_ENTER,PET_STATUS,PET_BASKET_ENTER,PET_SIMON_ENTER, PET_RYUK_ENTER,
PREP_PETSELECT,PREP_PETSTATUS,PREP_PETIDLE,PREP_PETFOODSELECT, PREP_PETGAMESELECT,PREP_PETFOODENTER, PREP_BASKETGAMEENTER,PREP_PETSIMONENTER,PREP_RYUKENTER};
bool food_option = 0;
int game_option = 0 ;
bool food = 1 ;
bool game = 0 ;

int status_tick = 0 ;
int idle_tick = 0;
bool display_select_switch = 1;
//Function: Controls Overall Pet State 
int pet_tick(int state){
  
  switch (state)
  {
  case PET_INIT:
    state = PET_IDLE;
    break;
  case PET_IDLE:
    if(B1){
      state = PREP_PETSELECT;
      in_pet_idle = 0;
    }else if(B3){
      in_pet_idle = 0;
      state = PREP_PETSTATUS;
    }
    break;
  //Prep States
  case PREP_PETSELECT:
    state = PET_SELECT;
    break;
  case PREP_PETSTATUS:
    state = PET_STATUS;
    break;
  case PREP_PETIDLE:
    if(!buzzing ){
    state = PET_IDLE;
    }
    break;
  case PREP_PETFOODSELECT:
    state = PET_FOOD_SELECT;
    display_food_switch = 1;
    food_option = 0;
    break;
  case PREP_PETGAMESELECT:
    state = PET_GAME_SELECT;
    display_select_switch = 1;
    break;
  case PREP_PETFOODENTER:
    state = PET_FOOD_ENTER  ;
    break;
  case PREP_BASKETGAMEENTER:
    state = PET_BASKET_ENTER;
    break;
  case PREP_PETSIMONENTER:
    state = PET_SIMON_ENTER;
    break;
  case PREP_RYUKENTER:
    state = PET_RYUK_ENTER;
    break;
  case PET_SELECT:
      if(B3){
        state = PREP_PETIDLE;
      }
      else if(food && B2){
        state = PREP_PETFOODSELECT;
        
      }
      else if(game && B2){
        state = PREP_PETGAMESELECT;
        
      }
    break;
  case PET_FOOD_SELECT:
      if(B3){
        state = PREP_PETIDLE;
        
      }else if(B2){
        state = PREP_PETFOODENTER;
      
      }
      break;
  case PET_FOOD_ENTER:
      if(!check_food){
        state = PREP_PETIDLE;
      }
      break;
  case PET_GAME_SELECT:
      if(B3){
        state = PREP_PETIDLE;
      }else if(B2 && game_option == 0){
        state = PREP_BASKETGAMEENTER;
      }else if(B2 && game_option == 1){
        state = PREP_PETSIMONENTER;
      }else if(B2 && game_option == 2){
        state = PREP_RYUKENTER;
      }
      break;
  case PET_STATUS:
      ++status_tick;
      if(status_tick > 8){
      state = PREP_PETIDLE;
      status_tick = 0 ;
      }
      break;
    case PET_BASKET_ENTER:
    if(!basket_game_on){
      state = PREP_PETIDLE;
    }
    break;
  case PET_SIMON_ENTER:
    if(!simon_on){
      state = PREP_PETIDLE;
    }
    break;
  case PET_RYUK_ENTER:
    if(!ryuk_on){
      state = PREP_PETIDLE;
    }
    break;
  default:
    break;
  }

  //State Actions
  switch (state)
  {
  case PET_INIT:

    break;
  case PET_IDLE:
    game = 0 ;
    food = 1 ;
    display_select_switch = 1;
    in_pet_idle = 1 ;
    if(displaying_wanted == 1){
      WhiteOut();
      if(!PetSelected)
      { clear2(2,11);
        PrintSprite(Pet);
        print1(2,11);
      }else{
        clear1(2,11);
        PrintSprite(Pet_2);
        print2(2,11);
      }
      displaying_wanted = 0;
    }
    if(B2){
      PetSelected = !PetSelected;
      displaying_wanted = 1;
    }
    if(B4){
      music_on = !music_on;
    }
    if(B5){
      if(!PetSelected){
        ResetPet(Pet);
      }else if(PetSelected){
        ResetPet(Pet_2);
      }
      displaying_wanted = 1;
    }

    ++idle_tick;
    if(idle_tick == 10){
      if(!PetSelected){
       PrintIdle1(Pet);
      }else{
        PrintIdle1(Pet_2);
      }
    }else if(idle_tick == 20){
       if(!PetSelected){
        PrintIdle2(Pet);
      }else{
       PrintIdle2(Pet_2);
      }
      idle_tick = 0;
    }
    break;
  case PREP_PETSELECT:
    WhiteOut();
    PrintApple(8,3);
    PrintBasket(8,11);
    //PrintSelect1();
    break;
  case PREP_PETIDLE:
      //Global Flag
     pause = 0;
     displaying_wanted = 1;
      break;
  case PREP_PETFOODSELECT:
      PrintCake(9,8);
      ClearBasket(8,11);
      break;
  case PREP_PETFOODENTER:
        ClearSelect1and2();
        ClearApple(8,3);
        ClearCake(9,8);
        check_food = 1;
    break;
  case PREP_PETGAMESELECT:
      ClearApple(8,3);
      ClearSelect1and2();
      ClearBasket(8,11);
      break;
  case PREP_BASKETGAMEENTER:
        ClearBasket(8,9);
        ClearSelect1and2();
        basket_game_on = 1;
        break;
  case PREP_PETSIMONENTER:
      ClearA();
      simon_on = 1;
      break;
  case PREP_RYUKENTER:
      ClearHead(8,8);
      ryuk_on = 1;
      break;
  case PET_SELECT:
  
    if(B1){
     game = food;
     food = !food;
     display_select_switch = 1;
    }

    
    if(food && display_select_switch){
      PrintSelect1();
      display_select_switch = 0;
    }else if(game && display_select_switch){
      PrintSelect2();
      display_select_switch = 0 ;
    }
      break;
    
  case PET_FOOD_SELECT:
    if(B1){
      food_option = !food_option;
      display_food_switch = 1;
    }
    if(food_option == 0 && display_food_switch){
      PrintSelect1();
      display_food_switch = 0;
    }else if(food_option == 1 && display_food_switch){
      PrintSelect2();
      display_food_switch = 0;
    }
       break;
  case PET_GAME_SELECT:
      
      if(B1){
        ++game_option;
        if(game_option > 2){
          game_option = 0;
        }
        display_select_switch = 1;
      }
      if(game_option == 0 && display_select_switch){
        ClearHead(8,8);
        PrintBasket(8,9);
        display_select_switch = 0;
      }else if (game_option == 1 && display_select_switch){
        ClearBasket(8,9);
        PrintA();
        display_select_switch = 0;
      }
      else if(game_option == 2 && display_select_switch){
        ClearA();
        RyukHead(8,8);
        display_select_switch = 0;
      }

       break;
  case PET_FOOD_ENTER:

      break;
  case PREP_PETSTATUS:
      WhiteOut();
      break;

  case PET_STATUS:

  if(status_tick == 0){
    if(PetSelected == 0){
    
      if(Pet.food_option == 0){
        PrintApple(8,3);

      }else{
        PrintCake(9,7);
      }

      if(Pet.hunger_score > 60){
        PrintHungry(8,11);
      }else if(Pet.hapiness_score < 50){
        PrintSad(8,11);
      }else{
        PrintHappy(8,11);
      }
    }else{
      if(Pet_2.food_option == 0){
      PrintApple(8,3);

    }else{
      PrintCake(9,7);
    }
    if(Pet_2.hunger_score > 60){
        PrintHungry(8,11);
      }else if(Pet_2.hapiness_score < 50){
        PrintSad(8,11);
      }else{
        PrintHappy(8,11);
      }
    }
  }
  default:
    break;
  }
  return state;
}
enum food_check_states {NOT_CHECKING,DISPLAY_CHOICE,DISPLAY_BITE,SHOW_RESULT};
int food_display_ticks = 0;
int food_check_tick(int state){
  //State Transistions
  switch (state)
  {
  case NOT_CHECKING:
    if(check_food){
      state = DISPLAY_CHOICE;
      food_display_ticks = 0;
    }
    break;
  case DISPLAY_CHOICE:
    if(food_display_ticks > 8){
      state = DISPLAY_BITE;
      food_display_ticks = 0;
    }
    break;
  case DISPLAY_BITE:
    if(food_display_ticks > 8){
      state = SHOW_RESULT;
      food_display_ticks = 0;
    }
    break;
  case SHOW_RESULT:
    if(food_display_ticks > 20){
      state = NOT_CHECKING;
      check_food = 0;
    }
    break;
  default:
    break;
  } 
  //State Actions
  switch (state)
  {
  case DISPLAY_CHOICE:
    if(food_display_ticks == 0){
      if(food_option == 0){
        PrintApple(7,5);
      }else{
        PrintCake(8,8);
      }
    }
    ++food_display_ticks;
    break;
  case DISPLAY_BITE:
  if(food_display_ticks == 0){
    if(food_option == 0 ){
      BiteApple(7,5);
    }else{
      EatCake(8,8);
    }
  }
  ++food_display_ticks;

  break;
  case SHOW_RESULT:
     if(food_display_ticks == 0 ){
     
     if(PetSelected == 0){
      PrintSprite(Pet);
      if(food_option == Pet.food_option){
        Pet.hunger_score -= 10;
        if(Pet.hunger_score < 0){
          Pet.hunger_score = 0;
        }
        PrintHappy(Pet);
        good = 1; //Global
      }else{
        PrintSad(Pet);
        bad = 1;
        Pet.hapiness_score -= 10;
      }
      //srand(time(NULL));                //Needs more testing
      //Pet.food_option = rand()% 2 == 1 ;
      Pet.food_option = !Pet.food_option;
     }else{
       PrintSprite(Pet_2);
        if(food_option == Pet_2.food_option){
        Pet_2.hunger_score -= 10;
        if(Pet_2.hunger_score < 0){
          Pet_2.hunger_score = 0;
        }
        PrintHappy(Pet_2);
        good = 1;
      }else{
       PrintSad(Pet_2);
        bad = 1;
        Pet_2.hapiness_score -= 10;
      }
     
      Pet_2.food_option = !Pet_2.food_option;


     }
     }
     ++food_display_ticks;
    break;
  }
  
return state;
}
Game basket_game_1;
int basket_ticks = 0 ;
bool update_game = 0;
bool basket_display_flag = 0;
enum basket_states{BASKET_OFF, BASKET_GAME, BASKET_FAILED,BASKET_WON};
int basket_game_tick(int state){

    switch (state)
    {
    case BASKET_OFF:
      caught = 0;
      if(basket_game_on){
        state = BASKET_GAME;
        basket_game_1.PrintCursor();
      }
      
      break;
    case BASKET_GAME:
      if(basket_game_1.missed > 1){
        state = BASKET_FAILED;
        basket_ticks = 0;
        basket_display_flag = 1;
      }else if(basket_game_1.game_score > 6){
        state = BASKET_WON;
        basket_display_flag = 1;
        basket_ticks = 0;
      }
      break;
    case BASKET_FAILED:
      if(basket_ticks > 9){
      state = BASKET_OFF;
      basket_ticks = 0;
      basket_game_on = 0;
      }
      break;
    case BASKET_WON:
      if(basket_ticks > 9){
      state = BASKET_OFF;
      basket_ticks = 0;
      basket_game_on = 0;
      }
      
      break;

      break;
    default:
      break;
    }
    switch (state)
    {
    case BASKET_GAME:
    ++basket_ticks;
    if(B1){
        basket_game_1.ClearCursor();
        basket_game_1.move_cursor_left();
        basket_game_1.PrintCursor();
    }else if(B2){
        basket_game_1.ClearCursor();
        basket_game_1.move_cursor_right();
        basket_game_1.PrintCursor();

    }

      //basket_game_1.PrintApples();
      if(basket_ticks > 9){
        caught = basket_game_1.check_score();
        basket_game_1.ClearApples();
        basket_game_1.game_move_down_rows();
        if(rand()%2 == 1){
        basket_game_1.game_addrow(rand()%3);
        }else{
          basket_game_1.add_empty_row();
        }
         basket_game_1.PrintApples();
        basket_ticks = 0;
      }

      break;
    case BASKET_FAILED: //FIX ME: need to have flag or new state
     if(basket_display_flag == 1){
     pause = 1;
     Pet.hapiness_score -= 10;
     basket_game_1.ClearApples();
     basket_game_1.ClearCursor();
     basket_game_1.reset_game();
    if(!PetSelected){
    PrintSprite(Pet);
    PrintSad(Pet);
    
    }else{
    PrintSprite(Pet_2);
    PrintSad(Pet_2);
    }
     basket_display_flag = 0;
     bad = 1;
     }
    ++basket_ticks;
    break;
    case BASKET_WON:  //FIX ME: need to have flag or new state
    pause = 1;
    if(basket_display_flag == 1){
  
    basket_game_1.ClearApples();
    basket_game_1.ClearCursor();
    basket_game_1.reset_game();
    if(!PetSelected){
    PrintSprite(Pet);
    PrintHappy(Pet);
    ++Pet.basket_won;
    Pet.hapiness_score += 10;
    }else{
    PrintSprite(Pet_2);
    PrintHappy(Pet_2);
    ++Pet_2.basket_won;
    Pet_2.hapiness_score += 10;
    }
    basket_display_flag = 0;
    good = 1;
    }
    
    ++basket_ticks;
    break;
 
    default:
      break;
    }




return state;
}


SimonSays game_2; //Simon Says Struct
enum simon_states{SIMON_OFF, SIMON_PRINT1,SIMON_PRINT2, SIMON_WAIT, SIMON_CHECK,SIMON_CORRECT,SIMON_INCORRECT, SIMON_WON,SIMON_LOST};
int simon_ticks = 0 ;
int simon_display = 0;
int simon_entered = 0 ;
int entered_correct = 0 ;
int simon_tick_fct(int state){
    //State Transitions
    switch (state)
    {
    case SIMON_OFF:
      if(simon_on){
        state = SIMON_PRINT1;
        simon_ticks = 0 ;
        entered_correct = 0;
        simon_entered = 0;
        game_2.initiliaze_game(rand()%2,rand()%2);
        simon_display = 1;
      }
      break;
    case SIMON_PRINT1:
      if(simon_ticks > 5){
        simon_ticks = 0 ;
        simon_display = 1;
        state = SIMON_PRINT2;
      }
      break;
    case SIMON_PRINT2:
      if(simon_ticks > 5){
        simon_ticks = 0 ;
        simon_display = 1;
        state = SIMON_WAIT;
      }
      break;
    case SIMON_WAIT:
      if(simon_entered == 2){
        simon_entered = 0;
        simon_ticks = 0;
        state = SIMON_CHECK;
      }else if(simon_ticks > 100){
        state = SIMON_LOST;
      }
      break;
    case SIMON_CHECK:
      if(entered_correct == 2){
        state = SIMON_CORRECT;
        simon_display = 1;
      }else{
        state = SIMON_INCORRECT;
        simon_display = 1;
      }
      break;
    case SIMON_CORRECT:
      if(game_2.success == 3){
        state = SIMON_WON;
        simon_display = 1;
        simon_ticks = 0;
      }else if(simon_ticks > 5){
        state = SIMON_PRINT1;
        simon_display = 1;
      }
      break;
    case SIMON_INCORRECT:
      if(simon_ticks > 5){
        state = SIMON_LOST;
        simon_display = 1;
        simon_ticks = 0;
      }
      break;
    case SIMON_LOST:
      if(simon_ticks>5){
        simon_on = 0;
        state = SIMON_OFF;
      }
      break;
    case SIMON_WON:
      if(simon_ticks > 5){
        simon_on = 0;
        state = SIMON_OFF;
      }
      break;
    default:
      break;
    }
    //State Actions
    switch (state)
    {
    case SIMON_PRINT1:
      
      if(simon_display == 1){
        ClearCheck();
        if(game_2.game_array[0] == 0){
          PrintA();
        }else{
          PrintB();
        }
        simon_display = 0;
      }
      ++simon_ticks;
      break;
    case SIMON_PRINT2:
   
     if(simon_display == 1){
        ClearA();
        ClearB();
        if(game_2.game_array[1] == 0){
          PrintA();
        }else{
          PrintB();
        }
        simon_display = 0;
      }
    ++simon_ticks;
      break;
    case SIMON_WAIT:
    
      if(simon_display == 1){
        ClearA();
        ClearB();
        simon_display = 0;
      }
      if(B1){
        if(game_2.game_array[simon_entered] == 0){
          ++entered_correct;
        }
        ++simon_entered;
      }else if(B2){
        if(game_2.game_array[simon_entered] == 1){
          ++entered_correct;
        }
        ++simon_entered;
      }
    ++simon_ticks;
      break;
    case SIMON_CHECK:

      break;
    case SIMON_CORRECT:

      if(simon_display == 1){
      PrintCorrect();
      ++game_2.success;
      simon_display = 0;
      game_2.newArray(rand()%2,rand()%2);
      entered_correct = 0;
      }
      ++simon_ticks;
      break;
    case SIMON_INCORRECT: 
 
      if(simon_display == 1){
      PrintInCorrect();
      simon_display = 0 ;
      }
      ++simon_ticks;
      break;
    case SIMON_LOST:
    
      if(simon_display == 1){
    ClearCheck();
    if(!PetSelected){
    PrintSprite(Pet);
    PrintSad(Pet);
    }else{
    PrintSprite(Pet_2);
    PrintSad(Pet_2);
    }
    simon_display = 0;
    bad = 1;
      }
      ++simon_ticks;
      break;
    case SIMON_WON:
    if(simon_display == 1){
    ClearCheck();
    if(!PetSelected){
    PrintSprite(Pet);
    PrintHappy(Pet);
    ++Pet.simon_won;
    Pet.hapiness_score += 10;
    }else{
    PrintSprite(Pet_2);
    PrintHappy(Pet_2);
    Pet_2.hapiness_score += 10;
    ++Pet_2.simon_won;
    }
    simon_display = 0;
    good = 1;
    }
    ++simon_ticks;
      break;
    default:
      break;
    }

  return state;
}

FlappyApple game_3;
int ryuk_tick = 0;
int ryuk_game_tick = 0;
bool display_ryu_flag = 0;
enum ryuk_states{RYUK_OFF,RYUK_LOOP,RYUK_PRINT,RYUK_WON,RYUK_LOST};
int ryuk_game_time(int state){

  switch (state)
  {
  case RYUK_OFF:
    if(ryuk_on){
      state = RYUK_LOOP;
      game_3.setUp_flappy();
      game_3.print_cursor();
      game_3.printRyuk();
      ryuk_game_tick = 0;
      ryuk_tick = 0;
    }
    break;
  case RYUK_LOOP:
    if(game_3.got_clipped){
      state = RYUK_LOST;
      display_ryu_flag  = 1;
      ryuk_tick = 0;
    }else if (ryuk_game_tick > 150){
      state =  RYUK_WON;
      display_ryu_flag = 1;
      ryuk_tick = 0;
    }else if(ryuk_tick > 10){
      state = RYUK_PRINT;
      ryuk_tick = 0;
    }
    break;
  case RYUK_PRINT:
    state = RYUK_LOOP;
    break;
  case RYUK_WON:
  if(ryuk_tick > 5){
    state = RYUK_OFF;
    ryuk_on = 0;
  }
    break;
  case RYUK_LOST:
   if(ryuk_tick > 5){
   state = RYUK_OFF;
    ryuk_on = 0;
   }
    break;
  default:
    break;
  }
 
  switch (state)
  {
  case RYUK_LOOP:
    ++ryuk_tick;
    ++ryuk_game_tick;
    game_3.check_collision(rand()%2);
    if(B1){
      
      game_3.clear_cursor();
      game_3.switch_cursor();
      game_3.print_cursor();
    }
  
    
    break;
  case RYUK_PRINT:
     game_3.clearRyuk();
      game_3.shift_rows();
      game_3.printRyuk();
    break;
  case RYUK_WON: //ADD PET VALUES
    if(display_ryu_flag == 1){
      game_3.clear_cursor();
      game_3.clearRyuk();
      pause = 1;
      if(!PetSelected){
       PrintSprite(Pet);
        PrintHappy(Pet);
        ++Pet.ryuk_won;
        Pet.hapiness_score += 10;
      }else{
       PrintSprite(Pet_2);
      PrintHappy(Pet_2);
        ++Pet_2.ryuk_won;
        Pet_2.hapiness_score += 10;
      }
      display_ryu_flag = 0;
      good = 1;
    }
    ++ryuk_tick;
    break;


  case RYUK_LOST: //ADD PET VALUES
     if(display_ryu_flag == 1){
       game_3.clear_cursor();
      game_3.clearRyuk();
      pause = 1;

      if(!PetSelected){
      PrintSprite(Pet);
      PrintSad(Pet);
      }else{
      PrintSprite(Pet_2);
     PrintSad(Pet_2);
      }
      display_ryu_flag = 0;
      bad = 1;
    }
    ++ryuk_tick;
    break;
  default:
    break;
  }
  
  return state;
}

int PET_CHECK_TICK = 0;
enum pet_checking_states{WAITING, CHECK,HATCH};

int pet_timer_tick(int state){

  switch (state)
  {
  case WAITING:
    if(PET_CHECK_TICK > 5 && (!Pet.hatched || !Pet_2.hatched)){
      PET_CHECK_TICK = 0;
      state = HATCH;
    }else if(PET_CHECK_TICK > 5){
      state = CHECK;
    }
    break;
  case HATCH:
    state = WAITING;
    PET_CHECK_TICK = 0;
    break;
  case CHECK:
    state = WAITING;
    PET_CHECK_TICK = 0;
    break;
  default:
    break;
  }

  switch (state)
  {
  case WAITING:
    ++PET_CHECK_TICK;

  //  EEPROM.put(0,Pet);      NOT USED PERSONAL USE
   // EEPROM.put(sizeof(Pet),Pet_2); 
    break;
  case HATCH:

    if(!Pet.hatched){
      if(Pet.ryuk_won > Pet.basket_won ){
        Pet.sprite = misa;
      }else if(Pet.simon_won > 5){
        Pet.sprite = kiara;
      }else if (Pet.basket_won > Pet.simon_won && Pet.ryuk_won > 3){
        Pet.sprite = alice;
      }else{
        int choice = rand()%3;
        if( choice == 1){
          Pet.sprite = misa;
        }
        if( choice == 0){
          Pet.sprite = kiara;
        }
        if( choice == 2){
          Pet.sprite = alice;
        }
      }
      Pet.hatched = 1;
    }
   if(!Pet_2.hatched){
      if(Pet_2.ryuk_won > Pet_2.basket_won ){
        Pet_2.sprite = misa;
      }else if(Pet_2.simon_won > 5){
        Pet_2.sprite = kiara;
      }else if (Pet_2.basket_won > Pet_2.simon_won && Pet_2.ryuk_won > 3){
        Pet_2.sprite = alice;
      }else{
        int choice = rand()%3;
        if( choice == 1){
          Pet_2.sprite = misa;
        }
        if( choice == 0){
          Pet_2.sprite = kiara;
        }
        if( choice == 2){
          Pet_2.sprite = alice;
        }
      }
      Pet_2.hatched = 1;
    }
    displaying_wanted = 1;
    break;
  case CHECK:
  ++PET_CHECK_TICK;
  Pet.hunger_score += 10;
  Pet.hapiness_score -= 10;
  Pet_2.hunger_score += 10;
  Pet_2.hapiness_score -= 10;
  //DIED
  if(Pet.hunger_score > 90 || Pet.hapiness_score < 10 || (Pet.hapiness_score < 40 && Pet.hunger_score > 70)){
    ++Pet.strikes;
    if(Pet.strikes > 3){ResetPet(Pet);}
  }
  if(Pet_2.hunger_score > 90 || Pet_2.hapiness_score < 10 || (Pet_2.hapiness_score < 40 && Pet_2.hunger_score > 70)){
    ++Pet_2.strikes;
    if(Pet_2.strikes > 3){
    ResetPet(Pet_2);
    }
  }

  break;
  default:
    break;
  }


  return state;
}

int main(void){
    
    //Initialize
    int i = 0 ;
    DDRD = 0xFF ; PORTD = 0x00; //all outputs
    DDRB = 0xFF; PORTB = 0x00;  //all outputs
    DDRC = 0x00 ; PORTC = 0xFF; // all inputs
    srand(time(NULL));
    SPI_INIT();
    ADC_init(); 
    // PERSONAL USE NOT USED : Retrieve Pets
    //EEPROM.get(0,Pet);
    //EEPROM.get(sizeof(Pet),Pet_2);
    //NOT A BUILD UPON

    serial_init(9600); 
    S7735_init();
   
    //Setup
    WhiteOut();
    TCCR1A |= (1 << WGM11) | (1 << COM1A1); //COM1A1 sets it to channel A
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); //CS11 sets the prescaler to be 8
    OCR1A = 2550;




    //TODO: Initialize tasks here
    //e.g. 
    tasks[i].period = BUTTON_period;
    tasks[i].state = IDLE;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = button_tick ;
    ++i;
    tasks[i].period = BUZZER_period;
    tasks[i].state = B_IDLE;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = buzzer_tick;
    ++i;
    tasks[i].period = PET_period;
    tasks[i].state = PET_INIT;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = pet_tick;
    ++i;
    tasks[i].period = FOOD_period;
    tasks[i].state = NOT_CHECKING;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = food_check_tick;
    ++i;
    tasks[i].period = BASKET_game;
    tasks[i].state = BASKET_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = basket_game_tick;
    ++i;
    tasks[i].period = SIMON_period;
    tasks[i].state = SIMON_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = simon_tick_fct;
    ++i;
    tasks[i].period = RYUK_period;
    tasks[i].state = RYUK_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = ryuk_game_time;
    ++i;
    tasks[i].period = CHECK_period;
    tasks[i].state = WAITING;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = pet_timer_tick;



    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {
    }

    return 0;
}