#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_USERS 1000

typedef struct {
    char username[50];
    char password[50];
    char role[10];
} User;

User users[MAX_USERS];
int user_count = 0;

char current_user[50];
char current_role[10];

void log_action(const char *username, const char *action, const char *filename) {
    FILE *log = fopen("log.txt", "a");
    if (!log) return;
    time_t now = time(NULL);
    fprintf(log, "[%s] User: %s | Action: %s | File: %s\n", strtok(ctime(&now), "\n"), username, action, filename);
    fclose(log);
}

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

int generate_otp() {
    srand(time(NULL));
    return 100000 + rand() % 900000;
}

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
                log_action(username, "Login", "-");
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

void read_file(const char *username) {
    char filename[100];
    printf("Enter filename to read: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: File not found.\n");
        log_action(username, "Read-Failed", filename);
        return;
    }

    printf("\n--- File Content ---\n");
    char ch;
    while ((ch = fgetc(fp)) != EOF) putchar(ch);
    fclose(fp);
    printf("\n--- End of File ---\n");

    log_action(username, "Read", filename);
}

void write_file(const char *username) {
    char filename[100];
    char data[1000];

    printf("Enter filename to write: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter content to write (type END on a new line to finish):\n");
    getchar();
    while (1) {
        fgets(data, sizeof(data), stdin);
        if (strncmp(data, "END", 3) == 0)
            break;
        fputs(data, fp);
    }
    fclose(fp);
    log_action(username, "Write", filename);
    printf("File written successfully.\n");
}

void share_file(const char *username) {
    char filename[100], target_user[50];
    printf("Enter filename to share: ");
    scanf("%s", filename);
    printf("Enter username to share with: ");
    scanf("%s", target_user);
    printf("File '%s' shared with '%s'. (Simulation)\n", filename, target_user);
    log_action(username, "Share", filename);
}

void view_metadata(const char *username) {
    char filename[100];
    printf("Enter filename to view metadata: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("File not found.\n");
        log_action(username, "Metadata-Failed", filename);
        return;
    }
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    printf("File: %s | Size: %ld bytes\n", filename, size);
    log_action(username, "Metadata", filename);
}
void menu() {
    int choice;
    do {
        printf("\n1. Read File\n2. Write File\n3. Share File\n4. View Metadata\n5. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: read_file(current_user); break;
            case 2: write_file(current_user); break;
            case 3: share_file(current_user); break;
            case 4: view_metadata(current_user); break;
            case 5: log_action(current_user, "Logout", "-"); printf("Logged out.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}
int main() {
    load_users();
    if (authenticate()) menu();
    return 0;
}
