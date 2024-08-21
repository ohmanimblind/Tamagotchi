#ifndef FLAPPY_APPLE_H
#define FLAPPY_APPLE_H
#include "S7735.h"
struct FlappyApple
{
    //int game_array[2][6] = {{0,0,0,0,0,1},{0,0,0,0,0,0}};
    bool got_clipped = 0 ;
    int ryuk_index = 0;
    int ryuk_row = 0;
    int cursor_row = 0;
    void setUp_flappy(){
        // for (int i = 0 ; i < 2 ; ++i){
        //     for(int j = 0 ; j < 2 ; ++j){
        //          game_array[i][j] = 0;
        //     }
           
        // }
        // game_array[0][5] = 1;
        ryuk_index = 0;
        ryuk_row = 0;
        cursor_row = 0 ;
        got_clipped = 0;
    }
    void MakeRyukInGame(int rand){
        if(rand == 0){
            ryuk_row = 0 ;
            
        }else(ryuk_row = 1);
        ryuk_index = 0;
    }
    // void zero_row(){
    //     game_array[0][0] = 0;
    //     game_array[1][0] = 0;
    // }
    void shift_rows(){
        // for (int i = 0; i < 2; ++i){
        //     for(int j = 1 ; j < 5; ++j){
        //         game_array[i][j] = game_array[i][j-1] ;
        //     }
        // }
        ++ryuk_index;
    }
    void check_collision(int rand){
        if(ryuk_index == 4){
           if(ryuk_row == cursor_row){
            got_clipped = 1;
           }
           clearRyuk();
           ryuk_index = 0;
           MakeRyukInGame(rand);
        }
        
    }

    void switch_cursor(){
        if(cursor_row == 0){
            cursor_row = 1;
        }else {
            cursor_row = 0;
        }
    }
    void print_cursor(){
        PrintApple(12,1 + 10 * (cursor_row));
    }

    void clear_cursor(){
        ClearApple(12,1 + 10 * (cursor_row));
    }
    void printRyuk(){
        RyukHead( 4 + (ryuk_index * 2) , 4 + (5 * ryuk_row));
    }
    void clearRyuk(){
        ClearHead( 4 + (ryuk_index*2) , 4 + (5 * ryuk_row));
    }
}Apple_game;



#endif