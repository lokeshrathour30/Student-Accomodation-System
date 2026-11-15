// owner module: owner accounts, registration, owner menu (add / view / remove listings, inquiries).
#pragma once
#include "ds.cpp"

struct OwnerAccount {
    int id;
    string name;
    string contact;
};

// Defined in file_handling.cpp (that module also needs this one, so they are declared here).
Accommodation* makeAccommodation(
    const string& type, int id, int ownerId,
    const string& ownerName, const string& ownerInfo,
    const string& name, const string& location,
    double rent, double size, int beds,
    bool mess, bool laundry, bool market, bool bathroom,
    bool independent, bool available, double distance);
void saveListings(const AccommodationList& listings);
void saveOwnerAccounts();

// STL: vector stores the demo accounts and newly registered owners.
vector<OwnerAccount> ownerAccounts = {
    {101, "Aarav Sharma", "Demo contact: 9000000101"},
    {102, "Meera Joshi", "Demo contact: 9000000102"},
    {103, "Rohan Negi", "Demo contact: 9000000103"}
};

const OwnerAccount* findOwner(int ownerId) {
    for (const OwnerAccount& owner : ownerAccounts)
        if (owner.id == ownerId)
            return &owner;
    return nullptr;
}


int registerNewOwner() {
    cout << "\n========== NEW OWNER REGISTRATION ==========\n";
    string name = readNonEmptyLine("Enter your full name: ");
    string contact = readNonEmptyLine("Enter your contact information (phone/email): ");
    int largestId = 100;
    for (const OwnerAccount& owner : ownerAccounts)
        largestId = max(largestId, owner.id);
    int newId = largestId + 1;
    ownerAccounts.push_back({newId, name, contact});
    cout << "\nRegistration successful!\n"
         << "Your new owner ID is: " << newId << '\n'
         << "Keep this ID safe; you will need it to log in next time.\n";
    saveOwnerAccounts();
    return newId;
}

void addAccommodation(AccommodationList& listings, int ownerId) {
    const OwnerAccount* owner = findOwner(ownerId);
    if (owner == nullptr) {
        cout << "Unknown demo owner account.\n";
        return;
    }
    string name = readNonEmptyLine("Property name: ");
    string location = readNonEmptyLine("Location: ");
    int typeChoice = readChoice("Type: 1 = PG, 2 = Flat, 3 = Hostel, 4 = Room", "Choose type: ", 1, 4);
    double rent = readDouble("Monthly rent (Rs.): ", 0, 10000000);
    double size = readDouble("Room size in sq. ft.: ", 1, 100000);
    int beds = readInt("Number of beds/occupants: ", 1, 100);
    bool mess = readYesNo("Mess available?");
    bool laundry = readYesNo("Laundry available?");
    bool market = readYesNo("Market nearby?");
    bool bathroom = readYesNo("Bathroom available?");
    bool independent = readYesNo("Is it independent accommodation?");
    bool available = readYesNo("Is it currently listed as available?");
    double distance = readDouble("Distance from university (km): ", 0, 10000);
    int id = listings.nextId();
    Accommodation* property = makeAccommodation(
        TYPE_NAMES[typeChoice], id, owner->id, owner->name, owner->contact,
        name, location, rent, size, beds, mess, laundry, market,
        bathroom, independent, available, distance);
    if (property == nullptr) {
        cout << "Could not create this property.\n";
        return;
    }
    listings.add(property);
    cout << "Property added with ID " << id << ".\n";
    saveListings(listings);
}


void ownerMenu(AccommodationList& listings, ContactQueue& inquiries, int ownerId) {
    const OwnerAccount* owner = findOwner(ownerId);
    if (owner == nullptr) {
        cout << "Login failed: unknown demo owner ID.\n";
        return;
    }
    cout << "Logged in as " << owner->name << ".\n";
    while (true) {
        cout << "\n========== OWNER MENU ==========\n"
             << "1. View my listings\n"
             << "2. Add an accommodation\n"
             << "3. View my pending inquiries\n"
             << "4. Process my next inquiry\n"
             << "5. Remove one of my listings\n"
             << "6. Return to main menu\n";
        int choice = readInt("Choose an option: ", 1, 6);
        if (choice == 1)
            listings.showForOwner(ownerId);
        else if (choice == 2)
            addAccommodation(listings, ownerId);
        else if (choice == 3)
            inquiries.showForOwner(ownerId);
        else if (choice == 4)
            inquiries.processNextForOwner(ownerId);
        else if (choice == 5) {
            int id = readInt("Enter the ID of your property to remove: ", 1, 1000000000);
            Accommodation* property = listings.findById(id);
            if (property == nullptr)
                cout << "Property not found.\n";
            else if (property->getOwnerId() != ownerId)
                cout << "Access denied: you can remove only your own listings.\n";
            else if (readYesNo("Confirm removal?")) {
                listings.removeById(id);
                saveListings(listings);
                cout << "Property removed.\n";
            }
        }
        else
            return;
    }
}

