#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define MAX 1000
#define MAX_USERS 100
#define XOR_KEY 0xAA

typedef struct {
    char username[50];
    char password[50];
    char role[10]; // "admin" or "user"
} User;

User users[MAX_USERS];
int user_count = 0;

// Function Prototypes
void load_users();
int authenticate(char *role_out);
int verify_otp();
void generate_otp(char *otp);
void write_file();
void read_file();
void delete_file();
void view_metadata();
void encrypt_decrypt(char *data);
int threat_detected(const char *content);

int main() {
    char role[10];
    load_users();

    if (!authenticate(role)) return 0;
    if (!verify_otp()) return 0;

    int choice;
    while (1) {
        printf("\n--- Secure File Management System ---\n");
        printf("1. Write File\n2. Read File\n3. Delete File (Admin Only)\n4. View Metadata\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // flush newline

        switch (choice) {
            case 1: write_file(); break;
            case 2: read_file(); break;
            case 3:
                if (strcmp(role, "admin") == 0) delete_file();
                else printf("Access Denied: Only admin can delete files.\n");
                break;
            case 4: view_metadata(); break;
            case 5: printf("Session ended.\n"); return 0;
            default: printf("Invalid option.\n");
        }
    }
    return 0;
}

void load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf("User file not found.\n");
        exit(1);
    }

    while (fscanf(fp, "%[^,],%[^,],%s\n", users[user_count].username, users[user_count].password, users[user_count].role) == 3) {
        user_count++;
    }

    fclose(fp);
}

int authenticate(char *role_out) {
    char username[50], password[50];

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            strcpy(role_out, users[i].role);
            printf("Authentication successful. Role: %s\n", role_out);
            return 1;
        }
    }

    printf("Authentication failed.\n");
    return 0;
}

void generate_otp(char *otp) {
    srand(time(NULL));
    for (int i = 0; i < 6; i++) {
        otp[i] = '0' + rand() % 10;
    }
    otp[6] = '\0';
}

int verify_otp() {
    char generated[7], entered[10];
    generate_otp(generated);

    printf("[Simulated OTP sent]: %s\n", generated);
    printf("Enter OTP: ");
    scanf("%s", entered);

    if (strcmp(generated, entered) == 0) {
        printf("OTP Verified.\n");
        return 1;
    } else {
        printf("Incorrect OTP.\n");
        return 0;
    }
}

void encrypt_decrypt(char *data) {
    for (int i = 0; i < strlen(data); i++) {
        data[i] ^= XOR_KEY;
    }
}

int threat_detected(const char *content) {
    return strstr(content, "overflow") || strstr(content, "malware");
}

void write_file() {
    char filename[100], content[MAX];
    printf("Enter filename to write: ");
    scanf("%s", filename);
    getchar();

    printf("Enter file content:\n");
    fgets(content, MAX, stdin);
    content[strcspn(content, "\n")] = '\0';

    if (threat_detected(content)) {
        printf("Threat detected in content! Write aborted.\n");
        return;
    }

    encrypt_decrypt(content);
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Failed to write file.\n");
        return;
    }
    fputs(content, fp);
    fclose(fp);
    printf("File written and encrypted successfully.\n");
}

void read_file() {
    char filename[100], content[MAX];
    printf("Enter filename to read: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("File not found.\n");
        return;
    }

    fgets(content, MAX, fp);
    encrypt_decrypt(content);
    printf("Decrypted content:\n%s\n", content);
    fclose(fp);
}

void delete_file() {
    char filename[100];
    printf("Enter filename to delete: ");
    scanf("%s", filename);

    if (remove(filename) == 0)
        printf("File deleted successfully.\n");
    else
        printf("Failed to delete file.\n");
}

void view_metadata() {
    char filename[100];
    struct stat info;
    printf("Enter filename to view metadata: ");
    scanf("%s", filename);

    if (stat(filename, &info) == 0) {
        printf("Size: %ld bytes\n", info.st_size);
        printf("Created: %s", ctime(&info.st_ctime));
        printf("Last Modified: %s", ctime(&info.st_mtime));
    } else {
        printf("Failed to retrieve metadata.\n");
    }
}
