# Notificataions
A simple text notification system

## Usage
Note that the interface may change in a future version. I'll also likely expose controls to the user to determine the size and duration of the notifications.

```cpp
// Import
RECOMP_IMPORT("ProxyMM_Notifications", void Notifications_Emit(const char* prefix, const char* msg, const char* suffix));

Notifications_Emit(
    "", // Prefix (Purple)
    "Hello World", // Main Message (white)
    "" // Suffix (Blue)
);
```
