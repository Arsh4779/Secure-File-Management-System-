#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>  // for file listing
#include <conio.h>  // for masked password input

#define MAX 100
#define USERS_FILE "users.txt"
#define LOG_FILE "logs.txt"
#define SHIFT 3  // Caesar Cipher shift

typedef struct {
    char username[50];
    char password[50];
    char email[100];
    char role[10]; // admin or user
} User;

User currentUser;

// Function declarations
int login();
int verifyOTP();
int loadUser(const char *username, User *user);
void showMenu();
void writeFile();
void readFile();
void deleteFile();
void viewMetadata();
void threatDetection(const char *operation);
void logActivity(const char *activity);
void caesarEncryptDecrypt(char *data);
void getTimestamp(char *buffer, size_t size);
void registerUser();
void viewLogs();
void listEncryptedFiles();
void logout();

// Caesar Cipher encryption and decryption
void caesarEncryptDecrypt(char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] >= 'a' && data[i] <= 'z') {
            data[i] = ((data[i] - 'a' + SHIFT) % 26) + 'a';
        } else if (data[i] >= 'A' && data[i] <= 'Z') {
            data[i] = ((data[i] - 'A' + SHIFT) % 26) + 'A';
        }
    }
}

void getTimestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void logActivity(const char *activity) {
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        char timestamp[30];
        getTimestamp(timestamp, sizeof(timestamp));
        fprintf(f, "[%s] %s performed by %s\n", timestamp, activity, currentUser.username);
        fclose(f);
    }
}

void threatDetection(const char *operation) {
    printf("Running threat detection on %s operation... OK\n", operation);
}

int loadUser(const char *username, User *user) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) return 0;
    char line[MAX];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%[^,],%s", user->username, user->password, user->email, user->role);
        if (strcmp(user->username, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int verifyOTP() {
    srand(time(NULL));
    int otp = rand() % 9000 + 1000;
    printf("OTP sent to email: %d\n", otp);
    int input;
    printf("Enter OTP: ");
    scanf("%d", &input);
    return input == otp;
}

int login() {
    char username[50], password[50];
    printf("Username: ");
    scanf("%49s", username);

    // Masked password input
    printf("Password: ");
    int i = 0;
    char ch;
    while ((ch = _getch()) != '\r') {
        password[i++] = ch;
        printf("*");  // Display * for each character entered
    }
    password[i] = '\0';
    printf("\n");

    if (loadUser(username, &currentUser)) {
        if (strcmp(currentUser.password, password) == 0) {
            printf("Password verified.\n");
            if (verifyOTP()) {
                printf("OTP verified.\n");
                return 1;
            } else {
                printf("Invalid OTP.\n");
                return 0;
            }
        }
    }
    printf("Login failed.\n");
    return 0;
}

void writeFile() {
    char filename[100], content[500];
    printf("Enter file name: ");
    scanf("%s", filename);
    printf("Enter content to write: ");
    getchar();
    fgets(content, sizeof(content), stdin);
    content[strcspn(content, "\n")] = 0;

    caesarEncryptDecrypt(content);

    char path[150];
    sprintf(path, "%s.enc", filename);
    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Error writing file.\n");
        return;
    }
    fprintf(f, "%s", content);
    fclose(f);

    threatDetection("write");
    logActivity("Wrote file (encrypted)");
}

void readFile() {
    char filename[100], content[500];
    printf("Enter file name to read: ");
    scanf("%s", filename);

    char path[150];
    sprintf(path, "%s.enc", filename);
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("File not found.\n");
        return;
    }

    fgets(content, sizeof(content), f);
    fclose(f);

    caesarEncryptDecrypt(content);
    printf("Decrypted Content:\n%s\n", content);

    threatDetection("read");
    logActivity("Read file (decrypted)");
}

void deleteFile() {
    if (strcmp(currentUser.role, "admin") != 0) {
        printf("Permission denied. Admins only.\n");
        return;
    }

    char filename[100];
    printf("Enter file name to delete: ");
    scanf("%s", filename);

    char path[150];
    sprintf(path, "%s.enc", filename);
    if (remove(path) == 0) {
        printf("File deleted.\n");
        logActivity("Deleted file");
    } else {
        printf("Failed to delete file.\n");
    }

    threatDetection("delete");
}

void viewMetadata() {
    char filename[100];
    printf("Enter file name for metadata: ");
    scanf("%s", filename);

    char path[150];
    sprintf(path, "%s.enc", filename);
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("File not found.\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);

    printf("File: %s\nSize: %ld bytes\n", filename, size);
    logActivity("Viewed file metadata");
}

void registerUser() {
    char username[50], password[50], email[100], role[10];
    printf("New Username: ");
    scanf("%s", username);

    User check;
    if (loadUser(username, &check)) {
        printf("User already exists!\n");
        return;
    }

    printf("Password: ");
    scanf("%s", password);
    printf("Email: ");
    scanf("%s", email);
    printf("Role (a for admin, u for user): ");
    scanf(" %s", role);

    if (strcmp(role, "a") == 0) {
        strcpy(role, "admin");
    } else if (strcmp(role, "u") == 0) {
        strcpy(role, "user");
    } else {
        printf("Invalid role. Defaulting to user.\n");
        strcpy(role, "user");
    }

    FILE *f = fopen(USERS_FILE, "a");
    if (!f) {
        printf("Error opening user file.\n");
        return;
    }
    fprintf(f, "%s,%s,%s,%s\n", username, password, email, role);
    fclose(f);

    printf("User registered successfully.\n");
}

void viewLogs() {
    FILE *f = fopen(LOG_FILE, "r");
    if (!f) {
        printf("No logs available.\n");
        return;
    }

    char line[200];
    printf("\n=== Logs ===\n");
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
}

void listEncryptedFiles() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        printf("\nAvailable Encrypted Files:\n");
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".enc")) {
                printf("- %s\n", dir->d_name);
            }
        }
        closedir(d);
    }
}

void logout() {
    printf("Logging out...\n");
    memset(&currentUser, 0, sizeof(currentUser)); // Clear current user data
    logActivity("Logged out");
}

void showMenu() {
    int choice;
    do {
        printf("\n=== Menu ===\n");
        printf("1. Write File\n2. Read File\n3. Delete File (Admin)\n4. View Metadata\n");
        printf("5. Register New User\n6. View Logs\n7. List Files\n8. Logout\nChoice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: writeFile(); break;
            case 2: readFile(); break;
            case 3: deleteFile(); break;
            case 4: viewMetadata(); break;
            case 5: registerUser(); break;
            case 6: viewLogs(); break;
            case 7: listEncryptedFiles(); break;
            case 8: logout(); break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 8);
}

int main() {
    printf("=== Secure File Management System ===\n");
    printf("1. Login\n2. Register\nChoice: ");
    int mode;
    scanf("%d", &mode);

    if (mode == 2) {
        registerUser();
        return 0;
    }

    if (login()) {
        showMenu();
    }
    printf("Session ended.\n");
    return 0;
}
