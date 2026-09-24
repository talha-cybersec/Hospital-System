# Hospital Patient Management System (C++)

A console-based hospital management system written in C++. It uses **custom-built** linked lists, queues and a binary search tree (plus `std::stack`) to manage patient registration, doctor queues, emergency priority and treatment history.

## ✨ Features

- **Patient registration:** stores ID, name, age, gender, disease and registration label; every patient gets a token number
- **4 doctors with separate waiting queues:** patients are assigned to a doctor of their choice
- **Emergency queue:** whenever any doctor calls the next patient, waiting emergency patients are seen first
- **Token display system:** shows the next token for each doctor and for emergencies
- **Search:** by patient ID (linked list) or by name (BST)
- **Update and delete:** deleting a patient removes them from both the linked list and the BST
- **Treatment history:** each patient has their own treatment stack, with **undo last treatment**
- **Reports:** all patients in registration order, or alphabetically (BST in-order traversal)

## 🧱 Data Structures Used

| Structure | Implementation | Purpose |
|---|---|---|
| Singly linked list | Custom `PatientList` | Master record of all patients |
| Queue | Custom `PatientQueue` (linked nodes) | Doctor waiting lists and emergency queue |
| Binary search tree | Custom `BSTNode` | Alphabetical search and sorted display by name |
| Stack | `std::stack<std::string>` | Per-patient treatment history with undo |

## 🚀 Build & Run

```bash
git clone https://github.com/talha-cybersec/Hospital-System.git
cd Hospital-System
g++ -std=c++17 -o hms main.cpp
./hms          # on Windows: hms.exe
```

## 📋 Menu

```
1.  Register Patient
2.  Display Waiting Queues
3.  Call Next Patient for a Doctor
4.  Token Display System
5.  Search Patient by ID
6.  Search Patient by Name (BST)
7.  Update Patient
8.  Delete Patient
9.  Add Treatment to Patient
10. View Treatment History
11. Undo Last Treatment
12. Display All Patient Records (Linked List)
13. Display Patients Alphabetically (BST In-order)
0.  Exit
```

## 📝 Notes

- All data is kept in memory while the program runs; nothing is saved to a file.
- If a patient is deleted while still waiting in a queue, their token stays in the queue and is reported as a missing record when called.

## 👤 Author

**Muhammad Talha** · [@talha-cybersec](https://github.com/talha-cybersec)
