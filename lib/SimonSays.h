#ifndef SIMONSAYS_H
#define SIMONSAYS_H

struct SimonSays{

    int game_array[2];
    int success = 0;
    void initiliaze_game(int rand1, int rand2){
        success = 0 ;
        game_array[0] = rand1;
        game_array[1] = rand2;
    }
    void check_input(int input){
        if(game_array[success] == input){
            ++success;
        }
    }
    void newArray(int rand1,int rand2){
        game_array[0] = rand1;
        game_array[1] = rand2;
    }


}Simon_Game;



#endif