#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

// Constants
#define MAX_STRING_LENGTH 256
#define DB_FILE "restaurant_pos.db"
#define VERSION "1.0.0"

// Structs for data representation
typedef struct
{
    int id;
    char username[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];
    char role[MAX_STRING_LENGTH];
} Staff;

typedef struct
{
    int id;
    char name[MAX_STRING_LENGTH];
    char description[MAX_STRING_LENGTH];
    double price;
    int is_available;
    int category_id;
} MenuItem;

typedef struct
{
    int id;
    int table_id;
    int staff_id;
    char status[MAX_STRING_LENGTH];
    char created_at[MAX_STRING_LENGTH];
    double total_amount;
} Order;

typedef struct
{
    int id;
    int order_id;
    int item_id;
    char item_name[MAX_STRING_LENGTH];
    int quantity;
    double item_price;
    char status[MAX_STRING_LENGTH];
} OrderItem;

// Global variables
sqlite3 *db;
Staff current_user = {0};

// Function prototypes

// Database functions
int initialize_database();
int execute_query(const char *query, int (*callback)(void *, int, char **, char **), void *data);
int check_db_exists();

// Authentication functions
int login(char *username, char *password);
int logout();

// Menu management functions
int load_menu_items(MenuItem **items, int category_id);
int add_menu_item(MenuItem item);
int update_menu_item(MenuItem item);
int delete_menu_item(int item_id);

// Order management functions
int create_new_order(int table_id);
int add_item_to_order(int order_id, int item_id, int quantity, char *notes);
int update_order_item_status(int order_item_id, const char *status);
int complete_order(int order_id, const char *payment_method);
int cancel_order(int order_id);

// Table management functions
int load_tables();
int update_table_status(int table_id, const char *status);

// Staff management functions
int load_staff_members(Staff **staff);
int add_staff_member(Staff staff, char *password);
int update_staff_member(Staff staff);
int delete_staff_member(int staff_id);

// Reporting functions
int generate_daily_sales_report(const char *date);
int generate_item_popularity_report(const char *start_date, const char *end_date);
int generate_staff_performance_report(const char *start_date, const char *end_date);
int generate_inventory_status_report();

// Receipt printing functions
int print_receipt(int order_id);
void format_receipt(int order_id, char *receipt_text);

// Utility functions
char *get_current_datetime();
void clear_screen();
void pause_screen();

// UI Menu functions
void show_main_menu();
void show_order_menu();
void show_admin_menu();
void show_reports_menu();

// Menu callback function for SQLite
static int menu_callback(void *data, int argc, char **argv, char **azColName)
{
    MenuItem **items = (MenuItem **)data;
    static int row = 0;

    if (*items == NULL)
    {
        // First row - allocate memory based on count in first column
        int count = atoi(argv[0]);
        *items = (MenuItem *)malloc(count * sizeof(MenuItem));
        if (*items == NULL)
        {
            fprintf(stderr, "Memory allocation error\n");
            return 1;
        }
        row = 0;
    }
    else
    {
        // Subsequent rows - fill data
        MenuItem *item = &(*items)[row];
        item->id = atoi(argv[0]);
        item->category_id = atoi(argv[1]);
        strncpy(item->name, argv[2], MAX_STRING_LENGTH - 1);
        strncpy(item->description, argv[3] ? argv[3] : "", MAX_STRING_LENGTH - 1);
        item->price = atof(argv[4]);
        item->is_available = atoi(argv[5]);
        row++;
    }
    return 0;
}

int main()
{
    int choice;
    char username[MAX_STRING_LENGTH];
    char password[MAX_STRING_LENGTH];

    // Initialize the database
    if (initialize_database() != SQLITE_OK)
    {
        fprintf(stderr, "Failed to initialize database.\n");
        return 1;
    }

    // Login screen
    clear_screen();
    printf("=============================================\n");
    printf("      RESTAURANT POS SYSTEM v%s\n", VERSION);
    printf("=============================================\n\n");

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    if (login(username, password) != SQLITE_OK)
    {
        printf("Login failed. Invalid credentials.\n");
        return 1;
    }

    printf("Login successful. Welcome, %s!\n", current_user.name);
    pause_screen();

    // Main program loop
    do
    {
        clear_screen();
        show_main_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // New Order
            show_order_menu();
            break;
        case 2:
            // Manage Orders
            // Code for managing orders
            break;
        case 3:
            // Reports
            show_reports_menu();
            break;
        case 4:
            // Admin Functions (if user is admin)
            if (strcmp(current_user.role, "Manager") == 0)
            {
                show_admin_menu();
            }
            else
            {
                printf("Access denied. Manager privileges required.\n");
                pause_screen();
            }
            break;
        case 0:
            // Exit
            printf("Logging out...\n");
            logout();
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            pause_screen();
            break;
        }
    } while (choice != 0);

    // Close database connection
    sqlite3_close(db);
    printf("Thank you for using the Restaurant POS System.\n");

    return 0;
}

// Database initialization function
int initialize_database()
{
    int rc;

    // Check if database file exists
    if (check_db_exists())
    {
        // Open existing database
        rc = sqlite3_open(DB_FILE, &db);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return rc;
        }
        return SQLITE_OK;
    }

    // Create new database
    rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }

    // Load and execute schema.sql
    FILE *schema_file = fopen("schema.sql", "r");
    if (schema_file == NULL)
    {
        fprintf(stderr, "Cannot open schema.sql file\n");
        return SQLITE_ERROR;
    }

    // Read schema file content
    fseek(schema_file, 0, SEEK_END);
    long schema_size = ftell(schema_file);
    fseek(schema_file, 0, SEEK_SET);

    char *schema_content = (char *)malloc(schema_size + 1);
    if (schema_content == NULL)
    {
        fclose(schema_file);
        return SQLITE_ERROR;
    }

    fread(schema_content, 1, schema_size, schema_file);
    schema_content[schema_size] = '\0';
    fclose(schema_file);

    // Execute schema SQL
    char *err_msg = NULL;
    rc = sqlite3_exec(db, schema_content, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        free(schema_content);
        return rc;
    }

    free(schema_content);
    printf("Database initialized successfully.\n");
    return SQLITE_OK;
}

// Execute SQL query with callback
int execute_query(const char *query, int (*callback)(void *, int, char **, char **), void *data)
{
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, query, callback, data, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

// Check if database file exists
int check_db_exists()
{
    FILE *file = fopen(DB_FILE, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

// Login function
int login(char *username, char *password)
{
    char query[512];
    sprintf(query, "SELECT staff_id, username, name, role FROM staff WHERE username = '%s' AND password = '%s' AND is_active = 1",
            username, password);

    int found = 0;
    int callback(void *data, int argc, char **argv, char **azColName)
    {
        found = 1;
        current_user.id = atoi(argv[0]);
        strncpy(current_user.username, argv[1], MAX_STRING_LENGTH - 1);
        strncpy(current_user.name, argv[2], MAX_STRING_LENGTH - 1);
        strncpy(current_user.role, argv[3], MAX_STRING_LENGTH - 1);
        return 0;
    }

    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    return found ? SQLITE_OK : SQLITE_ERROR;
}

// Logout function
int logout()
{
    // Clear current user data
    memset(&current_user, 0, sizeof(Staff));
    return SQLITE_OK;
}

// Get current datetime string
char *get_current_datetime()
{
    static char datetime[MAX_STRING_LENGTH];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(datetime, MAX_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", t);
    return datetime;
}

// Clear screen function (platform dependent)
void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Pause screen function
void pause_screen()
{
    printf("\nPress Enter to continue...");
    getchar(); // Consume any previous newline
    getchar(); // Wait for Enter
}

// Show main menu
void show_main_menu()
{
    printf("=============================================\n");
    printf("      RESTAURANT POS SYSTEM v%s\n", VERSION);
    printf("      User: %s (%s)\n", current_user.name, current_user.role);
    printf("      %s\n", get_current_datetime());
    printf("=============================================\n\n");

    printf("1. New Order\n");
    printf("2. Manage Orders\n");
    printf("3. Reports\n");

    if (strcmp(current_user.role, "Manager") == 0)
    {
        printf("4. Admin Functions\n");
    }

    printf("0. Exit\n\n");
}

// Show order menu
void show_order_menu()
{
    int table_id, item_id, quantity, order_id;
    char notes[MAX_STRING_LENGTH];
    char payment_method[MAX_STRING_LENGTH];

    clear_screen();
    printf("=============================================\n");
    printf("             NEW ORDER\n");
    printf("=============================================\n\n");

    // Load and display tables
    if (load_tables() < 0) {
        printf("No tables available. Please add tables first.\n");
        pause_screen();
        return;
    }

    printf("\nSelect Table ID: ");
    scanf("%d", &table_id);

    // Create new order
    order_id = create_new_order(table_id);
    if (order_id <= 0)
    {
        printf("Failed to create order.\n");
        pause_screen();
        return;
    }

    // Load and display menu items
    MenuItem *items = NULL;
    int item_count = load_menu_items(&items, 0); // 0 = all categories

    if (item_count <= 0 || items == NULL)
    {
        printf("No menu items available.\n");
        pause_screen();
        return;
    }

    // Add items to order
    do
    {
        clear_screen();
        printf("Order #%d - Table #%d\n\n", order_id, table_id);

        printf("Menu Items:\n");
        printf("------------------------------------------\n");
        printf("%-5s %-30s %-10s\n", "ID", "Item", "Price");
        printf("------------------------------------------\n");
        for (int i = 0; i < item_count; i++)
        {
            printf("%-5d %-30s $%-9.2f\n", items[i].id, items[i].name, items[i].price);
        }

        printf("\nEnter Item ID (0 to finish): ");
        scanf("%d", &item_id);

        if (item_id > 0)
        {
            printf("Enter Quantity: ");
            scanf("%d", &quantity);

            // Consume newline
            getchar();

            printf("Special Notes: ");
            fgets(notes, MAX_STRING_LENGTH, stdin);
            notes[strcspn(notes, "\n")] = 0; // Remove newline

            if (add_item_to_order(order_id, item_id, quantity, notes) == 0) {
                printf("Item added to order.\n");
            } else {
                printf("Failed to add item to order.\n");
            }
            pause_screen();
        }
    } while (item_id != 0);

    free(items);

    // Show order summary and checkout
    clear_screen();
    printf("=============================================\n");
    printf("             ORDER SUMMARY\n");
    printf("=============================================\n\n");
    
    // Display order items
    char query[512];
    sprintf(query, 
            "SELECT oi.item_name, oi.quantity, oi.item_price, "
            "(oi.quantity * oi.item_price) as subtotal "
            "FROM order_items oi "
            "WHERE oi.order_id = %d", 
            order_id);
    
    printf("%-30s %-8s %-10s %-10s\n", "Item", "Quantity", "Price", "Subtotal");
    printf("------------------------------------------------------------------\n");
    
    double total = 0.0;
    int found = 0;
    
    int callback(void *data, int argc, char **argv, char **azColName) {
        found = 1;
        printf("%-30s %-8s $%-9.2f $%-9.2f\n", 
               argv[0], argv[1], atof(argv[2]), atof(argv[3]));
        total += atof(argv[3]);
        return 0;
    };
    
    int rc = execute_query(query, callback, NULL);
    
    printf("------------------------------------------------------------------\n");
    printf("%-50s $%-9.2f\n", "Total:", total);
    
    if (found) {
        printf("\nComplete order? (yes/no): ");
        char response[10];
        scanf("%s", response);
        
        if (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0) {
            printf("Enter payment method (cash/card/other): ");
            scanf("%s", payment_method);
            
            if (complete_order(order_id, payment_method) == 0) {
                printf("Order completed successfully!\n");
            } else {
                printf("Failed to complete order.\n");
            }
        } else {
            printf("Order saved as pending.\n");
        }
    } else {
        printf("No items in order. Order will be cancelled.\n");
        cancel_order(order_id);
    }

    pause_screen();
}

// Show admin menu
void show_admin_menu()
{
    int choice;

    do
    {
        clear_screen();
        printf("=============================================\n");
        printf("           ADMIN FUNCTIONS\n");
        printf("=============================================\n\n");

        printf("1. Manage Menu Items\n");
        printf("2. Manage Staff\n");
        printf("3. Manage Tables\n");
        printf("4. Manage Inventory\n");
        printf("0. Back to Main Menu\n\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // Manage Menu Items
            break;
        case 2:
            // Manage Staff
            break;
        case 3:
            // Manage Tables
            break;
        case 4:
            // Manage Inventory
            break;
        case 0:
            // Back to main menu
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            pause_screen();
            break;
        }
    } while (choice != 0);
}

// Show reports menu
void show_reports_menu()
{
    int choice;
    char start_date[MAX_STRING_LENGTH];
    char end_date[MAX_STRING_LENGTH];

    do
    {
        clear_screen();
        printf("=============================================\n");
        printf("              REPORTS\n");
        printf("=============================================\n\n");

        printf("1. Daily Sales Report\n");
        printf("2. Menu Item Popularity Report\n");
        printf("3. Staff Performance Report\n");
        printf("4. Inventory Status Report\n");
        printf("0. Back to Main Menu\n\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            // Daily Sales Report
            printf("Enter date (YYYY-MM-DD) or today: ");
            char date[MAX_STRING_LENGTH];
            scanf("%s", date);

            if (strcmp(date, "today") == 0)
            {
                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                strftime(date, MAX_STRING_LENGTH, "%Y-%m-%d", t);
            }

            generate_daily_sales_report(date);
            break;
        }
        case 2:
            // Menu Item Popularity Report
            printf("Enter start date (YYYY-MM-DD): ");
            scanf("%s", start_date);
            printf("Enter end date (YYYY-MM-DD): ");
            scanf("%s", end_date);

            generate_item_popularity_report(start_date, end_date);
            break;
        case 3:
            // Staff Performance Report
            printf("Enter start date (YYYY-MM-DD): ");
            scanf("%s", start_date);
            printf("Enter end date (YYYY-MM-DD): ");
            scanf("%s", end_date);

            generate_staff_performance_report(start_date, end_date);
            break;
        case 4:
            // Inventory Status Report
            generate_inventory_status_report();
            break;
        case 0:
            // Back to main menu
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            pause_screen();
            break;
        }

        if (choice > 0 && choice <= 4)
        {
            pause_screen();
        }
    } while (choice != 0);
}

// Implementation of the daily sales report function
int generate_daily_sales_report(const char *date)
{
    char query[1024];
    printf("\n=== DAILY SALES REPORT: %s ===\n\n", date);

    // Format query to get daily sales
    sprintf(query,
            "SELECT "
            "  COUNT(DISTINCT o.order_id) as order_count, "
            "  SUM(o.total_amount) as total_sales, "
            "  AVG(o.total_amount) as average_order, "
            "  COUNT(DISTINCT o.table_id) as tables_served "
            "FROM orders o "
            "WHERE DATE(o.created_at) = '%s' AND o.order_status = 'completed'",
            date);

    int found = 0;
    int callback(void *data, int argc, char **argv, char **azColName)
    {
        found = 1;
        printf("Total Orders: %s\n", argv[0]);
        printf("Total Sales: $%.2f\n", argv[1] ? atof(argv[1]) : 0.0);
        printf("Average Order: $%.2f\n", argv[2] ? atof(argv[2]) : 0.0);
        printf("Tables Served: %s\n\n", argv[3]);
        return 0;
    }

    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    if (!found)
    {
        printf("No sales data found for this date.\n");
        return SQLITE_OK;
    }

    // Get top selling items
    sprintf(query,
            "SELECT "
            "  mi.name, "
            "  SUM(oi.quantity) as quantity, "
            "  SUM(oi.quantity * oi.item_price) as total "
            "FROM order_items oi "
            "JOIN orders o ON oi.order_id = o.order_id "
            "JOIN menu_items mi ON oi.item_id = mi.item_id "
            "WHERE DATE(o.created_at) = '%s' AND o.order_status = 'completed' "
            "GROUP BY oi.item_id "
            "ORDER BY quantity DESC "
            "LIMIT 5",
            date);

    printf("Top Selling Items:\n");
    printf("--------------------------------\n");
    printf("%-20s | %-8s | %-10s\n", "Item", "Quantity", "Total");
    printf("--------------------------------\n");

    int found_items = 0;
    int item_callback(void *data, int argc, char **argv, char **azColName)
    {
        found_items = 1;
        printf("%-20s | %-8s | $%-9.2f\n",
               argv[0],
               argv[1],
               argv[2] ? atof(argv[2]) : 0.0);
        return 0;
    }

    rc = execute_query(query, item_callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    if (!found_items)
    {
        printf("No item sales data found.\n");
    }

    // Get sales by payment method
    sprintf(query,
            "SELECT "
            "  payment_method, "
            "  COUNT(*) as count, "
            "  SUM(total_amount) as total "
            "FROM orders "
            "WHERE DATE(created_at) = '%s' AND order_status = 'completed' "
            "GROUP BY payment_method",
            date);

    printf("\nSales by Payment Method:\n");
    printf("--------------------------------\n");
    printf("%-20s | %-8s | %-10s\n", "Method", "Count", "Total");
    printf("--------------------------------\n");

    int found_payments = 0;
    int payment_callback(void *data, int argc, char **argv, char **azColName)
    {
        found_payments = 1;
        printf("%-20s | %-8s | $%-9.2f\n",
               argv[0],
               argv[1],
               argv[2] ? atof(argv[2]) : 0.0);
        return 0;
    }

    rc = execute_query(query, payment_callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    if (!found_payments)
    {
        printf("No payment data found.\n");
    }

    return SQLITE_OK;
}

// Print receipt function
int print_receipt(int order_id)
{
    char receipt_text[4096] = {0}; // Buffer for receipt text

    // Format the receipt
    format_receipt(order_id, receipt_text);

    // Print to console (in a real system, you would send to a printer)
    printf("%s", receipt_text);

    // Save receipt to file
    char filename[100];
    sprintf(filename, "receipt_%d.txt", order_id);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error saving receipt to file.\n");
        return -1;
    }

    fprintf(file, "%s", receipt_text);
    fclose(file);

    printf("Receipt saved to %s\n", filename);
    return 0;
}

// Format receipt
void format_receipt(int order_id, char *receipt_text)
{
    char query[1024];
    char buffer[4096] = {0};

    // Get order details
    sprintf(query,
            "SELECT o.order_id, o.created_at, s.name as server_name, t.table_number, "
            "o.total_amount, o.payment_method "
            "FROM orders o "
            "JOIN staff s ON o.staff_id = s.staff_id "
            "JOIN restaurant_tables t ON o.table_id = t.table_id "
            "WHERE o.order_id = %d",
            order_id);

    int found = 0;
    char order_time[MAX_STRING_LENGTH] = {0};
    char server_name[MAX_STRING_LENGTH] = {0};
    char table_number[MAX_STRING_LENGTH] = {0};
    double total_amount = 0.0;
    char payment_method[MAX_STRING_LENGTH] = {0};

    int callback(void *data, int argc, char **argv, char **azColName)
    {
        found = 1;
        strncpy(order_time, argv[1], MAX_STRING_LENGTH - 1);
        strncpy(server_name, argv[2], MAX_STRING_LENGTH - 1);
        strncpy(table_number, argv[3], MAX_STRING_LENGTH - 1);
        total_amount = atof(argv[4]);
        strncpy(payment_method, argv[5] ? argv[5] : "N/A", MAX_STRING_LENGTH - 1);
        return 0;
    }
    
    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK || !found)
    {
        strcpy(receipt_text, "Error: Order not found.\n");
        return;
    }

    // Format header and order details
    sprintf(buffer,
            "============================================\n"
            "               RESTAURANT POS              \n"
            "============================================\n\n"
            "Receipt #: %d\n"
            "Date/Time: %s\n"
            "Server: %s\n"
            "Table: %s\n\n"
            "Items:\n"
            "--------------------------------------------\n"
            "%-30s %5s %10s\n"
            "--------------------------------------------\n",
            order_id,
            order_time,
            server_name,
            table_number,
            "Item", "Qty", "Amount");

    // Copy the formatted header to receipt_text
    strcpy(receipt_text, buffer);
    
    // Get order items
    sprintf(query,
            "SELECT item_name, quantity, item_price, quantity * item_price as subtotal "
            "FROM order_items "
            "WHERE order_id = %d",
            order_id);
    
    double subtotal = 0.0;
    
    int items_callback(void *data, int argc, char **argv, char **azColName)
    {
        char item_line[256];
        sprintf(item_line, "%-30s %5s $%9.2f\n", 
                argv[0], 
                argv[1], 
                atof(argv[3]));
                
        // Append to receipt_text
        strcat(receipt_text, item_line);
        return 0;
    };
    
    rc = execute_query(query, items_callback, NULL);
    
    // Add footer with totals
    char footer[1024];
    sprintf(footer,
            "--------------------------------------------\n"
            "%-36s $%9.2f\n"
            "%-36s $%9.2f\n"
            "%-36s $%9.2f\n\n"
            "Payment Method: %s\n\n"
            "============================================\n"
            "          Thank You For Your Visit!         \n"
            "============================================\n",
            "Subtotal:", total_amount,
            "Tax (0%):", 0.0,
            "Total:", total_amount,
            payment_method);
            
    // Append footer to receipt_text
    strcat(receipt_text, footer);
}

// Menu management functions
int load_menu_items(MenuItem **items, int category_id)
{
    char query[512];
    
    // First, count how many items we'll have
    if (category_id > 0) {
        sprintf(query, "SELECT COUNT(*) FROM menu_items WHERE category_id = %d AND is_available = 1", category_id);
    } else {
        sprintf(query, "SELECT COUNT(*) FROM menu_items WHERE is_available = 1");
    }
    
    // Execute count query
    int rc = execute_query(query, menu_callback, items);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Now get the actual items
    if (category_id > 0) {
        sprintf(query, "SELECT item_id, category_id, name, description, price, is_available "
                "FROM menu_items WHERE category_id = %d AND is_available = 1", category_id);
    } else {
        sprintf(query, "SELECT item_id, category_id, name, description, price, is_available "
                "FROM menu_items WHERE is_available = 1");
    }
    
    // Execute items query
    rc = execute_query(query, menu_callback, items);
    if (rc != SQLITE_OK) {
        // Free allocated memory if there was an error
        if (*items != NULL) {
            free(*items);
            *items = NULL;
        }
        return -1;
    }
    
    // Get the count that was returned in the first query
    int item_count = 0;
    if (*items != NULL) {
        // Count the number of items by checking their IDs
        int i = 0;
        while ((*items)[i].id > 0) {
            item_count++;
            i++;
        }
    }
    
    return item_count;
}

// Order management functions
int create_new_order(int table_id)
{
    char query[512];
    char *datetime = get_current_datetime();
    
    // Create a new order with pending status
    sprintf(query, "INSERT INTO orders (table_id, staff_id, order_status, created_at, total_amount) "
            "VALUES (%d, %d, 'pending', '%s', 0.0)",
            table_id, current_user.id, datetime, current_user.id);
    
    // Execute query
    int rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Get the last inserted order ID
    sprintf(query, "SELECT last_insert_rowid()");
    
    int order_id = -1;
    int callback(void *data, int argc, char **argv, char **azColName) {
        order_id = atoi(argv[0]);
        return 0;
    }
    
    rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Update table status to occupied
    sprintf(query, "UPDATE restaurant_tables SET table_status = 'occupied' WHERE table_id = %d", table_id);
    rc = execute_query(query, NULL, NULL);
    
    return order_id;
}

int add_item_to_order(int order_id, int item_id, int quantity, char *notes)
{
    char query[512];
    
    // Get item details
    sprintf(query, "SELECT name, price FROM menu_items WHERE item_id = %d", item_id);
    
    char item_name[MAX_STRING_LENGTH];
    double item_price = 0.0;
    int found = 0;
    
    int callback(void *data, int argc, char **argv, char **azColName) {
        found = 1;
        strncpy(item_name, argv[0], MAX_STRING_LENGTH - 1);
        item_price = atof(argv[1]);
        return 0;
    }
    
    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK || !found) {
        return -1;
    }
    
    // Add item to order_items table
    sprintf(query, "INSERT INTO order_items (order_id, item_id, item_name, quantity, item_price, status, notes) "
            "VALUES (%d, %d, '%s', %d, %.2f, 'pending', '%s')",
            order_id, item_id, item_name, quantity, item_price, notes ? notes : "");
    
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Update order total amount
    sprintf(query, "UPDATE orders SET total_amount = total_amount + (%.2f * %d) WHERE order_id = %d",
            item_price, quantity, order_id);
    
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    return 0;
}

int complete_order(int order_id, const char *payment_method)
{
    char query[512];
    
    // First check if the order exists and is pending
    sprintf(query, "SELECT order_id, table_id FROM orders WHERE order_id = %d AND order_status = 'pending'", order_id);
    
    int found = 0;
    int table_id = -1;
    
    int callback(void *data, int argc, char **argv, char **azColName) {
        found = 1;
        table_id = atoi(argv[1]);
        return 0;
    }
    
    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK || !found) {
        return -1;
    }
    
    // Update order status to completed and set payment method
    sprintf(query, "UPDATE orders SET order_status = 'completed', payment_method = '%s' WHERE order_id = %d",
            payment_method, order_id);
    
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Update all pending order items to completed
    sprintf(query, "UPDATE order_items SET status = 'completed' WHERE order_id = %d AND status = 'pending'", order_id);
    
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Free up the table
    if (table_id > 0) {
        sprintf(query, "UPDATE restaurant_tables SET table_status = 'available' WHERE table_id = %d", table_id);
        rc = execute_query(query, NULL, NULL);
        if (rc != SQLITE_OK) {
            return -1;
        }
    }
    
    // Print receipt
    print_receipt(order_id);
    
    return 0;
}

int cancel_order(int order_id)
{
    char query[512];
    
    // First check if the order exists and get the table_id
    sprintf(query, "SELECT table_id FROM orders WHERE order_id = %d", order_id);
    
    int found = 0;
    int table_id = -1;
    
    int callback(void *data, int argc, char **argv, char **azColName) {
        found = 1;
        table_id = atoi(argv[0]);
        return 0;
    }
    
    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK || !found) {
        return -1;
    }
    
    // Delete order items
    sprintf(query, "DELETE FROM order_items WHERE order_id = %d", order_id);
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Delete the order
    sprintf(query, "DELETE FROM orders WHERE order_id = %d", order_id);
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Update table status to available
    if (table_id > 0) {
        sprintf(query, "UPDATE restaurant_tables SET table_status = 'available' WHERE table_id = %d", table_id);
        rc = execute_query(query, NULL, NULL);
        if (rc != SQLITE_OK) {
            return -1;
        }
    }
    
    return 0;
}

// Table management functions
int load_tables()
{
    char query[512];
    sprintf(query, "SELECT table_id, table_number, capacity, table_status FROM restaurant_tables ORDER BY table_number");
    
    printf("\nAvailable Tables:\n");
    printf("------------------------------------------\n");
    printf("%-8s %-15s %-10s %-10s\n", "ID", "Table Number", "Capacity", "Status");
    printf("------------------------------------------\n");
    
    int found = 0;
    
    int callback(void *data, int argc, char **argv, char **azColName)
    {
        found = 1;
        printf("%-8s %-15s %-10s %-10s\n", 
               argv[0], 
               argv[1], 
               argv[2], 
               argv[3]);
        return 0;
    }
    
    int rc = execute_query(query, callback, NULL);
    if (rc != SQLITE_OK)
    {
        return -1;
    }
    
    if (!found)
    {
        printf("No tables found.\n");
    }
    
    return found ? 0 : -1;
}

int update_table_status(int table_id, const char *status)
{
    char query[512];
    sprintf(query, "UPDATE restaurant_tables SET table_status = '%s' WHERE table_id = %d", 
            status, table_id);
    
    return execute_query(query, NULL, NULL);
}

int generate_item_popularity_report(const char *start_date, const char *end_date)
{
    char query[1024];
    printf("\n=== ITEM POPULARITY REPORT: %s to %s ===\n\n", start_date, end_date);

    // Format query to get popular items
    sprintf(query,
            "SELECT "
            "  mi.name as item_name, "
            "  mi.category_id, "
            "  c.name as category_name, "
            "  SUM(oi.quantity) as quantity_sold, "
            "  SUM(oi.quantity * oi.item_price) as total_sales "
            "FROM order_items oi "
            "JOIN orders o ON oi.order_id = o.order_id "
            "JOIN menu_items mi ON oi.item_id = mi.item_id "
            "LEFT JOIN categories c ON mi.category_id = c.category_id "
            "WHERE DATE(o.created_at) BETWEEN '%s' AND '%s' "
            "  AND o.order_status = 'completed' "
            "GROUP BY oi.item_id "
            "ORDER BY quantity_sold DESC",
            start_date, end_date);

    printf("Top Selling Items:\n");
    printf("------------------------------------------------------------\n");
    printf("%-25s %-15s %-10s %-10s\n", "Item", "Category", "Quantity", "Total");
    printf("------------------------------------------------------------\n");

    int found = 0;
    int item_callback(void *data, int argc, char **argv, char **azColName)
    {
        found = 1;
        printf("%-25s %-15s %-10s $%-9.2f\n",
               argv[0],                            // item_name
               argv[2] ? argv[2] : "Uncategorized", // category_name
               argv[3],                            // quantity_sold
               argv[4] ? atof(argv[4]) : 0.0);      // total_sales
        return 0;
    }

    int rc = execute_query(query, item_callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    if (!found)
    {
        printf("No sales data found for this date range.\n");
    }

    // Show sales by category
    sprintf(query,
            "SELECT "
            "  c.name as category_name, "
            "  COUNT(DISTINCT oi.item_id) as item_count, "
            "  SUM(oi.quantity) as quantity_sold, "
            "  SUM(oi.quantity * oi.item_price) as total_sales "
            "FROM order_items oi "
            "JOIN orders o ON oi.order_id = o.order_id "
            "JOIN menu_items mi ON oi.item_id = mi.item_id "
            "LEFT JOIN categories c ON mi.category_id = c.category_id "
            "WHERE DATE(o.created_at) BETWEEN '%s' AND '%s' "
            "  AND o.order_status = 'completed' "
            "GROUP BY mi.category_id "
            "ORDER BY total_sales DESC",
            start_date, end_date);

    printf("\nSales by Category:\n");
    printf("------------------------------------------------------------\n");
    printf("%-20s %-15s %-10s %-10s\n", "Category", "Items", "Quantity", "Total");
    printf("------------------------------------------------------------\n");

    int found_categories = 0;
    int category_callback(void *data, int argc, char **argv, char **azColName)
    {
        found_categories = 1;
        printf("%-20s %-15s %-10s $%-9.2f\n",
               argv[0] ? argv[0] : "Uncategorized",
               argv[1],
               argv[2],
               argv[3] ? atof(argv[3]) : 0.0);
        return 0;
    }

    rc = execute_query(query, category_callback, NULL);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    if (!found_categories)
    {
        printf("No category data found.\n");
    }

    return SQLITE_OK;
}

// Staff management functions
int load_staff_members(Staff **staff)
{
    char query[512];
    
    // First, count how many staff members we'll have
    sprintf(query, "SELECT COUNT(*) FROM staff WHERE is_active = 1");
    
    int staff_count = 0;
    int count_callback(void *data, int argc, char **argv, char **azColName) {
        staff_count = atoi(argv[0]);
        return 0;
    }
    
    int rc = execute_query(query, count_callback, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    // Allocate memory for staff members
    *staff = (Staff *)malloc(staff_count * sizeof(Staff));
    if (*staff == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }
    
    // Now get the actual staff members
    sprintf(query, "SELECT staff_id, username, name, role FROM staff WHERE is_active = 1 ORDER BY name");
    
    int current_index = 0;
    int staff_callback(void *data, int argc, char **argv, char **azColName) {
        if (current_index < staff_count) {
            Staff *s = &(*staff)[current_index];
            s->id = atoi(argv[0]);
            strncpy(s->username, argv[1], MAX_STRING_LENGTH - 1);
            strncpy(s->name, argv[2], MAX_STRING_LENGTH - 1);
            strncpy(s->role, argv[3], MAX_STRING_LENGTH - 1);
            current_index++;
        }
        return 0;
    }
    
    rc = execute_query(query, staff_callback, NULL);
    if (rc != SQLITE_OK) {
        free(*staff);
        *staff = NULL;
        return -1;
    }
    
    return staff_count;
}

int add_staff_member(Staff staff, char *password)
{
    char query[512];
    
    // Check if username already exists
    sprintf(query, "SELECT COUNT(*) FROM staff WHERE username = '%s'", staff.username);
    
    int exists = 0;
    int check_callback(void *data, int argc, char **argv, char **azColName) {
        exists = atoi(argv[0]);
        return 0;
    }
    
    int rc = execute_query(query, check_callback, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    if (exists) {
        fprintf(stderr, "Username already exists\n");
        return -1;
    }
    
    // Insert new staff member
    sprintf(query, 
            "INSERT INTO staff (username, password, name, role, is_active) "
            "VALUES ('%s', '%s', '%s', '%s', 1)",
            staff.username, password, staff.name, staff.role);
    
    rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    return 0;
}

int update_staff_member(Staff staff)
{
    char query[512];
    
    // Update staff member details
    sprintf(query, 
            "UPDATE staff SET name = '%s', role = '%s' WHERE staff_id = %d",
            staff.name, staff.role, staff.id);
    
    int rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    return 0;
}

int delete_staff_member(int staff_id)
{
    char query[512];
    
    // Set staff member as inactive instead of actually deleting
    sprintf(query, "UPDATE staff SET is_active = 0 WHERE staff_id = %d", staff_id);
    
    int rc = execute_query(query, NULL, NULL);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    return 0;
}