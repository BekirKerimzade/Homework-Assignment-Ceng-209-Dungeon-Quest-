#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INVENTORY 5
#define MAX_ROOMS 10
#define BUFFER_SIZE 100

// Struct definitions for game entities
typedef struct Item {
    char name[50];
} Item;

typedef struct Creature {
    char name[50];
    int health;
} Creature;

typedef struct Room {
    char description[100];
    Item *items[MAX_INVENTORY];
    Creature *creature;
    struct Room *up;
    struct Room *down;
    struct Room *left;
    struct Room *right;
} Room;

typedef struct Player {
    int health;
    int strength;
    Item *inventory[MAX_INVENTORY];
    Room *currentRoom;
} Player;

// Function declarations
void initializeGame(Player *player, Room **rooms);
void cleanupGame(Room **rooms, Player *player);
void handleCommand(Player *player, char *command);
void move(Player *player, char *direction);
void look(Player *player);
void inventory(Player *player);
void pickup(Player *player, char *itemName);
void attack(Player *player);
Room *createRoom(const char *description);
void connectRooms(Room *from, Room *to, const char *direction);

// Debug function
void debug(const char *message) {
    printf("[DEBUG]: %s\n", message);
}

// Main function
int main() {
    Player player;
    Room *rooms[MAX_ROOMS] = {0};
    char command[BUFFER_SIZE];

    initializeGame(&player, rooms);
    printf("Welcome to the Dungeon Adventure! Type 'help' for a list of commands.\n");

    while (1) {
        printf("\n> ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character

        if (strcmp(command, "exit") == 0) {
            printf("Exiting game. Goodbye!\n");
            break;
        }
        handleCommand(&player, command);
    }

    cleanupGame(rooms, &player);
    return 0;
}

// Game initialization
void initializeGame(Player *player, Room **rooms) {
    debug("Initializing game...");
    // Initialize player
    player->health = 100;
    player->strength = 10;
    for (int i = 0; i < MAX_INVENTORY; i++) {
        player->inventory[i] = NULL;
    }

    // Create rooms
    rooms[0] = createRoom("You are in a dark and damp dungeon room. Exits are to the north and east.");
    rooms[1] = createRoom("You are in a room with an old wooden chest. Exits are to the south.");
    rooms[2] = createRoom("You are in a room filled with cobwebs and broken pottery. Exits are to the west.");

    // Connect rooms
    connectRooms(rooms[0], rooms[1], "up");
    connectRooms(rooms[0], rooms[2], "right");

    // Assign items and creatures
    rooms[0]->items[0] = malloc(sizeof(Item));
    strcpy(rooms[0]->items[0]->name, "Sword");
    debug("Added Sword to Room 0.");

    rooms[1]->creature = malloc(sizeof(Creature));
    strcpy(rooms[1]->creature->name, "Goblin");
    rooms[1]->creature->health = 30;
    debug("Added Goblin to Room 1.");

    // Set initial room
    player->currentRoom = rooms[0];
    debug("Player set to initial room.");
}

void cleanupGame(Room **rooms, Player *player) {
    debug("Cleaning up game resources...");
    // Free rooms and their contents
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i]) {
            for (int j = 0; j < MAX_INVENTORY; j++) {
                if (rooms[i]->items[j]) {
                    free(rooms[i]->items[j]);
                }
            }
            if (rooms[i]->creature) {
                free(rooms[i]->creature);
            }
            free(rooms[i]);
        }
    }

    // Free player inventory
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (player->inventory[i]) {
            free(player->inventory[i]);
        }
    }
}

// Command handling
void handleCommand(Player *player, char *command) {
    debug("Handling command...");
    if (strncmp(command, "move ", 5) == 0) {
        move(player, command + 5);
    } else if (strcmp(command, "look") == 0) {
        look(player);
    } else if (strcmp(command, "inventory") == 0) {
        inventory(player);
    } else if (strncmp(command, "pickup ", 7) == 0) {
        pickup(player, command + 7);
    } else if (strcmp(command, "attack") == 0) {
        attack(player);
    } else if (strcmp(command, "help") == 0) {
        printf("Available commands:\n");
        printf("  move <direction>  - Move to another room (directions: up, down, left, right)\n");
        printf("  look              - Look around the current room\n");
        printf("  inventory         - Check your inventory\n");
        printf("  pickup <item>     - Pick up an item in the room\n");
        printf("  attack            - Attack a creature in the room\n");
        printf("  help              - Show this help message\n");
        printf("  exit              - Exit the game\n");
    } else {
        printf("Unknown command. Try 'move', 'look', 'inventory', 'pickup', 'attack', 'help', or 'exit'.\n");
    }
}

// Command implementations
void move(Player *player, char *direction) {
    debug("Executing move command...");
    Room *nextRoom = NULL;
    if (strcmp(direction, "up") == 0) nextRoom = player->currentRoom->up;
    else if (strcmp(direction, "down") == 0) nextRoom = player->currentRoom->down;
    else if (strcmp(direction, "left") == 0) nextRoom = player->currentRoom->left;
    else if (strcmp(direction, "right") == 0) nextRoom = player->currentRoom->right;

    if (nextRoom) {
        player->currentRoom = nextRoom;
        printf("You move %s.\n", direction);
        look(player);
    } else {
        printf("You can't move %s from here.\n", direction);
    }
}

void look(Player *player) {
    debug("Executing look command...");
    printf("%s\n", player->currentRoom->description);
    printf("Items in the room: ");
    int hasItems = 0;
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (player->currentRoom->items[i]) {
            printf("%s ", player->currentRoom->items[i]->name);
            hasItems = 1;
        }
    }
    if (!hasItems) printf("None");
    printf("\n");

    if (player->currentRoom->creature) {
        printf("A %s is here!\n", player->currentRoom->creature->name);
    }
}

void inventory(Player *player) {
    debug("Executing inventory command...");
    printf("Your inventory: ");
    int hasItems = 0;
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (player->inventory[i]) {
            printf("%s ", player->inventory[i]->name);
            hasItems = 1;
        }
    }
    if (!hasItems) printf("Empty");
    printf("\n");
}

void pickup(Player *player, char *itemName) {
    debug("Executing pickup command...");
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (player->currentRoom->items[i] && strcmp(player->currentRoom->items[i]->name, itemName) == 0) {
            for (int j = 0; j < MAX_INVENTORY; j++) {
                if (!player->inventory[j]) {
                    player->inventory[j] = player->currentRoom->items[i];
                    player->currentRoom->items[i] = NULL;
                    printf("You picked up %s.\n", itemName);
                    return;
                }
            }
            printf("Your inventory is full!\n");
            return;
        }
    }
    printf("Item not found in the room.\n");
}

void attack(Player *player) {
    debug("Executing attack command...");
    if (player->currentRoom->creature) {
        Creature *creature = player->currentRoom->creature;
        printf("You attack the %s!\n", creature->name);
        creature->health -= player->strength;
        if (creature->health <= 0) {
            printf("You defeated the %s!\n", creature->name);
            free(creature);
            player->currentRoom->creature = NULL;
        } else {
            printf("The %s attacks you back!\n", creature->name);
            player->health -= 10; // Example damage
            if (player->health <= 0) {
                printf("You have been defeated. Game over!\n");
                exit(0);
            }
        }
    } else {
        printf("There is nothing to attack here.\n");
    }
}

Room *createRoom(const char *description) {
    debug("Creating room...");
    Room *room = malloc(sizeof(Room));
    strcpy(room->description, description);
    room->up = room->down = room->left = room->right = NULL;
    for (int i = 0; i < MAX_INVENTORY; i++) {
        room->items[i] = NULL;
    }
    room->creature = NULL;
    return room;
}

void connectRooms(Room *from, Room *to, const char *direction) {
    debug("Connecting rooms...");
    if (strcmp(direction, "up") == 0) {
        from->up = to;
        to->down = from;
    } else if (strcmp(direction, "down") == 0) {
        from->down = to;
        to->up = from;
    } else if (strcmp(direction, "left") == 0) {
        from->left = to;
        to->right = from;
    } else if (strcmp(direction, "right") == 0) {
        from->right = to;
        to->left = from;
    }
}
