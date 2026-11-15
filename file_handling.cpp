// file handling module: implementation.
#pragma once
#include "ds.cpp"
#include "owner.cpp"   // OwnerAccount, ownerAccounts, findOwner
#include <fstream>      // STL: CSV file handling
#include <sstream>      // STL: converting text to numbers
#include <cmath>        // isfinite

const string DATA_FILE = "accommodations.csv";
const string OWNER_FILE = "owners.csv";

// --------------------------- CSV HELPERS ---------------------------

// CSV writer: surrounds fields with quotes and doubles any quote inside.
string csvEscape(const string& field) {
    string result = "\"";
    for (char ch : field) {
        if (ch == '"') result += "\"\"";
        else result += ch;
    }
    return result + '"';
}

// CSV reader: supports quoted commas and doubled quotes.
bool parseCsvLine(const string& line, vector<string>& fields) {
    fields.clear();
    string field;
    bool insideQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char ch = line[i];
        if (ch == '"') {
            if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                i++;
            }
            else
                insideQuotes = !insideQuotes;
        }
        else if (ch == ',' && !insideQuotes) {
            fields.push_back(field);
            field.clear();
        }
        else
            field += ch;
    }
    if (insideQuotes) return false;
    fields.push_back(field);
    return true;
}

bool parseIntField(const string& text, int& value) {
    istringstream input(text); // STL: string stream converts text to a number
    char extra;
    return (input >> value) && !(input >> extra);
}

bool parseDoubleField(const string& text, double& value) {
    istringstream input(text);
    char extra;
    if (!(input >> value) || (input >> extra)) return false;
    return isfinite(value);
}

bool parseBoolField(const string& text, bool& value) {
    if (text == "1") { value = true; return true; }
    if (text == "0") { value = false; return true; }
    return false;
}

Accommodation* makeAccommodation(
    const string& type, int id, int ownerId,
    const string& ownerName, const string& ownerInfo,
    const string& name, const string& location,
    double rent, double size, int beds,
    bool mess, bool laundry, bool market, bool bathroom,
    bool independent, bool available, double distance) {
    if (type == "PG")
        return new PG(id, ownerId, ownerName, ownerInfo, name, location, rent, size, beds,
                      mess, laundry, market, bathroom, independent, available, distance);
    if (type == "Flat")
        return new Flat(id, ownerId, ownerName, ownerInfo, name, location, rent, size, beds,
                        mess, laundry, market, bathroom, independent, available, distance);
    if (type == "Hostel")
        return new Hostel(id, ownerId, ownerName, ownerInfo, name, location, rent, size, beds,
                          mess, laundry, market, bathroom, independent, available, distance);
    if (type == "Room")
        return new RentRoom(id, ownerId, ownerName, ownerInfo, name, location, rent, size, beds,
                            mess, laundry, market, bathroom, independent, available, distance);
    return nullptr;
}

void saveListings(const AccommodationList& listings) {
    ofstream file(DATA_FILE);
    if (!file) {
        cout << "ERROR: Could not open " << DATA_FILE << " for saving. Changes may not persist.\n";
        return;
    }
    file << "id,ownerId,ownerName,ownerInfo,propertyName,location,type,rent,"
            "roomSize,beds,mess,laundry,marketNearby,bathroom,independent,"
            "available,distanceKm\n";
    vector<Accommodation*> all = listings.getAllPointers(); // STL: vector snapshot
    for (const Accommodation* p : all) {
        file << p->getId() << ',' << p->getOwnerId() << ','
             << csvEscape(p->getOwnerName()) << ',' << csvEscape(p->getOwnerInfo()) << ','
             << csvEscape(p->getPropertyName()) << ',' << csvEscape(p->getLocation()) << ','
             << csvEscape(p->getType()) << ','
             << p->getRent() << ',' << p->getRoomSize() << ',' << p->getBeds() << ','
             << p->hasMess() << ',' << p->hasLaundry() << ',' << p->hasMarket() << ','
             << p->hasBathroom() << ',' << p->isIndependent() << ',' << p->isAvailable() << ','
             << p->getDistanceKm() << '\n';
    }
    file.close();
    if (file) cout << "Listings saved to " << DATA_FILE << ".\n";
    else cout << "ERROR: A problem occurred while saving " << DATA_FILE << ".\n";
}

bool loadListings(AccommodationList& listings) {
    ifstream file(DATA_FILE);
    if (!file) return false;
    string line;
    vector<string> fields; // STL: vector stores the parsed CSV columns
    bool firstLine = true;
    int loaded = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (firstLine) {
            firstLine = false;
            if (line.find("id,ownerId,ownerName") == 0)
                continue; // skip header
        }
        if (!parseCsvLine(line, fields) || fields.size() != 17) {
            cout << "Skipping malformed CSV record.\n";
            continue;
        }
        int id, ownerId, beds;
        double rent, size, distance;
        bool mess, laundry, market, bathroom, independent, available;
        bool valid =
            parseIntField(fields[0], id) && parseIntField(fields[1], ownerId) &&
            parseDoubleField(fields[7], rent) && parseDoubleField(fields[8], size) &&
            parseIntField(fields[9], beds) &&
            parseBoolField(fields[10], mess) && parseBoolField(fields[11], laundry) &&
            parseBoolField(fields[12], market) && parseBoolField(fields[13], bathroom) &&
            parseBoolField(fields[14], independent) && parseBoolField(fields[15], available) &&
            parseDoubleField(fields[16], distance);
        if (!valid || id <= 0 || ownerId <= 0 || rent < 0 || size <= 0 || beds < 1 ||
            distance < 0 || fields[2].empty() || fields[4].empty() || fields[5].empty()) {
            cout << "Skipping invalid CSV record.\n";
            continue;
        }
        if (listings.containsId(id)) {
            cout << "Skipping duplicate property ID " << id << ".\n";
            continue;
        }
        Accommodation* property = makeAccommodation(
            fields[6], id, ownerId, fields[2], fields[3], fields[4], fields[5],
            rent, size, beds, mess, laundry, market, bathroom, independent, available, distance);
        if (property == nullptr) {
            cout << "Skipping record with unknown accommodation type.\n";
            continue;
        }
        listings.add(property);
        loaded++;
    }
    return loaded > 0;
}

void addSampleListings(AccommodationList& listings) {
    // Fictional demonstration data; these are not real advertisements.
    const string a = "Demo contact: 9000000101", m = "Demo contact: 9000000102",
                 r = "Demo contact: 9000000103";
    listings.add(new PG(1001, 101, "Aarav Sharma", a, "Green View PG",
        "Prem Nagar, Dehradun", 7500, 180, 1, true, true, true, true, false, true, 3.2));
    listings.add(new Flat(1002, 102, "Meera Joshi", m, "Student Corner Flat",
        "Clement Town, Dehradun", 12000, 620, 2, false, true, true, true, true, true, 5.5));
    listings.add(new Hostel(1003, 103, "Rohan Negi", r, "Himalayan Student Hostel",
        "Sahastradhara Road, Dehradun", 6000, 150, 2, true, false, true, true, false, true, 4.1));
    listings.add(new RentRoom(1004, 101, "Aarav Sharma", a, "Quiet Study Room",
        "Ballupur, Dehradun", 5000, 140, 1, false, false, true, true, false, true, 2.4));
    listings.add(new PG(1005, 102, "Meera Joshi", m, "Independent Studio PG",
        "Rajpur Road, Dehradun", 9500, 300, 1, true, true, true, true, true, true, 6.0));
    listings.add(new PG(1006, 101, "Aarav Sharma", a, "Campus Nest PG",
        "Sudhowala, Dehradun", 6800, 200, 1, true, false, true, true, false, true, 4.8));
    listings.add(new Flat(1007, 102, "Meera Joshi", m, "Doon Scholars Flat",
        "Kargi Chowk, Dehradun", 14500, 780, 3, false, true, true, true, true, true, 7.2));
    listings.add(new Hostel(1008, 103, "Rohan Negi", r, "Valley View Hostel",
        "Prem Nagar, Dehradun", 5500, 160, 3, true, true, true, true, false, true, 3.6));
    listings.add(new RentRoom(1009, 101, "Aarav Sharma", a, "Sunrise Single Room",
        "Nehru Colony, Dehradun", 4200, 125, 1, false, false, true, true, false, true, 6.4));
    listings.add(new Flat(1010, 102, "Meera Joshi", m, "Oakwood Independent Flat",
        "Vijay Park, Dehradun", 11000, 540, 2, false, true, true, true, true, true, 3.9));
}

void saveOwnerAccounts() {
    ofstream file(OWNER_FILE);
    if (!file) {
        cout << "ERROR: Could not save owner accounts to " << OWNER_FILE << ".\n";
        return;
    }
    file << "ownerId,name,contact\n";
    for (const OwnerAccount& owner : ownerAccounts)
        file << owner.id << ',' << csvEscape(owner.name) << ',' << csvEscape(owner.contact) << '\n';
    file.close();
    if (file) cout << "Owner accounts saved to " << OWNER_FILE << ".\n";
    else cout << "ERROR: A problem occurred while saving " << OWNER_FILE << ".\n";
}

void loadOwnerAccounts() {
    ifstream file(OWNER_FILE);
    // First run: keep the built-in demo accounts and create owners.csv.
    if (!file) {
        saveOwnerAccounts();
        return;
    }
    string line;
    vector<string> fields; // STL: parsed columns from owners.csv
    int loadedNewOwners = 0;
    bool firstLine = true;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (firstLine) {
            firstLine = false;
            if (line.find("ownerId,name,contact") == 0)
                continue;
        }
        if (!parseCsvLine(line, fields) || fields.size() != 3) {
            cout << "Skipping malformed owner record.\n";
            continue;
        }
        int id;
        if (!parseIntField(fields[0], id) || id <= 0 || fields[1].empty() || fields[2].empty()) {
            cout << "Skipping invalid owner record.\n";
            continue;
        }
        if (findOwner(id) == nullptr) {
            ownerAccounts.push_back({id, fields[1], fields[2]});
            loadedNewOwners++;
        }
    }
    cout << "Loaded " << loadedNewOwners << " additional owner account(s) from " << OWNER_FILE << ".\n";
    // Ensure the file contains the demo accounts and all registered owners.
    saveOwnerAccounts();
}

