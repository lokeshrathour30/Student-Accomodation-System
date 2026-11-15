# Student Accommodation System

A C++17-based, menu-driven application designed to simplify student accommodation management by connecting students with available rental properties and providing property owners with basic listing management facilities.

## Features

- **Student Module:** Browse, search, filter, sort, and view accommodation listings; maintain recently viewed properties and submit inquiries.
- **Owner Module:** Add, view, and remove property listings, and manage student inquiries.
- **Accommodation Management:** Supports PGs, flats, hostels, and rental rooms through an extensible class hierarchy.
- **Data Structures:** Custom singly linked list, stack, and queue implementations for accommodation management, browsing history, and inquiry handling.
- **File Handling:** CSV-based storage for accommodation listings, enabling data retrieval and persistence between application sessions.

## Technical Implementation

- **Language:** C++17
- **Programming Paradigm:** Object-Oriented Programming
- **OOP Concepts:** Encapsulation, Abstraction, Inheritance, and Polymorphism
- **Data Structures:** Linked List, Stack, and Queue
- **Storage:** CSV File Handling
- **Interface:** Command-Line Interface (CLI)

## Project Structure

```text
Student-Accommodation-System/
├── main.cpp
├── student.cpp
├── owner.cpp
├── accommodations.cpp
├── file_handling.cpp
├── accommodations.csv
└── README.md
```

## Compilation and Execution

Compile the project using GCC:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp student.cpp owner.cpp file_handling.cpp accommodations.cpp -o accommodation
```

Run the application:

**Windows**
```bash
.\accommodation.exe
```

**Linux/macOS**
```bash
./accommodation
```

## Project Objective

To develop a structured accommodation management application while applying core concepts of C++, object-oriented programming, data structures, and file handling to a practical real-world problem.

## Future Scope

- User registration and authentication
- Database integration
- Online booking and payment facilities
- Real-time communication between students and property owners
- Web-based application interface

---

**Developed as an academic project by Team Cheatanbehal08.**
