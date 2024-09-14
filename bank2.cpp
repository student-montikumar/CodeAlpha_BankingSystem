#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;

// Transaction Class
class Transaction {
private:
    int transactionID;
    string transactionType;
    double amount;
    time_t date;

public:
    Transaction(int id, string type, double amt) : transactionID(id), transactionType(type), amount(amt) {
        date = time(0);
    }

    void displayTransaction() const {
        cout << "Transaction ID: " << transactionID
             << ", Type: " << transactionType
             << ", Amount: " << amount
             << ", Date: " << ctime(&date);
    }

    string getTransactionDetails() const {
        return to_string(transactionID) + "," + transactionType + "," + to_string(amount) + "," + to_string(date) + "\n";
    }
};

// Account Class
class Account {
private:
    int accountNumber;
    double balance;
    vector<Transaction> transactions;
    
public:
    Account(int accNum, double bal = 0.0) : accountNumber(accNum), balance(bal) {}

    void deposit(double amount) {
        balance += amount;
        transactions.push_back(Transaction(transactions.size() + 1, "Deposit", amount));
        cout << "Deposited: $" << amount << " to account " << accountNumber << endl;
    }

    void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            transactions.push_back(Transaction(transactions.size() + 1, "Withdrawal", amount));
            cout << "Withdrew: $" << amount << " from account " << accountNumber << endl;
        } else {
            cout << "Insufficient balance for withdrawal." << endl;
        }
    }

    void transfer(Account &targetAccount, double amount) {
        if (amount <= balance) {
            balance -= amount;
            targetAccount.balance += amount;
            transactions.push_back(Transaction(transactions.size() + 1, "Transfer", amount));
            targetAccount.transactions.push_back(Transaction(targetAccount.transactions.size() + 1, "Transfer", amount));
            cout << "Transferred: $" << amount << " to account " << targetAccount.accountNumber << endl;
        } else {
            cout << "Insufficient balance for transfer." << endl;
        }
    }

    void displayBalance() const {
        cout << "Account Number: " << accountNumber << ", Balance: $" << balance << endl;
    }

    void displayTransactions() const {
        cout << "Transactions for Account " << accountNumber << ":" << endl;
        for (const auto &trans : transactions) {
            trans.displayTransaction();
        }
    }

    string getAccountDetails() const {
        return to_string(accountNumber) + "," + to_string(balance) + "\n";
    }

    int getAccountNumber() const {
        return accountNumber;
    }
};

// Customer Class
class Customer {
private:
    string name;
    int customerID;
    vector<Account> accounts;

public:
    Customer(string custName, int custID) : name(custName), customerID(custID) {}

    void addAccount(int accountNumber) {
        accounts.push_back(Account(accountNumber));
        cout << "Account " << accountNumber << " created for customer " << name << endl;
    }

    void viewAccounts() const {
        cout << "Accounts for " << name << " (ID: " << customerID << "):" << endl;
        for (const auto &acc : accounts) {
            acc.displayBalance();
        }
    }

    Account* getAccount(int accountNumber) {
        for (auto &acc : accounts) {
            if (acc.getAccountNumber() == accountNumber) {
                return &acc;
            }
        }
        return nullptr;
    }

    string getCustomerDetails() const {
        string details = to_string(customerID) + "," + name + "\n";
        for (const auto &acc : accounts) {
            details += acc.getAccountDetails();
        }
        return details;
    }

    int getCustomerID() const {
        return customerID;
    }
};

// Banking Service Class with File I/O
class BankingService {
private:
    vector<Customer> customers;

    // Load customer data from a file
    void loadCustomersFromFile() {
        ifstream inFile("customers.txt");
        if (!inFile) {
            cout << "No customer data file found." << endl;
            return;
        }

        string line;
        while (getline(inFile, line)) {
            string name;
            int customerID;
            sscanf(line.c_str(), "%d,%s", &customerID, &name[0]);

            Customer customer(name, customerID);
            customers.push_back(customer);

            while (getline(inFile, line) && !line.empty()) {
                int accountNumber;
                double balance;
                sscanf(line.c_str(), "%d,%lf", &accountNumber, &balance);
                customer.addAccount(accountNumber);
            }
        }

        inFile.close();
    }

    // Save customer data to a file
    void saveCustomersToFile() const {
        ofstream outFile("customers.txt");
        for (const auto &customer : customers) {
            outFile << customer.getCustomerDetails();
        }
        outFile.close();
    }

public:
    BankingService() {
        loadCustomersFromFile();  // Load data on startup
    }

    ~BankingService() {
        saveCustomersToFile();  // Save data on exit
    }

    void addCustomer(string name, int customerID) {
        customers.push_back(Customer(name, customerID));
        cout << "Customer " << name << " with ID " << customerID << " added." << endl;
    }

    Customer* getCustomer(int customerID) {
        for (auto &cust : customers) {
            if (cust.getCustomerID() == customerID) {
                return &cust;
            }
        }
        return nullptr;
    }

    void performDeposit(int customerID, int accountNumber, double amount) {
        Customer* customer = getCustomer(customerID);
        if (customer) {
            Account* account = customer->getAccount(accountNumber);
            if (account) {
                account->deposit(amount);
            } else {
                cout << "Account not found." << endl;
            }
        } else {
            cout << "Customer not found." << endl;
        }
    }

    void performWithdrawal(int customerID, int accountNumber, double amount) {
        Customer* customer = getCustomer(customerID);
        if (customer) {
            Account* account = customer->getAccount(accountNumber);
            if (account) {
                account->withdraw(amount);
            } else {
                cout << "Account not found." << endl;
            }
        } else {
            cout << "Customer not found." << endl;
        }
    }

    void performTransfer(int customerID, int fromAccountNumber, int toAccountNumber, double amount) {
        Customer* customer = getCustomer(customerID);
        if (customer) {
            Account* fromAccount = customer->getAccount(fromAccountNumber);
            Account* toAccount = customer->getAccount(toAccountNumber);
            if (fromAccount && toAccount) {
                fromAccount->transfer(*toAccount, amount);
            } else {
                cout << "One or both accounts not found." << endl;
            }
        } else {
            cout << "Customer not found." << endl;
        }
    }
};

// Main function
int main() {
    BankingService bank;

    // Adding Customers
    bank.addCustomer("Alice", 1001);
    bank.addCustomer("Bob", 1002);

    // Creating Accounts for Customers
    Customer* alice = bank.getCustomer(1001);
    Customer* bob = bank.getCustomer(1002);

    if (alice) alice->addAccount(2001);
    if (bob) bob->addAccount(2002);

    // Depositing and Withdrawing Funds
    bank.performDeposit(1001, 2001, 500.0);
    bank.performWithdrawal(1001, 2001, 100.0);

    // Viewing Customer Account Details
    if (alice) alice->viewAccounts();

    // Transferring Funds
    bank.performTransfer(1001, 2001, 2002, 200.0);

    // Viewing Customer Account Details
    if (alice) alice->viewAccounts();
    if (bob) bob->viewAccounts();

    return 0;
}

