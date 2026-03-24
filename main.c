#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#define MAX_ROUTES 50
#define MAX_SEATS 40
#define MAX_TICKETS 200
#define ROUTES_FILE "routes.dat"
#define TICKETS_FILE "tickets.dat"

typedef struct {
    int routeID;
    char source[50];
    char destination[50];
    char departureTime[20]; 
    float fare;
    int totalSeats;
    int busType; 
    int seats[MAX_SEATS]; 
} Route;

typedef struct {
    int ticketID;
    char passengerName[50];
    char phoneNumber[20]; // NEW: Added Phone Number
    int routeID;
    int seatNumbers[5];
    int numSeats;
    float totalAmount;
    int isCancelled;
} Ticket;

Route routes[MAX_ROUTES];
Ticket tickets[MAX_TICKETS];
int totalRoutes = 0;
int totalTickets = 0;

void saveRoutes() {
    FILE *fp = fopen(ROUTES_FILE, "wb");
    if (fp) { fwrite(&totalRoutes, sizeof(int), 1, fp); fwrite(routes, sizeof(Route), totalRoutes, fp); fclose(fp); }
}

void loadRoutes() {
    FILE *fp = fopen(ROUTES_FILE, "rb");
    if (!fp) {
        totalRoutes = 5; 
        routes[0].routeID = 1; strcpy(routes[0].source, "Chennai"); strcpy(routes[0].destination, "Bangalore"); strcpy(routes[0].departureTime, "08:00 PM"); routes[0].fare = 600.0; routes[0].totalSeats = 40; routes[0].busType = 0; memset(routes[0].seats, 0, sizeof(routes[0].seats));
        routes[1].routeID = 2; strcpy(routes[1].source, "Chennai"); strcpy(routes[1].destination, "Bangalore"); strcpy(routes[1].departureTime, "10:30 PM"); routes[1].fare = 1200.0; routes[1].totalSeats = 30; routes[1].busType = 3; memset(routes[1].seats, 0, sizeof(routes[1].seats));
        routes[2].routeID = 3; strcpy(routes[2].source, "Chennai"); strcpy(routes[2].destination, "Hyderabad"); strcpy(routes[2].departureTime, "07:00 PM"); routes[2].fare = 800.0; routes[2].totalSeats = 40; routes[2].busType = 1; memset(routes[2].seats, 0, sizeof(routes[2].seats));
        routes[3].routeID = 4; strcpy(routes[3].source, "Chennai"); strcpy(routes[3].destination, "Hyderabad"); strcpy(routes[3].departureTime, "09:15 PM"); routes[3].fare = 1500.0; routes[3].totalSeats = 30; routes[3].busType = 3; memset(routes[3].seats, 0, sizeof(routes[3].seats));
        routes[4].routeID = 5; strcpy(routes[4].source, "Bangalore"); strcpy(routes[4].destination, "Hyderabad"); strcpy(routes[4].departureTime, "11:00 PM"); routes[4].fare = 1000.0; routes[4].totalSeats = 30; routes[4].busType = 2; memset(routes[4].seats, 0, sizeof(routes[4].seats));
        saveRoutes();
        return;
    }
    fread(&totalRoutes, sizeof(int), 1, fp);
    fread(routes, sizeof(Route), totalRoutes, fp);
    fclose(fp);
}

void saveTickets() {
    FILE *fp = fopen(TICKETS_FILE, "wb");
    if (fp) { fwrite(&totalTickets, sizeof(int), 1, fp); fwrite(tickets, sizeof(Ticket), totalTickets, fp); fclose(fp); }
}

void loadTickets() {
    FILE *fp = fopen(TICKETS_FILE, "rb");
    if (!fp) { totalTickets = 0; return; }
    fread(&totalTickets, sizeof(int), 1, fp);
    fread(tickets, sizeof(Ticket), totalTickets, fp);
    fclose(fp);
}

int main() { loadRoutes(); loadTickets(); return 0; }

EMSCRIPTEN_KEEPALIVE void reloadDatabase() { loadRoutes(); loadTickets(); }

EMSCRIPTEN_KEEPALIVE
int addWebRoute(char* src, char* dest, char* time, float fare, int type) {
    if (totalRoutes >= MAX_ROUTES) return 0; 
    int maxID = 0;
    for(int i=0; i<totalRoutes; i++) { if(routes[i].routeID > maxID) maxID = routes[i].routeID; }
    Route r;
    r.routeID = maxID + 1;
    strncpy(r.source, src, 49); r.source[49] = '\0';
    strncpy(r.destination, dest, 49); r.destination[49] = '\0';
    strncpy(r.departureTime, time, 19); r.departureTime[19] = '\0';
    r.fare = fare; r.busType = type; r.totalSeats = (type >= 2) ? 30 : 40; 
    memset(r.seats, 0, sizeof(r.seats));
    routes[totalRoutes++] = r;
    saveRoutes();
    return r.routeID;
}

EMSCRIPTEN_KEEPALIVE
int deleteWebRoute(int routeID) {
    int rIdx = -1;
    for (int i = 0; i < totalRoutes; i++) { if (routes[i].routeID == routeID) { rIdx = i; break; } }
    if (rIdx == -1) return 0; 
    for (int i = rIdx; i < totalRoutes - 1; i++) { routes[i] = routes[i + 1]; }
    totalRoutes--;
    saveRoutes();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int getTotalRoutes() { return totalRoutes; }
EMSCRIPTEN_KEEPALIVE int getRouteID(int idx) { return routes[idx].routeID; }
EMSCRIPTEN_KEEPALIVE char* getRouteSource(int idx) { return routes[idx].source; }
EMSCRIPTEN_KEEPALIVE char* getRouteDest(int idx) { return routes[idx].destination; }
EMSCRIPTEN_KEEPALIVE char* getRouteTime(int idx) { return routes[idx].departureTime; }
EMSCRIPTEN_KEEPALIVE float getRouteFare(int idx) { return routes[idx].fare; }
EMSCRIPTEN_KEEPALIVE int getRouteSeats(int idx) { return routes[idx].totalSeats; }
EMSCRIPTEN_KEEPALIVE int getRouteType(int idx) { return routes[idx].busType; }

// ==========================================
// TICKET MANIFEST GETTERS FOR ADMIN
// ==========================================
EMSCRIPTEN_KEEPALIVE int getTotalTickets() { return totalTickets; }
EMSCRIPTEN_KEEPALIVE int getTicketRouteID(int idx) { return tickets[idx].routeID; }
EMSCRIPTEN_KEEPALIVE int getTicketSeat(int idx) { return tickets[idx].seatNumbers[0]; }
EMSCRIPTEN_KEEPALIVE char* getTicketNameByIdx(int idx) { return tickets[idx].passengerName; }
EMSCRIPTEN_KEEPALIVE char* getTicketPhoneByIdx(int idx) { return tickets[idx].phoneNumber; }
EMSCRIPTEN_KEEPALIVE int isTicketCancelled(int idx) { return tickets[idx].isCancelled; }

// ==========================================
// UPDATED BOOKING & CANCELLATION
// ==========================================
EMSCRIPTEN_KEEPALIVE
int bookWebSeat(int routeID, int seatNo, char* passName, char* phoneNum) {
    if (totalTickets >= MAX_TICKETS) return 0;
    int rIdx = -1;
    for (int i = 0; i < totalRoutes; i++) { if (routes[i].routeID == routeID) { rIdx = i; break; } }
    if (rIdx == -1 || seatNo < 1 || seatNo > routes[rIdx].totalSeats) return 0;
    if (routes[rIdx].seats[seatNo - 1] == 1) return 0;

    routes[rIdx].seats[seatNo - 1] = 1;
    Ticket t;
    t.ticketID = totalTickets + 1;
    
    strncpy(t.passengerName, passName, 49); t.passengerName[49] = '\0';
    strncpy(t.phoneNumber, phoneNum, 19); t.phoneNumber[19] = '\0';
    
    t.routeID = routeID; t.seatNumbers[0] = seatNo; t.numSeats = 1; t.totalAmount = routes[rIdx].fare; t.isCancelled = 0;
    tickets[totalTickets++] = t;
    saveRoutes(); saveTickets();
    return t.ticketID;
}

// NEW: Cancel by Name and Phone
EMSCRIPTEN_KEEPALIVE
int cancelTicketByNamePhone(char* name, char* phone) {
    int count = 0;
    for (int i = 0; i < totalTickets; i++) {
        if (!tickets[i].isCancelled && 
            strcmp(tickets[i].passengerName, name) == 0 && 
            strcmp(tickets[i].phoneNumber, phone) == 0) {
            
            int rIdx = -1;
            for (int j = 0; j < totalRoutes; j++) {
                if (routes[j].routeID == tickets[i].routeID) { rIdx = j; break; }
            }
            if (rIdx != -1) routes[rIdx].seats[tickets[i].seatNumbers[0] - 1] = 0;
            
            tickets[i].isCancelled = 1;
            count++;
        }
    }
    if (count > 0) { saveRoutes(); saveTickets(); }
    return count; // Returns how many tickets were cancelled
}

EMSCRIPTEN_KEEPALIVE
int getAvailableSeats(int routeID) {
    int rIdx = -1;
    for (int i = 0; i < totalRoutes; i++) { if (routes[i].routeID == routeID) { rIdx = i; break; } }
    if (rIdx == -1) return 0;
    int count = 0;
    for (int s = 0; s < routes[rIdx].totalSeats; s++) { if (routes[rIdx].seats[s] == 0) count++; }
    return count;
}

EMSCRIPTEN_KEEPALIVE
int isSeatBooked(int routeID, int seatNo) {
    int rIdx = -1;
    for (int i = 0; i < totalRoutes; i++) { if (routes[i].routeID == routeID) { rIdx = i; break; } }
    if (rIdx == -1 || seatNo < 1 || seatNo > routes[rIdx].totalSeats) return 0;
    return routes[rIdx].seats[seatNo - 1]; 
}

EMSCRIPTEN_KEEPALIVE
float getRouteRevenue(int routeID) {
    float rev = 0;
    for (int i = 0; i < totalTickets; i++) { if (tickets[i].routeID == routeID && !tickets[i].isCancelled) rev += tickets[i].totalAmount; }
    return rev;
}

EMSCRIPTEN_KEEPALIVE
int getRouteBooked(int routeID) {
    int count = 0;
    for (int i = 0; i < totalTickets; i++) { if (tickets[i].routeID == routeID && !tickets[i].isCancelled) count++; }
    return count;
}