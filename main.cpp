// main.cpp
// Complete Hospital Patient Management System (demo)
// Features included:
// - Linked List for patients
// - 4 Doctors with manual selection (each has a queue implemented with linked nodes)
// - Global Emergency Queue (priority)
// - Token Display System (shows front token for each doctor and emergency)
// - Per-patient treatment history using std::stack<std::string>
// - BST (Binary Search Tree) storing Patient* and sorted by name (alphabetical)
// - Automatic insertion into both Linked List and BST at registration
// - Deletion removes from both Linked List and BST
// - Uses only <iostream> and <stack>

#include <iostream>
#include <stack>

using namespace std;

// ---------------------------- Patient Node (Linked List) ----------------------------
struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string disease;
    string regLabel;            // simple registration label (not real timestamp)
    stack<string> treatments;   // per-patient treatment history
    Patient* next;

    Patient(int _id, const string& _name, int _age, const string& _gender, const string& _disease, const string& _reg)
        : id(_id), name(_name), age(_age), gender(_gender), disease(_disease), regLabel(_reg), next(nullptr) {}
};

// ---------------------------- BST Node ----------------------------
struct BSTNode {
    Patient* pdata;
    BSTNode* left;
    BSTNode* right;
    BSTNode(Patient* p): pdata(p), left(nullptr), right(nullptr) {}
};

// ---------------------------- Patient Linked List Manager ----------------------------
class PatientList {
private:
    Patient* head;
public:
    PatientList(): head(nullptr) {}

    ~PatientList() {
        Patient* cur = head;
        while (cur) {
            Patient* nx = cur->next;
            delete cur;
            cur = nx;
        }
    }

    void addPatient(Patient* p) {
        if (!head) { head = p; return; }
        Patient* cur = head;
        while (cur->next) cur = cur->next;
        cur->next = p;
    }

    Patient* findById(int id) {
        Patient* cur = head;
        while (cur) {
            if (cur->id == id) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    bool deleteById(int id) {
        Patient* cur = head;
        Patient* prev = nullptr;
        while (cur) {
            if (cur->id == id) {
                if (prev) prev->next = cur->next;
                else head = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    void displayAll() {
        if (!head) { cout << "No patient records available.\n"; return; }
        cout << "=== All Patient Records ===\n";
        Patient* cur = head;
        while (cur) {
            cout << "ID: " << cur->id
                 << " | Name: " << cur->name
                 << " | Age: " << cur->age
                 << " | Gender: " << cur->gender
                 << " | Disease: " << cur->disease
                 << " | Reg: " << cur->regLabel << '\n';
            cur = cur->next;
        }
    }

    // iterate and call function f(Patient*)
    template<typename Fn>
    void forEach(Fn f) {
        Patient* cur = head;
        while (cur) { f(cur); cur = cur->next; }
    }
};

// ---------------------------- Queue Node & Queue ----------------------------
struct QNode {
    int patientId;
    QNode* next;
    QNode(int id): patientId(id), next(nullptr) {}
};

class PatientQueue {
private:
    QNode* frontNode;
    QNode* rearNode;
public:
    PatientQueue(): frontNode(nullptr), rearNode(nullptr) {}
    ~PatientQueue() {
        while (frontNode) {
            QNode* t = frontNode;
            frontNode = frontNode->next;
            delete t;
        }
    }

    void enqueue(int pid) {
        QNode* n = new QNode(pid);
        if (!rearNode) { frontNode = rearNode = n; return; }
        rearNode->next = n;
        rearNode = n;
    }

    int dequeue() {
        if (!frontNode) return -1;
        QNode* t = frontNode;
        int id = t->patientId;
        frontNode = frontNode->next;
        if (!frontNode) rearNode = nullptr;
        delete t;
        return id;
    }

    int peek() const {
        if (!frontNode) return -1;
        return frontNode->patientId;
    }

    bool isEmpty() const { return frontNode == nullptr; }

    // display queue tokens with patient names (needs patient list)
    void display(PatientList& plist) {
        if (!frontNode) { cout << "Empty\n"; return; }
        QNode* cur = frontNode;
        while (cur) {
            Patient* p = plist.findById(cur->patientId);
            if (p) cout << "[#" << p->id << " " << p->name << "] -> ";
            else cout << "[#" << cur->patientId << " (not found)] -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }
};

// ---------------------------- Global Components ----------------------------
PatientList patients;
PatientQueue emergencyQueue;
const int DOCTOR_COUNT = 4;
PatientQueue doctorQueues[DOCTOR_COUNT]; // doctorQueues[0] -> Doctor 1, etc.
BSTNode* bstRoot = nullptr;
int nextId = 1;

// ---------------------------- Utilities ----------------------------
void clearInputLine() {
    if (cin.peek() == '\n') cin.get();
}

string nowLabel() {
    static int lbl = 1000;
    lbl++;
    return "R" + to_string(lbl);
}

// ---------------------------- BST Helper Functions ----------------------------
// Compare keys: primary by name (lexicographic), secondary by id to keep uniqueness
int comparePatientKey(Patient* a, const string& name, int id) {
    if (a->name < name) return -1;
    if (a->name > name) return 1;
    // names equal -> compare id
    if (a->id < id) return -1;
    if (a->id > id) return 1;
    return 0;
}

BSTNode* bstInsert(BSTNode* root, Patient* p) {
    if (!root) {
        BSTNode* node = new BSTNode(p);
        return node;
    }
    // compare p->name with root->pdata->name
    if (p->name < root->pdata->name) {
        root->left = bstInsert(root->left, p);
    } else if (p->name > root->pdata->name) {
        root->right = bstInsert(root->right, p);
    } else {
        // same name -> use id to decide side (to keep deterministic tree)
        if (p->id < root->pdata->id) root->left = bstInsert(root->left, p);
        else root->right = bstInsert(root->right, p);
    }
    return root;
}

// find minimum node in subtree
BSTNode* bstFindMin(BSTNode* root) {
    if (!root) return nullptr;
    while (root->left) root = root->left;
    return root;
}

// delete node by (name,id)
BSTNode* bstDelete(BSTNode* root, const string& name, int id) {
    if (!root) return nullptr;
    if (name < root->pdata->name) {
        root->left = bstDelete(root->left, name, id);
    } else if (name > root->pdata->name) {
        root->right = bstDelete(root->right, name, id);
    } else {
        // name equals; use id to choose direction or delete
        if (id < root->pdata->id) root->left = bstDelete(root->left, name, id);
        else if (id > root->pdata->id) root->right = bstDelete(root->right, name, id);
        else {
            // found node to delete
            if (!root->left && !root->right) {
                delete root;
                return nullptr;
            } else if (!root->left) {
                BSTNode* r = root->right;
                delete root;
                return r;
            } else if (!root->right) {
                BSTNode* l = root->left;
                delete root;
                return l;
            } else {
                BSTNode* mn = bstFindMin(root->right);
                // copy mn's pdata pointer into root
                root->pdata = mn->pdata;
                // delete mn node (by key)
                root->right = bstDelete(root->right, mn->pdata->name, mn->pdata->id);
            }
        }
    }
    return root;
}

// inorder traversal for sorted display (alphabetical)
void bstInorderDisplay(BSTNode* root) {
    if (!root) return;
    bstInorderDisplay(root->left);
    Patient* p = root->pdata;
    cout << "ID: " << p->id << " | Name: " << p->name << " | Age: " << p->age
         << " | Gender: " << p->gender << " | Disease: " << p->disease << '\n';
    bstInorderDisplay(root->right);
}

// search for exact name matches (possibly multiple). Print matches as we find them.
void bstSearchByName(BSTNode* root, const string& name) {
    if (!root) return;
    // Since BST is ordered by name, we can prune
    if (name < root->pdata->name) {
        bstSearchByName(root->left, name);
    } else if (name > root->pdata->name) {
        bstSearchByName(root->right, name);
    } else {
        // name equals: there may be multiple nodes with same name both sides
        // search left subtree for equal name
        bstSearchByName(root->left, name);
        // print this node
        Patient* p = root->pdata;
        cout << "ID: " << p->id << " | Name: " << p->name << " | Age: " << p->age
             << " | Gender: " << p->gender << " | Disease: " << p->disease << '\n';
        // search right subtree for equal name
        bstSearchByName(root->right, name);
    }
}

// ---------------------------- Core Operations ----------------------------
void insertIntoBST(Patient* p) {
    bstRoot = bstInsert(bstRoot, p);
}

void deleteFromBST(Patient* p) {
    if (!p) return;
    bstRoot = bstDelete(bstRoot, p->name, p->id);
}

// ---------------------------- Global Data and Functions ----------------------------
void registerPatient() {
    clearInputLine();
    string name, gender, disease;
    int age;
    char emergencyChar;
    int docChoice;

    cout << "Enter patient name: ";
    getline(cin, name);
    cout << "Enter age: ";
    cin >> age;
    clearInputLine();
    cout << "Enter gender (M/F/O): ";
    getline(cin, gender);
    cout << "Enter disease/complaint: ";
    getline(cin, disease);
    cout << "Is this an emergency? (y/n): ";
    cin >> emergencyChar;

    int id = nextId++;
    string reg = nowLabel();
    Patient* p = new Patient(id, name, age, gender, disease, reg);
    // add to linked list
    patients.addPatient(p);
    // add to BST
    insertIntoBST(p);

    if (emergencyChar == 'y' || emergencyChar == 'Y') {
        emergencyQueue.enqueue(id);
        cout << "Registered as EMERGENCY. Token #" << id << "\n";
    } else {
        // manual doctor selection
        cout << "Choose doctor to assign (1 - " << DOCTOR_COUNT << "): ";
        cin >> docChoice;
        if (docChoice < 1 || docChoice > DOCTOR_COUNT) {
            cout << "Invalid doctor choice. Assigned to Doctor 1 by default.\n";
            doctorQueues[0].enqueue(id);
            cout << "Registered as Normal. Token #" << id << " (Doctor 1)\n";
        } else {
            doctorQueues[docChoice - 1].enqueue(id);
            cout << "Registered as Normal. Token #" << id << " (Doctor " << docChoice << ")\n";
        }
    }
}

void displayWaiting() {
    cout << "\n--- Waiting Queues ---\n";
    cout << "Emergency Queue: ";
    emergencyQueue.display(patients);
    for (int i = 0; i < DOCTOR_COUNT; ++i) {
        cout << "Doctor " << (i+1) << " Queue: ";
        doctorQueues[i].display(patients);
    }
    cout << "----------------------\n";
}

void callNextForDoctor() {
    cout << "Select doctor to call next (1 - " << DOCTOR_COUNT << "): ";
    int d; cin >> d;
    if (d < 1 || d > DOCTOR_COUNT) { cout << "Invalid doctor number.\n"; return; }
    int pid = -1;
    if (!emergencyQueue.isEmpty()) {
        pid = emergencyQueue.dequeue();
        cout << "Assigning EMERGENCY patient to Doctor " << d << ".\n";
    } else if (!doctorQueues[d-1].isEmpty()) {
        pid = doctorQueues[d-1].dequeue();
    } else {
        cout << "No patients waiting for Doctor " << d << ".\n";
        return;
    }

    Patient* p = patients.findById(pid);
    if (!p) {
        cout << "Token #" << pid << " found but patient record is missing.\n";
        return;
    }

    cout << "Doctor " << d << " is now seeing: #" << p->id << " - " << p->name << "\n";
    cout << "1) View record  2) Add treatment  3) Skip\n";
    int choice; cin >> choice;
    clearInputLine();
    if (choice == 1) {
        cout << "ID: " << p->id << " | Name: " << p->name << " | Age: " << p->age
             << " | Gender: " << p->gender << " | Disease: " << p->disease
             << " | Reg: " << p->regLabel << '\n';
    } else if (choice == 2) {
        cout << "Enter treatment notes (single line): ";
        string note;
        getline(cin, note);
        p->treatments.push(note);
        cout << "Treatment added for patient #" << p->id << ".\n";
    } else {
        cout << "Skipped adding treatment.\n";
    }
}

void displayTokenSystem() {
    cout << "\n---------------------------------------------\n";
    cout << "        HOSPITAL TOKEN DISPLAY SYSTEM\n";
    cout << "---------------------------------------------\n";
    // Emergency front
    int efront = emergencyQueue.peek();
    if (efront == -1) cout << "Emergency Next Token: EMPTY\n";
    else {
        Patient* p = patients.findById(efront);
        if (p) cout << "Emergency Next Token: #" << p->id << " (" << p->name << ")\n";
        else cout << "Emergency Next Token: #" << efront << " (no record)\n";
    }
    cout << "---------------------------------------------\n";
    // Each doctor
    for (int i = 0; i < DOCTOR_COUNT; ++i) {
        int front = doctorQueues[i].peek();
        if (front == -1) cout << "Doctor " << (i+1) << " -> Next Token: EMPTY\n";
        else {
            Patient* p = patients.findById(front);
            if (p) cout << "Doctor " << (i+1) << " -> Next Token: #" << p->id << " (" << p->name << ")\n";
            else cout << "Doctor " << (i+1) << " -> Next Token: #" << front << " (no record)\n";
        }
    }
    cout << "---------------------------------------------\n";
}

void searchPatientById() {
    cout << "Enter patient ID to search: ";
    int id; cin >> id;
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    cout << "ID: " << p->id << " | Name: " << p->name << " | Age: " << p->age
         << " | Gender: " << p->gender << " | Disease: " << p->disease
         << " | Reg: " << p->regLabel << '\n';
}

void searchPatientByName() {
    clearInputLine();
    cout << "Enter full name to search (exact match): ";
    string name; getline(cin, name);
    cout << "Search results for name: " << name << '\n';
    bstSearchByName(bstRoot, name);
}

void updatePatient() {
    cout << "Enter patient ID to update: ";
    int id; cin >> id;
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    clearInputLine();
    cout << "Updating patient #" << p->id << " (" << p->name << ")\n";
    cout << "Enter new name (or press enter to keep): ";
    string tmp;
    getline(cin, tmp);
    string oldName = p->name;
    if (!tmp.empty()) p->name = tmp;
    cout << "Enter new age (0 to keep): ";
    int newAge; cin >> newAge; if (newAge > 0) p->age = newAge;
    clearInputLine();
    cout << "Enter new gender (or press enter to keep): ";
    getline(cin, tmp); if (!tmp.empty()) p->gender = tmp;
    cout << "Enter new disease (or press enter to keep): ";
    getline(cin, tmp); if (!tmp.empty()) p->disease = tmp;
    cout << "Patient updated.\n";
    // if name changed, update BST: remove old key and insert new
    if (p->name != oldName) {
        // remove by oldName and id, then insert pointer again
        bstRoot = bstDelete(bstRoot, oldName, p->id);
        bstRoot = bstInsert(bstRoot, p);
    }
}

void deletePatient() {
    cout << "Enter patient ID to delete: ";
    int id; cin >> id;
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    // delete from BST by name and id
    bstRoot = bstDelete(bstRoot, p->name, p->id);
    // delete from linked list
    bool ok = patients.deleteById(id);
    if (ok) cout << "Patient #" << id << " deleted. (Note: token may still appear in queues but will show as not found.)\n";
    else cout << "Error deleting patient.\n";
}

void addTreatmentToPatient() {
    cout << "Enter patient ID to add treatment: ";
    int id; cin >> id;
    clearInputLine();
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    cout << "Enter treatment notes: ";
    string note;
    getline(cin, note);
    p->treatments.push(note);
    cout << "Treatment added.\n";
}

void viewTreatmentHistory() {
    cout << "Enter patient ID to view treatments: ";
    int id; cin >> id;
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    if (p->treatments.empty()) { cout << "No treatments recorded.\n"; return; }
    // copy stack to display without destroying
    stack<string> tmp = p->treatments;
    cout << "Treatment history for #" << p->id << " (" << p->name << "):\n";
    int step = 1;
    while (!tmp.empty()) {
        cout << step << ") " << tmp.top() << '\n';
        tmp.pop();
        step++;
    }
}

void undoLastTreatment() {
    cout << "Enter patient ID to undo last treatment: ";
    int id; cin >> id;
    Patient* p = patients.findById(id);
    if (!p) { cout << "Patient not found.\n"; return; }
    if (p->treatments.empty()) { cout << "No treatments to undo.\n"; return; }
    string last = p->treatments.top();
    p->treatments.pop();
    cout << "Undone treatment: " << last << '\n';
}

void displayBSTInorder() {
    if (!bstRoot) { cout << "BST is empty.\n"; return; }
    cout << "=== Patients (Alphabetical Order) ===\n";
    bstInorderDisplay(bstRoot);
}

// ---------------------------- Menu ----------------------------
void showMenu() {
    cout << "\n=== Hospital Patient Management (Demo) ===\n";
    cout << "1. Register Patient\n";
    cout << "2. Display Waiting Queues\n";
    cout << "3. Call Next Patient for a Doctor\n";
    cout << "4. Token Display System\n";
    cout << "5. Search Patient by ID\n";
    cout << "6. Search Patient by Name (BST)\n";
    cout << "7. Update Patient\n";
    cout << "8. Delete Patient\n";
    cout << "9. Add Treatment to Patient\n";
    cout << "10. View Treatment History\n";
    cout << "11. Undo Last Treatment\n";
    cout << "12. Display All Patient Records (LinkedList)\n";
    cout << "13. Display Patients Alphabetically (BST Inorder)\n";
    cout << "0. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    cout << "Welcome to Hospital Management Demo (4 Doctors, manual selection, BST by Name, no file save).\n";
    bool running = true;
    while (running) {
        showMenu();
        int opt;
        if (!(cin >> opt)) {
            cin.clear();
            clearInputLine();
            cout << "Invalid input. Try again.\n";
            continue;
        }
        switch (opt) {
            case 1: registerPatient(); break;
            case 2: displayWaiting(); break;
            case 3: callNextForDoctor(); break;
            case 4: displayTokenSystem(); break;
            case 5: searchPatientById(); break;
            case 6: searchPatientByName(); break;
            case 7: updatePatient(); break;
            case 8: deletePatient(); break;
            case 9: addTreatmentToPatient(); break;
            case 10: viewTreatmentHistory(); break;
            case 11: undoLastTreatment(); break;
            case 12: patients.displayAll(); break;
            case 13: displayBSTInorder(); break;
            case 0:
                cout << "Exit. Goodbye.\n";
                running = false;
                break;
            default:
                cout << "Unknown option. Try again.\n";
        }
    }
    return 0;
}
