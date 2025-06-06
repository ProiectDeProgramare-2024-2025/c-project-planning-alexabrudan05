#include "../include/UserPurchase.h"

UserPurchase::UserPurchase() : user_id(0) {}

UserPurchase::UserPurchase(int id) : user_id(id) {}

void UserPurchase::loadFromStream(const std::string& line) {
    std::stringstream ss(line);
    ss >> user_id;
    int game_id;
    while (ss >> game_id) {
        owned_games.push_back(game_id);
    }
}

std::string UserPurchase::saveToString() const {
    std::stringstream ss;
    ss << user_id;
    for (int game_id : owned_games) {
        ss << " " << game_id;
    }
    return ss.str();
}

void UserPurchase::addGame(int game_id) {
    owned_games.push_back(game_id);
}

bool UserPurchase::ownsGame(int game_id) const {
    for (int id : owned_games) {
        if (id == game_id)
            return true;
    }
    return false;
}

int UserPurchase::getUserId() const {
    return user_id;
}

const std::vector<int>& UserPurchase::getOwnedGames() const {
    return owned_games;
}