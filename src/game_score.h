#ifndef GAME_SCORE_H
#define GAME_SCORE_H

class GameScore {
public:
    GameScore();                 
    void incrementScore(); 
    int getScore() const;

private:
    int score;
};

#endif