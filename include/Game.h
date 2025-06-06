#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

class Game {
private:
    int id;
    std::string title;
    float price;
    float rating;
    int ratingCount;

public:
    Game();
    Game(int id, const std::string& title, float price, float rating = 0.0, int ratingCount = 0);

    void loadFromStream(std::istream& in);
    void saveToStream(std::ostream& out) const;
    void display() const;

    int getId() const;
    std::string getTitle() const;
    float getPrice() const;
    float getRating() const;
    int getRatingCount() const;

    void setTitle(const std::string& newTitle);
    void setPrice(float newPrice);
    void updateRating(float newRating);
};

#endif