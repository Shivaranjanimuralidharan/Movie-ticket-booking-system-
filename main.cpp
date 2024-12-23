#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <windows.h>
#include <sstream>
#include <vector>
#include <string>
#include <conio.h>

using namespace std;

const char* HOST = "localhost";
const char* USER = "root";
const char* PW = "13041304"; // Update with your actual password
const char* DB = "mydb";

class Seats {
private:
    int Seat[5][10]; // 5 rows and 10 seats

public:
    Seats() {
        // Initialize all seats to available (1)
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 10; j++) {
                Seat[i][j] = 1; // 1 indicates available
            }
        }
    }

    int getSeatStatus(int row, int seatNumber) {
        if (row < 1 || row > 5 || seatNumber < 1 || seatNumber > 10) {
            return -1; // Invalid input
        }
        return Seat[row - 1][seatNumber - 1];
    }

    void reserveSeat(int row, int seatNumber) {
        if (row < 1 || row > 5 || seatNumber < 1 || seatNumber > 10) {
            return; // Invalid input
        }
        Seat[row - 1][seatNumber - 1] = 0; // 0 indicates reserved
    }

   void displaySeatsFromDatabase( MYSQL* conn,const string& movieName, const string& showtime, const string& date) {
    // Construct the query string
    string query = "SELECT RowNumber, SeatNumber, Seat FROM " + movieName +
                   " WHERE Showtime = '" + showtime + "' AND Date = '" + date + "'";
    
    if (mysql_query(conn, query.c_str())) {
        cerr << "Query failed: " << mysql_error(conn) << endl;
        return;
    }
    
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Failed to store result: " << mysql_error(conn) << endl;
        return;
    }

    // Initialize seat array (all seats available by default)
    int Seat[5][10];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            Seat[i][j] = 1; // All seats marked as available
        }
    }

    // Fetch rows from the result set
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        // Convert RowNumber and SeatNumber to int using string streams
        int rowNumber, seatNumber, seatStatus;
        
        istringstream(row[0]) >> rowNumber;
        istringstream(row[1]) >> seatNumber;
        istringstream(row[2]) >> seatStatus;
        
        rowNumber -= 1; // Adjust for 0-based index
        seatNumber -= 1; // Adjust for 0-based index

        Seat[rowNumber][seatNumber] = seatStatus; // Set seat status
    }
    mysql_free_result(res);

    // Display seat availability
    cout << "  ";
    for (int i = 0; i < 10; i++) {
        cout << "  " << i + 1;
    }
    cout << endl;

    for (int row = 0; row < 5; row++) {
        cout << row + 1 << " ";
        for (int col = 0; col < 10; col++) {
            if (Seat[row][col] == 1) {
                cout << " - "; // Available seat
            } else {
                cout << " X "; // Reserved seat
            }
        }
        cout << " |" << endl;
    }
    cout << "--------------------------------" << endl;
}
 void display(MYSQL* conn,const int& moviechoice,const int& datechoice,const int& showchoice)
 {
 	string moviename;
 	if(moviechoice==1)
 	{
 	 moviename="Amaran";
	 }
	 else if(moviechoice==2)
	 {
	 	moviename="Dheema";
	 }
	 else if(moviechoice==3)
	 {
	  moviename="Sitaramam";
	 }
	 else{
	 	moviename="Shershaa";
	 }
	 string showtime;
	 if(showchoice==1)
	 {
	 	showtime="11am";
	 }
	 else if(showchoice==2){
	 showtime="3pm";
	 }
	 else{
	 	showtime="7pm";
	 }
	 string date;
	 if(datechoice==1)
	 {
	 	date="2024-11-14";
	 }
	 else if(datechoice==2)
	 {
	 	date="2024-11-15";
	 }
	 else{
	 	date="2024-11-16";
	 }
	 displaySeatsFromDatabase( conn,moviename, showtime,date) ;
 }

    void getDB(MYSQL* conn, const string& movieName, const string& showtime, const string& date) {
        // Corrected query to match table schema
        string query = "SELECT RowNumber, SeatNumber, Seat FROM " + movieName + " WHERE Showtime = '" + showtime + "' AND Date = '" + date + "'";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        MYSQL_RES* result;
        result = mysql_store_result(conn);
        if (!result) {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            int rowNumber = atoi(row[0]);
            int seatNumber = atoi(row[1]);
            int seatStatus = atoi(row[2]);
            Seat[rowNumber - 1][seatNumber - 1] = seatStatus; // Update seat status from DB
        }
        mysql_free_result(result);
    }

    void updateDB(MYSQL* conn, const string& movieName, const string& showtime, const string& date) {
        // Update the database with current seat status
        for (int row = 1; row <= 5; row++) {
            for (int seatNumber = 1; seatNumber <= 10; seatNumber++) {
                stringstream ss;
                ss << "UPDATE " << movieName << " SET Seat = " << Seat[row - 1][seatNumber - 1]
                   << " WHERE RowNumber = " << row << " AND SeatNumber = " << seatNumber 
                   << " AND Showtime = '" << showtime << "' AND Date = '" << date << "'";
                string updateQuery = ss.str();
                if (mysql_query(conn, updateQuery.c_str())) {
                    cout << "Error: " << mysql_error(conn) << endl;
                }
            }
        }
    }
};


class Membership {
public:
    static void createMembershipTable(MYSQL* conn) {
        string createTableQuery = "CREATE TABLE IF NOT EXISTS Membership (CardNumber VARCHAR(20) PRIMARY KEY, Password VARCHAR(15) UNIQUE)";
        if (mysql_query(conn, createTableQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
    }

    static bool validateMembership(MYSQL* conn, const string& cardNumber, const string& password) {
        string query = "SELECT * FROM Membership WHERE CardNumber = '" + cardNumber + "' AND Password = '" + password + "'";

        int attempts = 0;
        while (attempts < 2) {
            if (mysql_query(conn, query.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                return false;
            }

            MYSQL_RES* result = mysql_store_result(conn);
            if (!result) {
                cout << "Error: " << mysql_error(conn) << endl;
                return false;
            }

            bool isValid = (mysql_num_rows(result) > 0);
            mysql_free_result(result);

            if (isValid) {
                return true;  // Membership is valid
            } else {
                cout << "Invalid card number or password. Please try again." << endl;
                attempts++;
                if (attempts < 2) {
                    cout << "You have " << (2 - attempts) << " attempt(s) left." << endl;
                    // Ask for details again if attempts left
                    string newCardNumber, newPassword;
                    cout << "Enter your membership card number: ";
                    cin >> newCardNumber;
                    cout << "Enter your password: ";
                    cin >> newPassword;
                    query = "SELECT * FROM Membership WHERE CardNumber = '" + newCardNumber + "' AND Password = '" + newPassword + "'";
                }
            }
        }

        // If reached here, two attempts were made, move to payment
        cout << "You have entered incorrect details twice. Moving to payment section." << endl;
        return false;
    }

    static void registerMembership(MYSQL* conn) {
        string cardNumber, password;

        while (true) {
            cout << "Enter a new membership card number to register: ";
            cin >> cardNumber;

            // Check if the new card number already exists
            string checkQuery = "SELECT * FROM Membership WHERE CardNumber = '" + cardNumber + "'";

            if (mysql_query(conn, checkQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                continue;
            }

            MYSQL_RES* result = mysql_store_result(conn);
            if (result && mysql_num_rows(result) > 0) {
                cout << "Error: Card number already exists. Please enter a different card number." << endl;
                mysql_free_result(result);
                continue;
            }
            mysql_free_result(result);

            std::string password;
    char ch;

    std::cout << "Enter your pin number: ";

    while ((ch = _getch()) != '\r') {  // '\r' is the Enter key
        if (ch == '\b') {  // Handle backspace
            if (!password.empty()) {
                if (!password.empty()) {
    password.erase(password.size() - 1); // Erase the last character
}

                std::cout << "\b \b";  // Erase last character on console
            }
        } else {
            password.push_back(ch);
            std::cout << '*';
        }}
            // Check if the password is unique
            string passwordCheckQuery = "SELECT * FROM Membership WHERE Password = '" + password + "'";

            if (mysql_query(conn, passwordCheckQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                continue;
            }

            MYSQL_RES* passResult = mysql_store_result(conn);
            if (passResult && mysql_num_rows(passResult) > 0) {
                cout << "Error: Password already exists. Try a different password." << endl;
                mysql_free_result(passResult);
                continue;
            }
            mysql_free_result(passResult);

            // Insert new membership record
            string insertQuery = "INSERT INTO Membership (CardNumber, Password) VALUES ('" + cardNumber + "', '" + password + "')";

            if (mysql_query(conn, insertQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
            } else {
                cout << "Membership card registered successfully!" << endl;
                break;
            }
        }
    }
};

class CancelReservation {
public:
    CancelReservation(MYSQL* conn, const std::string& name, const std::string& phonenumber)
        : conn(conn), name(name), phonenumber(phonenumber) {}

    // Function to initiate the cancellation process
    void initiateCancellation() {
        char confirm;
        std::cout << "Do you really want to cancel the ticket? Y/N: ";
        std::cin >> confirm;
        std::cin.ignore();  // Ignore the newline character from previous input

        if (confirm == 'y' || confirm == 'Y') {
            std::string moviename, showtime, date;
            std::cout << "Enter movie name: [Amaran/Dheema/Sitaramam/Shershaa]: ";
            std::cin >> moviename;
            std::cout << "Enter date: [2024-11-14/2024-11-15/2024-11-16]: ";
            std::cin >> date;
            std::cout << "Enter show timing: [11 am/3 pm/7 pm]: ";
            std::cin >> showtime;

            // Validate reservation details
            if (validateUserReservation(name,phonenumber,moviename, date, showtime)) {
                // If reservation is found, proceed to cancel it
                fetchSeatDetails(name,moviename, showtime, date);
                deleteReservation(moviename, showtime, date);
                std::cout << "Ticket cancelled successfully.\n";

                char ch;
                std::cout << "Do you want to continue or exit (Y/N): ";
                std::cin >> ch;
                if (ch == 'y' || ch == 'Y') {
                    return;  // Continue with other tasks
                } else {
                    std::cout << "Exiting program.\n";
                    exit(0);  // Exit the program
                }
            } else {
                std::cout << "No matching reservation found.\n";
            }
        }
    }

private:
    MYSQL* conn;
    std::string name;
    std::string phonenumber;

    bool validateUserReservation(const std::string& name, const std::string& phoneNumber, const std::string& movieName, const std::string& date, const std::string& showTime) {
    // Construct the user table name using the provided name
    std::string userTableName = "User_" + name;  // Ensure this matches your table naming convention
    
    // Construct the SQL query to check if a reservation exists
    std::string query = "SELECT * FROM " + userTableName +
                        " WHERE user_name = '" + name + "' AND phone_number = '" + phoneNumber + 
                        "' AND movie_name = '" + movieName + "' AND date = '" + date + 
                        "' AND show_time = '" + showTime + "';";
    
    // Print the query for debugging purposes
    std::cout << "Executing query: " << query << std::endl;

    // Execute the query
    if (mysql_query(conn, query.c_str())) {
        std::cout << "Error executing query: " << mysql_error(conn) << std::endl;
        return false;
    }

    // Retrieve the result of the query
    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cout << "Error fetching result: " << mysql_error(conn) << std::endl;
        return false;
    }

    // Validate if a matching record was found
    bool isValid = (mysql_num_rows(result) > 0);
    mysql_free_result(result);  // Free the result set to prevent memory leaks

    if (isValid) {
        std::cout << "Reservation validated successfully.\n";
    } else {
        std::cout << "No matching reservation found.\n";
    }

    return isValid;
}


    // Function to delete the reservation from the user table
    void deleteReservation(const std::string& moviename, const std::string& showtime, const std::string& date) {
        std::string userTableName = "User_" + name;  // User-specific table name
        
        std::stringstream queryStream;
        queryStream << "DELETE FROM " << userTableName
                    << " WHERE user_name = '" << name
                    << "' AND phone_number = '" << phonenumber
                    << "' AND movie_name = '" << moviename
                    << "' AND date = '" << date
                    << "' AND show_time = '" << showtime << "';";

        std::string query = queryStream.str();
        std::cout << "Executing delete query: " << query << std::endl;  // Debugging: Print query
        
        if (mysql_query(conn, query.c_str())) {
            std::cout << "Error deleting reservation: " << mysql_error(conn) << std::endl;
        } else {
            std::cout << "Reservation deleted successfully.\n";
        }

       
    }
// Function to fetch seat details
void fetchSeatDetails(const std::string& userName, const std::string& movieName, const std::string& showTime, const std::string& date) {
    // Construct the user table name
    std::string userTableName = "User_" + userName;

    std::stringstream queryStream;
    queryStream << "SELECT `row_number`, `seat_number` "
                << "FROM " << userTableName << " "
                << "WHERE user_name = '" << userName << "' "
                << "AND movie_name = '" << movieName << "' "
                << "AND show_time = '" << showTime << "' "
                << "AND date = '" << date << "';";

    std::string query = queryStream.str();
    std::cout << "Executing SQL query: " << query << std::endl;

    // Execute the query
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Error executing query: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == NULL) {
        std::cerr << "Error retrieving result: " << mysql_error(conn) << std::endl;
        return;
    }

    // Fetch row and seat numbers
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0] && row[1]) {
            int rowNumber = atoi(row[0]);
            int seatNumber = atoi(row[1]);
            std::cout << "Row Number: " << rowNumber << ", Seat Number: " << seatNumber << std::endl;

            // Call to update seat availability
            updateSeatAvailability(movieName, showTime, date, rowNumber, seatNumber);
        }
    }

    // Free the result set
    mysql_free_result(result);
}

// Function to update seat availability
void updateSeatAvailability(const std::string& movieName, const std::string& showTime, const std::string& date, int rowNumber, int seatNumber) {
    // Construct the SQL UPDATE query
    std::stringstream ss;
    ss << "UPDATE " << movieName << " "
       << "SET Seat = 1 "  // Assuming '1' indicates the seat is now available
       << "WHERE RowNumber = " << rowNumber << " "
       << "AND SeatNumber = " << seatNumber << " "
       << "AND Showtime = '" << showTime << "' "
       << "AND Date = '" << date << "';";  // Use backticks if 'Date' is a reserved word

    std::string updateQuery = ss.str();
    std::cout << "Executing update seat availability query: " << updateQuery << std::endl;

    // Execute the query
    if (mysql_query(conn, updateQuery.c_str())) {
        std::cerr << "Error updating seat availability: " << mysql_error(conn) << std::endl;
    } else {
        std::cout << "Seat availability for Row " << rowNumber << ", Seat " << seatNumber << " updated successfully." << std::endl;
    }
}


};
bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);

    // Check if '@' and '.' are present and in the correct order
    if (atPos == string::npos || dotPos == string::npos) {
        return false; // Missing '@' or '.'
    }

    // Check if '@' is not the first character and '.' is after '@'
    if (atPos == 0 || dotPos < atPos + 2) {
        return false; // Invalid positioning of '@' or '.'
    }

    // Check if there is at least one character after the last '.'
    if (dotPos == email.length() - 1) {
        return false; // No characters after '.'
    }

    // Ensure there are no spaces in the email
    if (email.find(' ') != string::npos) {
        return false; // Email should not contain spaces
    }

    return true; // Passed all checks
}

int main() {
    // Initialize vectors without brace initialization
    vector<string> movies;
    movies.push_back("Amaran");
    movies.push_back("Dheema");
    movies.push_back("Sitaramam");
    movies.push_back("Shershaa");

    vector<string> showtimes;
    showtimes.push_back("11am");
    showtimes.push_back("3pm");
    showtimes.push_back("7pm");

    vector<string> availableDates;
    availableDates.push_back("2024-11-14");
    availableDates.push_back("2024-11-15");
    availableDates.push_back("2024-11-16"); // Sample available dates
    Seats s;
   
    MYSQL* conn;
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, HOST, USER, PW, DB, 3306, NULL, 0)) {
        cout << "Error: " << mysql_error(conn) << endl;
        return 1; // Exit if connection fails
    } else {
        cout << "Logged In Database!" << endl;
    }
    Sleep(3000);

    // Create movie tables with showtimes and dates
    for (size_t i = 0; i < movies.size(); i++) {
        string createTableQuery = "CREATE TABLE IF NOT EXISTS " + movies[i] + " (RowNumber INT, SeatNumber INT, Seat INT, Showtime VARCHAR(5), Date DATE)";
        if (mysql_query(conn, createTableQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
    }

    // Initialize seats in the database for each movie and date
    for (size_t i = 0; i < movies.size(); i++) {
        for (size_t j = 0; j < showtimes.size(); j++) {
            for (size_t k = 0; k < availableDates.size(); k++) {
                for (int row = 1; row <= 5; row++) {
                    for (int seatNumber = 1; seatNumber <= 10; seatNumber++) {
                        stringstream ss;
                        ss << "INSERT INTO " << movies[i] << " (RowNumber, SeatNumber, Seat, Showtime, Date) "
                           << "SELECT " << row << ", " << seatNumber << ", 1, '" << showtimes[j] << "', '" << availableDates[k] << "' "
                           << "WHERE NOT EXISTS (SELECT * FROM " << movies[i] << " WHERE RowNumber = " << row << " AND SeatNumber = " << seatNumber << " AND Showtime = '" << showtimes[j] << "' AND Date = '" << availableDates[k] << "')";
                        string insertQuery = ss.str();
                        if (mysql_query(conn, insertQuery.c_str())) {
                            cout << "Error: " << mysql_error(conn) << endl;
                        }
                    }
                }
            }
        }
    }

   bool exit = false;
while (!exit) {
    vector<pair<int, int> > userReservedSeats(10, make_pair(0, 0));
    system("cls");
    cout << "Welcome To Movie Ticket Booking System!!!" << endl;
    cout << "" << endl;
    cout << "Do you want to 1)Reserve tickets 2)Cancell tickets : ";
    int choice;
    cin >> choice;
if(choice==1){
    // Display movies
    cout << "Select a movie:" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i + 1 << ". " << movies[i] << endl;
    }

    int movieChoice;
    cout << "Enter your choice: ";
    cin >> movieChoice;

    if (movieChoice < 1 || movieChoice > movies.size()) {
        cout << "Invalid choice! Please try again." << endl;
        continue;
    }

    // Display available dates
    cout << "Available Dates:" << endl;
    for (size_t i = 0; i < availableDates.size(); i++) {
        cout << i + 1 << ". " << availableDates[i] << endl;
    }

    int dateChoice;
    cout << "Select Date: ";
    cin >> dateChoice;

    if (dateChoice < 1 || dateChoice > availableDates.size()) {
        cout << "Invalid choice! Please try again." << endl;
        continue;
    }

    // Display showtimes
    cout << "Available Showtimes for " << movies[movieChoice - 1] << ":" << endl;
    for (size_t i = 0; i < showtimes.size(); i++) {
        cout << i + 1 << ". " << showtimes[i] << endl;
    }

    int showtimeChoice;
    cout << "Select Show Time: ";
    cin >> showtimeChoice;

    if (showtimeChoice < 1 || showtimeChoice > showtimes.size()) {
        cout << "Invalid choice! Please try again." << endl;
        continue;
    }

    // Load existing seat status from the database
    s.getDB(conn, movies[movieChoice - 1], showtimes[showtimeChoice - 1], availableDates[dateChoice - 1]);

    // Display seats
   s.display(conn,movieChoice,showtimeChoice, dateChoice);

    // Ask if the user wants to continue or change the selection
    int c;
    cout << "Do you want to 1) continue further 2) change selection [movie/date/showtime] 3) exit ? (1/2/3) : ";
    cin >> c;

    switch (c) {
        case 1:
            // Proceed to the next step, booking
            cout << "Proceeding to booking...." << endl;
            break;

        case 2:
           continue;
        case 3:
        	return 0;
    }
    

        // Seat selection process
        while (true) {
            int row, col;
            cout << "Row and Seat Number: ";
            cin >> row >> col;

            if (row == 0 && col == 0) {
                break; // Finish seat selection
            }

            if (row < 1 || row > 5 || col < 1 || col > 10) {
                cout << "Invalid input! Please try again." << endl;
                continue;
            }
          
            if (s.getSeatStatus(row, col) == 1) {
                s.reserveSeat(row, col); // Reserve seat
                userReservedSeats.push_back(make_pair(row, col)); 
                cout << "Seat " << row << "-" << col << " reserved." << endl;
            } else {
                cout << "Seat " << row << "-" << col << " is already reserved!" << endl;
            }
        }

        // Update the database with the reserved seats
        s.updateDB(conn, movies[movieChoice - 1], showtimes[showtimeChoice - 1], availableDates[dateChoice - 1]);

        // Ask for user details
        string name;
        string emailid;
        string phoneNumber;

        cout << "Enter your name: ";
        cin.ignore(); // Clear newline character from the input buffer
        getline(cin, name);
        
        while(true){
        cout << "Enter your Email id: ";
        cin >> emailid;
        if (isValidEmail(emailid)) {
        cout << "Valid email ID." << endl;
        break;
    } else {
        cout << "Invalid email ID. Please enter a valid email.\n" << endl;
    }
}

        cout << "Enter your phone number: ";
        while (true) {
        cin >> phoneNumber;

        // Check if phone number is exactly 10 digits and all characters are digits
        if (phoneNumber.length() == 10) {
            bool isValid = true;
            for (size_t i = 0; i < phoneNumber.length(); ++i) {
                if (!isdigit(phoneNumber[i])) {
                    isValid = false;
                    break;
                }
            }
            if (isValid) {
                break; // Valid phone number, exit loop
            }
        }

        // If not valid, prompt again
        cout << "Phone number is invalid\n. Please enter a valid 10-digit phone number: ";
    }
     string b;
     cout << "Do you want beverages : Y/N";
     cin >> b;
        
        
      //membership 
      
      // Inside main function
Membership::createMembershipTable(conn);

char membershipCard;
cout << "Do you have a membership card? (y/n): ";
cin >> membershipCard;

if (membershipCard == 'y' || membershipCard == 'Y') {
    string cardNumber, password;
    cout << "Enter your membership card number: ";
    cin >> cardNumber;
     std::string pin;
    char ch;

    std::cout << "Enter your pin number: ";

    while ((ch = _getch()) != '\r') {  // '\r' is the Enter key
        if (ch == '\b') {  // Handle backspace
            if (!pin.empty()) {
                if (!pin.empty()) {
    pin.erase(pin.size() - 1); // Erase the last character
}

                std::cout << "\b \b";  // Erase last character on console
            }
        } else {
            pin.push_back(ch);
            std::cout << '*';
        }}
    if (Membership::validateMembership(conn, cardNumber, pin)) {
        cout << "20% Discount will be provided!" << endl;
    } else {
        cout << "No membership found. No discount applied." << endl;
    }
} else {
    string n;
    cout << "Do you want to apply for membership card? (Y/N): ";
    cin >> n;
    
    // Fix the condition to correctly check for both "Y" and "y"
    if (n == "Y" || n == "y") {
        Membership::registerMembership(conn);
    } else {
        // If user selects N/n, proceed to the next step (like payment)
        cout << "Proceeding without membership registration." << endl;
    }
}

// Continue with payment and ticket printing logic...

        // Payment options
        cout << "Select Payment Method: " << endl;
        cout << "1. Credit Card" << endl;
        cout << "2. Google Pay" << endl;

        int paymentMethod;
        cout << "Enter your choice: ";
        cin >> paymentMethod;

        if (paymentMethod == 1) {
            string cardNumber, pinNumber;
            if(b=="Y")
            {
            	cout<<"beverages charges included";
			}
            cout << "Enter Card Number: ";
            cin >> cardNumber;
            std::string password;
    char ch;

    std::cout << "Enter your pin number: ";

    while ((ch = _getch()) != '\r') {  // '\r' is the Enter key
        if (ch == '\b') {  // Handle backspace
            if (!password.empty()) {
                if (!password.empty()) {
    password.erase(password.size() - 1); // Erase the last character
}

                std::cout << "\b \b";  // Erase last character on console
            }
        } else {
            password.push_back(ch);
            std::cout << '*';
        }}
            // Here you would handle payment processing logic
        } else if (paymentMethod == 2) {
        	if(b=="Y")
            {
            	cout<<"beverages charges included";
			}
            cout << "Processing Google Pay..." << endl;
            // Here you would handle Google Pay processing logic
        }

        cout << "\nPayment Successful! Thank you for your booking!" << endl;
        Sleep(3000);
     
       
// After collecting user details and completing seat selection

// Create a unique table for the user if not already created
std::string userTableName = "User_" + name; // Unique table name based on the user's name or phone number

// Create table query with backticks around reserved column names
std::string createUserTableQuery = "CREATE TABLE IF NOT EXISTS " + userTableName + " ("
                                   "user_name VARCHAR(50), "
                                   "email_id VARCHAR(50), "
                                   "phone_number VARCHAR(10), "
                                   "movie_name VARCHAR(50), "
                                   "date DATE, "
                                   "show_time VARCHAR(5), "
                                   "`row_number` INT, "  // Backticks for reserved keyword
                                   "`seat_number` INT, " // Backticks for reserved keyword
                                   "beverages_status VARCHAR(3))" 
								   ";";

if (mysql_query(conn, createUserTableQuery.c_str())) {
    std::cout << "Error creating table: " << mysql_error(conn) << std::endl;
}

// Insert booking details into the user's table
for (std::vector<std::pair<int, int> >::const_iterator it = userReservedSeats.begin(); it != userReservedSeats.end(); ++it) {
    if (it->first > 0 && it->second > 0) { // Check valid seat selections
        std::stringstream ss;
        ss << "INSERT INTO " << userTableName 
           << " (user_name, email_id, phone_number, movie_name, date, show_time, `row_number`, `seat_number`, beverages_status) "
           << "VALUES ('" << name << "', '" << emailid << "', '" << phoneNumber << "', '"
           << movies[movieChoice - 1] << "', '" << availableDates[dateChoice - 1] << "', '"
           << showtimes[showtimeChoice - 1] << "', " << it->first << ", " << it->second << ", '" << b << "')";

        if (mysql_query(conn, ss.str().c_str())) {
            std::cout << "Error inserting data: " << mysql_error(conn) << std::endl;
        }
    }
}

// Display the ticket from the table
std::cout << "-------------------- TICKET --------------------" << std::endl;
std::string fetchTicketQuery = "SELECT * FROM " + userTableName;
if (mysql_query(conn, fetchTicketQuery.c_str())) {
    std::cout << "Error fetching ticket: " << mysql_error(conn) << std::endl;
} else {
    MYSQL_RES* ticketResult = mysql_store_result(conn);
    if (!ticketResult) {
        std::cout << "Error storing result: " << mysql_error(conn) << std::endl;
    } else {
        MYSQL_ROW ticketRow;
        if (mysql_num_rows(ticketResult) == 0) {
            std::cout << "No reservation details found." << std::endl;
        } else {
            while ((ticketRow = mysql_fetch_row(ticketResult))) {
                std::cout << "Name: " << ticketRow[0] << std::endl;
                std::cout << "Email ID: " << ticketRow[1] << std::endl;
                std::cout << "Phone Number: " << ticketRow[2] << std::endl;
                std::cout << "Movie: " << ticketRow[3] << std::endl;
                std::cout << "Date: " << ticketRow[4] << std::endl;
                std::cout << "Show Time: " << ticketRow[5] << std::endl;
                std::cout << "Seat: " << ticketRow[6] << "-" << ticketRow[7] << std::endl;
                std::cout << "Beverages: " << ticketRow[8] << std::endl;
                std::cout << "-----------------------------------------------" << std::endl;
            }
        }
        mysql_free_result(ticketResult);
    }
}

std::cout << "Enjoy your movie time!" << std::endl;
std::cout << "-----------------------------------------------" << std::endl;
}
else if(choice==2){
	string name,phonenumber;
        	cout<< "Enter your name to proceed cancellation:";
		    cin >> name;
		    cout<<"Phone number:";
		    cin >> phonenumber;
		    
		     CancelReservation cancel(conn, name, phonenumber);
		     cancel.initiateCancellation();
}
else{
	cout << "Invalid choice";
}
        // Ask if the user wants to exit
        cout << "Do you want to book another ticket / cancel a ticket ? (1: Yes, 0: No): ";
        int anotherBooking;
        cin >> anotherBooking;
        exit = (anotherBooking != 1);
}

    mysql_close(conn);
    return 0;
}
