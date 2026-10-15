//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Liam Jones
// Course      : CS-300
// Date        : 04/19/2026
// Description : Project Two - Advising Assistance Program using BST
//============================================================================

#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// This struct stores one course from the file.
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

// Each BST node holds one course and links to child nodes.
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

class BinarySearchTree {
private:
    Node* root;
    unordered_map<string, Node*> courseIndex;

    // Recursively follows prerequisites and detects circular dependencies.
    bool findPrerequisitePath(
        const Course& course,
        set<string>& visited,
        set<string>& currentPath) const {

        // A course already in the current path means there is a cycle.
        if (currentPath.find(course.courseNumber) != currentPath.end()) {
            cout << "Circular prerequisite detected involving "
                << course.courseNumber << endl;
            return false;
        }

        // If already processed, we do not need to process it again.
        if (visited.find(course.courseNumber) != visited.end()) {
            return true;
        }

        currentPath.insert(course.courseNumber);

        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            Course* prerequisite = Search(course.prerequisites[i]);

            if (prerequisite != nullptr) {
                if (!findPrerequisitePath(*prerequisite, visited, currentPath)) {
                    return false;
                }

                cout << prerequisite->courseNumber
                    << ", " << prerequisite->courseName << endl;
            }
        }

        currentPath.erase(course.courseNumber);
        visited.insert(course.courseNumber);

        return true;
    }

    // This helper inserts a course into the correct place in the BST
    // and returns the node that was created.
    Node* addNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
                return node->left;
            }
            else {
                return addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
                return node->right;
            }
            else {
                return addNode(node->right, course);
            }
        }
    }

    // In-order traversal prints the courses in alphanumeric order.
    void inOrder(Node* node) const {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseName << endl;
            inOrder(node->right);
        }
    }

    // This deletes every node so there are no memory leaks.
    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    // Returns true if the tree is empty.
    bool IsEmpty() const {
        return root == nullptr;
    }

    // Inserts one course into the BST and adds it to the lookup index.
    void Insert(const Course& course) {
        Node* newNode = nullptr;

        if (root == nullptr) {
            root = new Node(course);
            newNode = root;
        }
        else {
            newNode = addNode(root, course);
        }

        courseIndex[course.courseNumber] = newNode;
    }

    // Searches for a course by course number using the hash-based lookup index.
    Course* Search(const string& courseNumber) const {
        auto it = courseIndex.find(courseNumber);

        if (it != courseIndex.end()) {
            return &it->second->course;
        }

        return nullptr;
    }

    // Prints all courses using in-order traversal.
    void PrintAll() const {
        inOrder(root);
    }

    // Finds and prints the complete prerequisite path for a course.
    void PrintPrerequisitePath(const string& courseNumber) const {
        Course* course = Search(courseNumber);

        if (course == nullptr) {
            cout << "Course not found." << endl;
            return;
        }

        cout << "Complete prerequisite path for "
            << course->courseNumber << ":" << endl;

        set<string> visited;
        set<string> currentPath;

        if (course->prerequisites.empty()) {
            cout << "This course has no prerequisites." << endl;
            return;
        }

        findPrerequisitePath(*course, visited, currentPath);
    }
};

// This makes searching easier because user input can be lowercase.
string toUpperCase(string text) {
    for (size_t i = 0; i < text.length(); ++i) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// This removes spaces from the start and end of a field from the CSV file.
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    size_t end = text.find_last_not_of(" \t\r\n");

    if (start == string::npos) {
        return "";
    }

    return text.substr(start, end - start + 1);
}

// This splits one CSV line into separate fields.
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// This checks whether a course number is already in a vector.
bool courseExistsInList(const vector<string>& courseNumbers, const string& courseNumber) {
    for (size_t i = 0; i < courseNumbers.size(); ++i) {
        if (courseNumbers[i] == courseNumber) {
            return true;
        }
    }
    return false;
}

// This loads the courses, validates prerequisites, and then inserts valid courses into the BST.
bool LoadCourses(const string& fileName, BinarySearchTree& bst) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error opening file." << endl;
        return false;
    }

    vector<Course> courseList;
    vector<string> validCourseNumbers;
    set<string> seenCourseNumbers;
    bool duplicateFound = false;
    string line;

    while (getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        if (tokens.size() < 2) {
            cout << "Invalid line format." << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseName = tokens[1];
        if (seenCourseNumbers.find(course.courseNumber) != seenCourseNumbers.end()) {
            cout << "Duplicate course entry found: "
                << course.courseNumber << ". Entry skipped." << endl;
            duplicateFound = true;
            continue;
        }

        seenCourseNumbers.insert(course.courseNumber);

        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpperCase(tokens[i]));
            }
        }

        courseList.push_back(course);
        validCourseNumbers.push_back(course.courseNumber);
    }

    file.close();

    bool allValid = true;

    // This checks to make sure every prerequisite actually exists in the file.
    for (size_t i = 0; i < courseList.size(); ++i) {
        bool courseIsValid = true;

        for (size_t j = 0; j < courseList[i].prerequisites.size(); ++j) {
            if (!courseExistsInList(validCourseNumbers, courseList[i].prerequisites[j])) {
                cout << "Invalid prerequisite: " << courseList[i].prerequisites[j]
                    << " for course " << courseList[i].courseNumber << endl;
                courseIsValid = false;
                allValid = false;
            }
        }

        if (courseIsValid) {
            bst.Insert(courseList[i]);
        }
    }

    if (allValid && !duplicateFound) {
        cout << "Data loaded successfully." << endl;
    }
    else {
        cout << "Data loaded with some invalid or duplicate entries skipped." << endl;
    }
}

// This prints one course and its prerequisites.
void PrintCourse(BinarySearchTree& bst, string courseNumber) {
    courseNumber = toUpperCase(courseNumber);
    Course* course = bst.Search(courseNumber);

    if (course == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseName << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            cout << course->prerequisites[i];
            if (i < course->prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

// This prints the menu each time through the loop.
void DisplayMenu() {
    cout << "\n1. Load Data Structure" << endl;
    cout << "2. Print Course List" << endl;
    cout << "3. Print Course" << endl;
    cout << "4. Print Complete Prerequisite Path." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree bst;
    int choice = 0;
    string fileName;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        DisplayMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid menu number." << endl;
            continue;
        }

        if (choice == 1) {
            cout << "Enter file name: ";
            getline(cin >> ws, fileName);
            dataLoaded = LoadCourses(fileName, bst);
        }
        else if (choice == 2) {
            if (!dataLoaded || bst.IsEmpty()) {
                cout << "Please load the data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                bst.PrintAll();
            }
        }
        else if (choice == 3) {
            if (!dataLoaded || bst.IsEmpty()) {
                cout << "Please load the data first." << endl;
            }
            else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                PrintCourse(bst, courseNumber);
            }
        }
        else if (choice == 4) {
            if (!dataLoaded || bst.IsEmpty()) {
                cout << "Please load the data first." << endl;
            }
            else {
                string courseNumber;

                cout << "What course do you want to view the prerequisite path for? ";
                cin >> courseNumber;

                courseNumber = toUpperCase(courseNumber);

                bst.PrintPrerequisitePath(courseNumber);
            }
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
