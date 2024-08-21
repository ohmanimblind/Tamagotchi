#ifndef PET_H
#define PET_H
#include "S7735.h"

enum sprites{egg,misa,kiara,alice}sprite ;
struct pet
{ 
  int hapiness_score = 100;
  int hunger_score = 20;
  bool food_option = 0;
  sprites sprite = egg;
  int strikes = 0;
  int basket_won = 0;
  int ryuk_won = 0;
  int simon_won = 0;
  bool hatched = 0;

};
  void PrintSprite(pet &pet){
    if(pet.sprite == misa){
      PrintMisa();
    }else if(pet.sprite == kiara){
      PrintKiara();
    }else if(pet.sprite == egg){
      PrintEgg();
    }else if(pet.sprite == alice){
     PrintAlice();
    }
  }

  void PrintHappy(pet &pet){
     if(pet.sprite == misa){
      PrintMisaHappy();
    }else if(pet.sprite == kiara){
      PrintKiaraHappy();
    }else if(pet.sprite == egg){
      PrintEgg();
    }else if (pet.sprite == alice){
    AliceHappy();
    }
  }
  void ResetPet(pet &Pet){
   Pet.hapiness_score = 100;
   Pet.hunger_score = 20;
   Pet.food_option = 0;
   Pet.sprite = egg;
   Pet.strikes = 0;
   Pet.basket_won = 0;
   Pet.ryuk_won = 0;
   Pet.simon_won = 0;
   Pet.hatched = 0;
  }
  void PrintSad(pet &Pet){
     if(Pet.sprite == misa){
      PrintMisaAngry();
    }else if(Pet.sprite == kiara){
      PrintKiaraSad();
    }else if(Pet.sprite == egg){
      PrintEgg();
    }else if(Pet.sprite == alice){
     AliceSad();
    }
  
  }
    void PrintIdle1(pet &Pet){
        if(Pet.sprite == misa){
            MisaIdle1();
        }
        if(Pet.sprite == kiara){
            KiaraIdle1();
        }
        if(Pet.sprite == alice){

        }
    }
     void PrintIdle2(pet &pet){
        if(pet.sprite == misa){
            MisaIdle2();
        }
        if(pet.sprite == kiara){
            KiaraIdle2();
        }
        if(pet.sprite == alice){
        }
    }


#endif