#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "../include/Game.h"
#include "../include/UserPurchase.h"

using namespace std;


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

void addGame(const string& filename, const string& title, float price) {
    vector<Game> games = loadGames(filename);
    
    // Check for duplicate title
    for (const auto& g : games) {
        if (g.getTitle() == title) {
            printError("Game title already exists. Cannot add duplicate.");
            return;
        }
    }

    int maxId = 0;
    for (const auto& g : games) {
        if (g.getId() > maxId) maxId = g.getId();
    }
    
    Game newGame(maxId + 1, title, price);
    games.push_back(newGame);
    saveGames(filename, games);
    cout << "Game added successfully.\n";
}

void editGame(const string& filename, int id, const string& field, const string& new_value) {
    vector<Game> games = loadGames(filename);
    bool found = false;
    for (auto& g : games) {
        if (g.getId() == id) {
            if (field == "title") {
                g.setTitle(new_value);
            } else if (field == "price") {
                g.setPrice(stof(new_value));
            }
            found = true;
            break;
        }
    }
    if (found) {
        saveGames(filename, games);
        cout << "Game edited successfully.\n";
    } else {
        printError("Game not found.");
    }
}

void deleteGame(const string& gameFile, const string& purchaseFile, int id) {
    vector<Game> games = loadGames(gameFile);
    vector<Game> updatedGames;
    bool found = false;

    for (const auto& g : games) {
        if (g.getId() != id)
            updatedGames.push_back(g);
        else
            found = true;
    }

    if (!found) {
        printError("Game not found.");
        return;
    }

    saveGames(gameFile, updatedGames);

    // Cleanup purchases
    vector<UserPurchase> purchases = loadPurchases(purchaseFile);
    for (auto& up : purchases) {
        vector<int> updatedOwned;
        for (int gid : up.getOwnedGames()) {
            if (gid != id) updatedOwned.push_back(gid);
        }
        up = UserPurchase(up.getUserId());
        for (int gid : updatedOwned)
            up.addGame(gid);
    }
    savePurchases(purchaseFile, purchases);

    cout << "Game deleted and purchases updated.\n";
}

void viewGames(const string& filename) {
    vector<Game> games = loadGames(filename);
    for (const auto& g : games)
        g.display();
}

void viewPurchases(const string& gameFile, const string& purchaseFile) {
    vector<Game> games = loadGames(gameFile);
    vector<UserPurchase> purchases = loadPurchases(purchaseFile);

    for (const auto& up : purchases) {
        cout << "User " << up.getUserId() << " owns:" << endl;
        for (int id : up.getOwnedGames()) {
            auto it = find_if(games.begin(), games.end(), [id](const Game& g) { return g.getId() == id; });
            if (it != games.end())
                cout << "  - " << it->getTitle() << endl;
            else
                cout << "  - (Deleted game ID " << id << ")" << endl;
        }
    }
}

void showCommands() {
    cout << "Available admin commands:\n";
    cout << "1. add_game <title> <price>\n";
    cout << "2. edit_game <id> <title|price> <new_value>\n";
    cout << "3. delete_game <id>\n";
    cout << "4. view_games\n";
    cout << "5. view_purchases\n";
    cout << "6. commands\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printError("Invalid command.");
        return 1;
    }

    string command = argv[1];
    const string gamesFile = "../data/games.txt";
    const string purchasesFile = "../data/purchases.txt";

    if (command == "add_game") {
        if (argc < 4) {
            printError("Usage: ./admin_app add_game <title words> <price>");
            return 1;
        }
        string title = "";
        for (int i = 2; i < argc - 1; i++) {
            title += argv[i];
            if (i != argc - 2) title += " ";
        }
        float price = stof(argv[argc - 1]);
        addGame(gamesFile, title, price);

    } else if (command == "edit_game") {
        if (argc != 5) {
            printError("Usage: ./admin_app edit_game <id> <title|price> <new_value>");
            return 1;
        }
        editGame(gamesFile, stoi(argv[2]), argv[3], argv[4]);

    } else if (command == "delete_game") {
        if (argc != 3) {
            printError("Usage: ./admin_app delete_game <id>");
            return 1;
        }
        deleteGame(gamesFile, purchasesFile, stoi(argv[2]));

    } else if (command == "view_games") {
        viewGames(gamesFile);

    } else if (command == "view_purchases") {
        viewPurchases(gamesFile, purchasesFile);

    } else if (command == "commands") {
        showCommands();

    } else {
        printError("Unknown command.");
    }

    return 0;
}