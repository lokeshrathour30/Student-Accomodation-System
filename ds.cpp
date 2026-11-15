// ds module: data structures (OOP classes, linked list, stack, queue) + shared input helpers.
#pragma once
#include <iostream>
#include <string>
#include <vector>       // STL: temporary search results and CSV fields
#include <iomanip>      // STL: readable decimal output
#include <algorithm>    // STL: max / sort / find
#include <limits>       // STL: clearing invalid input
#include <cmath>        // isfinite
#include <cctype>       // STL: safe character conversion
#include <cstdlib>      // exit() when terminal input reaches EOF
using namespace std;

const char* const TYPE_NAMES[] = {"", "PG", "Flat", "Hostel", "Room"};   // index = menu choice

// --------------------------- INPUT HELPERS (shared by every module) ---------------------------
void clearInputLine() { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

void exitOnEof() {
    cout << "\nInput stream closed. Exiting safely.\n";
    exit(0);
}

string readNonEmptyLine(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        if (!getline(cin, value)) exitOnEof();
        if (!value.empty()) return value;
        cout << "Input cannot be empty. Please try again.\n";
    }
}

int readInt(const string& prompt, int minimum, int maximum) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minimum && value <= maximum) {
            clearInputLine();
            return value;
        }
        cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
        if (cin.eof()) exitOnEof();
        clearInputLine();
    }
}

double readDouble(const string& prompt, double minimum, double maximum) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && isfinite(value) && value >= minimum && value <= maximum) {
            clearInputLine();
            return value;
        }
        cout << "Enter a valid number from " << minimum << " to " << maximum << ".\n";
        if (cin.eof()) exitOnEof();
        clearInputLine();
    }
}

bool readYesNo(const string& prompt) {
    return readInt(prompt + " (1 = Yes, 0 = No): ", 0, 1) == 1;
}

// Prints a legend line, then reads a bounded choice.
int readChoice(const string& legend, const string& prompt, int minimum, int maximum) {
    cout << legend << '\n';
    return readInt(prompt, minimum, maximum);
}

string lowerCase(string text) {
    for (char& ch : text)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    return text;
}

// --------------------------- ACCOMMODATION CLASSES ---------------------------

class Accommodation {
private:
    int id, ownerId;
    string ownerName, ownerInfo, propertyName, location;
    double monthlyRent, roomSize;
    int beds;
    bool mess, laundry, marketNearby, bathroom, independent, available;
    double distanceKm;

public:
    Accommodation(int propertyId, int propertyOwnerId,
                  const string& propertyOwnerName, const string& propertyOwnerInfo,
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
          available(isAvailable), distanceKm(distance) {}
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
    virtual void display() const {
        auto yn = [](bool b) { return b ? "Yes" : "No"; };
        cout << "\n----------------------------------------\n"
             << "Property ID       : " << id << '\n'
             << "Property name     : " << propertyName << '\n'
             << "Type              : " << getType() << '\n'
             << "Location          : " << location << '\n'
             << "Monthly rent      : Rs. " << fixed << setprecision(2) << monthlyRent << '\n'
             << "Room size         : " << roomSize << " sq. ft.\n"
             << "Beds / occupancy  : " << beds << '\n'
             << "Mess              : " << yn(mess) << '\n'
             << "Laundry           : " << yn(laundry) << '\n'
             << "Market nearby     : " << yn(marketNearby) << '\n'
             << "Bathroom          : " << (bathroom ? "Available" : "Not available") << '\n'
             << "Accommodation     : " << (independent ? "Independent" : "Dependent/shared") << '\n'
             << "Distance to uni.  : " << distanceKm << " km\n"
             << "Availability      : " << (available ? "Listed as available" : "Unavailable") << '\n'
             << "Owner             : " << ownerName << " (ID " << ownerId << ")\n"
             << "Owner information : " << ownerInfo << '\n'
             << "----------------------------------------\n";
    }
};

class PG : public Accommodation { public: using Accommodation::Accommodation; string getType() const override { return "PG"; } };

class Flat : public Accommodation { public: using Accommodation::Accommodation; string getType() const override { return "Flat"; } };

class Hostel : public Accommodation { public: using Accommodation::Accommodation; string getType() const override { return "Hostel"; } };

class RentRoom : public Accommodation { public: using Accommodation::Accommodation; string getType() const override { return "Room"; } };

// --------------------------- CUSTOM LINKED LIST ---------------------------

class AccommodationList {
private:
    struct Node {
        Accommodation* property;
        Node* next;
        Node(Accommodation* item) : property(item), next(nullptr) {}
    };
    Node* head;
    int count;

public:
    AccommodationList() : head(nullptr), count(0) {}
    ~AccommodationList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current->property; // deletes derived object safely via virtual destructor
            delete current;
            current = nextNode;
        }
    }
    bool isEmpty() const { return head == nullptr; }
    int size() const { return count; }
    bool containsId(int propertyId) const { return findById(propertyId) != nullptr; }
    Accommodation* findById(int propertyId) const {
        for (Node* current = head; current != nullptr; current = current->next)
            if (current->property->getId() == propertyId)
                return current->property;
        return nullptr;
    }
    void add(Accommodation* property) {
        if (property == nullptr || containsId(property->getId())) {
            delete property;
            return;
        }
        Node* newNode = new Node(property);
        if (head == nullptr)
            head = newNode;
        else {
            Node* current = head;
            while (current->next != nullptr)
                current = current->next;
            current->next = newNode;
        }
        count++;
    }
    bool removeById(int propertyId) {
        Node* current = head;
        Node* previous = nullptr;
        while (current != nullptr) {
            if (current->property->getId() == propertyId) {
                if (previous == nullptr) head = current->next;
                else previous->next = current->next;
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
    void showAll() const {
        if (isEmpty()) {
            cout << "No accommodation listings are present.\n";
            return;
        }
        for (Node* current = head; current != nullptr; current = current->next)
            current->property->display(); // runtime polymorphism
    }
    void showForOwner(int ownerId) const {
        bool found = false;
        for (Node* current = head; current != nullptr; current = current->next) {
            if (current->property->getOwnerId() == ownerId) {
                current->property->display();
                found = true;
            }
        }
        if (!found)
            cout << "This owner has no listings.\n";
    }
    vector<Accommodation*> getAllPointers() const {
        vector<Accommodation*> result; // STL: vector stores temporary search results
        for (Node* current = head; current != nullptr; current = current->next)
            result.push_back(current->property);
        return result;
    }
    int nextId() const {
        int largest = 1000;
        for (Node* current = head; current != nullptr; current = current->next)
            largest = max(largest, current->property->getId());
        return largest + 1;
    }
};

// --------------------------- CUSTOM STACK: RECENTLY VIEWED ---------------------------

class RecentStack {
private:
    struct StackNode {
        int propertyId;
        StackNode* next;
        StackNode(int id) : propertyId(id), next(nullptr) {}
    };
    StackNode* topNode;

public:
    RecentStack() : topNode(nullptr) {}
    ~RecentStack() {
        while (topNode != nullptr) {
            StackNode* oldTop = topNode;
            topNode = topNode->next;
            delete oldTop;
        }
    }
    bool isEmpty() const { return topNode == nullptr; }
    void push(int propertyId) {
        StackNode* newNode = new StackNode(propertyId);
        newNode->next = topNode;
        topNode = newNode;
    }
    void show(const AccommodationList& listings) const {
        if (isEmpty()) {
            cout << "You have not viewed any properties in this session.\n";
            return;
        }
        cout << "\nRecently viewed (latest first):\n";
        for (StackNode* current = topNode; current != nullptr; current = current->next) {
            Accommodation* property = listings.findById(current->propertyId);
            if (property != nullptr)
                property->display();
        }
    }
};

// --------------------------- CUSTOM QUEUE: CONTACT INQUIRIES ---------------------------

class ContactQueue {
private:
    struct Request {
        int requestId, propertyId, ownerId;
        string studentName, studentContact, message;
        Request* next;
        Request(int reqId, int propId, int ownId, const string& name,
                const string& contact, const string& msg)
            : requestId(reqId), propertyId(propId), ownerId(ownId),
              studentName(name), studentContact(contact), message(msg),
              next(nullptr) {}
    };
    Request* frontNode;
    Request* rearNode;
    int nextRequestId;

public:
    ContactQueue() : frontNode(nullptr), rearNode(nullptr), nextRequestId(1) {}
    ~ContactQueue() {
        while (frontNode != nullptr) {
            Request* oldFront = frontNode;
            frontNode = frontNode->next;
            delete oldFront;
        }
        rearNode = nullptr;
    }
    bool isEmpty() const { return frontNode == nullptr; }
    void enqueue(int propertyId, int ownerId, const string& studentName,
                 const string& studentContact, const string& message) {
        Request* newRequest = new Request(nextRequestId++, propertyId, ownerId,
                                          studentName, studentContact, message);
        if (rearNode == nullptr)
            frontNode = rearNode = newRequest;
        else {
            rearNode->next = newRequest;
            rearNode = newRequest;
        }
        cout << "Inquiry submitted. Request ID: " << newRequest->requestId << '\n';
    }
    void showForOwner(int ownerId) const {
        bool found = false;
        for (Request* current = frontNode; current != nullptr; current = current->next) {
            if (current->ownerId == ownerId) {
                cout << "\nRequest ID: " << current->requestId
                     << "\nProperty ID: " << current->propertyId
                     << "\nStudent: " << current->studentName
                     << "\nContact: " << current->studentContact
                     << "\nMessage: " << current->message << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "No pending inquiries for your properties.\n";
    }
    bool processNextForOwner(int ownerId) {
        Request* current = frontNode;
        Request* previous = nullptr;
        // Find the earliest queued request belonging to this owner.
        while (current != nullptr && current->ownerId != ownerId) {
            previous = current;
            current = current->next;
        }
        if (current == nullptr) {
            cout << "No pending inquiries for your properties.\n";
            return false;
        }
        cout << "\nProcessing inquiry " << current->requestId
             << " for property " << current->propertyId << ".\n"
             << "Student: " << current->studentName
             << "\nContact: " << current->studentContact
             << "\nMessage: " << current->message << '\n';
        if (previous == nullptr) frontNode = current->next;
        else previous->next = current->next;
        if (current == rearNode)
            rearNode = previous;
        delete current;
        cout << "Inquiry marked as processed.\n";
        return true;
    }
};
