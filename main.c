/*
 * ╔══════════════════════════════════════════════════════╗
 * ║       BUS TICKET RESERVATION + CANCELLATION         ║
 * ║              C Mini Project — DA1                   ║
 * ╠══════════════════════════════════════════════════════╣
 * ║  Student Name  : [Your Name]                        ║
 * ║  Register No   : [Your RegNo]                       ║
 * ║  Department    : [Your Dept]                        ║
 * ╚══════════════════════════════════════════════════════╝
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── CONSTANTS ──────────────────────────────────────── */
#define MAX_ROUTES   3
#define MAX_SEATS    40
#define MAX_TICKETS  200
#define ROUTES_FILE  "routes.dat"
#define TICKETS_FILE "tickets.dat"

/* ── COLOUR CODES (works on most terminals + Replit) ── */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define DIM     "\033[2m"

/* ── STRUCTS ─────────────────────────────────────────── */
typedef struct {
    int   routeID;
    char  source[50];
    char  destination[50];
    float fare;
    int   totalSeats;
    int   seats[MAX_SEATS];   /* 0 = free, 1 = booked */
} Route;

typedef struct {
    char  ticketID[10];
    char  passengerName[50];
    int   routeID;
    int   seatNumbers[5];
    int   numSeats;
    float totalAmount;
    int   isCancelled;
} Ticket;

/* ── GLOBALS ─────────────────────────────────────────── */
Route  routes[MAX_ROUTES];
Ticket tickets[MAX_TICKETS];
int    totalRoutes  = 0;
int    totalTickets = 0;

/* ── PROTOTYPES ──────────────────────────────────────── */
void initializeRoutes();
void saveRoutes();    void loadRoutes();
void saveTickets();   void loadTickets();
int  findRouteIndex(int routeID);
int  findTicketIndex(char *ticketID);
void displayRoutes();
void displayAvailableSeats(int routeID);
void bookTicket();
void cancelTicket();
void searchTicket();
void dailyCollectionReport();
void printMenu();
void printBanner();
void printDivider(char ch, int len);
void clearInputBuffer();
int  countFreeSeats(int rIdx);

/* ════════════════════════════════════════════════════════
   MAIN
════════════════════════════════════════════════════════ */
int main() {
    printBanner();
    loadRoutes();
    loadTickets();

    int choice;
    do {
        printMenu();
        printf(CYAN "  =>  Enter choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "\n  [X] Invalid input -- please enter a number.\n" RESET);
            clearInputBuffer();
            choice = -1;
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: displayRoutes();         break;
            case 2: {
                int rid;
                displayRoutes();
                printf(CYAN "\n  =>  Enter Route ID to view seats: " RESET);
                if (scanf("%d", &rid) != 1) {
                    printf(RED "  [X] Invalid input.\n" RESET);
                    clearInputBuffer(); break;
                }
                clearInputBuffer();
                if (findRouteIndex(rid) == -1)
                    printf(RED "\n  [X] Route not found.\n" RESET);
                else
                    displayAvailableSeats(rid);
                break;
            }
            case 3: bookTicket();            break;
            case 4: cancelTicket();          break;
            case 5: searchTicket();          break;
            case 6: dailyCollectionReport(); break;
            case 0:
                printf(GREEN "\n");
                printf("  +====================================================+\n");
                printf("  |    Thank you! Have a safe journey!  :)             |\n");
                printf("  +====================================================+\n");
                printf(RESET "\n");
                break;
            default:
                printf(RED "\n  [X] Invalid choice. Please try again.\n" RESET);
        }
    } while (choice != 0);

    return 0;
}

/* ── VISUALS ─────────────────────────────────────────── */

void printDivider(char ch, int len) {
    for (int i = 0; i < len; i++) putchar(ch);
    putchar('\n');
}

void printBanner() {
    printf(BLUE "\n");
    printf("  +==================================================+\n");
    printf("  |                                                  |\n");
    printf("  |   " YELLOW "  [BUS]  SPEEDLINE BUS RESERVATION SYSTEM      " BLUE "|\n");
    printf("  |                                                  |\n");
    printf("  |   " WHITE "       Book . Cancel . Track . Report           " BLUE "|\n");
    printf("  |                                                  |\n");
    printf("  +==================================================+\n");
    printf(RESET "\n");
}

void printMenu() {
    printf(BLUE "\n  +======================================+\n");
    printf(     "  |        " YELLOW "***  MAIN MENU  ***            " BLUE "|\n");
    printf(     "  +======================================+\n");
    printf(     "  |  " GREEN " 1" RESET " |  View All Routes              " BLUE "|\n");
    printf(     "  |  " GREEN " 2" RESET " |  View Seats for a Route       " BLUE "|\n");
    printf(     "  |  " GREEN " 3" RESET " |  Book Ticket                  " BLUE "|\n");
    printf(     "  |  " GREEN " 4" RESET " |  Cancel Ticket                " BLUE "|\n");
    printf(     "  |  " GREEN " 5" RESET " |  Search Ticket by ID          " BLUE "|\n");
    printf(     "  |  " GREEN " 6" RESET " |  Daily Collection Report      " BLUE "|\n");
    printf(     "  |  " RED  " 0" RESET " |  Exit                         " BLUE "|\n");
    printf(     "  +======================================+\n" RESET);
}

/* ── UTILITIES ───────────────────────────────────────── */

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int findRouteIndex(int routeID) {
    for (int i = 0; i < totalRoutes; i++)
        if (routes[i].routeID == routeID) return i;
    return -1;
}

int findTicketIndex(char *ticketID) {
    for (int i = 0; i < totalTickets; i++)
        if (strcmp(tickets[i].ticketID, ticketID) == 0) return i;
    return -1;
}

int countFreeSeats(int rIdx) {
    int count = 0;
    for (int s = 0; s < routes[rIdx].totalSeats; s++)
        if (routes[rIdx].seats[s] == 0) count++;
    return count;
}

/* ── INIT ROUTES ─────────────────────────────────────── */

void initializeRoutes() {
    totalRoutes = 3;

    routes[0].routeID = 1;
    strcpy(routes[0].source,      "Chennai");
    strcpy(routes[0].destination, "Bangalore");
    routes[0].fare       = 450.0;
    routes[0].totalSeats = 40;
    memset(routes[0].seats, 0, sizeof(routes[0].seats));

    routes[1].routeID = 2;
    strcpy(routes[1].source,      "Chennai");
    strcpy(routes[1].destination, "Hyderabad");
    routes[1].fare       = 700.0;
    routes[1].totalSeats = 40;
    memset(routes[1].seats, 0, sizeof(routes[1].seats));

    routes[2].routeID = 3;
    strcpy(routes[2].source,      "Bangalore");
    strcpy(routes[2].destination, "Hyderabad");
    routes[2].fare       = 350.0;
    routes[2].totalSeats = 30;
    memset(routes[2].seats, 0, sizeof(routes[2].seats));

    saveRoutes();
    printf(GREEN "  [OK] Default routes initialized.\n" RESET);
}

/* ── FILE I/O ────────────────────────────────────────── */

void saveRoutes() {
    FILE *fp = fopen(ROUTES_FILE, "wb");
    if (!fp) { printf(RED "  [X] Error saving routes.\n" RESET); return; }
    fwrite(&totalRoutes, sizeof(int), 1, fp);
    fwrite(routes, sizeof(Route), totalRoutes, fp);
    fclose(fp);
}

void loadRoutes() {
    FILE *fp = fopen(ROUTES_FILE, "rb");
    if (!fp) { initializeRoutes(); return; }
    fread(&totalRoutes, sizeof(int), 1, fp);
    fread(routes, sizeof(Route), totalRoutes, fp);
    fclose(fp);
    printf(GREEN "  [OK] Routes loaded.\n" RESET);
}

void saveTickets() {
    FILE *fp = fopen(TICKETS_FILE, "wb");
    if (!fp) { printf(RED "  [X] Error saving tickets.\n" RESET); return; }
    fwrite(&totalTickets, sizeof(int), 1, fp);
    fwrite(tickets, sizeof(Ticket), totalTickets, fp);
    fclose(fp);
}

void loadTickets() {
    FILE *fp = fopen(TICKETS_FILE, "rb");
    if (!fp) { totalTickets = 0; return; }
    fread(&totalTickets, sizeof(int), 1, fp);
    fread(tickets, sizeof(Ticket), totalTickets, fp);
    fclose(fp);
    printf(GREEN "  [OK] %d booking(s) loaded.\n" RESET, totalTickets);
}

/* ── DISPLAY ROUTES ──────────────────────────────────── */

void displayRoutes() {
    printf(BLUE "\n  +------+---------------+---------------+----------+------------+\n");
    printf(     "  | " YELLOW " ID  " BLUE " | " YELLOW " FROM          " BLUE " | " YELLOW " TO            " BLUE " | " YELLOW " FARE     " BLUE " | " YELLOW " SEATS      " BLUE " |\n");
    printf(     "  +------+---------------+---------------+----------+------------+\n" RESET);

    for (int i = 0; i < totalRoutes; i++) {
        int avail = countFreeSeats(i);
        const char *seatColor = (avail > 10) ? GREEN : (avail > 0) ? YELLOW : RED;

        printf(BLUE "  | " WHITE " %2d   " BLUE
                    "| " WHITE " %-13s " BLUE
                    "| " WHITE " %-13s " BLUE
                    "| " WHITE " Rs.%4.0f  " BLUE
                    "|  %s%2d/%-2d%s avail  " BLUE "|\n",
               routes[i].routeID,
               routes[i].source,
               routes[i].destination,
               routes[i].fare,
               seatColor, avail, routes[i].totalSeats, BLUE);
    }
    printf("  +------+---------------+---------------+----------+------------+\n" RESET);
}

/* ── SEAT LAYOUT ─────────────────────────────────────── */

void displayAvailableSeats(int routeID) {
    int idx = findRouteIndex(routeID);
    if (idx == -1) { printf(RED "  [X] Route not found.\n" RESET); return; }

    int free = countFreeSeats(idx);

    printf(YELLOW "\n  [BUS]  %s  -->  %s\n" RESET, routes[idx].source, routes[idx].destination);
    printf(DIM    "  Seats: %d total | " GREEN "%d available" DIM " | " RED "%d booked\n\n" RESET,
           routes[idx].totalSeats, free, routes[idx].totalSeats - free);
    printf(DIM    "  Legend:  " GREEN "[ ##]" RESET DIM " = Available     " RED "[ ##]" RESET DIM " = Booked\n\n" RESET);

    for (int i = 0; i < routes[idx].totalSeats; i++) {
        if (i % 5 == 0) printf("  ");
        if (routes[idx].seats[i] == 0)
            printf(GREEN "[%3d]" RESET, i + 1);
        else
            printf(RED   "[%3d]" RESET, i + 1);
        printf(" ");
        if ((i + 1) % 5 == 0) {
            if ((i + 1) % 10 == 0 && (i + 1) < routes[idx].totalSeats)
                printf(DIM "\n  ------- AISLE -------\n" RESET);
            else
                printf("\n");
        }
    }
    printf("\n");
}

/* ── BOOK TICKET ─────────────────────────────────────── */

void bookTicket() {
    printf(YELLOW "\n  +==============================+\n");
    printf(        "  |   [T]  BOOK A TICKET          |\n");
    printf(        "  +==============================+\n" RESET);

    displayRoutes();

    int routeID;
    printf(CYAN "\n  =>  Enter Route ID: " RESET);
    if (scanf("%d", &routeID) != 1) {
        printf(RED "  [X] Invalid input.\n" RESET);
        clearInputBuffer(); return;
    }
    clearInputBuffer();

    int rIdx = findRouteIndex(routeID);
    if (rIdx == -1) { printf(RED "  [X] Route not found.\n" RESET); return; }

    int freeCount = countFreeSeats(rIdx);
    if (freeCount == 0) {
        printf(RED "\n  [X] Sorry! Route is fully booked.\n" RESET);
        return;
    }

    int numSeats;
    printf(CYAN "  =>  How many seats? (1-5): " RESET);
    if (scanf("%d", &numSeats) != 1 || numSeats < 1 || numSeats > 5) {
        printf(RED "  [X] Invalid. Enter between 1 and 5.\n" RESET);
        clearInputBuffer(); return;
    }
    clearInputBuffer();

    if (numSeats > freeCount) {
        printf(RED "  [X] Only %d seat(s) available on this route.\n" RESET, freeCount);
        return;
    }

    if (totalTickets >= MAX_TICKETS) {
        printf(RED "  [X] Ticket database is full.\n" RESET); return;
    }

    displayAvailableSeats(routeID);

    Ticket t;
    t.numSeats    = numSeats;
    t.routeID     = routeID;
    t.isCancelled = 0;
    int picked    = 0;

    for (int i = 0; i < numSeats; i++) {
        int seatNo;
        printf(CYAN "  =>  Seat number %d of %d: " RESET, i + 1, numSeats);
        if (scanf("%d", &seatNo) != 1) {
            printf(RED "  [X] Invalid.\n" RESET);
            clearInputBuffer(); i--; continue;
        }
        clearInputBuffer();

        if (seatNo < 1 || seatNo > routes[rIdx].totalSeats) {
            printf(RED "  [X] Seat %d out of range (1-%d).\n" RESET, seatNo, routes[rIdx].totalSeats);
            i--; continue;
        }
        if (routes[rIdx].seats[seatNo - 1] == 1) {
            printf(RED "  [X] Seat %d is already booked. Choose another.\n" RESET, seatNo);
            i--; continue;
        }
        int dup = 0;
        for (int j = 0; j < picked; j++) {
            if (t.seatNumbers[j] == seatNo) { dup = 1; break; }
        }
        if (dup) {
            printf(RED "  [X] Seat %d already selected in this booking.\n" RESET, seatNo);
            i--; continue;
        }

        t.seatNumbers[picked++] = seatNo;
        routes[rIdx].seats[seatNo - 1] = 1;
        printf(GREEN "  [OK] Seat %d reserved.\n" RESET, seatNo);
    }

    printf(CYAN "  =>  Passenger name: " RESET);
    fgets(t.passengerName, sizeof(t.passengerName), stdin);
    t.passengerName[strcspn(t.passengerName, "\n")] = '\0';
    if (strlen(t.passengerName) == 0) strcpy(t.passengerName, "Unknown");

    snprintf(t.ticketID, sizeof(t.ticketID), "TK%03d", totalTickets + 1);
    t.totalAmount = numSeats * routes[rIdx].fare;

    tickets[totalTickets++] = t;
    saveTickets();
    saveRoutes();

    /* Receipt */
    char routeStr[40], seatStr[30] = "", amtStr[20];
    snprintf(routeStr, sizeof(routeStr), "%s --> %s",
             routes[rIdx].source, routes[rIdx].destination);
    for (int i = 0; i < numSeats; i++) {
        char tmp[6];
        snprintf(tmp, sizeof(tmp), "%d ", t.seatNumbers[i]);
        strncat(seatStr, tmp, sizeof(seatStr) - strlen(seatStr) - 1);
    }
    snprintf(amtStr, sizeof(amtStr), "Rs. %.2f", t.totalAmount);

    printf(GREEN "\n  +==========================================+\n");
    printf(       "  |        [OK]  BOOKING CONFIRMED!           |\n");
    printf(       "+============================================+\n");
    printf(       "  |  Ticket ID    : " WHITE "%-24s" GREEN "|\n", t.ticketID);
    printf(       "  |  Passenger    : " WHITE "%-24s" GREEN "|\n", t.passengerName);
    printf(       "  |  Route        : " WHITE "%-24s" GREEN "|\n", routeStr);
    printf(       "  |  Seat(s)      : " WHITE "%-24s" GREEN "|\n", seatStr);
    printf(       "  |  Total Amount : " WHITE "%-24s" GREEN "|\n", amtStr);
    printf(       "  +==========================================+\n");
    printf(       "  |   Keep your Ticket ID safe for records! |\n");
    printf(       "  +==========================================+\n" RESET);
}

/* ── CANCEL TICKET ───────────────────────────────────── */

void cancelTicket() {
    printf(YELLOW "\n  +==============================+\n");
    printf(        "  |   [X]  CANCEL A TICKET        |\n");
    printf(        "  +==============================+\n" RESET);

    char ticketID[10];
    printf(CYAN "  =>  Enter Ticket ID (e.g. TK001): " RESET);
    scanf("%s", ticketID);
    clearInputBuffer();

    int tIdx = findTicketIndex(ticketID);
    if (tIdx == -1) {
        printf(RED "  [X] Ticket '%s' not found.\n" RESET, ticketID); return;
    }
    if (tickets[tIdx].isCancelled) {
        printf(RED "  [X] Ticket '%s' is already cancelled.\n" RESET, ticketID); return;
    }

    int rIdx = findRouteIndex(tickets[tIdx].routeID);
    if (rIdx != -1)
        for (int i = 0; i < tickets[tIdx].numSeats; i++)
            routes[rIdx].seats[tickets[tIdx].seatNumbers[i] - 1] = 0;

    tickets[tIdx].isCancelled = 1;
    saveTickets();
    saveRoutes();

    char amtStr[20];
    snprintf(amtStr, sizeof(amtStr), "Rs. %.2f", tickets[tIdx].totalAmount);

    printf(GREEN "\n  +==========================================+\n");
    printf(       "  |         TICKET CANCELLED                  |\n");
    printf(       "  +==========================================+\n");
    printf(       "  |  Ticket ID    : " WHITE "%-24s" GREEN "|\n", ticketID);
    printf(       "  |  Refund Amount: " WHITE "%-24s" GREEN "|\n", amtStr);
    printf(       "  +==========================================+\n" RESET);
}

/* ── SEARCH TICKET ───────────────────────────────────── */

void searchTicket() {
    printf(YELLOW "\n  +==============================+\n");
    printf(        "  |   [?]  SEARCH TICKET          |\n");
    printf(        "  +==============================+\n" RESET);

    char ticketID[10];
    printf(CYAN "  =>  Enter Ticket ID (e.g. TK001): " RESET);
    scanf("%s", ticketID);
    clearInputBuffer();

    int tIdx = findTicketIndex(ticketID);
    if (tIdx == -1) {
        printf(RED "  [X] Ticket '%s' not found.\n" RESET, ticketID); return;
    }

    Ticket *t    = &tickets[tIdx];
    int     rIdx = findRouteIndex(t->routeID);

    const char *statusColor = t->isCancelled ? RED   : GREEN;
    const char *statusText  = t->isCancelled ? "CANCELLED" : "CONFIRMED";

    char routeStr[40] = "N/A", seatStr[30] = "", amtStr[20];
    if (rIdx != -1)
        snprintf(routeStr, sizeof(routeStr), "%s --> %s",
                 routes[rIdx].source, routes[rIdx].destination);
    for (int i = 0; i < t->numSeats; i++) {
        char tmp[6];
        snprintf(tmp, sizeof(tmp), "%d ", t->seatNumbers[i]);
        strncat(seatStr, tmp, sizeof(seatStr) - strlen(seatStr) - 1);
    }
    snprintf(amtStr, sizeof(amtStr), "Rs. %.2f", t->totalAmount);

    printf(BLUE "\n  +==========================================+\n");
    printf(      "  |          [T]  TICKET DETAILS              |\n");
    printf(      "+============================================+\n");
    printf(      "  |  Ticket ID    : " WHITE "%-24s" BLUE  "|\n", t->ticketID);
    printf(      "  |  Passenger    : " WHITE "%-24s" BLUE  "|\n", t->passengerName);
    printf(      "  |  Route        : " WHITE "%-24s" BLUE  "|\n", routeStr);
    printf(      "  |  Seat(s)      : " WHITE "%-24s" BLUE  "|\n", seatStr);
    printf(      "  |  Amount Paid  : " WHITE "%-24s" BLUE  "|\n", amtStr);
    printf(      "  |  Status       : %s%-24s" BLUE         "|\n", statusColor, statusText);
    printf(      "  +==========================================+\n" RESET);
}

/* ── DAILY COLLECTION REPORT ─────────────────────────── */

void dailyCollectionReport() {
    printf(MAGENTA "\n  +==================================================+\n");
    printf(         "  |        [$$]  DAILY COLLECTION REPORT             |\n");
    printf(         "+====================================================+\n" RESET);

    float grandTotal = 0;

    for (int r = 0; r < totalRoutes; r++) {
        int   ticketCount = 0;
        int   seatsBooked = 0;
        float revenue     = 0;

        for (int t = 0; t < totalTickets; t++) {
            if (tickets[t].routeID == routes[r].routeID && !tickets[t].isCancelled) {
                seatsBooked += tickets[t].numSeats;
                revenue     += tickets[t].totalAmount;
                ticketCount++;
            }
        }

        int   bookedSeats = routes[r].totalSeats - countFreeSeats(r);
        int   availSeats  = routes[r].totalSeats - bookedSeats;
        float occ         = (routes[r].totalSeats > 0)
                            ? (bookedSeats * 100.0f / routes[r].totalSeats) : 0;

        /* ASCII progress bar (20 chars wide, 5% per block) */
        int  barFill = (int)(occ / 5);
        char bar[22] = "";
        for (int b = 0; b < 20; b++)
            bar[b] = (b < barFill) ? '#' : '-';
        bar[20] = '\0';

        const char *occColor = (occ > 80) ? RED : (occ > 40) ? YELLOW : GREEN;

        printf(MAGENTA "  +--------------------------------------------------+\n");
        printf(         "  |  Route %d:  " WHITE "%s  -->  %s\n",
               routes[r].routeID, routes[r].source, routes[r].destination);
        printf(MAGENTA  "  |\n");
        printf(         "  |  Tickets Booked  : " WHITE "%-6d"
                        MAGENTA "   Available : " WHITE "%d / %d\n",
               ticketCount, availSeats, routes[r].totalSeats);
        printf(MAGENTA  "  |  Revenue         : " GREEN "Rs. %8.2f\n" RESET, revenue);
        printf(MAGENTA  "  |  Occupancy       : %s[%-20s]  %.1f%%\n" RESET,
               occColor, bar, occ);
        printf(MAGENTA  "  |\n");

        grandTotal += revenue;
    }

    char grandStr[30];
    snprintf(grandStr, sizeof(grandStr), "Rs. %.2f", grandTotal);

    printf(MAGENTA "  +==================================================+\n");
    printf(         "  |  [$$] TOTAL REVENUE (ALL ROUTES):  " GREEN "%-14s" MAGENTA "|\n", grandStr);
    printf(         "  +==================================================+\n" RESET);
}