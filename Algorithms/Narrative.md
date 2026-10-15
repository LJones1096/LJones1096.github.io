
# Enhancement Two: Algorithms and Data Structures

Liam Jones  
CS-499  
09/27/2026

## Artifact Description

For my second artifact, I selected the Advising Assistance Program that I originally created in CS 300: Data Structures and Algorithms. The program was designed to load course information from a CSV file and store the courses in a binary search tree. A user could print the full course list in alphanumeric order or search for an individual course to view its name and prerequisites. I selected this artifact because it demonstrates my understanding of data structures and algorithms while also giving me several areas that I could improve for my ePortfolio.

## Enhancement

For my enhancement, I focused on making the program more useful for someone trying to understand the order in which courses should be taken. The original program only displayed the immediate prerequisites for a selected course. I added a recursive algorithm that follows the complete prerequisite path instead. For example, if one prerequisite has its own prerequisites, the program continues through those courses instead of stopping after the first level. I also used sets to keep track of the courses being processed and added circular dependency detection. This prevents the recursive algorithm from continuing indefinitely if the course data contains a circular prerequisite relationship. These changes followed the enhancement plan I developed earlier in the course.

Another improvement was adding an unordered_map as a second data structure for course lookups. I kept the original binary search tree because it still works well for displaying the courses in alphanumeric order, but course searches now use the hash-based lookup structure. This gave me an opportunity to use two different data structures for different purposes instead of trying to make one structure handle everything.

I also improved the validation when course information is loaded. The program already checked for prerequisites that did not exist in the course data, but I added duplicate course detection so duplicate entries are identified and skipped. I also improved the file input so paths containing spaces can be entered correctly and added input handling to prevent invalid menu input from causing the program to repeatedly display the menu.

## Course Outcomes

The enhancement supports the course outcomes I originally selected for this artifact. Course Outcome Three focuses on designing and evaluating computing solutions using algorithmic principles, computer science practices, and trade-offs. I demonstrated this by keeping the binary search tree for ordered traversal while adding an unordered_map for faster course-number lookups. I also used recursion and sets to solve the prerequisite traversal and circular dependency problems.

Course Outcome Four focuses on using well-founded techniques and tools to create computing solutions that provide value. I believe the enhanced program demonstrates this because it provides more useful prerequisite information, handles more types of invalid data, and is more reliable than the original version. These are the same outcomes I planned to demonstrate when I originally proposed the enhancement.

## Reflection

The enhancement process helped me better understand that choosing a data structure depends on what the program needs to accomplish. The binary search tree was still useful, so there was no reason to completely replace it. Instead, I was able to add another structure that was better suited for direct lookups. The recursive prerequisite feature was also useful because it showed me how quickly recursion can become a problem if the program does not keep track of what it has already visited. Adding circular dependency detection made the algorithm safer and gave me a better understanding of why validation is important when working with connected data.

One challenge during the enhancement was making changes without breaking the functionality that already worked. I tested the program throughout the process instead of waiting until the end. I used the original course data to verify that course loading, searching, sorted output, and the complete prerequisite path still worked. I also created separate test data containing a circular prerequisite relationship and duplicate course entries. This allowed me to verify that the new validation worked without changing the original course data. Overall, the enhancement improved the original program while giving me more experience working with multiple data structures, recursive algorithms, validation, and testing.

## AI Use Statement

I used Grammarly to assist with grammar, spelling, and clarity while reviewing this document.
