// Entry point: wires the four modules together.
#include "ds.cpp"
#include "file_handling.cpp"
#include "owner.cpp"
#include "student.cpp"

int main() {
    AccommodationList listings;
    RecentStack recent;
    ContactQueue inquiries;
    cout << "============================================\n"
         << "     STUDENT ACCOMMODATION SYSTEM\n"
         << "============================================\n";
    loadOwnerAccounts(); // Load registered owners before owner login.
    if (!loadListings(listings)) {
        cout << (listings.isEmpty() ? "No usable saved listings found. Creating sample data.\n"
                                    : "No usable CSV records were loaded; using sample listings.\n");
        // If the file contained only invalid rows, discard them before samples.
        while (!listings.isEmpty())
            listings.removeById(listings.getAllPointers().back()->getId());
        addSampleListings(listings);
        saveListings(listings);
    }
    else
        cout << "Loaded " << listings.size() << " saved listing(s).\n";
    while (true) {
        cout << "\n============= MAIN MENU =============\n"
             << "1. Student\n"
             << "2. Owner/Admin login\n"
             << "3. Exit\n";
        int choice = readInt("Choose an option: ", 1, 3);
        if (choice == 1)
            studentMenu(listings, recent, inquiries);
        else if (choice == 2) {
            cout << "\n========== OWNER ACCESS ==========\n"
                 << "1. Log in as an existing owner\n"
                 << "2. Register as a new owner\n"
                 << "3. Return to main menu\n";
            int ownerChoice = readInt("Choose an option: ", 1, 3);
            if (ownerChoice == 1)
                ownerMenu(listings, inquiries, readInt("Enter your owner ID: ", 1, 1000000000));
            else if (ownerChoice == 2)
                ownerMenu(listings, inquiries, registerNewOwner());
        }
        else {
            saveListings(listings);
            cout << "Thank you for using the Student Accommodation System.\n";
            break;
        }
    }
    return 0;
}
