#include "../include/Game.h"

Game::Game() : id(0), title(""), price(0), rating(0), ratingCount(0) {}

Game::Game(int id, const std::string& title, float price, float rating, int ratingCount)
    : id(id), title(title), price(price), rating(rating), ratingCount(ratingCount) {}

void Game::loadFromStream(std::istream& in) {
    in >> id;
    in.ignore();
    std::getline(in, title);
    in >> price >> rating >> ratingCount;
    in.ignore();
}

void Game::saveToStream(std::ostream& out) const {
    out << id << std::endl;
    out << title << std::endl;
    out << price << " " << rating << " " << ratingCount << std::endl;
}

std::string colorizeRating(float rating) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (rating < 3.0)
        ss << "\033[38;5;9m" << rating << "\033[0m";
    else if (rating <= 4.2)
        ss << "\033[38;5;11m" << rating << "\033[0m";
    else
        ss << "\033[38;5;10m" << rating << "\033[0m";
    return ss.str();
}

void Game::display() const {
    std::cout << "ID: " << id
              << ", Title: " << title
              << ", Price: $" << std::fixed << std::setprecision(2) << price
              << ", Rating: " << colorizeRating(rating)
              << " (" << ratingCount << " ratings)" << std::endl;
}

int Game::getId() const { return id; }
std::string Game::getTitle() const { return title; }
float Game::getPrice() const { return price; }
float Game::getRating() const { return rating; }
int Game::getRatingCount() const { return ratingCount; }

void Game::setTitle(const std::string& newTitle) { title = newTitle; }
void Game::setPrice(float newPrice) { price = newPrice; }
void Game::updateRating(float newRating) {
    rating = (rating * ratingCount + newRating) / (ratingCount + 1);
    ratingCount++;
}