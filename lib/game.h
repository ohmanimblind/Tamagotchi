#ifndef GAME_H
#define GAME_H
#include "stdlib.h"
#include "time.h"
#include "S7735.h"
struct Game
{
    int game_array[4][3] = {{0,0,0},{0,0,0},{0,0,0},{1,0,0}};
    int current_cursor = 0 ; //j index where cursor located
    int game_score = 0;
    int missed = 0;

    void reset_game(){
          for(int i = 0 ; i < 4 ; ++i){
            for (int j = 0 ; j < 3; ++j){
                game_array[i][j] = 0;
            }
          }
          game_score = 0;
          missed = 0;
          current_cursor = 0;
          game_array[3][current_cursor] = 1;
    }
    void game_addrow(int rand){
        game_array[0][0] = 0;
        game_array[0][1] = 0;
        game_array[0][2] = 0;
        game_array[0][rand] = 1;
    }

    void add_empty_row(){
        game_array[0][0] = 0;
        game_array[0][1] = 0;
        game_array[0][2] = 0;
    }
    void game_move_down_rows(){
        for(int i = 2 ; i >= 0 ; --i ){
            for(int j = 0; j < 3 ; ++j){
                if(i == 0){
                game_array[i][j] = 0;
                }else{
                game_array[i][j] = game_array[i-1][j];
            }
        }
    }
    }
    bool check_score(){
        for(int j = 0 ; j < 3 ; ++j){
            if(game_array[2][j] == 1){
                if(game_array[3][j] == 1){
                    ++game_score;
                    return true;
                }else{
                    ++missed;
                }
            }
        }
        return false;
    }
    void move_cursor_left(){
        game_array[3][current_cursor] = 0;
            if(current_cursor == 0){
                current_cursor = 2 ;
            }else{
                --current_cursor;
            }
        game_array[3][current_cursor] = 1;
    }
        void move_cursor_right(){
        game_array[3][current_cursor] = 0;
            if(current_cursor == 2){
                current_cursor = 0 ;
            }else{
                ++current_cursor;
            }
        game_array[3][current_cursor] = 1;
    }
    void ClearApples(){
        for(int i =0 ; i < 3 ; ++i){
            for (int j = 0 ; j < 3; ++j){
                if(game_array[i][j] == 1){
                    ClearApple(j*5 + 2,i*4);
                }
            } 
        }
    }
    void PrintApples(){
        for(int i =0 ; i < 3 ; ++i){
            for (int j = 0 ; j < 3; ++j){
                if(game_array[i][j] == 1){
                    PrintApple(j*5 + 2,i*4);
                }
            } 
        }
    }
    void PrintCursor(){
       PrintBasket(2+(current_cursor*5),13);
    }
    void ClearCursor(){
        ClearBasket(2+(current_cursor*5),13);
    } 
}basket_game;



#endif