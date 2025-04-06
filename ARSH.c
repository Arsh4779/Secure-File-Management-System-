#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_USERS 1000
#define MAX_FILES 100

typedef struct {
    char username[50];
    char password[50];
    char role[10]; // "admin" or "user"
} User;

User users[MAX_USERS];
int user_count = 0;

char current_user[50];
char current_role[10];

// Log action to file
void log_action(const char *action) {
    FILE *log = fopen("log.txt", "a");
    time_t now = time(NULL);
    fprintf(log, "[%s] %s by %s (%s)\n", strtok(ctime(&now), "\n"), action, current_user, current_role);
    fclose(log);
}

// Load users from users.txt
void load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf("Error: users.txt file not found.\n");
        exit(1);
    }

    char line[150];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (sscanf(line, "%49[^,],%49[^,],%9s", 
                   users[user_count].username, 
                   users[user_count].password, 
                   users[user_count].role) == 3) {
            user_count++;
        }
        if (user_count >= MAX_USERS) break;
    }
    fclose(fp);
}

// OTP generator for 2FA
int generate_otp() {
    srand(time(NULL));
    return 100000 + rand() % 900000;
}

// User authentication with 2FA
bool authenticate() {
    char username[50], password[50];
    printf("Username: "); scanf("%s", username);
    printf("Password: "); scanf("%s", password);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, users[i].username) == 0 && 
            strcmp(password, users[i].password) == 0) {
            int otp = generate_otp();
            printf("\nYour OTP is: %d\nEnter OTP: ", otp);
            int user_otp;
            scanf("%d", &user_otp);
            if (user_otp == otp) {
                strcpy(current_user, username);
                strcpy(current_role, users[i].role);
                log_action("Login successful");
                return true;
            } else {
                printf("Incorrect OTP.\n");
                return false;
            }
        }
    }
    printf("Invalid credentials.\n");
    return false;
}

// File operations
void read_file() {
    char filename[50];
    printf("Enter filename to read: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("File not found.\n");
        log_action("Failed to read file");
        return;
    }
    char ch;
    while ((ch = fgetc(fp)) != EOF) putchar(ch);
    fclose(fp);
    log_action("Read file");
}

void write_file() {
    char filename[50], data[100];
    printf("Enter filename to write: ");
    scanf("%s", filename);
    printf("Enter data: ");
    scanf(" %[^"]", data);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write file.\n");
        log_action("Failed to write file");
        return;
    }
    fprintf(fp, "%s", data);
    fclose(fp);
    log_action("Wrote to file");
}

void share_file() {
    char filename[50], user[50];
    printf("Enter filename to share: ");
    scanf("%s", filename);
    printf("Share with user: ");
    scanf("%s", user);
    // Simulate sharing
    printf("File '%s' shared with '%s'.\n", filename, user);
    log_action("Shared file");
}

void view_metadata() {
    char filename[50];
    printf("Enter filename to view metadata: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("File not found.\n");
        log_action("Failed to view metadata");
        return;
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    printf("File: %s | Size: %ld bytes\n", filename, size);
    log_action("Viewed metadata");
}

// Main menu
void show_menu() {
    int choice;
    do {
        printf("\n1. Read File\n2. Write File\n3. Share File\n4. View Metadata\n5. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: read_file(); break;
            case 2: write_file(); break;
            case 3: share_file(); break;
            case 4: view_metadata(); break;
            case 5: log_action("Logout"); printf("Logged out.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

int main() {
    load_users();
    if (authenticate()) {
        show_menu();
    }
    return 0;
}
