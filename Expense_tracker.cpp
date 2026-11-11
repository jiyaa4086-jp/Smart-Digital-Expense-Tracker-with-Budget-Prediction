
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
using namespace std;

const int MAX_USERS = 100;
const int MAX_EXPENSES = 500;
const int MAX_BUDGETS = 200;

// Structure for user details
struct User {
    string username;
    string password;
};

// Structure for expense details
struct Expense {
    int id;
    string username;
    string itemName;
    string category;
    double amount;
    string date;
};

// Structure for monthly budget
struct Budget {
    string username;
    string month;
    double amount;
};

// Global arrays
User users[MAX_USERS];
Expense expenses[MAX_EXPENSES];
Budget budgets[MAX_BUDGETS];

int userCount = 0;
int expenseCount = 0;
int budgetCount = 0;
int nextID = 1;

string currentUser = "";

// Function to read a line
string getText(string message) {
    string value;
    cout << message;
    getline(cin, value);
    return value;
}

// Function to read an integer safely
int getInteger(string message) {
    string input;
    int value;
    char extra;

    while (true) {
        cout << message;
        getline(cin, input);

        stringstream ss(input);

        if (ss >> value && !(ss >> extra)) {
            return value;
        }

        cout << "Please enter a valid number!\n";
    }
}

// Function to read an amount
double getAmount(string message) {
    string input;
    double value;
    char extra;

    while (true) {
        cout << message;
        getline(cin, input);

        stringstream ss(input);

        if (ss >> value && !(ss >> extra) && value > 0) {
            return value;
        }

        cout << "Please enter a valid positive amount!\n";
    }
}

// Convert string into lowercase
string toLowerCase(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char c) {
                  return tolower(c);
              });

    return value;
}

// Replace file separator
void cleanText(string &value) {
    replace(value.begin(), value.end(), '|', '/');
}

// Get current month in YYYY-MM format
string getCurrentMonth() {
    time_t now = time(0);
    tm *localTime = localtime(&now);

    stringstream ss;

    ss << localTime->tm_year + 1900 << "-"
       << setw(2) << setfill('0')
       << localTime->tm_mon + 1;

    return ss.str();
}

// Get current day
int getCurrentDay() {
    time_t now = time(0);
    tm *localTime = localtime(&now);

    return localTime->tm_mday;
}

// Convert DD-MM-YYYY into YYYY-MM
string getMonth(string date) {
    if (date.length() == 10 &&
        date[2] == '-' && date[5] == '-') {
        return date.substr(6, 4) + "-" + date.substr(3, 2);
    }

    return "";
}

// Check date format
bool validDate(string date) {
    if (date.length() != 10 ||
        date[2] != '-' || date[5] != '-') {
        return false;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;

        if (!isdigit((unsigned char)date[i])) {
            return false;
        }
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (year < 2000 || month < 1 || month > 12 ||
        day < 1 || day > 31) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear = (year % 400 == 0) ||
                    (year % 4 == 0 && year % 100 != 0);

    if (leapYear && month == 2) {
        daysInMonth[1] = 29;
    }

    return day <= daysInMonth[month - 1];
}

// Module 12: Save users
void saveUsers() {
    ofstream file("users.txt");

    for (int i = 0; i < userCount; i++) {
        file << users[i].username << "|"
             << users[i].password << "\n";
    }

    file.close();
}

// Module 12: Load users
void loadUsers() {
    ifstream file("users.txt");
    string line;

    userCount = 0;

    while (getline(file, line) && userCount < MAX_USERS) {
        stringstream ss(line);
        User temp;

        if (getline(ss, temp.username, '|') &&
            getline(ss, temp.password)) {
            users[userCount++] = temp;
        }
    }

    file.close();
}

// Save expenses
void saveExpenses() {
    ofstream file("expenses.txt");

    for (int i = 0; i < expenseCount; i++) {
        file << expenses[i].id << "|"
             << expenses[i].username << "|"
             << expenses[i].itemName << "|"
             << expenses[i].category << "|"
             << expenses[i].amount << "|"
             << expenses[i].date << "\n";
    }

    file.close();
}

// Load expenses
void loadExpenses() {
    ifstream file("expenses.txt");
    string line;

    expenseCount = 0;
    nextID = 1;

    while (getline(file, line) &&
           expenseCount < MAX_EXPENSES) {

        stringstream ss(line);
        string field;
        Expense temp;

        try {
            if (!getline(ss, field, '|')) continue;
            temp.id = stoi(field);

            if (!getline(ss, temp.username, '|')) continue;
            if (!getline(ss, temp.itemName, '|')) continue;
            if (!getline(ss, temp.category, '|')) continue;

            if (!getline(ss, field, '|')) continue;
            temp.amount = stod(field);

            if (!getline(ss, temp.date)) continue;

            expenses[expenseCount++] = temp;

            if (temp.id >= nextID) {
                nextID = temp.id + 1;
            }
        }
        catch (...) {
            continue;
        }
    }

    file.close();
}

// Save budgets
void saveBudgets() {
    ofstream file("budgets.txt");

    for (int i = 0; i < budgetCount; i++) {
        file << budgets[i].username << "|"
             << budgets[i].month << "|"
             << budgets[i].amount << "\n";
    }

    file.close();
}

// Load budgets
void loadBudgets() {
    ifstream file("budgets.txt");
    string line;

    budgetCount = 0;

    while (getline(file, line) &&
           budgetCount < MAX_BUDGETS) {

        stringstream ss(line);
        string field;
        Budget temp;

        try {
            if (!getline(ss, temp.username, '|')) continue;
            if (!getline(ss, temp.month, '|')) continue;
            if (!getline(ss, field)) continue;

            temp.amount = stod(field);

            budgets[budgetCount++] = temp;
        }
        catch (...) {
            continue;
        }
    }

    file.close();
}

// Module 1: User Registration
void registerUser() {
    if (userCount >= MAX_USERS) {
        cout << "\nUser storage is full!\n";
        return;
    }

    string username, password;

    cout << "\n===== USER REGISTRATION =====\n";

    username = getText("Enter Username: ");
    password = getText("Enter Password: ");

    if (username.empty() || password.empty() ||
        username.find('|') != string::npos ||
        password.find('|') != string::npos) {
        cout << "\nInvalid username or password!\n";
        return;
    }

    for (int i = 0; i < userCount; i++) {
        if (users[i].username == username) {
            cout << "\nUsername Already Exists!\n";
            return;
        }
    }

    users[userCount].username = username;
    users[userCount].password = password;

    userCount++;

    saveUsers();

    cout << "\nRegistration Successful!\n";
}

// Module 2: Login System
bool loginSystem() {
    string username, password;

    cout << "\n===== USER LOGIN =====\n";

    username = getText("Enter Username: ");
    password = getText("Enter Password: ");

    for (int i = 0; i < userCount; i++) {

        if (users[i].username == username &&
            users[i].password == password) {

            currentUser = username;

            cout << "\nLogin Successful!\n";
            cout << "Welcome, " << currentUser << "!\n";

            return true;
        }
    }

    cout << "\nInvalid Username or Password!\n";
    return false;
}

// Calculate total expense for a month
double calculateMonthlyExpense(string month) {
    double total = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].username == currentUser &&
            getMonth(expenses[i].date) == month) {

            total += expenses[i].amount;
        }
    }

    return total;
}

// Find budget for a month
int findBudget(string month) {
    for (int i = 0; i < budgetCount; i++) {
        if (budgets[i].username == currentUser &&
            budgets[i].month == month) {
            return i;
        }
    }

    return -1;
}

// Module 3: Add Daily Expense
void addExpense() {
    if (expenseCount >= MAX_EXPENSES) {
        cout << "\nExpense storage is full!\n";
        return;
    }

    Expense newExpense;

    cout << "\n===== ADD DAILY EXPENSE =====\n";

    newExpense.id = nextID++;
    newExpense.username = currentUser;

    newExpense.itemName = getText("Enter Expense Name: ");
    newExpense.category = getText(
        "Enter Category (Food, Travel, Shopping, Education, etc.): "
    );

    if (newExpense.itemName.empty() ||
        newExpense.category.empty()) {
        cout << "\nExpense name and category cannot be empty!\n";
        nextID--;
        return;
    }

    cleanText(newExpense.itemName);
    cleanText(newExpense.category);

    newExpense.amount = getAmount("Enter Amount: ");

    while (true) {
        string date = getText("Enter Date (DD-MM-YYYY): ");

        if (validDate(date)) {
            newExpense.date = date;
            break;
        }

        cout << "Invalid date! Please enter a valid date.\n";
    }

    expenses[expenseCount++] = newExpense;

    saveExpenses();

    cout << "\nExpense Added Successfully!\n";
    cout << "Expense ID: " << newExpense.id << endl;
}

// Module 4: View Expense History
void viewExpenseHistory() {
    bool found = false;

    // Sort expenses by ID using Bubble Sort
    for (int i = 0; i < expenseCount - 1; i++) {
        for (int j = 0; j < expenseCount - i - 1; j++) {

            if (expenses[j].id > expenses[j + 1].id) {
                swap(expenses[j], expenses[j + 1]);
            }
        }
    }

    cout << "\n===== EXPENSE HISTORY =====\n";

    for (int i = 0; i < expenseCount; i++) {

        if (expenses[i].username == currentUser) {
            cout << "\nExpense ID : " << expenses[i].id << endl;
            cout << "Name       : " << expenses[i].itemName << endl;
            cout << "Category   : " << expenses[i].category << endl;
            cout << "Amount     : Rs. "
                 << fixed << setprecision(2)
                 << expenses[i].amount << endl;
            cout << "Date       : " << expenses[i].date << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "\nNo Expenses Found!\n";
    }

    saveExpenses();
}

// Module 5: Category-wise Expense
void categoryWiseExpense() {
    string categories[MAX_EXPENSES];
    int categoryCount = 0;

    cout << "\n===== CATEGORY-WISE EXPENSE =====\n";

    for (int i = 0; i < expenseCount; i++) {

        if (expenses[i].username != currentUser) {
            continue;
        }

        string category = toLowerCase(expenses[i].category);
        bool exists = false;

        for (int j = 0; j < categoryCount; j++) {
            if (categories[j] == category) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            categories[categoryCount++] = category;
        }
    }

    if (categoryCount == 0) {
        cout << "\nNo Expense Categories Available!\n";
        return;
    }

    for (int i = 0; i < categoryCount; i++) {
        double total = 0;

        for (int j = 0; j < expenseCount; j++) {
            if (expenses[j].username == currentUser &&
                toLowerCase(expenses[j].category) == categories[i]) {

                total += expenses[j].amount;
            }
        }

        cout << categories[i] << " : Rs. "
             << fixed << setprecision(2) << total << endl;
    }
}

// Module 6: Monthly Expense Report
void monthlyExpenseReport() {
    string month;

    cout << "\n===== MONTHLY EXPENSE REPORT =====\n";

    month = getText("Enter Month (YYYY-MM): ");

    if (month.length() != 7 ||
        month[4] != '-' ||
        !all_of(month.begin(), month.end(),
            [position = 0](char) mutable { return true; })) {
        cout << "\nInvalid month format!\n";
        return;
    }

    int year, monthNumber;

    try {
        year = stoi(month.substr(0, 4));
        monthNumber = stoi(month.substr(5, 2));
    }
    catch (...) {
        cout << "\nInvalid month format!\n";
        return;
    }

    if (year < 2000 || monthNumber < 1 || monthNumber > 12) {
        cout << "\nInvalid month!\n";
        return;
    }

    double total = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].username == currentUser &&
            getMonth(expenses[i].date) == month) {

            cout << "\n" << expenses[i].itemName
                 << " - Rs. " << expenses[i].amount;

            total += expenses[i].amount;
        }
    }

    cout << "\n\nTotal Expense for " << month
         << " : Rs. " << fixed << setprecision(2)
         << total << endl;

    int budgetIndex = findBudget(month);

    if (budgetIndex != -1) {
        cout << "Monthly Budget: Rs. "
             << budgets[budgetIndex].amount << endl;
    }
}

// Module 7: Budget Setting
void setBudget() {
    string month;

    cout << "\n===== SET MONTHLY BUDGET =====\n";

    month = getText("Enter Month (YYYY-MM): ");

    int year, monthNumber;

    try {
        if (month.length() != 7 || month[4] != '-') {
            throw 1;
        }

        year = stoi(month.substr(0, 4));
        monthNumber = stoi(month.substr(5, 2));

        if (year < 2000 || monthNumber < 1 || monthNumber > 12) {
            throw 1;
        }
    }
    catch (...) {
        cout << "\nInvalid Month Format!\n";
        return;
    }

    double amount = getAmount("Enter Monthly Budget (Rs.): ");

    int index = findBudget(month);

    if (index != -1) {
        budgets[index].amount = amount;
    }
    else {
        if (budgetCount >= MAX_BUDGETS) {
            cout << "\nBudget storage is full!\n";
            return;
        }

        budgets[budgetCount].username = currentUser;
        budgets[budgetCount].month = month;
        budgets[budgetCount].amount = amount;

        budgetCount++;
    }

    saveBudgets();

    cout << "\nMonthly Budget Set Successfully!\n";
}

// Module 8: Budget Warning
void budgetWarning() {
    string month = getCurrentMonth();

    int index = findBudget(month);

    cout << "\n===== BUDGET WARNING =====\n";

    if (index == -1) {
        cout << "Please Set Your Budget First!\n";
        return;
    }

    double budget = budgets[index].amount;
    double spent = calculateMonthlyExpense(month);

    cout << "Monthly Budget : Rs. " << budget << endl;
    cout << "Total Spending : Rs. " << spent << endl;

    double percentage = (spent / budget) * 100;

    cout << "Budget Used    : "
         << fixed << setprecision(2)
         << percentage << "%\n";

    if (spent > budget) {
        cout << "\nWARNING! You Have Exceeded Your Budget!\n";
    }
    else if (percentage >= 80) {
        cout << "\nALERT! You Have Used 80% or More of Your Budget!\n";
    }
    else {
        cout << "\nGood! You Are Within Your Budget.\n";
    }
}

// Module 9: Expense Prediction
void expensePrediction() {
    string month = getCurrentMonth();

    int index = findBudget(month);

    cout << "\n===== EXPENSE PREDICTION =====\n";

    if (index == -1) {
        cout << "Please Set Your Monthly Budget First!\n";
        return;
    }

    double budget = budgets[index].amount;
    double spent = calculateMonthlyExpense(month);

    int currentDay = getCurrentDay();

    time_t now = time(0);
    tm *localTime = localtime(&now);

    int year = localTime->tm_year + 1900;
    int monthNumber = localTime->tm_mon + 1;

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear = (year % 400 == 0) ||
                    (year % 4 == 0 && year % 100 != 0);

    if (leapYear && monthNumber == 2) {
        daysInMonth[1] = 29;
    }

    int totalDays = daysInMonth[monthNumber - 1];

    double predictedExpense =
        (spent / currentDay) * totalDays;

    cout << "Current Spending : Rs. " << spent << endl;
    cout << "Monthly Budget   : Rs. " << budget << endl;
    cout << "Predicted Expense: Rs. "
         << fixed << setprecision(2)
         << predictedExpense << endl;

    if (predictedExpense > budget) {
        cout << "\nPrediction: Your Budget May Be Exceeded!\n";
    }
    else {
        cout << "\nPrediction: Your Spending Is Within Budget.\n";
    }

    cout << "Note: Prediction assumes your average daily spending "
         << "continues for the rest of the month.\n";
}

// Module 10: Search Expense
void searchExpense() {
    string searchText;
    bool found = false;

    cout << "\n===== SEARCH EXPENSE =====\n";

    searchText = toLowerCase(
        getText("Enter Expense Name or Category: ")
    );

    for (int i = 0; i < expenseCount; i++) {

        if (expenses[i].username != currentUser) {
            continue;
        }

        string name = toLowerCase(expenses[i].itemName);
        string category = toLowerCase(expenses[i].category);

        if (name.find(searchText) != string::npos ||
            category.find(searchText) != string::npos) {

            cout << "\nExpense ID: " << expenses[i].id << endl;
            cout << "Name      : " << expenses[i].itemName << endl;
            cout << "Category  : " << expenses[i].category << endl;
            cout << "Amount    : Rs. " << expenses[i].amount << endl;
            cout << "Date      : " << expenses[i].date << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "\nNo Matching Expense Found!\n";
    }
}

// Module 11: Delete Expense
void deleteExpense() {
    int deleteID;
    bool found = false;

    cout << "\n===== DELETE EXPENSE =====\n";

    deleteID = getInteger("Enter Expense ID: ");

    for (int i = 0; i < expenseCount; i++) {

        if (expenses[i].id == deleteID &&
            expenses[i].username == currentUser) {

            for (int j = i; j < expenseCount - 1; j++) {
                expenses[j] = expenses[j + 1];
            }

            expenseCount--;

            saveExpenses();

            cout << "\nExpense Deleted Successfully!\n";

            found = true;
            break;
        }
    }

    if (!found) {
        cout << "\nExpense ID Not Found!\n";
    }
}

// Main Menu
void displayMenu() {
    cout << "\n\n====================================\n";
    cout << "   SMART EXPENSE MANAGEMENT SYSTEM\n";
    cout << "====================================\n";

    cout << "1. Add Daily Expense\n";
    cout << "2. View Expense History\n";
    cout << "3. Category-wise Expense\n";
    cout << "4. Monthly Expense Report\n";
    cout << "5. Set Monthly Budget\n";
    cout << "6. Budget Warning\n";
    cout << "7. Expense Prediction\n";
    cout << "8. Search Expense\n";
    cout << "9. Delete Expense\n";
    cout << "10. Save Data\n";
    cout << "11. Logout\n";

    cout << "====================================\n";
}

// Main Function
int main() {
    int choice;
    bool loggedIn = false;

    // Load saved data
    loadUsers();
    loadExpenses();
    loadBudgets();

    cout << "====================================\n";
    cout << " WELCOME TO SMART EXPENSE MANAGEMENT\n";
    cout << "====================================\n";

    // Registration and login menu
    while (!loggedIn) {
        cout << "\n1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";

        choice = getInteger("Enter Your Choice: ");

        switch (choice) {
            case 1:
                registerUser();
                break;

            case 2:
                if (loginSystem()) {
                    loggedIn = true;
                }
                break;

            case 3:
                cout << "\nThank You! Exiting Program...\n";
                return 0;

            default:
                cout << "\nInvalid Choice!\n";
        }
    }

    // Main application loop
    do {
        displayMenu();

        choice = getInteger("Enter Your Choice: ");

        switch (choice) {
            case 1:
                addExpense();
                break;

            case 2:
                viewExpenseHistory();
                break;

            case 3:
                categoryWiseExpense();
                break;

            case 4:
                monthlyExpenseReport();
                break;

            case 5:
                setBudget();
                break;

            case 6:
                budgetWarning();
                break;

            case 7:
                expensePrediction();
                break;

            case 8:
                searchExpense();
                break;

            case 9:
                deleteExpense();
                break;

            case 10:
                saveUsers();
                saveExpenses();
                saveBudgets();
                cout << "\nAll Data Saved Successfully!\n";
                break;

            case 11:
                saveUsers();
                saveExpenses();
                saveBudgets();

                currentUser = "";
                loggedIn = false;

                cout << "\nLogged Out Successfully!\n";
                break;

            default:
                cout << "\nInvalid Choice! Try Again.\n";
        }

    } while (loggedIn);

    return 0;
}
