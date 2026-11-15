// student module: implementation.
#pragma once
#include "ds.cpp"
#include <sstream>      // STL: parsing the comma-separated filter choices

// Tri-state filter: 0 = any, 1 = must have the feature, 2 = must not have it.
bool failsTriState(int choice, bool hasFeature) {
    return (choice == 1 && !hasFeature) || (choice == 2 && hasFeature);
}

void searchListings(const AccommodationList& listings) {
    cout << "\n--- Search and Filter ---\n"
         << "Choose only the filters you care about. Everything else uses its default.\n\n"
         << " 1. Keyword / location\n"
         << " 2. Accommodation type (PG / Flat / Hostel / Room)\n"
         << " 3. Maximum monthly rent\n"
         << " 4. Number of beds / seaters\n"
         << " 5. Mess / food facility\n"
         << " 6. Laundry facility\n"
         << " 7. Attached bathroom\n"
         << " 8. Nearby market\n"
         << " 9. Independent or shared accommodation\n"
         << "10. Minimum room size\n"
         << "11. Maximum distance from university\n"
         << "12. Sort results by rent\n"
         << "13. Availability (include unavailable listings)\n"
         << "\nEnter option numbers separated by commas (example: 1,3,5).\n"
         << "Enter 0 to skip filters and show available listings: ";
    vector<int> selected;
    while (true) {
        string line;
        if (!getline(cin, line)) exitOnEof();
        selected.clear();
        stringstream input(line);
        string token;
        bool valid = true, choseZero = false;
        while (getline(input, token, ',')) {
            // Permit spaces around the option number.
            token.erase(0, token.find_first_not_of(" \t"));
            size_t last = token.find_last_not_of(" \t");
            if (last == string::npos) { valid = false; break; }
            token.erase(last + 1);
            stringstream numberInput(token);
            int number;
            char extra;
            if (!(numberInput >> number) || (numberInput >> extra) || number < 0 || number > 13) {
                valid = false;
                break;
            }
            if (number == 0) choseZero = true;
            else if (find(selected.begin(), selected.end(), number) == selected.end())
                selected.push_back(number);
        }
        if (choseZero && !selected.empty()) valid = false;
        if (valid && (choseZero || !selected.empty())) {
            if (choseZero) selected.clear();
            break;
        }
        cout << "Invalid choice. Enter numbers from 1 to 13 separated by commas, or just 0: ";
    }
    auto wants = [&selected](int option) {
        return find(selected.begin(), selected.end(), option) != selected.end();
    };
    // Unselected filters are disabled by default (0 = off).
    string keyword;
    if (wants(1)) {
        cout << "Enter property name or location keyword (blank = no keyword): ";
        getline(cin, keyword);
        keyword = lowerCase(keyword);
    }
    double maxRent = wants(3) ? readDouble("Maximum monthly rent in rupees: ", 1, 10000000) : 0;
    int typeChoice = wants(2)
        ? readChoice("Type: 1 = PG, 2 = Flat, 3 = Hostel, 4 = Room, 0 = Any", "Choose type: ", 0, 4) : 0;
    int bedsChoice = wants(4) ? readInt("Exact number of beds/occupancy (0 = any): ", 0, 100) : 0;
    int messChoice = wants(5)
        ? readChoice("Mess: 1 = Required, 2 = Not required, 0 = Any", "Choose mess preference: ", 0, 2) : 0;
    int laundryChoice = wants(6)
        ? readChoice("Laundry: 1 = Required, 2 = Not required, 0 = Any", "Choose laundry preference: ", 0, 2) : 0;
    int bathroomChoice = wants(7)
        ? readChoice("Bathroom: 1 = Available, 2 = Not available, 0 = Any", "Choose bathroom preference: ", 0, 2) : 0;
    int marketChoice = wants(8)
        ? readChoice("Nearby market: 1 = Required, 2 = Not required, 0 = Any", "Choose market preference: ", 0, 2) : 0;
    int independentChoice = wants(9)
        ? readChoice("Arrangement: 1 = Independent, 2 = Shared/dependent, 0 = Any", "Choose arrangement: ", 0, 2) : 0;
    double minSize = wants(10) ? readDouble("Minimum room size in square feet: ", 1, 100000) : 0;
    double maxDistance = wants(11) ? readDouble("Maximum distance from university in km: ", 0.1, 10000) : 0;
    bool includeUnavailable = wants(13) && readYesNo("Include unavailable listings too?");
    vector<Accommodation*> results; // STL: temporary list of matches
    vector<Accommodation*> all = listings.getAllPointers();
    for (Accommodation* p : all) {
        // Available listings are shown by default. Option 13 can include unavailable ones.
        if (!includeUnavailable && !p->isAvailable()) continue;
        if (!keyword.empty() &&
            lowerCase(p->getPropertyName()).find(keyword) == string::npos &&
            lowerCase(p->getLocation()).find(keyword) == string::npos)
            continue;
        if (maxRent > 0 && p->getRent() > maxRent) continue;
        if (typeChoice != 0 && p->getType() != TYPE_NAMES[typeChoice]) continue;
        if (bedsChoice > 0 && p->getBeds() != bedsChoice) continue;
        if (minSize > 0 && p->getRoomSize() < minSize) continue;
        if (failsTriState(messChoice, p->hasMess())) continue;
        if (failsTriState(laundryChoice, p->hasLaundry())) continue;
        if (failsTriState(bathroomChoice, p->hasBathroom())) continue;
        if (failsTriState(marketChoice, p->hasMarket())) continue;
        if (failsTriState(independentChoice, p->isIndependent())) continue;
        if (maxDistance > 0 && p->getDistanceKm() > maxDistance) continue;
        results.push_back(p);
    }
    if (results.empty()) {
        cout << "No listings match the selected conditions. Try removing one or more filters.\n";
        return;
    }
    if (wants(12)) {
        int sortChoice = readChoice(
            "Sort by rent: 1 = Lowest to highest, 2 = Highest to lowest, 0 = No sorting",
            "Choose sorting: ", 0, 2);
        if (sortChoice != 0) {
            sort(results.begin(), results.end(),
                 [sortChoice](const Accommodation* a, const Accommodation* b) {
                     return sortChoice == 1 ? a->getRent() < b->getRent()
                                            : a->getRent() > b->getRent();
                 });
        }
    }
    cout << "\nMatching listings: " << results.size() << '\n';
    for (const Accommodation* p : results)
        p->display();
}

void studentMenu(AccommodationList& listings, RecentStack& recent, ContactQueue& inquiries) {
    while (true) {
        cout << "\n========== STUDENT MENU ==========\n"
             << "1. View all accommodations\n"
             << "2. Search and filter\n"
             << "3. View property details by ID\n"
             << "4. Recently viewed properties\n"
             << "5. Submit contact inquiry\n"
             << "6. Return to main menu\n";
        int choice = readInt("Choose an option: ", 1, 6);
        if (choice == 1)
            listings.showAll();
        else if (choice == 2)
            searchListings(listings);
        else if (choice == 3) {
            int id = readInt("Enter property ID: ", 1, 1000000000);
            Accommodation* property = listings.findById(id);
            if (property == nullptr)
                cout << "No property exists with that ID.\n";
            else {
                property->display();
                recent.push(id);
            }
        }
        else if (choice == 4)
            recent.show(listings);
        else if (choice == 5) {
            int id = readInt("Enter property ID to contact: ", 1, 1000000000);
            Accommodation* property = listings.findById(id);
            if (property == nullptr)
                cout << "No property exists with that ID. Inquiry not created.\n";
            else {
                string studentName = readNonEmptyLine("Your name: ");
                string contact = readNonEmptyLine("Your contact information: ");
                string message = readNonEmptyLine("Message to owner: ");
                inquiries.enqueue(id, property->getOwnerId(), studentName, contact, message);
            }
        }
        else
            return;
    }
}

