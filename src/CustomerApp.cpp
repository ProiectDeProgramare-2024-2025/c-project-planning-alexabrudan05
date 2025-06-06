#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "../include/Game.h"
#include "../include/UserPurchase.h"

using namespace std;

// Error printing function (red)
void printError(const string& message) {
    cout << "\033[38;5;9m" << message << "\033[0m" << endl;
}

vector<Game> loadGames(const string& filename) {
    ifstream fin(filename);
    vector<Game> games;
    if (!fin.is_open()) return games;

    int n;
    fin >> n;
    fin.ignore();

    for (int i = 0; i < n; i++) {
        Game g;
        g.loadFromStream(fin);
        games.push_back(g);
    }
    fin.close();
    return games;
}

void saveGames(const string& filename, const vector<Game>& games) {
    ofstream fout(filename);
    fout << games.size() << endl;
    for (const auto& g : games) {
        g.saveToStream(fout);
    }
    fout.close();
}

vector<UserPurchase> loadPurchases(const string& filename) {
    ifstream fin(filename);
    vector<UserPurchase> purchases;
    if (!fin.is_open()) return purchases;

    string line;
    while (getline(fin, line)) {
        UserPurchase up;
        up.loadFromStream(line);
        purchases.push_back(up);
    }
    fin.close();
    return purchases;
}

void savePurchases(const string& filename, const vector<UserPurchase>& purchases) {
    ofstream fout(filename);
    for (const auto& up : purchases) {
        fout << up.saveToString() << endl;
    }
    fout.close();
}

UserPurchase* findUser(vector<UserPurchase>& purchases, int user_id) {
    for (auto& up : purchases) {
        if (up.getUserId() == user_id)
            return &up;
    }
    return nullptr;
}

void viewCatalogue(const vector<Game>& games) {
    for (const auto& g : games) {
        g.display();
    }
}

void purchaseGame(const string& purchaseFile, const vector<Game>& games, int user_id, int game_id) {
    vector<UserPurchase> purchases = loadPurchases(purchaseFile);
    UserPurchase* user = findUser(purchases, user_id);
    if (!user) {
        purchases.push_back(UserPurchase(user_id));
        user = &purchases.back();
    }

    if (user->ownsGame(game_id)) {
        printError("You already own this game.");
        return;
    }

    bool exists = false;
    for (const auto& g : games) {
        if (g.getId() == game_id) exists = true;
    }

    if (!exists) {
        printError("Game ID not found.");
        return;
    }

    user->addGame(game_id);
    savePurchases(purchaseFile, purchases);
    cout << "Game purchased successfully.\n";
}

void rateGame(const string& gameFile, int game_id, float rating) {
    vector<Game> games = loadGames(gameFile);
    bool found = false;

    for (auto& g : games) {
        if (g.getId() == game_id) {
            g.updateRating(rating);
            found = true;
            break;
        }
    }

    if (found) {
        saveGames(gameFile, games);
        cout << "Game rated successfully.\n";
    } else {
        printError("Game ID not found.");
    }
}

void viewOwnedGames(const vector<Game>& games, const vector<UserPurchase>& purchases, int user_id) {
    for (const auto& up : purchases) {
        if (up.getUserId() == user_id) {
            for (int id : up.getOwnedGames()) {
                for (const auto& g : games) {
                    if (g.getId() == id)
                        cout << "ID: " << g.getId() << ", Title: " << g.getTitle() << endl;
                }
            }
            return;
        }
    }
    cout << "No games owned.\n";
}

void deleteOwnedGame(const string& purchaseFile, vector<UserPurchase>& purchases, int user_id, int game_id) {
    for (auto& up : purchases) {
        if (up.getUserId() == user_id) {
            vector<int> updated;
            bool found = false;
            for (int gid : up.getOwnedGames()) {
                if (gid == game_id)
                    found = true;
                else
                    updated.push_back(gid);
            }

            if (!found) {
                printError("You do not own this game.");
                return;
            }

            up = UserPurchase(user_id);
            for (int gid : updated)
                up.addGame(gid);

            savePurchases(purchaseFile, purchases);
            cout << "Game successfully removed from your owned games.\n";
            return;
        }
    }
    printError("You do not own any games yet.");
}

void showCommands() {
    cout << "Available customer commands:\n";
    cout << "1. enter_id <user_id>\n";
    cout << "2. view_catalogue\n";
    cout << "3. purchase_game <game_id>\n";
    cout << "4. rate_game <game_id> <rating>\n";
    cout << "5. view_owned_games\n";
    cout << "6. delete_game <game_id>\n";
    cout << "7. commands\n";
}

int main(int argc, char* argv[]) {
    static int user_id = -1;
    const string gamesFile = "../data/games.txt";
    const string purchaseFile = "../data/purchases.txt";

    if (argc < 2) {
        printError("Invalid command.");
        return 1;
    }

    string command = argv[1];

    if (command == "commands") {
        showCommands();
        return 0;
    }

    if (command == "enter_id") {
        if (argc != 3) {
            printError("Usage: ./customer_app enter_id <user_id>");
            return 1;
        }
        user_id = stoi(argv[2]);
        ofstream fout("current_user.txt");
        fout << user_id;
        fout.close();
        cout << "User ID set.\n";
        return 0;
    }

    ifstream fin("current_user.txt");
    if (!fin.is_open()) {
        printError("Please enter ID first using enter_id.");
        return 1;
    }
    fin >> user_id;
    fin.close();

    vector<Game> games = loadGames(gamesFile);
    vector<UserPurchase> purchases = loadPurchases(purchaseFile);

    if (command == "view_catalogue") {
        viewCatalogue(games);
    } 
    else if (command == "purchase_game") {
        if (argc != 3) {
            printError("Usage: ./customer_app purchase_game <game_id>");
            return 1;
        }
        purchaseGame(purchaseFile, games, user_id, stoi(argv[2]));
    } 
    else if (command == "rate_game") {
        if (argc != 4) {
            printError("Usage: ./customer_app rate_game <game_id> <rating>");
            return 1;
        }
        rateGame(gamesFile, stoi(argv[2]), stof(argv[3]));
    } 
    else if (command == "view_owned_games") {
        viewOwnedGames(games, purchases, user_id);
    }
    else if (command == "delete_game") {
        if (argc != 3) {
            printError("Usage: ./customer_app delete_game <game_id>");
            return 1;
        }
        deleteOwnedGame(purchaseFile, purchases, user_id, stoi(argv[2]));
    }
    else {
        printError("Unknown command.");
    }

    return 0;
}