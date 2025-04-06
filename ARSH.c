#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define MAX 1000
#define ADMIN_PASS "admin123"
#define USER_PASS "user123"
#define OTP "123456" // Simulated OTP for 2FA
#define XOR_KEY 0xAA  // XOR key for encryption

// Function Prototypes
int authenticate();
void generate_otp();
void write_file();
void read_file();
void delete_file();
void view_metadata();
void encrypt_decrypt(char *data);
int threat_detected(const char *content);

int main() {
    int role = authenticate(); // 1 = admin, 2 = user, 0 = fail
    if (role == 0) return 0;

    int choice;
    while (1) {
        printf("\n--- Secure File Management System ---\n");
        printf("1. Write File\n2. Read File\n3. Delete File (Admin only)\n4. View Metadata\n5. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // flush newline

        switch (choice) {
            case 1: write_file(); break;
            case 2: read_file(); break;
            case 3:
                if (role == 1) delete_file();
                else printf("Access Denied: Only admin can delete files.\n");
                break;
            case 4: view_metadata(); break;
            case 5: printf("Session ended.\n"); return 0;
            default: printf("Invalid option.\n");
        }
    }
    return 0;
}

int authenticate() {
    char username[50], password[50], otp_input[10];

    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);

    if ((strcmp(username, "admin") == 0 && strcmp(password, ADMIN_PASS) == 0) ||
        (strcmp(username, "user") == 0 && strcmp(password, USER_PASS) == 0)) {
        
        generate_otp();
        printf("Enter OTP: ");
        scanf("%s", otp_input);

        if (strcmp(otp_input, OTP) != 0) {
            printf("Incorrect OTP.\n");
            return 0;
        }

        printf("Login successful. Role: %s\n", strcmp(username, "admin") == 0 ? "Admin" : "User");
        return strcmp(username, "admin") == 0 ? 1 : 2;
    } else {
        printf("Authentication failed.\n");
        return 0;
    }
}

void generate_otp() {
    printf("[Simulated] OTP sent to your email: %s\n", OTP);
}

void encrypt_decrypt(char *data) {
    for (int i = 0; i < strlen(data); i++) {
        data[i] ^= XOR_KEY;
    }
}

int threat_detected(const char *content) {
    return strstr(content, "overflow") != NULL || strstr(content, "malware") != NULL;
}

void write_file() {
    char filename[100], content[MAX];
    printf("Enter filename to write: ");
    scanf("%s", filename);
    getchar(); // consume newline

    printf("Enter file content:\n");
    fgets(content, MAX, stdin);
    content[strcspn(content, "\n")] = '\0'; // remove newline

    if (threat_detected(content)) {
        printf("Threat detected in content! Operation aborted.\n");
        return;
    }

    encrypt_decrypt(content);

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error writing file.\n");
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
    if (fp == NULL) {
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
    struct stat stats;

    printf("Enter filename to view metadata: ");
    scanf("%s", filename);

    if (stat(filename, &stats) == 0) {
        printf("File Size: %ld bytes\n", stats.st_size);
        printf("Created: %s", ctime(&stats.st_ctime));
        printf("Last Modified: %s", ctime(&stats.st_mtime));
    } else {
        printf("Error retrieving file metadata.\n");
    }
}
