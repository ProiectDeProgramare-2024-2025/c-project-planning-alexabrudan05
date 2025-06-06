#ifndef USERPURCHASE_H
#define USERPURCHASE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Game.h"

class UserPurchase {
private:
    int user_id;
    std::vector<int> owned_games;

public:
    UserPurchase();
    UserPurchase(int id);

    void loadFromStream(const std::string& line);
    std::string saveToString() const;
    void addGame(int game_id);
    bool ownsGame(int game_id) const;
    int getUserId() const;
    const std::vector<int>& getOwnedGames() const;
};

#endif