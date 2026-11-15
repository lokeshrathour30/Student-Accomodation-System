#include <iostream>
#include <string>
#include <vector>       // STL: temporary search results and CSV fields
#include <fstream>      // STL: CSV file handling
#include <sstream>      // STL: converting numbers and parsing input
#include <iomanip>      // STL: readable decimal output
#include <limits>       // STL: clearing invalid input
#include <algorithm>    // STL: sorting search results
#include <cmath>        // STL: checking decimal values
#include <cctype>       // STL: safe character conversion
#include <cstdlib>      // exit() when terminal input reaches EOF

using namespace std;    // STL: allows standard-library names without std::

const string DATA_FILE = "accommodations.csv";

// --------------------------- INPUT HELPERS ---------------------------

void clearInputLine()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readNonEmptyLine(const string& prompt)
{
    string value;

    while (true)
    {
        cout << prompt;
        if (!getline(cin, value))
        {
            cout << "\nInput stream closed. Exiting safely.\n";
            exit(0);
        }

        if (!value.empty())
            return value;

        cout << "Input cannot be empty. Please try again.\n";
    }
}

int readInt(const string& prompt, int minimum, int maximum)
{
    int value;

    while (true)
    {
        cout << prompt;

        if (cin >> value && value >= minimum && value <= maximum)
        {
            clearInputLine();
            return value;
        }

        cout << "Enter a whole number from " << minimum
             << " to " << maximum << ".\n";
        if (cin.eof())
        {
            cout << "Input stream closed. Exiting safely.\n";
            exit(0);
        }
        clearInputLine();
    }
}

double readDouble(const string& prompt, double minimum, double maximum)
{
    double value;

    while (true)
    {
        cout << prompt;

        if (cin >> value && isfinite(value) &&
            value >= minimum && value <= maximum)
        {
            clearInputLine();
            return value;
        }

        cout << "Enter a valid number from " << minimum
             << " to " << maximum << ".\n";
        if (cin.eof())
        {
            cout << "Input stream closed. Exiting safely.\n";
            exit(0);
        }
        clearInputLine();
    }
}

bool readYesNo(const string& prompt)
{
    int choice = readInt(prompt + " (1 = Yes, 0 = No): ", 0, 1);
    return choice == 1;
}

string lowerCase(string text)
{
    // STL: string is used for text; tolower normalizes type names.
    for (char& ch : text)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));

    return text;
}

// --------------------------- ACCOMMODATION CLASSES ---------------------------

class Accommodation
{
private:
    int id;
    int ownerId;
    string ownerName;
    string ownerInfo;
    string propertyName;
    string location;
    double monthlyRent;
    double roomSize;
    int beds;
    bool mess;
    bool laundry;
    bool marketNearby;
    bool bathroom;
    bool independent;
    bool available;
    double distanceKm;

public:
    Accommodation(int propertyId, int propertyOwnerId,
                  const string& propertyOwnerName,
                  const string& propertyOwnerInfo,
                  const string& name, const string& place,
                  double rent, double size, int bedCount,
                  bool hasMess, bool hasLaundry, bool hasMarket,
                  bool hasBathroom, bool isIndependent,
                  bool isAvailable, double distance)
        : id(propertyId), ownerId(propertyOwnerId),
          ownerName(propertyOwnerName), ownerInfo(propertyOwnerInfo),
          propertyName(name), location(place), monthlyRent(rent),
          roomSize(size), beds(bedCount), mess(hasMess),
          laundry(hasLaundry), marketNearby(hasMarket),
          bathroom(hasBathroom), independent(isIndependent),
          available(isAvailable), distanceKm(distance)
    {
    }

    virtual ~Accommodation() = default; // virtual: safe deletion through base pointer

    int getId() const { return id; }
    int getOwnerId() const { return ownerId; }
    const string& getOwnerName() const { return ownerName; }
    const string& getOwnerInfo() const { return ownerInfo; }
    const string& getPropertyName() const { return propertyName; }
    const string& getLocation() const { return location; }
    double getRent() const { return monthlyRent; }
    double getRoomSize() const { return roomSize; }
    int getBeds() const { return beds; }
    bool hasMess() const { return mess; }
    bool hasLaundry() const { return laundry; }
    bool hasMarket() const { return marketNearby; }
    bool hasBathroom() const { return bathroom; }
    bool isIndependent() const { return independent; }
    bool isAvailable() const { return available; }
    double getDistanceKm() const { return distanceKm; }

    virtual string getType() const = 0; // pure virtual: each derived class supplies its type

    virtual void display() const
    {
        cout << "\n----------------------------------------\n";
        cout << "Property ID       : " << id << '\n';
        cout << "Property name     : " << propertyName << '\n';
        cout << "Type              : " << getType() << '\n';
        cout << "Location          : " << location << '\n';
        cout << "Monthly rent      : Rs. " << fixed << setprecision(2)
             << monthlyRent << '\n';
        cout << "Room size         : " << roomSize << " sq. ft.\n";
        cout << "Beds / occupancy  : " << beds << '\n';
        cout << "Mess              : " << (mess ? "Yes" : "No") << '\n';
        cout << "Laundry           : " << (laundry ? "Yes" : "No") << '\n';
        cout << "Market nearby     : " << (marketNearby ? "Yes" : "No") << '\n';
        cout << "Bathroom          : " << (bathroom ? "Available" : "Not available")
             << '\n';
        cout << "Accommodation     : "
             << (independent ? "Independent" : "Dependent/shared") << '\n';
        cout << "Distance to uni.  : " << distanceKm << " km\n";
        cout << "Availability      : " << (available ? "Listed as available" : "Unavailable")
             << '\n';
        cout << "Owner             : " << ownerName << " (ID " << ownerId << ")\n";
        cout << "Owner information : " << ownerInfo << '\n';
        cout << "----------------------------------------\n";
    }
};

class PG : public Accommodation
{
public:
    using Accommodation::Accommodation;
    string getType() const override { return "PG"; }
};

class Flat : public Accommodation
{
public:
    using Accommodation::Accommodation;
    string getType() const override { return "Flat"; }
};

class Hostel : public Accommodation
{
public:
    using Accommodation::Accommodation;
    string getType() const override { return "Hostel"; }
};

class RentRoom : public Accommodation
{
public:
    using Accommodation::Accommodation;
    string getType() const override { return "Room"; }
};

// --------------------------- CUSTOM LINKED LIST ---------------------------

class AccommodationList
{
private:
    struct Node
    {
        Accommodation* property;
        Node* next;

        Node(Accommodation* item) : property(item), next(nullptr) {}
    };

    Node* head;
    int count;

public:
    AccommodationList() : head(nullptr), count(0) {}

    ~AccommodationList()
    {
        Node* current = head;

        while (current != nullptr)
        {
            Node* nextNode = current->next;
            delete current->property; // deletes derived object safely via virtual destructor
            delete current;
            current = nextNode;
        }
    }

    bool isEmpty() const { return head == nullptr; }
    int size() const { return count; }

    bool containsId(int propertyId) const
    {
        return findById(propertyId) != nullptr;
    }

    Accommodation* findById(int propertyId) const
    {
        Node* current = head;

        while (current != nullptr)
        {
            if (current->property->getId() == propertyId)
                return current->property;

            current = current->next;
        }

        return nullptr;
    }

    void add(Accommodation* property)
    {
        if (property == nullptr || containsId(property->getId()))
        {
            delete property;
            return;
        }

        Node* newNode = new Node(property);

        if (head == nullptr)
            head = newNode;
        else
        {
            Node* current = head;
            while (current->next != nullptr)
                current = current->next;

            current->next = newNode;
        }

        count++;
    }

    bool removeById(int propertyId)
    {
        Node* current = head;
        Node* previous = nullptr;

        while (current != nullptr)
        {
            if (current->property->getId() == propertyId)
            {
                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                delete current->property;
                delete current;
                count--;
                return true;
            }

            previous = current;
            current = current->next;
        }

        return false;
    }

    void showAll() const
    {
        if (isEmpty())
        {
            cout << "No accommodation listings are present.\n";
            return;
        }

        Node* current = head;
        while (current != nullptr)
        {
            current->property->display(); // runtime polymorphism
            current = current->next;
        }
    }

    void showForOwner(int ownerId) const
    {
        bool found = false;
        Node* current = head;

        while (current != nullptr)
        {
            if (current->property->getOwnerId() == ownerId)
            {
                current->property->display();
                found = true;
            }

            current = current->next;
        }

        if (!found)
            cout << "This owner has no listings.\n";
    }

    vector<Accommodation*> getAllPointers() const
    {
        vector<Accommodation*> result; // STL: vector stores temporary search results
        Node* current = head;

        while (current != nullptr)
        {
            result.push_back(current->property);
            current = current->next;
        }

        return result;
    }

    int nextId() const
    {
        int largest = 1000;
        Node* current = head;

        while (current != nullptr)
        {
            if (current->property->getId() > largest)
                largest = current->property->getId();

            current = current->next;
        }

        return largest + 1;
    }
};

// --------------------------- CUSTOM STACK: RECENTLY VIEWED ---------------------------

class RecentStack
{
private:
    struct StackNode
    {
        int propertyId;
        StackNode* next;

        StackNode(int id) : propertyId(id), next(nullptr) {}
    };

    StackNode* topNode;

public:
    RecentStack() : topNode(nullptr) {}

    ~RecentStack()
    {
        while (topNode != nullptr)
        {
            StackNode* oldTop = topNode;
            topNode = topNode->next;
            delete oldTop;
        }
    }

    bool isEmpty() const { return topNode == nullptr; }

    void push(int propertyId)
    {
        StackNode* newNode = new StackNode(propertyId);
        newNode->next = topNode;
        topNode = newNode;
    }

    void show(const AccommodationList& listings) const
    {
        if (isEmpty())
        {
            cout << "You have not viewed any properties in this session.\n";
            return;
        }

        cout << "\nRecently viewed (latest first):\n";
        StackNode* current = topNode;

        while (current != nullptr)
        {
            Accommodation* property = listings.findById(current->propertyId);

            if (property != nullptr)
                property->display();

            current = current->next;
        }
    }
};

// --------------------------- CUSTOM QUEUE: CONTACT INQUIRIES ---------------------------

class ContactQueue
{
private:
    struct Request
    {
        int requestId;
        int propertyId;
        int ownerId;
        string studentName;
        string studentContact;
        string message;
        Request* next;

        Request(int reqId, int propId, int ownId,
                const string& name, const string& contact,
                const string& msg)
            : requestId(reqId), propertyId(propId), ownerId(ownId),
              studentName(name), studentContact(contact), message(msg),
              next(nullptr) {}
    };

    Request* frontNode;
    Request* rearNode;
    int nextRequestId;

    void deleteRequest(Request* request)
    {
        delete request;
    }

public:
    ContactQueue() : frontNode(nullptr), rearNode(nullptr), nextRequestId(1) {}

    ~ContactQueue()
    {
        while (frontNode != nullptr)
        {
            Request* oldFront = frontNode;
            frontNode = frontNode->next;
            deleteRequest(oldFront);
        }
        rearNode = nullptr;
    }

    bool isEmpty() const { return frontNode == nullptr; }

    void enqueue(int propertyId, int ownerId, const string& studentName,
                 const string& studentContact, const string& message)
    {
        Request* newRequest = new Request(nextRequestId++, propertyId, ownerId,
                                          studentName, studentContact, message);

        if (rearNode == nullptr)
            frontNode = rearNode = newRequest;
        else
        {
            rearNode->next = newRequest;
            rearNode = newRequest;
        }

        cout << "Inquiry submitted. Request ID: "
             << newRequest->requestId << '\n';
    }

    void showForOwner(int ownerId) const
    {
        bool found = false;
        Request* current = frontNode;

        while (current != nullptr)
        {
            if (current->ownerId == ownerId)
            {
                cout << "\nRequest ID: " << current->requestId
                     << "\nProperty ID: " << current->propertyId
                     << "\nStudent: " << current->studentName
                     << "\nContact: " << current->studentContact
                     << "\nMessage: " << current->message << "\n";
                found = true;
            }

            current = current->next;
        }

        if (!found)
            cout << "No pending inquiries for your properties.\n";
    }

    bool processNextForOwner(int ownerId)
    {
        Request* current = frontNode;
        Request* previous = nullptr;

        // Find the earliest queued request belonging to this owner.
        while (current != nullptr && current->ownerId != ownerId)
        {
            previous = current;
            current = current->next;
        }

        if (current == nullptr)
        {
            cout << "No pending inquiries for your properties.\n";
            return false;
        }

        cout << "\nProcessing inquiry " << current->requestId
             << " for property " << current->propertyId << ".\n";
        cout << "Student: " << current->studentName
             << "\nContact: " << current->studentContact
             << "\nMessage: " << current->message << '\n';

        if (previous == nullptr)
            frontNode = current->next;
        else
            previous->next = current->next;

        if (current == rearNode)
            rearNode = previous;

        deleteRequest(current);
        cout << "Inquiry marked as processed.\n";
        return true;
    }
};

// --------------------------- CSV HELPERS ---------------------------

// CSV writer: surrounds fields with quotes and doubles any quote inside.
string csvEscape(const string& field)
{
    string result = "\"";

    for (char ch : field)
    {
        if (ch == '"')
            result += "\"\"";
        else
            result += ch;
    }

    result += '"';
    return result;
}

// CSV reader: supports quoted commas and doubled quotes.
bool parseCsvLine(const string& line, vector<string>& fields)
{
    fields.clear();
    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); i++)
    {
        char ch = line[i];

        if (ch == '"')
        {
            if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                field += '"';
                i++;
            }
            else
                insideQuotes = !insideQuotes;
        }
        else if (ch == ',' && !insideQuotes)
        {
            fields.push_back(field);
            field.clear();
        }
        else
            field += ch;
    }

    if (insideQuotes)
        return false;

    fields.push_back(field);
    return true;
}

bool parseIntField(const string& text, int& value)
{
    istringstream input(text); // STL: string stream converts text to a number
    char extra;
    return (input >> value) && !(input >> extra);
}

bool parseDoubleField(const string& text, double& value)
{
    istringstream input(text);
    char extra;

    if (!(input >> value) || (input >> extra))
        return false;

    return isfinite(value);
}

bool parseBoolField(const string& text, bool& value)
{
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
    bool independent, bool available, double distance)
{
    if (type == "PG")
        return new PG(id, ownerId, ownerName, ownerInfo, name, location,
                      rent, size, beds, mess, laundry, market, bathroom,
                      independent, available, distance);

    if (type == "Flat")
        return new Flat(id, ownerId, ownerName, ownerInfo, name, location,
                        rent, size, beds, mess, laundry, market, bathroom,
                        independent, available, distance);

    if (type == "Hostel")
        return new Hostel(id, ownerId, ownerName, ownerInfo, name, location,
                          rent, size, beds, mess, laundry, market, bathroom,
                          independent, available, distance);

    if (type == "Room")
        return new RentRoom(id, ownerId, ownerName, ownerInfo, name, location,
                            rent, size, beds, mess, laundry, market, bathroom,
                            independent, available, distance);

    return nullptr;
}

void saveListings(const AccommodationList& listings)
{
    ofstream file(DATA_FILE);

    if (!file)
    {
        cout << "ERROR: Could not open " << DATA_FILE
             << " for saving. Changes may not persist.\n";
        return;
    }

    file << "id,ownerId,ownerName,ownerInfo,propertyName,location,type,rent,"
            "roomSize,beds,mess,laundry,marketNearby,bathroom,independent,"
            "available,distanceKm\n";

    vector<Accommodation*> all = listings.getAllPointers(); // STL: vector snapshot

    for (const Accommodation* p : all)
    {
        file << p->getId() << ','
             << p->getOwnerId() << ','
             << csvEscape(p->getOwnerName()) << ','
             << csvEscape(p->getOwnerInfo()) << ','
             << csvEscape(p->getPropertyName()) << ','
             << csvEscape(p->getLocation()) << ','
             << csvEscape(p->getType()) << ','
             << p->getRent() << ','
             << p->getRoomSize() << ','
             << p->getBeds() << ','
             << (p->hasMess() ? 1 : 0) << ','
             << (p->hasLaundry() ? 1 : 0) << ','
             << (p->hasMarket() ? 1 : 0) << ','
             << (p->hasBathroom() ? 1 : 0) << ','
             << (p->isIndependent() ? 1 : 0) << ','
             << (p->isAvailable() ? 1 : 0) << ','
             << p->getDistanceKm() << '\n';
    }

    file.close();

    if (file)
        cout << "Listings saved to " << DATA_FILE << ".\n";
    else
        cout << "ERROR: A problem occurred while saving " << DATA_FILE << ".\n";
}

bool loadListings(AccommodationList& listings)
{
    ifstream file(DATA_FILE);

    if (!file)
        return false;

    string line;
    vector<string> fields; // STL: vector stores the parsed CSV columns
    bool firstLine = true;
    int loaded = 0;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        if (firstLine)
        {
            firstLine = false;
            if (line.find("id,ownerId,ownerName") == 0)
                continue; // skip header
        }

        if (!parseCsvLine(line, fields) || fields.size() != 17)
        {
            cout << "Skipping malformed CSV record.\n";
            continue;
        }

        int id, ownerId, beds;
        double rent, size, distance;
        bool mess, laundry, market, bathroom, independent, available;

        bool valid =
            parseIntField(fields[0], id) &&
            parseIntField(fields[1], ownerId) &&
            parseDoubleField(fields[7], rent) &&
            parseDoubleField(fields[8], size) &&
            parseIntField(fields[9], beds) &&
            parseBoolField(fields[10], mess) &&
            parseBoolField(fields[11], laundry) &&
            parseBoolField(fields[12], market) &&
            parseBoolField(fields[13], bathroom) &&
            parseBoolField(fields[14], independent) &&
            parseBoolField(fields[15], available) &&
            parseDoubleField(fields[16], distance);

        if (!valid || id <= 0 || ownerId <= 0 || rent < 0 ||
            size <= 0 || beds < 1 || distance < 0 ||
            fields[2].empty() || fields[4].empty() || fields[5].empty())
        {
            cout << "Skipping invalid CSV record.\n";
            continue;
        }

        if (listings.containsId(id))
        {
            cout << "Skipping duplicate property ID " << id << ".\n";
            continue;
        }

        Accommodation* property = makeAccommodation(
            fields[6], id, ownerId, fields[2], fields[3],
            fields[4], fields[5], rent, size, beds,
            mess, laundry, market, bathroom, independent, available, distance);

        if (property == nullptr)
        {
            cout << "Skipping record with unknown accommodation type.\n";
            continue;
        }

        listings.add(property);
        loaded++;
    }

    return loaded > 0;
}

void addSampleListings(AccommodationList& listings)
{
    // Fictional demonstration data; these are not real advertisements.
    listings.add(new PG(1001, 101, "Aarav Sharma", "Demo contact: 9000000101",
        "Green View PG", "Prem Nagar, Dehradun", 7500, 180, 1,
        true, true, true, true, false, true, 3.2));

    listings.add(new Flat(1002, 102, "Meera Joshi", "Demo contact: 9000000102",
        "Student Corner Flat", "Clement Town, Dehradun", 12000, 620, 2,
        false, true, true, true, true, true, 5.5));

    listings.add(new Hostel(1003, 103, "Rohan Negi", "Demo contact: 9000000103",
        "Himalayan Student Hostel", "Sahastradhara Road, Dehradun", 6000, 150, 2,
        true, false, true, true, false, true, 4.1));

    listings.add(new RentRoom(1004, 101, "Aarav Sharma", "Demo contact: 9000000101",
        "Quiet Study Room", "Ballupur, Dehradun", 5000, 140, 1,
        false, false, true, true, false, true, 2.4));

    listings.add(new PG(1005, 102, "Meera Joshi", "Demo contact: 9000000102",
        "Independent Studio PG", "Rajpur Road, Dehradun", 9500, 300, 1,
        true, true, true, true, true, true, 6.0));
}

// --------------------------- SEARCH AND FILTERING ---------------------------

void searchListings(const AccommodationList& listings)
{
    cout << "\n--- Search and Filter ---\n";
    double maxRent = readDouble("Maximum monthly rent (0 = no limit): ", 0, 10000000);

    cout << "Type: 0 = Any, 1 = PG, 2 = Flat, 3 = Hostel, 4 = Room\n";
    int typeChoice = readInt("Choose type: ", 0, 4);

    int minBeds = readInt("Minimum beds/occupancy (0 = any): ", 0, 100);
    double minSize = readDouble("Minimum room size in sq. ft. (0 = any): ", 0, 100000);

    cout << "Mess: 0 = Any, 1 = Required, 2 = Not required\n";
    int messChoice = readInt("Choose mess preference: ", 0, 2);
    cout << "Laundry: 0 = Any, 1 = Required, 2 = Not required\n";
    int laundryChoice = readInt("Choose laundry preference: ", 0, 2);
    cout << "Nearby market: 0 = Any, 1 = Required, 2 = Not required\n";
    int marketChoice = readInt("Choose market preference: ", 0, 2);
    cout << "Bathroom: 0 = Any, 1 = Available, 2 = Not available\n";
    int bathroomChoice = readInt("Choose bathroom preference: ", 0, 2);
    cout << "Accommodation: 0 = Any, 1 = Independent, 2 = Dependent/shared\n";
    int independentChoice = readInt("Choose accommodation preference: ", 0, 2);
    double maxDistance = readDouble("Maximum distance from university in km (0 = any): ",
                                    0, 10000);

    vector<Accommodation*> results; // STL: temporary list of matches
    vector<Accommodation*> all = listings.getAllPointers();

    for (Accommodation* p : all)
    {
        if (!p->isAvailable())
            continue;

        if (maxRent > 0 && p->getRent() > maxRent)
            continue;

        if (typeChoice != 0)
        {
            string wantedType;
            if (typeChoice == 1) wantedType = "PG";
            else if (typeChoice == 2) wantedType = "Flat";
            else if (typeChoice == 3) wantedType = "Hostel";
            else wantedType = "Room";

            if (p->getType() != wantedType)
                continue;
        }

        if (minBeds > 0 && p->getBeds() < minBeds)
            continue;
        if (minSize > 0 && p->getRoomSize() < minSize)
            continue;
        if (messChoice == 1 && !p->hasMess()) continue;
        if (messChoice == 2 && p->hasMess()) continue;
        if (laundryChoice == 1 && !p->hasLaundry()) continue;
        if (laundryChoice == 2 && p->hasLaundry()) continue;
        if (marketChoice == 1 && !p->hasMarket()) continue;
        if (marketChoice == 2 && p->hasMarket()) continue;
        if (bathroomChoice == 1 && !p->hasBathroom()) continue;
        if (bathroomChoice == 2 && p->hasBathroom()) continue;
        if (independentChoice == 1 && !p->isIndependent()) continue;
        if (independentChoice == 2 && p->isIndependent()) continue;
        if (maxDistance > 0 && p->getDistanceKm() > maxDistance) continue;

        results.push_back(p);
    }

    if (results.empty())
    {
        cout << "No listings match all the selected conditions.\n";
        return;
    }

    if (readYesNo("Sort matching results by rent, lowest first?"))
    {
        // STL: sort orders the vector using a comparison function.
        sort(results.begin(), results.end(),
             [](const Accommodation* a, const Accommodation* b)
             {
                 return a->getRent() < b->getRent();
             });
    }

    cout << "\nMatching listings: " << results.size() << '\n';
    for (const Accommodation* p : results)
        p->display();
}

// --------------------------- OWNER ACCOUNTS AND MENUS ---------------------------

struct OwnerAccount
{
    int id;
    string name;
    string contact;
};

const vector<OwnerAccount> DEMO_OWNERS = {
    {101, "Aarav Sharma", "Demo contact: 9000000101"},
    {102, "Meera Joshi", "Demo contact: 9000000102"},
    {103, "Rohan Negi", "Demo contact: 9000000103"}
}; // STL: vector stores the fixed demo accounts

const OwnerAccount* findOwner(int ownerId)
{
    for (const OwnerAccount& owner : DEMO_OWNERS)
    {
        if (owner.id == ownerId)
            return &owner;
    }
    return nullptr;
}

void addAccommodation(AccommodationList& listings, int ownerId)
{
    const OwnerAccount* owner = findOwner(ownerId);

    if (owner == nullptr)
    {
        cout << "Unknown demo owner account.\n";
        return;
    }

    string name = readNonEmptyLine("Property name: ");
    string location = readNonEmptyLine("Location: ");

    cout << "Type: 1 = PG, 2 = Flat, 3 = Hostel, 4 = Room\n";
    int typeChoice = readInt("Choose type: ", 1, 4);

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

    string type;
    if (typeChoice == 1) type = "PG";
    else if (typeChoice == 2) type = "Flat";
    else if (typeChoice == 3) type = "Hostel";
    else type = "Room";

    int id = listings.nextId();

    Accommodation* property = makeAccommodation(
        type, id, owner->id, owner->name, owner->contact,
        name, location, rent, size, beds, mess, laundry, market,
        bathroom, independent, available, distance);

    if (property == nullptr)
    {
        cout << "Could not create this property.\n";
        return;
    }

    listings.add(property);
    cout << "Property added with ID " << id << ".\n";
    saveListings(listings);
}

void studentMenu(AccommodationList& listings, RecentStack& recent,
                 ContactQueue& inquiries)
{
    while (true)
    {
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
        else if (choice == 3)
        {
            int id = readInt("Enter property ID: ", 1, 1000000000);
            Accommodation* property = listings.findById(id);

            if (property == nullptr)
                cout << "No property exists with that ID.\n";
            else
            {
                property->display();
                recent.push(id);
            }
        }
        else if (choice == 4)
            recent.show(listings);
        else if (choice == 5)
        {
            int id = readInt("Enter property ID to contact: ", 1, 1000000000);
            Accommodation* property = listings.findById(id);

            if (property == nullptr)
                cout << "No property exists with that ID. Inquiry not created.\n";
            else
            {
                string studentName = readNonEmptyLine("Your name: ");
                string contact = readNonEmptyLine("Your contact information: ");
                string message = readNonEmptyLine("Message to owner: ");

                inquiries.enqueue(id, property->getOwnerId(),
                                  studentName, contact, message);
            }
        }
        else
            return;
    }
}

void ownerMenu(AccommodationList& listings, ContactQueue& inquiries, int ownerId)
{
    const OwnerAccount* owner = findOwner(ownerId);

    if (owner == nullptr)
    {
        cout << "Login failed: unknown demo owner ID.\n";
        return;
    }

    cout << "Logged in as " << owner->name << ".\n";

    while (true)
    {
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
        else if (choice == 5)
        {
            int id = readInt("Enter the ID of your property to remove: ",
                             1, 1000000000);
            Accommodation* property = listings.findById(id);

            if (property == nullptr)
                cout << "Property not found.\n";
            else if (property->getOwnerId() != ownerId)
                cout << "Access denied: you can remove only your own listings.\n";
            else
            {
                if (readYesNo("Confirm removal?"))
                {
                    listings.removeById(id);
                    saveListings(listings);
                    cout << "Property removed.\n";
                }
            }
        }
        else
            return;
    }
}

// --------------------------- MAIN ---------------------------

int main()
{
    AccommodationList listings;
    RecentStack recent;
    ContactQueue inquiries;

    cout << "============================================\n"
         << "     STUDENT ACCOMMODATION SYSTEM\n"
         << "============================================\n"
         << "Academic demo only. Sample listings are fictional.\n"
         << "Availability is manually recorded, not real-time.\n";

    bool loaded = loadListings(listings);

    if (!loaded)
    {
        if (!listings.isEmpty())
        {
            cout << "No usable CSV records were loaded; using sample listings.\n";
        }
        else
        {
            cout << "No usable saved listings found. Creating sample data.\n";
        }

        // If the file contained only invalid rows, discard them before samples.
        while (!listings.isEmpty())
        {
            vector<Accommodation*> existing = listings.getAllPointers();
            listings.removeById(existing.back()->getId());
        }

        addSampleListings(listings);
        saveListings(listings);
    }
    else
        cout << "Loaded " << listings.size() << " saved listing(s).\n";

    while (true)
    {
        cout << "\n============= MAIN MENU =============\n"
             << "1. Student\n"
             << "2. Owner/Admin login\n"
             << "3. Exit\n";

        int choice = readInt("Choose an option: ", 1, 3);

        if (choice == 1)
            studentMenu(listings, recent, inquiries);
        else if (choice == 2)
        {
            cout << "\nDemo owner IDs: 101, 102, 103\n";
            int ownerId = readInt("Enter owner ID: ", 1, 1000000000);
            ownerMenu(listings, inquiries, ownerId);
        }
        else
        {
            saveListings(listings);
            cout << "Thank you for using the Student Accommodation System.\n";
            break;
        }
    }

    return 0;
}
