// booking_system.cpp
// Simple console-based Bus/Train booking system
// Features:
// 1. List available services (bus/train)
// 2. Book ticket
// 3. Cancel booking
// 4. View my bookings
// 5. Save/load data to files (persistence between runs)

#include <bits/stdc++.h>
using namespace std;

struct Service {
    int id;
    string type; // "Bus" or "Train"
    string name; // e.g., "Bus 101" or "Train Rajdhani"
    string source;
    string dest;
    string depart_time;
    int seats_total;
    int seats_available;
};

struct Booking {
    int booking_id;
    int service_id;
    string passenger_name;
    int seats_booked;
    string contact;
};

vector<Service> services;
vector<Booking> bookings;
int nextBookingId = 1;
const string SERVICES_FILE = "services.dat";
const string BOOKINGS_FILE = "bookings.dat";

void seedDefaultServices() {
    // Adds default services if file not present or empty
    services.push_back({1, "Bus", "CityExpress-101", "Chandigarh", "Delhi", "07:00", 40, 40});
    services.push_back({2, "Bus", "InterCity-202", "Panchkula", "Ambala", "10:30", 30, 30});
    services.push_back({3, "Train", "Express-Rajdhani", "Chandigarh", "Mumbai", "18:45", 200, 200});
    services.push_back({4, "Train", "Passenger-301", "Delhi", "Jaipur", "13:00", 120, 120});
}

void saveServices() {
    ofstream ofs(SERVICES_FILE, ios::binary | ios::trunc);
    if (!ofs) return;
    int n = services.size();
    ofs.write((char*)&n, sizeof(n));
    for (auto &s : services) {
        ofs.write((char*)&s.id, sizeof(s.id));
        auto writeString = [&](const string &str){
            int len = str.size();
            ofs.write((char*)&len, sizeof(len));
            ofs.write(str.c_str(), len);
        };
        writeString(s.type);
        writeString(s.name);
        writeString(s.source);
        writeString(s.dest);
        writeString(s.depart_time);
        ofs.write((char*)&s.seats_total, sizeof(s.seats_total));
        ofs.write((char*)&s.seats_available, sizeof(s.seats_available));
    }
    ofs.close();
}

void loadServices() {
    services.clear();
    ifstream ifs(SERVICES_FILE, ios::binary);
    if (!ifs) {
        seedDefaultServices();
        saveServices();
        return;
    }
    int n;
    ifs.read((char*)&n, sizeof(n));
    if (!ifs) {
        // file empty or corrupted — seed defaults
        seedDefaultServices();
        saveServices();
        return;
    }
    for (int i=0;i<n;i++) {
        Service s;
        ifs.read((char*)&s.id, sizeof(s.id));
        auto readString = [&](string &str){
            int len;
            ifs.read((char*)&len, sizeof(len));
            str.resize(len);
            ifs.read(&str[0], len);
        };
        readString(s.type);
        readString(s.name);
        readString(s.source);
        readString(s.dest);
        readString(s.depart_time);
        ifs.read((char*)&s.seats_total, sizeof(s.seats_total));
        ifs.read((char*)&s.seats_available, sizeof(s.seats_available));
        services.push_back(s);
    }
    ifs.close();
}

void saveBookings() {
    ofstream ofs(BOOKINGS_FILE, ios::binary | ios::trunc);
    if (!ofs) return;
    int n = bookings.size();
    ofs.write((char*)&n, sizeof(n));
    ofs.write((char*)&nextBookingId, sizeof(nextBookingId));
    for (auto &b : bookings) {
        ofs.write((char*)&b.booking_id, sizeof(b.booking_id));
        ofs.write((char*)&b.service_id, sizeof(b.service_id));
        auto writeString = [&](const string &str){
            int len = str.size();
            ofs.write((char*)&len, sizeof(len));
            ofs.write(str.c_str(), len);
        };
        writeString(b.passenger_name);
        ofs.write((char*)&b.seats_booked, sizeof(b.seats_booked));
        writeString(b.contact);
    }
    ofs.close();
}

void loadBookings() {
    bookings.clear();
    ifstream ifs(BOOKINGS_FILE, ios::binary);
    if (!ifs) {
        nextBookingId = 1;
        return;
    }
    int n;
    ifs.read((char*)&n, sizeof(n));
    ifs.read((char*)&nextBookingId, sizeof(nextBookingId));
    for (int i=0;i<n;i++) {
        Booking b;
        ifs.read((char*)&b.booking_id, sizeof(b.booking_id));
        ifs.read((char*)&b.service_id, sizeof(b.service_id));
        auto readString = [&](string &str){
            int len;
            ifs.read((char*)&len, sizeof(len));
            str.resize(len);
            ifs.read(&str[0], len);
        };
        readString(b.passenger_name);
        ifs.read((char*)&b.seats_booked, sizeof(b.seats_booked));
        readString(b.contact);
        bookings.push_back(b);
    }
    ifs.close();
}

Service* findServiceById(int id) {
    for (auto &s : services) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

void listServices() {
    cout << "\nAvailable Services:\n";
    cout << left << setw(5) << "ID" << setw(8) << "Type" << setw(22) << "Name" << setw(15) << "Source" << setw(15) << "Destination" << setw(10) << "Depart" << setw(8) << "Avail\n";
    cout << string(85, '-') << "\n";
    for (auto &s : services) {
        cout << left << setw(5) << s.id << setw(8) << s.type << setw(22) << s.name << setw(15) << s.source << setw(15) << s.dest << setw(10) << s.depart_time << setw(8) << s.seats_available << "\n";
    }
}

void showMyBookings() {
    if (bookings.empty()) {
        cout << "\nNo bookings yet.\n";
        return;
    }
    cout << "\nMy Bookings:\n";
    cout << left << setw(8) << "BID" << setw(5) << "SID" << setw(22) << "Service Name" << setw(18) << "Passenger" << setw(8) << "Seats" << setw(15) << "Contact\n";
    cout << string(90, '-') << "\n";
    for (auto &b : bookings) {
        Service* s = findServiceById(b.service_id);
        string sname = s ? s->name : "Deleted/Unknown";
        cout << left << setw(8) << b.booking_id << setw(5) << b.service_id << setw(22) << sname << setw(18) << b.passenger_name << setw(8) << b.seats_booked << setw(15) << b.contact << "\n";
    }
}

void bookTicket() {
    listServices();
    cout << "\nEnter Service ID to book: ";
    int sid; cin >> sid;
    Service* s = findServiceById(sid);
    if (!s) {
        cout << "Invalid Service ID.\n";
        return;
    }
    cout << "Available seats: " << s->seats_available << "\n";
    if (s->seats_available <= 0) {
        cout << "Sorry, no seats available on this service.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Passenger name: ";
    string pname; getline(cin, pname);
    cout << "Number of seats: ";
    int seats; cin >> seats;
    if (seats <= 0 || seats > s->seats_available) {
        cout << "Invalid seats requested.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Contact (phone/email): ";
    string contact; getline(cin, contact);

    Booking b;
    b.booking_id = nextBookingId++;
    b.service_id = s->id;
    b.passenger_name = pname;
    b.seats_booked = seats;
    b.contact = contact;
    bookings.push_back(b);

    s->seats_available -= seats;

    saveServices();
    saveBookings();

    cout << "\nBooking successful! Your Booking ID: " << b.booking_id << "\n";
}

void cancelBooking() {
    if (bookings.empty()) {
        cout << "No bookings to cancel.\n";
        return;
    }
    showMyBookings();
    cout << "\nEnter Booking ID to cancel: ";
    int bid; cin >> bid;
    auto it = find_if(bookings.begin(), bookings.end(), [&](const Booking &b){ return b.booking_id == bid; });
    if (it == bookings.end()) {
        cout << "Booking ID not found.\n";
        return;
    }
    int sid = it->service_id;
    int seats = it->seats_booked;
    Service* s = findServiceById(sid);
    if (s) s->seats_available += seats;
    bookings.erase(it);

    saveServices();
    saveBookings();

    cout << "Booking cancelled and seats released.\n";
}

void addService() {
    Service s;
    int maxId = 0;
    for (auto &x : services) if (x.id > maxId) maxId = x.id;
    s.id = maxId + 1;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Type (Bus/Train): "; getline(cin, s.type);
    cout << "Name: "; getline(cin, s.name);
    cout << "Source: "; getline(cin, s.source);
    cout << "Destination: "; getline(cin, s.dest);
    cout << "Departure time (e.g., 07:30): "; getline(cin, s.depart_time);
    cout << "Total seats: "; cin >> s.seats_total;
    s.seats_available = s.seats_total;
    services.push_back(s);
    saveServices();
    cout << "Service added with ID: " << s.id << "\n";
}

void adminMenu() {
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Service\n2. List Services\n3. View Bookings\n4. Back\nChoose: ";
        int ch; cin >> ch;
        if (ch == 1) addService();
        else if (ch == 2) listServices();
        else if (ch == 3) showMyBookings();
        else break;
    }
}

int main() {
    loadServices();
    loadBookings();

    cout << "=== Simple Bus/Train Booking System ===\n";
    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. List Services\n";
        cout << "2. Book Ticket\n";
        cout << "3. Cancel Booking\n";
        cout << "4. View My Bookings\n";
        cout << "5. Admin Menu (password)\n";
        cout << "6. Exit\n";
        cout << "Choose option: ";
        int opt; cin >> opt;
        if (opt == 1) listServices();
        else if (opt == 2) bookTicket();
        else if (opt == 3) cancelBooking();
        else if (opt == 4) showMyBookings();
        else if (opt == 5) {
            cout << "Enter admin password: ";
            string pw; cin >> pw;
            // default simple password for admin access (change if required)
            if (pw == "admin123") adminMenu();
            else cout << "Wrong password.\n";
        }
        else if (opt == 6) {
            cout << "Saving data and exiting...\n";
            saveServices();
            saveBookings();
            break;
        }
        else cout << "Invalid option. Try again.\n";
    }

    return 0;
}