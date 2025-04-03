#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <map> // (Không sử dụng hiện tại, có thể dùng trong tương lai để nhóm chi tiêu)

// Cấu trúc để lưu trữ một khoản chi tiêu (không đổi)
struct Expense {
    std::string date;        // Ngày tháng (YYYY-MM-DD)
    std::string description;
    double amount;
    std::string category;

    Expense(std::string d = "", std::string desc = "", double amt = 0.0, std::string cat = "")
        : date(d), description(desc), amount(amt), category(cat) {}
};

// --- Các hàm cũ (giữ nguyên) ---
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void addExpense(std::vector<Expense>& expenses) {
    std::string date, description, category;
    double amount;

    std::cout << "\n--- Them chi tieu moi ---\n";

    // --- Nhập liệu (có thể thêm kiểm tra định dạng ngày YYYY-MM-DD) ---
    std::cout << "Ngay (YYYY-MM-DD): ";
    std::cin >> date;
    // Thêm kiểm tra định dạng đơn giản
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        std::cout << "Canh bao: Dinh dang ngay co ve khong dung (mong doi YYYY-MM-DD).\n";
        // Có thể bắt buộc nhập lại hoặc chỉ cảnh báo
    }
    clearInputBuffer();

    std::cout << "Mo ta: ";
    std::getline(std::cin, description);

    while (true) {
        std::cout << "So tien: ";
        if (std::cin >> amount && amount >= 0) {
             clearInputBuffer();
             break;
        } else {
            std::cout << "Loi: Vui long nham so khong am.\n";
            std::cin.clear();
            clearInputBuffer();
        }
    }

    std::cout << "Danh muc: ";
    std::getline(std::cin, category);

    expenses.emplace_back(date, description, amount, category);
    std::cout << "=> Da them chi tieu thanh cong!\n";
}

void viewExpenses(const std::vector<Expense>& expenses) {
    std::cout << "\n--- Danh sach chi tieu ---\n";
    if (expenses.empty()) {
        std::cout << "Chua co khoan chi tieu nao\n";
        return;
    }

    std::cout << std::left << std::setw(12) << "Ngay"
              << std::setw(30) << "Mo ta"
              << std::setw(15) << "So tien"
              << std::setw(20) << "Danh muc" << std::endl;
    std::cout << std::string(77, '-') << std::endl;

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& expense : expenses) {
        std::cout << std::left << std::setw(12) << expense.date
                  << std::setw(30) << expense.description
                  << std::setw(15) << expense.amount
                  << std::setw(20) << expense.category << std::endl;
    }
     std::cout << std::string(77, '-') << std::endl;
}

void calculateTotal(const std::vector<Expense>& expenses) {
    double total = 0.0;
    for (const auto& expense : expenses) {
        total += expense.amount;
    }
    std::cout << "\n--- Tong chi tieu toan bo ---\n";
    std::cout << "Tong so tien da chi: " << std::fixed << std::setprecision(2) << total << std::endl;
}

bool saveExpensesToFile(const std::vector<Expense>& expenses, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Loi khong the mo tep '" << filename << "' de ghi.\n";
        return false;
    }
    outFile << std::fixed << std::setprecision(2);
    for (const auto& expense : expenses) {
        // Thay thế dấu phẩy trong mô tả và danh mục để tránh lỗi CSV đơn giản
        // (Cách tốt hơn là dùng thư viện CSV hoặc lưu trữ chuẩn hơn)
        std::string safe_desc = expense.description;
        std::string safe_cat = expense.category;
        size_t pos;
        while ((pos = safe_desc.find(',')) != std::string::npos) safe_desc.replace(pos, 1, ";"); // Thay ',' bằng ';'
        while ((pos = safe_cat.find(',')) != std::string::npos) safe_cat.replace(pos, 1, ";");

        outFile << expense.date << ","
                << "\"" << safe_desc << "\"," // Vẫn giữ dấu "" cho chắc
                << expense.amount << ","
                << "\"" << safe_cat << "\"\n"; // Thêm "" cho category
    }
    outFile.close();
    std::cout << "=> Da luu du lieu vao tep '" << filename << "'.\n";
    return true;
}

bool loadExpensesFromFile(std::vector<Expense>& expenses, const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string field;
        Expense expense;
        int fieldCount = 0;
        std::vector<std::string> parts; // Lưu các phần tách được

        // Tách bằng dấu phẩy, xử lý trường hợp có dấu ngoặc kép
        std::string segment;
        while(std::getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() >= 4) { // Cần ít nhất 4 phần
             try {
                expense.date = parts[0];

                // Xử lý description có thể nằm trong ngoặc kép và chứa dấu phẩy (đã thay bằng ;)
                expense.description = parts[1];
                if (expense.description.front() == '"' && expense.description.back() == '"') {
                     expense.description = expense.description.substr(1, expense.description.length() - 2);
                }

                expense.amount = std::stod(parts[2]);

                // Xử lý category có thể nằm trong ngoặc kép
                expense.category = parts[3];
                 if (expense.category.front() == '"' && expense.category.back() == '"') {
                     expense.category = expense.category.substr(1, expense.category.length() - 2);
                 }

                 // Khôi phục dấu phẩy nếu cần (từ dấu ;)
                 size_t pos;
                 while ((pos = expense.description.find(';')) != std::string::npos) expense.description.replace(pos, 1, ",");
                 while ((pos = expense.category.find(';')) != std::string::npos) expense.category.replace(pos, 1, ",");


                expenses.push_back(expense);
             } catch (const std::invalid_argument& e) {
                 std::cerr << "Canh bao: Loi doc so tien o dong: " << line << ". Loi: " << e.what() << std::endl;
             } catch (const std::out_of_range& e) {
                 std::cerr << "Canh bao: Loi doc so tien (ngoai khoang) o dong: " << line << ". Loi: " << e.what() << std::endl;
             } catch (...) {
                 std::cerr << "Canh bao: Loi khong xac dinh duoc dong " << line << std::endl;
             }
        } else {
             std::cerr << "Canh bao: Bo qua dong khong dung dinh dang cua CSV: " << line << std::endl;
        }
    }
    inFile.close();
    std::cout << "=> Da tai du lieu tu tep '" << filename << "'.\n";
    return true;
}

// --- Hàm MỚI ---

// Hàm trợ giúp lấy và kiểm tra năm, tháng
bool getYearMonth(int& year, int& month) {
    std::cout << "Nhap nam (YYYY): ";
    while (!(std::cin >> year) || year < 1900 || year > 2100) { // Kiểm tra cơ bản
        std::cout << "Nam khong hop le. Vui long nhap lai (YYYY): ";
        std::cin.clear();
        clearInputBuffer();
    }
    clearInputBuffer();

    std::cout << "Nhap thang (1-12): ";
    while (!(std::cin >> month) || month < 1 || month > 12) {
        std::cout << "Thang khong hop le. Vui long nhap lai(1-12): ";
        std::cin.clear();
        clearInputBuffer();
    }
    clearInputBuffer(); // Xóa bộ đệm lần cuối
    return true;
}

// Hàm tính tổng chi tiêu cho một tháng cụ thể
double calculateMonthlySpending(const std::vector<Expense>& expenses, int year, int month) {
    double monthlyTotal = 0.0;
    std::string monthStr = (month < 10) ? "0" + std::to_string(month) : std::to_string(month); // Định dạng MM
    std::string yearStr = std::to_string(year);

    for (const auto& expense : expenses) {
        // Kiểm tra xem ngày có đủ dài và đúng định dạng cơ bản không
        if (expense.date.length() >= 7 && expense.date[4] == '-') {
            std::string expenseYear = expense.date.substr(0, 4);
            std::string expenseMonth = expense.date.substr(5, 2);

            if (expenseYear == yearStr && expenseMonth == monthStr) {
                monthlyTotal += expense.amount;
            }
        }
    }
    return monthlyTotal;
}

// Hàm xem chi tiết chi tiêu của một tháng
void viewMonthlyExpenses(const std::vector<Expense>& expenses) {
     std::cout << "\n--- Xem chi tieu thang ---\n";
    int year, month;
    if (!getYearMonth(year, month)) return; // Lấy năm tháng từ người dùng

    std::string monthStr = (month < 10) ? "0" + std::to_string(month) : std::to_string(month);
    std::string yearStr = std::to_string(year);
    double monthlyTotal = 0.0;
    bool found = false;

    // In tiêu đề
    std::cout << "\n--- Chi tieu thang " << monthStr << "/" << yearStr << " ---\n";
    std::cout << std::left << std::setw(12) << "Ngày"
              << std::setw(30) << "Mô tả"
              << std::setw(15) << "Số tiền"
              << std::setw(20) << "Danh mục" << std::endl;
    std::cout << std::string(77, '-') << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    // Lọc và in
    for (const auto& expense : expenses) {
         if (expense.date.length() >= 7 && expense.date[4] == '-') {
            std::string expenseYear = expense.date.substr(0, 4);
            std::string expenseMonth = expense.date.substr(5, 2);

            if (expenseYear == yearStr && expenseMonth == monthStr) {
                 std::cout << std::left << std::setw(12) << expense.date
                           << std::setw(30) << expense.description
                           << std::setw(15) << expense.amount
                           << std::setw(20) << expense.category << std::endl;
                 monthlyTotal += expense.amount;
                 found = true;
            }
        }
    }

    if (!found) {
        std::cout << "Khong co chi tieu nao duoc ghi nhan trong thang nay!\n";
    } else {
         std::cout << std::string(77, '-') << std::endl;
         std::cout << std::left << std::setw(42) << "Tong chi tieu thang nay:" << std::setw(15) << monthlyTotal << std::endl;
         std::cout << std::string(77, '-') << std::endl;
    }
}


// Hàm tính và hiển thị số dư tháng
void calculateMonthlyBalance(const std::vector<Expense>& expenses) {
    std::cout << "\n--- Tinh So Du Thang ---\n";
    int year, month;
    if (!getYearMonth(year, month)) return; // Lấy năm tháng từ người dùng

    double monthlyIncome;
    std::cout << "Nhap tong thu nhap cua thang " << month << "/" << year << ": ";
    while (!(std::cin >> monthlyIncome) || monthlyIncome < 0) {
        std::cout << "Thu nhap khong hop le. Vui long nhap so khong am: ";
        std::cin.clear();
        clearInputBuffer();
    }
    clearInputBuffer(); // Xóa bộ đệm sau khi nhập thu nhập

    // Tính chi tiêu tháng
    double spending = calculateMonthlySpending(expenses, year, month);

    // Tính số dư
    double balance = monthlyIncome - spending;

    // Hiển thị kết quả
    std::cout << "\n--- So du thang " << month << "/" << year << " ---\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(20) << "Tổng thu nhập:" << monthlyIncome << std::endl;
    std::cout << std::left << std::setw(20) << "Tổng chi tiêu:" << spending << std::endl;
    std::cout << std::string(35, '-') << std::endl;
    std::cout << std::left << std::setw(20) << "Số dư:" << balance << std::endl;
    std::cout << "-----------------------------------\n";
}

// Hàm hiển thị menu cập nhật
void displayMenu() {
    std::cout << "\n--- QUAN LY CHI TIEU CA NHAN ---\n";
    std::cout << "1. Them chi tieu moi\n";
    std::cout << "2. Xem tat ca chi tieu\n";
    std::cout << "3. Xem chi tieu theo thang\n"; // MỚI
    std::cout << "4. Tinh tong chi tieu toan bo\n"; // Đổi tên
    std::cout << "5. Tinh so du theo thang\n";      // MỚI
    std::cout << "6. Luu va thoat\n";          // Số thứ tự thay đổi
    std::cout << "---------------------------------\n";
    std::cout << "Nhap lua chon cua ban: ";
}

int main() {
    std::vector<Expense> expenses;
    const std::string dataFile = "output.csv";

    loadExpensesFromFile(expenses, dataFile);

    int choice;
    do {
        displayMenu();
        while (!(std::cin >> choice)) {
             std::cout << "Lua chon khong hop le. Vui long nhap lai mot so: ";
             std::cin.clear();
             clearInputBuffer();
        }
         clearInputBuffer(); // Quan trọng!

        switch (choice) {
            case 1:
                addExpense(expenses);
                break;
            case 2:
                viewExpenses(expenses);
                break;
            case 3: // MỚI
                viewMonthlyExpenses(expenses);
                break;
            case 4: // Trước là 3
                calculateTotal(expenses);
                break;
            case 5: // MỚI
                calculateMonthlyBalance(expenses);
                break;
            case 6: // Trước là 4
                saveExpensesToFile(expenses, dataFile);
                std::cout << "Đang thoát chương trình...\n";
                break;
            default:
                std::cout << "Lua chon khong hop le. Vui long chon lai\n";
        }
    } while (choice != 6); // Cập nhật điều kiện thoát

    return 0;
}