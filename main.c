#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERS 100
#define MAX_CANDIDATES 10
#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 20
#define MAX_ID_LENGTH 10

// Structure definitions
typedef struct {
    char id[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int hasVoted;
} User;

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int voteCount;
} Candidate;

// Global variables
User users[MAX_USERS];
Candidate candidates[MAX_CANDIDATES];
int userCount = 0;
int candidateCount = 0;
int isAdmin = 0;

// Function prototypes
void displayMainMenu();
void registerUser();
int loginUser();
void displayAdminMenu();
void displayVoterMenu();
void addCandidate();
void removeCandidate(); // New function to remove a candidate
void viewCandidates();
void castVote(int userIndex);
void viewResults();
void saveData();
void loadData();
void clearScreen();
int validatePassword(const char* password);

// Main function
int main() {
    int choice, loggedInUserIndex;
    
    // Load data from files (simulated)
    loadData();
    
    while (1) {
        clearScreen();
        displayMainMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loggedInUserIndex = loginUser();
                if (loggedInUserIndex >= 0) {
                    if (isAdmin) {
                        int adminChoice;
                        do {
                            clearScreen();
                            displayAdminMenu();
                            printf("Enter your choice: ");
                            scanf("%d", &adminChoice);
                            getchar(); // Clear input buffer
                            
                            switch (adminChoice) {
                                case 1:
                                    addCandidate();
                                    break;
                                case 2:
                                    removeCandidate(); // New case for removing a candidate
                                    break;
                                case 3:
                                    viewCandidates();
                                    break;
                                case 4:
                                    viewResults();
                                    break;
                                case 5:
                                    printf("Logging out...\n");
                                    break;
                                default:
                                    printf("Invalid choice. Please try again.\n");
                            }
                            
                            if (adminChoice != 5) {
                                printf("\nPress Enter to continue...");
                                getchar();
                            }
                        } while (adminChoice != 5);
                    } else {
                        int voterChoice;
                        do {
                            clearScreen();
                            displayVoterMenu();
                            printf("Enter your choice: ");
                            scanf("%d", &voterChoice);
                            getchar(); // Clear input buffer
                            
                            switch (voterChoice) {
                                case 1:
                                    castVote(loggedInUserIndex);
                                    break;
                                case 2:
                                    viewCandidates();
                                    break;
                                case 3:
                                    printf("Logging out...\n");
                                    break;
                                default:
                                    printf("Invalid choice. Please try again.\n");
                            }
                            
                            if (voterChoice != 3) {
                                printf("\nPress Enter to continue...");
                                getchar();
                            }
                        } while (voterChoice != 3);
                    }
                }
                break;
            case 3:
                saveData();
                printf("Thank you for using the Online Voting System!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                printf("Press Enter to continue...");
                getchar();
        }
    }
    
    return 0;
}

// Function to display the main menu
void displayMainMenu() {
    printf("===== ONLINE VOTING SYSTEM =====\n");
    printf("1. Register as a Voter\n");
    printf("2. Login\n");
    printf("3. Exit\n");
}

// Function to display the admin menu
void displayAdminMenu() {
    printf("===== ADMIN MENU =====\n");
    printf("1. Add Candidate\n");
    printf("2. Remove Candidate\n"); // New menu option
    printf("3. View Candidates\n");
    printf("4. View Results\n");
    printf("5. Logout\n");
}

// Function to display the voter menu
void displayVoterMenu() {
    printf("===== VOTER MENU =====\n");
    printf("1. Cast Vote\n");
    printf("2. View Candidates\n");
    printf("3. Logout\n");
}

// Function to register a new user
void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("Maximum user limit reached. Cannot register more users.\n");
        return;
    }
    
    User newUser;
    char confirmPassword[MAX_PASSWORD_LENGTH];
    
    printf("===== VOTER REGISTRATION =====\n");
    
    printf("Enter Voter ID: ");
    fgets(newUser.id, MAX_ID_LENGTH, stdin);
    newUser.id[strcspn(newUser.id, "\n")] = 0; // Remove newline
    
    // Check if user ID already exists
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, newUser.id) == 0) {
            printf("User ID already exists. Please try a different ID.\n");
            return;
        }
    }
    
    printf("Enter Name: ");
    fgets(newUser.name, MAX_NAME_LENGTH, stdin);
    newUser.name[strcspn(newUser.name, "\n")] = 0; // Remove newline
    
    do {
        printf("Enter Password (min 6 chars, must include uppercase, lowercase, and number): ");
        fgets(newUser.password, MAX_PASSWORD_LENGTH, stdin);
        newUser.password[strcspn(newUser.password, "\n")] = 0; // Remove newline
        
        if (!validatePassword(newUser.password)) {
            printf("Password does not meet requirements. Please try again.\n");
            continue;
        }
        
        printf("Confirm Password: ");
        fgets(confirmPassword, MAX_PASSWORD_LENGTH, stdin);
        confirmPassword[strcspn(confirmPassword, "\n")] = 0; // Remove newline
        
        if (strcmp(newUser.password, confirmPassword) != 0) {
            printf("Passwords do not match. Please try again.\n");
        }
    } while (strcmp(newUser.password, confirmPassword) != 0 || !validatePassword(newUser.password));
    
    newUser.hasVoted = 0;
    
    users[userCount++] = newUser;
    printf("Registration successful! You can now login.\n");
}

// Function to validate password strength
int validatePassword(const char* password) {
    int len = strlen(password);
    int hasUpper = 0, hasLower = 0, hasDigit = 0;
    
    if (len < 6) return 0;
    
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
    }
    
    return hasUpper && hasLower && hasDigit;
}

// Function to handle user login
int loginUser() {
    char id[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    
    printf("===== LOGIN =====\n");
    printf("Enter ID: ");
    fgets(id, MAX_ID_LENGTH, stdin);
    id[strcspn(id, "\n")] = 0; // Remove newline
    
    printf("Enter Password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline
    
    // Check for admin login
    if (strcmp(id, "admin") == 0 && strcmp(password, "Admin123") == 0) {
        printf("Admin login successful!\n");
        isAdmin = 1;
        return 0;
    } else {
        isAdmin = 0;
    }
    
    // Check for voter login
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].password, password) == 0) {
            printf("Login successful! Welcome, %s.\n", users[i].name);
            return i;
        }
    }
    
    printf("Invalid ID or password. Please try again.\n");
    return -1;
}

// Function to add a new candidate
void addCandidate() {
    if (candidateCount >= MAX_CANDIDATES) {
        printf("Maximum candidate limit reached. Cannot add more candidates.\n");
        return;
    }
    
    Candidate newCandidate;
    
    printf("===== ADD CANDIDATE =====\n");
    
    newCandidate.id = candidateCount + 1;
    
    printf("Enter Candidate Name: ");
    fgets(newCandidate.name, MAX_NAME_LENGTH, stdin);
    newCandidate.name[strcspn(newCandidate.name, "\n")] = 0; // Remove newline
    
    newCandidate.voteCount = 0;
    
    candidates[candidateCount++] = newCandidate;
    printf("Candidate added successfully!\n");
}

// Function to remove a candidate
void removeCandidate() {
    if (candidateCount == 0) {
        printf("No candidates available to remove.\n");
        return;
    }
    
    int candidateId;
    
    printf("===== REMOVE CANDIDATE =====\n");
    viewCandidates();
    
    printf("\nEnter the ID of the candidate you want to remove: ");
    scanf("%d", &candidateId);
    getchar(); // Clear input buffer
    
    if (candidateId < 1 || candidateId > candidateCount) {
        printf("Invalid candidate ID. Please try again.\n");
        return;
    }
    
    // Check if any votes have been cast for this candidate
    if (candidates[candidateId - 1].voteCount > 0) {
        char confirm;
        printf("WARNING: This candidate has %d votes. Removing will delete these votes.\n", 
               candidates[candidateId - 1].voteCount);
        printf("Are you sure you want to continue? (y/n): ");
        scanf("%c", &confirm);
        getchar(); // Clear input buffer
        
        if (tolower(confirm) != 'y') {
            printf("Candidate removal cancelled.\n");
            return;
        }
    }
    
    // Remove the candidate by shifting all elements after it
    for (int i = candidateId - 1; i < candidateCount - 1; i++) {
        candidates[i] = candidates[i + 1];
        // Update the ID to maintain sequential order
        candidates[i].id = i + 1;
    }
    
    candidateCount--;
    printf("Candidate removed successfully!\n");
}

// Function to view all candidates
void viewCandidates() {
    if (candidateCount == 0) {
        printf("No candidates available.\n");
        return;
    }
    
    printf("===== CANDIDATE LIST =====\n");
    printf("ID\tName\n");
    printf("------------------------\n");
    
    for (int i = 0; i < candidateCount; i++) {
        printf("%d\t%s\n", candidates[i].id, candidates[i].name);
    }
}

// Function to cast a vote
void castVote(int userIndex) {
    if (users[userIndex].hasVoted) {
        printf("You have already cast your vote.\n");
        return;
    }
    
    if (candidateCount == 0) {
        printf("No candidates available to vote.\n");
        return;
    }
    
    int choice;
    
    printf("===== CAST YOUR VOTE =====\n");
    viewCandidates();
    
    printf("\nEnter the ID of the candidate you want to vote for: ");
    scanf("%d", &choice);
    getchar(); // Clear input buffer
    
    if (choice < 1 || choice > candidateCount) {
        printf("Invalid candidate ID. Please try again.\n");
        return;
    }
    
    candidates[choice - 1].voteCount++;
    users[userIndex].hasVoted = 1;
    
    printf("Your vote has been cast successfully!\n");
}

// Function to view election results
void viewResults() {
    if (candidateCount == 0) {
        printf("No candidates available.\n");
        return;
    }
    
    int totalVotes = 0;
    for (int i = 0; i < candidateCount; i++) {
        totalVotes += candidates[i].voteCount;
    }
    
    printf("===== ELECTION RESULTS =====\n");
    printf("ID\tName\t\tVotes\tPercentage\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < candidateCount; i++) {
        float percentage = (totalVotes > 0) ? ((float)candidates[i].voteCount / totalVotes) * 100 : 0;
        printf("%d\t%s\t\t%d\t%.2f%%\n", candidates[i].id, candidates[i].name, candidates[i].voteCount, percentage);
    }
    
    printf("----------------------------------------\n");
    printf("Total Votes: %d\n", totalVotes);
}

// Function to save data to files (simulated)
void saveData() {
    printf("Saving data...\n");
    // In a real application, this would write to files
}

// Function to load data from files (simulated)
void loadData() {
    // In a real application, this would read from files
    
    // Add admin user (for demonstration)
    strcpy(users[0].id, "admin");
    strcpy(users[0].name, "Administrator");
    strcpy(users[0].password, "Admin123");
    users[0].hasVoted = 0;
    userCount = 1;
    
    // Add some sample candidates (for demonstration)
    strcpy(candidates[0].name, "Bishal");
    candidates[0].id = 1;
    candidates[0].voteCount = 0;
    
    strcpy(candidates[1].name, "Anup");
    candidates[1].id = 2;
    candidates[1].voteCount = 0;
    
    candidateCount = 2;
}

// Function to clear the screen (simulated)
void clearScreen() {
    // In a real application, this would use system("cls") or system("clear")
    printf("\n\n");
}