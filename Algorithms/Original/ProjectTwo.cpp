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
#include <sstream>
#include <string>
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

    // This helper inserts a course into the correct place in the BST.
    void addNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
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

    // Inserts one course into the BST.
    void Insert(const Course& course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    // Searches for a course by course number.
    Course* Search(const string& courseNumber) const {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return &current->course;
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return nullptr;
    }

    // Prints all courses using in-order traversal.
    void PrintAll() const {
        inOrder(root);
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

    if (allValid) {
        cout << "Data loaded successfully." << endl;
    }
    else {
        cout << "Data loaded with some invalid prerequisite entries skipped." << endl;
    }

    return true;
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

        if (choice == 1) {
            cout << "Enter file name: ";
            cin >> fileName;
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
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
