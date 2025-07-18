// Sistem Pemesanan Tiket Pesawat 
#include <iostream>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <limits>
#include <stack>

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\nTekan Enter untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

struct User {
    string nama;
    string email;
    string password;
    string nik;
    string tanggalLahir;
    string noHP;
    string jenisKelamin;
};

struct Flight {
    string maskapai;
    string asal;
    string tujuan;
    string tanggal;
    string jam;
    string kelas;
    int harga;
    int kapasitas;
    string kode;
};

struct Booking {
    string kodeBooking;
    string tanggalPesan;
    string namaPenumpang;
    string email;
    Flight flight;
    int jumlahPenumpang;
    int totalHarga;
    bool statusPembayaran;
};

// Node untuk Linked List User
struct UserNode {
    User data;
    UserNode* next;
    
    UserNode(const User& user) : data(user), next(nullptr) {}
};

// Node untuk Linked List Flight
struct FlightNode {
    Flight data;
    FlightNode* next;
    
    FlightNode(const Flight& flight) : data(flight), next(nullptr) {}
};

// Node untuk Linked List Booking
struct BookingNode {
    Booking data;
    BookingNode* next;
    
    BookingNode(const Booking& booking) : data(booking), next(nullptr) {}
};

// Linked List untuk User
class UserList {
private:
    UserNode* head;
    
public:
    UserList() : head(nullptr) {}
    
    ~UserList() {
        while (head) {
            UserNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void push_back(const User& user) {
        UserNode* newNode = new UserNode(user);
        if (!head) {
            head = newNode;
            return;
        }
        
        UserNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    
    User* findByEmail(const string& email) {
        UserNode* current = head;
        while (current) {
            if (current->data.email == email) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }
    
    bool emailExists(const string& email) {
        return findByEmail(email) != nullptr;
    }
};

// Linked List untuk Flight
class FlightList {
private:
    FlightNode* head;
    
public:
    FlightList() : head(nullptr) {}
    
    ~FlightList() {
        while (head) {
            FlightNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void push_back(const Flight& flight) {
        FlightNode* newNode = new FlightNode(flight);
        if (!head) {
            head = newNode;
            return;
        }
        
        FlightNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    
    Flight* findByCode(const string& code) {
        FlightNode* current = head;
        while (current) {
            if (current->data.kode == code) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }
    
    void displayAll() {
        FlightNode* current = head;
        while (current) {
            cout << left << setw(10) << current->data.kode
                 << setw(20) << current->data.maskapai
                 << setw(15) << (current->data.asal + "-" + current->data.tujuan)
                 << setw(15) << current->data.tanggal
                 << setw(10) << current->data.jam
                 << setw(10) << current->data.kelas
                 << "Rp " << current->data.harga << endl;
            current = current->next;
        }
    }
    
    void filterAndSort(const string& asal, const string& tujuan, FlightList& result) {
        FlightNode* current = head;
        while (current) {
            if ((asal.empty() || current->data.asal == asal) &&
                (tujuan.empty() || current->data.tujuan == tujuan)) {
                result.insertSorted(current->data);
            }
            current = current->next;
        }
    }
    
    void insertSorted(const Flight& flight) {
        FlightNode* newNode = new FlightNode(flight);
        
        if (!head || head->data.harga > flight.harga) {
            newNode->next = head;
            head = newNode;
            return;
        }
        
        FlightNode* current = head;
        while (current->next && current->next->data.harga <= flight.harga) {
            current = current->next;
        }
        
        newNode->next = current->next;
        current->next = newNode;
    }
    
    bool isEmpty() {
        return head == nullptr;
    }
};

// Linked List untuk Booking
class BookingList {
private:
    BookingNode* head;
    
public:
    BookingList() : head(nullptr) {}
    
    ~BookingList() {
        while (head) {
            BookingNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void push_back(const Booking& booking) {
        BookingNode* newNode = new BookingNode(booking);
        if (!head) {
            head = newNode;
            return;
        }
        
        BookingNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    
    void displayByEmail(const string& email) {
        BookingNode* current = head;
        bool found = false;
        
        while (current) {
            if (current->data.email == email) {
                cout << "Kode Booking: " << current->data.kodeBooking << endl;
                cout << "Tanggal Pesan: " << current->data.tanggalPesan << endl;
                cout << "Rute: " << current->data.flight.asal << " ke " << current->data.flight.tujuan << endl;
                cout << "Tanggal Penerbangan: " << current->data.flight.tanggal << ", " << current->data.flight.jam << endl;
                cout << "Status: " << (current->data.statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << endl;
                cout << string(50, '-') << endl;
                found = true;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "Belum ada history pemesanan.\n";
        }
    }
};

// Database dengan Linked List
UserList users;
FlightList availableFlights;
BookingList bookingHistory;

// Antrian pemesanan tiket (untuk proses pembayaran)
queue<Booking> bookingQueue;

// User yang sedang login
User* currentUser = nullptr;
stack<Booking> historyTiket;

// Fungsi untuk membuat kode booking unik
string generateBookingCode() {
    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string code = "BK-";
    srand(time(nullptr));
    for (int i = 0; i < 6; i++) {
        code += chars[rand() % chars.length()];
    }
    return code;
}

// Inisialisasi data penerbangan
void initializeFlights() {
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Bali", "2025-07-01", "09:00", "Ekonomi", 2000000, 100, "GA-101"});
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Bali", "2025-07-01", "13:00", "Ekonomi", 2000000, 100, "GA-102"});
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Bali", "2025-07-01", "09:00", "Bisnis", 2250000, 50, "GA-103"});
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Bali", "2025-07-01", "13:00", "Bisnis", 2250000, 50, "GA-104"});
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Jakarta", "2025-07-02", "08:00", "Ekonomi", 1500000, 100, "GA-201"});
    availableFlights.push_back({"Garuda Indonesia", "Jogja", "Jakarta", "2025-07-02", "15:00", "Ekonomi", 1500000, 100, "GA-202"});
    availableFlights.push_back({"Lion Air", "Jogja", "Bali", "2025-07-01", "10:30", "Ekonomi", 1800000, 120, "LA-301"});
    availableFlights.push_back({"Lion Air", "Jogja", "Jakarta", "2025-07-02", "07:15", "Ekonomi", 1300000, 120, "LA-302"});
    availableFlights.push_back({"Batik Air", "Jogja", "Singapore", "2025-07-03", "11:30", "Ekonomi", 3000000, 80, "BA-401"});
    availableFlights.push_back({"Batik Air", "Jogja", "Singapore", "2025-07-03", "15:00", "Bisnis", 3250000, 40, "BA-402"});
}

// Fungsi Registrasi
void registrasi() {
    clearScreen();
    User newUser;
    cout << "\n== REGISTRASI ==\n";
    cout << "Nama: ";
    getline(cin, newUser.nama);
    cout << "Email: ";
    getline(cin, newUser.email);
    
    // Cek apakah email sudah terdaftar
    if (users.emailExists(newUser.email)) {
        cout << "Email sudah terdaftar. Silakan gunakan email lain.\n";
        pauseScreen();
        return;
    }
    
    cout << "Password: ";
    getline(cin, newUser.password);
    cout << "NIK: ";
    getline(cin, newUser.nik);
    cout << "Tanggal Lahir (YYYY-MM-DD): ";
    getline(cin, newUser.tanggalLahir);
    cout << "No Handphone: ";
    getline(cin, newUser.noHP);
    cout << "Jenis Kelamin (L/P): ";
    getline(cin, newUser.jenisKelamin);
    
    users.push_back(newUser);
    cout << "Registrasi berhasil!\n";
    pauseScreen();
}

// Fungsi Login
bool login() {
    clearScreen();
    string email, password;
    cout << "\n== LOGIN ==\n";
    cout << "Email: ";
    getline(cin, email);
    cout << "Password: ";
    getline(cin, password);
    
    User* user = users.findByEmail(email);
    if (user && user->password == password) {
        currentUser = user;
        cout << "Login berhasil! Selamat datang, " << user->nama << "!\n";
        pauseScreen();
        return true;
    }
    
    cout << "Login gagal. Email atau password salah.\n";
    pauseScreen();
    return false;
}

// Fungsi Lupa Password
void lupaPassword() {
    clearScreen();
    string email;
    cout << "\n== LUPA PASSWORD ==\n";
    cout << "Email: ";
    getline(cin, email);
    
    User* user = users.findByEmail(email);
    if (user) {
        cout << "Password Anda: " << user->password << endl;
        pauseScreen();
        return;
    }
    
    cout << "Email tidak ditemukan.\n";
    pauseScreen();
}

// Tampilkan daftar penerbangan yang tersedia
void lihatPenerbanganTersedia() {
    clearScreen();
    cout << "\n== DAFTAR PENERBANGAN TERSEDIA ==\n";
    cout << left << setw(10) << "KODE"
         << setw(20) << "MASKAPAI"
         << setw(15) << "RUTE"
         << setw(15) << "TANGGAL"
         << setw(10) << "JAM"
         << setw(10) << "KELAS"
         << setw(15) << "HARGA" << endl;
    cout << string(95, '-') << endl;
    
    availableFlights.displayAll();
    pauseScreen();
}

// Fungsi Pembayaran
void pembayaran(Booking& booking) {
    clearScreen();
    string metode;
    cout << "\n== PEMBAYARAN ==\n";
    cout << "Total pembayaran: Rp " << booking.totalHarga << endl;
    
    while (true) {
        cout << "Metode (BCA/Mandiri/BNI/BRI): ";
        getline(cin, metode);
        if (metode == "BCA" || metode == "Mandiri" || metode == "BNI" || metode == "BRI") {
            break;
        } else {
            clearScreen();
            cout << "Metode tidak tersedia. Silakan coba lagi.\n";
            cout << "\n== PEMBAYARAN ==\n";
            cout << "Total pembayaran: Rp " << booking.totalHarga << endl;
        }
    }
    
    srand(time(0));
    int kodeUnik = rand() % 9000 + 1000;
    string kodePembayaran = metode + "-" + to_string(kodeUnik);
    cout << "Kode Pembayaran Anda: " << kodePembayaran << endl;
    
    char konfirmasi;
    cout << "\n== KONFIRMASI PEMBAYARAN ==\n";
    cout << "Sudah Membayar? (y/n): ";
    cin >> konfirmasi;
    cin.ignore();
    
    if (tolower(konfirmasi) == 'y') {
        booking.statusPembayaran = true;
        cout << "Pembayaran berhasil! Tiket telah dikonfirmasi.\n";
        bookingHistory.push_back(booking);
        historyTiket.push(booking);
    } else {
        bookingQueue.push(booking);
        bookingHistory.push_back(booking);
        cout << "Pembayaran ditunda. Silakan selesaikan pembayaran nanti.\n";
    }
    pauseScreen();
}

// Fungsi Profil
void profil() {
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    while (true) {
        clearScreen();
        cout << "\n== PROFIL PENGGUNA ==\n";
        cout << "1. Nama           : " << currentUser->nama << '\n';
        cout << "2. Email          : " << currentUser->email << '\n';
        cout << "3. Password       : " << currentUser->password << '\n';
        cout << "4. NIK            : " << currentUser->nik << '\n';
        cout << "5. Tanggal Lahir  : " << currentUser->tanggalLahir << '\n';
        cout << "6. No. HP         : " << currentUser->noHP << '\n';
        cout << "7. Jenis Kelamin  : " << currentUser->jenisKelamin << '\n';
        cout << string(40, '-') << '\n';
        cout << "Apakah ingin memperbarui data? (y/n): ";
        
        char jawab;
        cin >> jawab;
        cin.ignore();
        
        if (tolower(jawab) != 'y') {
            return;
        }
        
        int pilih;
        cout << "\nPilih nomor data yang ingin diubah (0 untuk batal): ";
        cin >> pilih;
        cin.ignore();
        
        if (pilih == 0) return;
        
        string baru;
        cout << "Masukkan data baru: ";
        getline(cin, baru);
        
        if (baru.empty()) {
            cout << "Input kosong. Data tidak diubah.\n";
            pauseScreen();
            return;
        }
        
        switch (pilih) {
            case 1: currentUser->nama = baru; break;
            case 2: {
                if (users.emailExists(baru)) {
                    cout << "Email sudah terdaftar oleh pengguna lain.\n";
                    pauseScreen();
                    return;
                }
                currentUser->email = baru;
                break;
            }
            case 3: currentUser->password = baru; break;
            case 4: currentUser->nik = baru; break;
            case 5: currentUser->tanggalLahir = baru; break;
            case 6: currentUser->noHP = baru; break;
            case 7: currentUser->jenisKelamin = baru; break;
            default:
                cout << "Pilihan tidak valid.\n";
                pauseScreen();
                return;
        }
        
        cout << "Data berhasil diperbarui!\n";
        pauseScreen();
        return;
    }
}

// Fungsi Sorting Tiket
void sortingTiket() {
    clearScreen();
    string asal, tujuan;
    cout << "\n== SORTING PENERBANGAN ==\n";
    cout << "Masukkan kota asal   : ";
    getline(cin, asal);
    cout << "Masukkan kota tujuan : ";
    getline(cin, tujuan);
    
    FlightList hasilFilter;
    availableFlights.filterAndSort(asal, tujuan, hasilFilter);
    
    if (hasilFilter.isEmpty()) {
        cout << "Tidak ditemukan penerbangan dengan rute tersebut.\n";
        pauseScreen();
        return;
    }
    
    cout << "\n== HASIL FILTER ==\n";
    cout << left << setw(10) << "KODE"
         << setw(20) << "MASKAPAI"
         << setw(15) << "RUTE"
         << setw(15) << "TANGGAL"
         << setw(10) << "JAM"
         << setw(10) << "KELAS"
         << setw(15) << "HARGA" << endl;
    cout << string(95, '-') << endl;
    
    hasilFilter.displayAll();
    
    while (true) {
        cout << "\n1. Pesan Tiket\n";
        cout << "0. Kembali\n";
        cout << "Pilih: ";
        int opsi;
        cin >> opsi;
        cin.ignore();
        
        if (opsi == 0) return;
        else if (opsi == 1) {
            Booking newBooking;
            string kodePenerbangan;
            cout << "\nMasukkan kode penerbangan: ";
            getline(cin, kodePenerbangan);
            
            Flight* flight = availableFlights.findByCode(kodePenerbangan);
            if (!flight) {
                cout << "Kode penerbangan tidak valid.\n";
                pauseScreen();
                return;
            }
            
            newBooking.flight = *flight;
            cout << "Jumlah Penumpang: ";
            cin >> newBooking.jumlahPenumpang;
            cin.ignore();
            
            newBooking.kodeBooking = generateBookingCode();
            newBooking.tanggalPesan = "2025-06-27";
            newBooking.namaPenumpang = currentUser->nama;
            newBooking.email = currentUser->email;
            newBooking.totalHarga = newBooking.flight.harga * newBooking.jumlahPenumpang;
            newBooking.statusPembayaran = false;
            
            clearScreen();
            cout << "\n== DETAIL PEMESANAN ==\n";
            cout << "Kode Booking: " << newBooking.kodeBooking << endl;
            cout << "Nama Penumpang: " << newBooking.namaPenumpang << endl;
            cout << "Maskapai: " << newBooking.flight.maskapai << endl;
            cout << "Rute: " << newBooking.flight.asal << " ke " << newBooking.flight.tujuan << endl;
            cout << "Tanggal: " << newBooking.flight.tanggal << endl;
            cout << "Jam: " << newBooking.flight.jam << endl;
            cout << "Kelas: " << newBooking.flight.kelas << endl;
            cout << "Jumlah Penumpang: " << newBooking.jumlahPenumpang << endl;
            cout << "Total Harga: Rp " << newBooking.totalHarga << endl;
            
            char lanjut;
            cout << "\nLanjut ke pembayaran? (y/n): ";
            cin >> lanjut;
            cin.ignore();
            
            if (tolower(lanjut) == 'y') {
                pembayaran(newBooking);
                return;
            } else {
                bookingQueue.push(newBooking);
                bookingHistory.push_back(newBooking);
                cout << "Pemesanan disimpan. Silakan selesaikan pembayaran nanti.\n";
                pauseScreen();
                return;
            }
        } else {
            cout << "Pilihan tidak valid.\n";
            pauseScreen();
        }
    }
}

// Fungsi Pemesanan Tiket
void pesanTiket() {
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    while (true) {
        clearScreen();
        cout << "\n== PESAN TIKET ==\n";
        lihatPenerbanganTersedia();
        
        cout << "\n1. Pesan Tiket\n";
        cout << "2. Sorting Tiket\n";
        cout << "0. Kembali ke Dashboard\n";
        cout << "Pilih: ";
        int opsi;
        cin >> opsi;
        cin.ignore();
        
        if (opsi == 0) return;
        else if (opsi == 1) {
            break;
        } else if (opsi == 2) {
            sortingTiket();
            return;
        } else {
            cout << "Pilihan tidak valid.\n";
            pauseScreen();
        }
    }
    
    Booking newBooking;
    string kodePenerbangan;
    cout << "\nMasukkan kode penerbangan: ";
    getline(cin, kodePenerbangan);
    
    Flight* flight = availableFlights.findByCode(kodePenerbangan);
    if (!flight) {
        cout << "Kode penerbangan tidak valid.\n";
        pauseScreen();
        return;
    }
    
    newBooking.flight = *flight;
    cout << "Jumlah Penumpang: ";
    cin >> newBooking.jumlahPenumpang;
    cin.ignore();
    
    newBooking.kodeBooking = generateBookingCode();
    newBooking.tanggalPesan = "2025-06-27";
    newBooking.namaPenumpang = currentUser->nama;
    newBooking.email = currentUser->email;
    newBooking.totalHarga = newBooking.flight.harga * newBooking.jumlahPenumpang;
    newBooking.statusPembayaran = false;
    
    clearScreen();
    cout << "\n== DETAIL PEMESANAN ==\n";
    cout << "Kode Booking: " << newBooking.kodeBooking << endl;
    cout << "Nama Penumpang: " << newBooking.namaPenumpang << endl;
    cout << "Maskapai: " << newBooking.flight.maskapai << endl;
    cout << "Rute: " << newBooking.flight.asal << " ke " << newBooking.flight.tujuan << endl;
    cout << "Tanggal: " << newBooking.flight.tanggal << endl;
    cout << "Jam: " << newBooking.flight.jam << endl;
    cout << "Kelas: " << newBooking.flight.kelas << endl;
    cout << "Jumlah Penumpang: " << newBooking.jumlahPenumpang << endl;
    cout << "Total Harga: Rp " << newBooking.totalHarga << endl;
    
    char lanjut;
    cout << "\nLanjut ke pembayaran? (y/n): ";
    cin >> lanjut;
    cin.ignore();
    
    if (tolower(lanjut) == 'y') {
        pembayaran(newBooking);
        return;
    } else {
        bookingQueue.push(newBooking);
        bookingHistory.push_back(newBooking);
        cout << "Pemesanan disimpan. Silakan selesaikan pembayaran nanti.\n";
        pauseScreen();
        return;
    }
}

// Lihat History Pemesanan Tiket
void lihatHistoryPemesanan() {
    clearScreen();
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    cout << "\n== HISTORY PEMESANAN ==\n";
    bookingHistory.displayByEmail(currentUser->email);
    pauseScreen();
}

void bayarTiketAntrian() {
    clearScreen();
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    if (bookingQueue.empty()) {
        cout << "Tidak ada antrian pembayaran.\n";
        pauseScreen();
        return;
    }
    
    cout << "\n== BAYAR TIKET ANTRIAN ==\n";
    queue<Booking> tempQueue = bookingQueue;
    vector<Booking> userBookings;
    
    while (!tempQueue.empty()) {
        Booking booking = tempQueue.front();
        tempQueue.pop();
        if (booking.email == currentUser->email) {
            userBookings.push_back(booking);
        }
    }
    
    if (userBookings.empty()) {
        cout << "Tidak ada tiket yang belum dibayar.\n";
        pauseScreen();
        return;
    }
    
    for (int i = 0; i < userBookings.size(); i++) {
        cout << (i+1) << ". Kode Booking: " << userBookings[i].kodeBooking << endl;
        cout << "   Rute: " << userBookings[i].flight.asal << " ke " << userBookings[i].flight.tujuan << endl;
        cout << "   Tanggal: " << userBookings[i].flight.tanggal << ", " << userBookings[i].flight.jam << endl;
        cout << "   Total: Rp " << userBookings[i].totalHarga << endl;
        cout << string(50, '-') << endl;
    }
    
    int pilih;
    cout << "Pilih tiket yang akan dibayar (1-" << userBookings.size() << "): ";
    cin >> pilih;
    cin.ignore();
    
    if (pilih < 1 || pilih > userBookings.size()) {
        cout << "Pilihan tidak valid.\n";
        pauseScreen();
        return;
    }
    
    Booking selectedBooking = userBookings[pilih-1];
    pembayaran(selectedBooking);
    
    if (selectedBooking.statusPembayaran) {
        queue<Booking> newQueue;
        while (!bookingQueue.empty()) {
            Booking booking = bookingQueue.front();
            bookingQueue.pop();
            if (booking.kodeBooking != selectedBooking.kodeBooking) {
                newQueue.push(booking);
            }
        }
        bookingQueue = newQueue;
    }
}

// Lihat Antrian Pembayaran
void lihatAntrianPembayaran() {
    clearScreen();
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    cout << "\n== ANTRIAN PEMBAYARAN ==\n";
    if (bookingQueue.empty()) {
        cout << "Tidak ada antrian pembayaran.\n";
        pauseScreen();
        return;
    }
    
    queue<Booking> tempQueue = bookingQueue;
    vector<Booking> userBookings;
    
    while (!tempQueue.empty()) {
        Booking booking = tempQueue.front();
        tempQueue.pop();
        if (booking.email == currentUser->email) {
            userBookings.push_back(booking);
        }
    }
    
    if (userBookings.empty()) {
        cout << "Tidak ada antrian pembayaran untuk akun Anda.\n";
        pauseScreen();
        return;
    }
    
    for (int i = 0; i < userBookings.size(); ++i) {
        cout << i + 1 << ". Kode Booking: " << userBookings[i].kodeBooking << endl;
        cout << "   Rute: " << userBookings[i].flight.asal << " ke " << userBookings[i].flight.tujuan << endl;
        cout << "   Tanggal: " << userBookings[i].flight.tanggal << ", " << userBookings[i].flight.jam << endl;
        cout << "   Total Harga: Rp " << userBookings[i].totalHarga << endl;
        cout << string(50, '-') << endl;
    }
    
    int pilih;
    cout << "\n1. Bayar\n";
    cout << "2. Cancel Tiket\n";
    cout << "0. Kembali\n";
    cout << "Pilih: ";
    cin >> pilih;
    cin.ignore();
    
    if (pilih == 1) {
        bayarTiketAntrian();
    } else if (pilih == 2) {
        string kode;
        cout << "Masukkan Kode Booking yang ingin dibatalkan: ";
        getline(cin, kode);
        
        bool found = false;
        queue<Booking> newQueue;
        
        while (!bookingQueue.empty()) {
            Booking booking = bookingQueue.front();
            bookingQueue.pop();
            if (booking.kodeBooking == kode && booking.email == currentUser->email) {
                found = true;
                cout << "Tiket dengan kode " << kode << " berhasil dibatalkan.\n";
            } else {
                newQueue.push(booking);
            }
        }
        
        bookingQueue = newQueue;
        
        if (!found) {
            cout << "Kode booking tidak ditemukan atau bukan milik Anda.\n";
        }
        pauseScreen();
    }
}

void lihatTiketTerakhir() {
    clearScreen();
    if (currentUser == nullptr) {
        cout << "Anda harus login terlebih dahulu.\n";
        pauseScreen();
        return;
    }
    
    if (historyTiket.empty()) {
        cout << "Belum ada tiket yang dipesan.\n";
        pauseScreen();
        return;
    }
    
    Booking last = historyTiket.top();
    
    if (last.email != currentUser->email) {
        cout << "Tidak ada tiket terbaru yang sesuai akun Anda.\n";
    } else {
        cout << "\n== TIKET TERAKHIR YANG DIPESAN ==\n";
        cout << "Kode Booking   : " << last.kodeBooking << endl;
        cout << "Nama Penumpang : " << last.namaPenumpang << endl;
        cout << "Maskapai       : " << last.flight.maskapai << endl;
        cout << "Rute           : " << last.flight.asal << " ke " << last.flight.tujuan << endl;
        cout << "Tanggal        : " << last.flight.tanggal << endl;
        cout << "Jam            : " << last.flight.jam << endl;
        cout << "Kelas          : " << last.flight.kelas << endl;
        cout << "Jumlah Penumpang: " << last.jumlahPenumpang << endl;
        cout << "Total Harga    : Rp " << last.totalHarga << endl;
        cout << "Status         : " << (last.statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << endl;
    }
    pauseScreen();
}

// Main
int main() {
    initializeFlights();
    
    clearScreen();
    cout << "=======================================\n";
    cout << "  Selamat Datang di FLYFIBMA Airlines!\n";
    cout << "=======================================\n";
    cout << "Kelompok:\n";
    cout << "- Fachri Arda Farros\n";
    cout << "- Arifin Ihsan Nugroho\n";
    cout << "- Bagas Putra Luqmana\n";
    cout << "- Maudi Kurnia\n";
    cout << "- Arundaya Xenia Nurachmawan\n";
    cout << "=======================================\n";
    
    char lanjutMenu;
    cout << "Lanjut ke Menu Utama? (y/n): ";
    cin >> lanjutMenu;
    cin.ignore();
    
    if (tolower(lanjutMenu) != 'y') {
        cout << "Terima kasih telah mengunjungi FLYBIMA.\n";
        return 0;
    }
    
    int pilih;
    do {
        clearScreen();
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Registrasi\n";
        cout << "2. Login\n";
        cout << "3. Lupa Password\n";
        cout << "4. Lihat Penerbangan Tersedia\n";
        cout << "5. Keluar\n";
        cout << "Pilih: ";
        cin >> pilih;
        cin.ignore();
        
        switch (pilih) {
            case 1:
                registrasi();
                break;
            case 2:
                if (login()) {
                    int p;
                    do {
                        clearScreen();
                        cout << "\n== DASHBOARD ==\n";
                        cout << "1. Profil\n";
                        cout << "2. Pesan Tiket\n";
                        cout << "3. My Ticket\n";
                        cout << "0. Logout\n";
                        cout << "Pilih: ";
                        cin >> p;
                        cin.ignore();
                        
                        switch (p) {
                            case 1:
                                profil();
                                break;
                            case 2:
                                pesanTiket();
                                break;
                            case 3:
                                int pilihan;
                                do {
                                    clearScreen();
                                    cout << "\n== MY TICKET ==\n";
                                    cout << "1. History Pemesanan\n";
                                    cout << "2. Antrian Pembayaran\n";
                                    cout << "3. Lihat Tiket Terakhir\n";
                                    cout << "0. Kembali\n";
                                    cout << "Pilih: ";
                                    cin >> pilihan;
                                    cin.ignore();
                                    
                                    switch (pilihan) {
                                        case 1:
                                            lihatHistoryPemesanan();
                                            break;
                                        case 2:
                                            lihatAntrianPembayaran();
                                            break;
                                        case 3:
                                            lihatTiketTerakhir();
                                            break;
                                        case 0:
                                            break;
                                        default:
                                            cout << "Pilihan tidak valid.\n";
                                            pauseScreen();
                                            break;
                                    }
                                } while (pilihan != 0);
                                break;
                        }
                    } while (p != 0);
                    
                    currentUser = nullptr;
                    cout << "Logout berhasil.\n";
                    pauseScreen();
                }
                break;
            case 3:
                lupaPassword();
                break;
            case 4:
                lihatPenerbanganTersedia();
                break;
        }
    } while (pilih != 5);
    
    clearScreen();
    cout << "Terima kasih telah menggunakan FLYFIBMA Airlines!\n";
    return 0;
}
