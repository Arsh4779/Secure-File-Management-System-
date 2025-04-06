# Secure File Management System (C Language)

A secure file management system implemented in C, featuring user authentication with two-factor verification (OTP), file access control, logging, and metadata operations.

## Features

- **Authentication:**
  - Password-based login
  - OTP (One-Time Password) verification for two-factor authentication
- **Access Control:**
  - User roles from `users.txt`
- **Secure File Operations:**
  - Read file content
  - Write content to file
  - Share file (simulated)
  - View file metadata
- **Logging:**
  - All actions (login, read, write, share, metadata, logout) are logged in `log.txt`

## File Structure

├── main.c          # Main source code
├── users.txt       # User credentials file
├── log.txt         # Action log file (generated at runtime)
├── README.md       # Project documentation

## Sample `users.txt`

Each line in `users.txt` should follow the format:

username,password,role

**Example:**

alice,alice123,admin  
bob,pass456,user

You can generate up to 1000 users using a script or randomly generated entries.

## How to Run

### Compile

gcc main.c -o secure_fs

### Run

./secure_fs

## Security Highlights

- OTP verification prevents unauthorized access
- Logs every action with timestamp and filename
- Gracefully handles read/write errors with logging

## Sample Log (`log.txt`)

[Sun Apr  6 15:35:10 2025] User: alice | Action: Login | File: -  
[Sun Apr  6 15:36:01 2025] User: alice | Action: Read | File: notes.txt  
[Sun Apr  6 15:38:10 2025] User: alice | Action: Logout | File: -

## Future Improvements

- Role-based access to file operations
- File encryption using OpenSSL
- GUI or Web Interface
- Malware scanning on upload

## License

This project is licensed under the MIT License.
