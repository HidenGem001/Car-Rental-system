#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include<math.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#define MAX_VEHICLES 50
#define MAX_BOOKINGS 100
#define MAX_USERS 100
#define MAX_EXPENSES 100
#define VEHICLE_FILE "vehicles.txt"
#define BOOKING_FILE "bookings.txt"
#define USER_FILE "users.txt"
#define EXPENSE_FILE "expenses.txt"
#ifdef _WIN32
#define CLEAR_SCREEN system("cls")
#else
#define CLEAR_SCREEN system("clear")
#endif
#ifdef _WIN32
#define SET_YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN)
#define RESET_COLOR SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#else
#define SET_YELLOW printf("\033[1;33m")
#define RESET_COLOR printf("\033[0m")
#endif
#ifdef _WIN32
#include <windows.h>
#define RESET_COLOR setColor(7)
#define SET_RED setColor(12)
#define SET_PINK setColor(13)
#define SET_LIGHT_BLUE setColor(9)
#define SET_BROWN setColor(6)

#else
#define SET_RED printf("\033[1;31m")
#define SET_LIGHT_BLUE printf("\033[1;36m")
#define SET_BROWN printf("\033[0;33m")
#define SET_PINK printf("\033[38;5;205m")
#define RESET_COLOR printf("\033[0m")
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

void rainbowColor(int i, int total, int *r, int *g, int *b, int shift) {
    double ratio = (double)(i + shift) / total;
    double h = fmod(ratio * 360.0, 360.0);
    double s = 1.0, v = 1.0;

    double C = v * s;
    double X = C * (1 - fabs(fmod(h / 60.0, 2) - 1));
    double m = v - C;

    double r1, g1, b1;
    if (h < 60) { r1 = C; g1 = X; b1 = 0; }
    else if (h < 120) { r1 = X; g1 = C; b1 = 0; }
    else if (h < 180) { r1 = 0; g1 = C; b1 = X; }
    else if (h < 240) { r1 = 0; g1 = X; b1 = C; }
    else if (h < 300) { r1 = X; g1 = 0; b1 = C; }
    else { r1 = C; g1 = 0; b1 = X; }

    *r = (int)((r1 + m) * 255);
    *g = (int)((g1 + m) * 255);
    *b = (int)((b1 + m) * 255);
}

void showRainbowBanner() {
    const char *lines[] = {
        "\t\t\t\t\t\t\tTTTTT  RRRR    III    OOO         GGG    OOO",
        "\t\t\t\t\t\t\t  T    R  R     I    O   O       G      O   O",
        "\t\t\t\t\t\t\t  T    RRR      I    O   O       G  GG  O   O",
        "\t\t\t\t\t\t\t  T    R R      I    O   O   .   G   G  O   O",
        "\t\t\t\t\t\t\t  T    R  RR   III    OOO    .    GGG    OOO"
    };

    int total = 0;
    for (int i = 0; i < 5; i++) total += strlen(lines[i]);

    for (int frame = 0; frame < 20; frame++) { // Short loop for banner animation
        printf("\033[H\033[J"); // clear screen
        int index = 0;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < strlen(lines[i]); j++) {
                int r, g, b;
                rainbowColor(index, total, &r, &g, &b, frame * 4);
                printf("\033[1;38;2;%d;%d;%dm%c", r, g, b, lines[i][j]);
                index++;
            }
            printf("\033[0m\n");
        }
        usleep(80000); // 80 ms delay
    }
}

struct Vehicle
{
    int id;
    char model[50];
    char type[20];
    float daily_rate;
    int available;
    char engine[20];
    int max_speed;
    char advantages[200];
    float service_rate;
    float maintenance_rate;
};

struct Booking
{
    int booking_id;
    char username[50];
    char license_number[20];
    int vehicle_id;
    char start_date[11];
    char end_date[11];
    float total_cost;
    char promo_code[20];
    char payment_method[20];
    char transaction_id[50];
    int payment_completed;
    int damage_agreement;
    float damage_cost;
};

struct User
{
    char username[50];
    char password[50];
    char name[100];
    char mobile[20];
    char dob[11];
    char email[50];
    char license[20];
};

struct Expense
{
    int id;
    char date[11];
    char description[100];
    float amount;
    char category[30];
};

struct Vehicle vehicles[MAX_VEHICLES];
struct Booking bookings[MAX_BOOKINGS];
struct User users[MAX_USERS];
struct Expense expenses[MAX_EXPENSES];
int vehicle_count = 0;
int booking_count = 0;
int user_count = 0;
int expense_count = 0;

void setColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
void get_masked_password(const char *prompt, char *buffer, int size)
{
    int i = 0;
    char ch;

    printf("%s", prompt);
    fflush(stdout);

#ifdef _WIN32
    while ((ch = _getch()) != '\r' && i < size - 1)
    {
        if (ch == '\b' && i > 0)
        {
            printf("\b \b");
            fflush(stdout);
            i--;
        }
        else if (ch != '\b')
        {
            buffer[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
#else
    struct termios old, newterm;
    tcgetattr(STDIN_FILENO, &old);
    newterm = old;
    newterm.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newterm);

    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && i < size - 1)
    {
        if (ch == '\b' || ch == 127)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else
        {
            buffer[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
#endif

    buffer[i] = '\0';
    printf("\n");
}

void load_data()
{
    FILE *file;
    file = fopen(VEHICLE_FILE, "r");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError opening vehicles file..!!!\n");
        RESET_COLOR;
        return;
    }
    vehicle_count = fread(vehicles, sizeof(struct Vehicle), MAX_VEHICLES, file);
    fclose(file);

    file = fopen(BOOKING_FILE, "r");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError opening bookings file..!!!\n");
        RESET_COLOR;
        return;
    }
    booking_count = fread(bookings, sizeof(struct Booking), MAX_BOOKINGS, file);
    fclose(file);

    file = fopen(USER_FILE, "r");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError opening users file..!!!\n");
        RESET_COLOR;
        return;
    }
    user_count = fread(users, sizeof(struct User), MAX_USERS, file);
    fclose(file);

    file = fopen(EXPENSE_FILE, "r");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError opening expenses file!\n");
        RESET_COLOR;
        return;
    }
    expense_count = fread(expenses, sizeof(struct Expense), MAX_EXPENSES, file);
    fclose(file);
}

void save_data()
{
    FILE *file;
    file = fopen(VEHICLE_FILE, "w");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError saving vehicles file..!!!\n");
        RESET_COLOR;
        return;
    }
    fwrite(vehicles, sizeof(struct Vehicle), vehicle_count, file);
    fclose(file);

    file = fopen(BOOKING_FILE, "w");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError saving bookings file..!!!\n");
        RESET_COLOR;
        return;
    }
    fwrite(bookings, sizeof(struct Booking), booking_count, file);
    fclose(file);

    file = fopen(USER_FILE, "w");
    if (file == NULL)
    {
        SET_RED;
        printf("\n\n\t\t\tError saving users file..!!!\n");
        SET_RED;
        return;
    }
    fwrite(users, sizeof(struct User), user_count, file);
    fclose(file);

    file = fopen(EXPENSE_FILE, "w");
    if (file == NULL)
    {
        printf("\n\n\t\t\tError saving expenses file..!!!\n");
        return;
    }
    fwrite(expenses, sizeof(struct Expense), expense_count, file);
    fclose(file);
}

void main_menu()
{
    int choice;
    do
    {
        CLEAR_SCREEN;
        showRainbowBanner();
        SET_LIGHT_BLUE;
        printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Car Rental System '.<-|.<-|.<-|.<\n\n\n");
        RESET_COLOR;
        SET_YELLOW;
        printf("\t\t\t\t\t\t1. Customer Registration\n");
        printf("\t\t\t\t\t\t2. Customer Login\n");
        printf("\t\t\t\t\t\t3. Admin Login\n");
        printf("\t\t\t\t\t\t4. Exit\n");
        RESET_COLOR;
        SET_PINK;
        printf("\n\n\t\t\t\tEnter your choice: ");
        RESET_COLOR;
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            customer_registration();
            break;
        case 2:
        {
            char username[50], password[50];
            SET_PINK;
            printf("\n\n\t\t\t\t\tEnter username: ");
            scanf("%s", username);
            get_masked_password("\n\t\t\t\t\tEnter password: ", password, sizeof(password));
            RESET_COLOR;
            if (customer_login(username, password))
            {
                SET_BROWN;
                printf("\n\n\t\t\t\tLogin successful!\n");
                RESET_COLOR;
                customer_menu(username);
            }
            else
            {
                SET_RED;
                printf("\n\n\t\t\t\tInvalid username or password!\n");
                RESET_COLOR;
            }
            break;
        }
        case 3:
        {
            char password[20];
            SET_PINK;
            get_masked_password("\n\n\t\t\t\tEnter admin password (default is 'admin123'): ", password, sizeof(password));
            RESET_COLOR;
            if (strcmp(password, "admin123") == 0)
            {
                admin_menu();
            }
            else
            {
                SET_RED;
                printf("\n\n\t\t\t\tInvalid password!\n");
                RESET_COLOR;
            }
            break;
        }
        case 4:
            SET_YELLOW;
            printf("\n\n\t\t\t\tExiting system...\n");
            RESET_COLOR;
            break;
        default:
            SET_RED;
            printf("\n\n\t\t\t\tInvalid choice. Please try again.\n");
            RESET_COLOR;
        }
    }
    while (choice != 4);
}

void customer_menu(char *username)
{
    int choice;
    do
    {
        CLEAR_SCREEN;
        showRainbowBanner();
        SET_LIGHT_BLUE;
        printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Customer Menu '.<-|.<-|.<-|.<\n\n\n");
        RESET_COLOR;
        SET_BROWN;
        printf("\n\n\t\tWelcome, %s\n", username);
        RESET_COLOR;
        SET_YELLOW;
        printf("\t\t\t\t\t\t1. View Available Vehicles\n");
        printf("\t\t\t\t\t\t2. View Vehicle Details\n");
        printf("\t\t\t\t\t\t3. Book a Vehicle\n");
        printf("\t\t\t\t\t\t4. Search Vehicles\n");
        printf("\t\t\t\t\t\t5. View My Bookings\n");
        printf("\t\t\t\t\t\t6. Logout\n");
        RESET_COLOR;
        SET_PINK;
        printf("\n\n\t\t\t\tEnter your choice: ");
        RESET_COLOR;
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            display_vehicles();
            break;
        case 2:
            display_vehicle_details();
            break;
        case 3:
            book_vehicle(username);
            break;
        case 4:
            search_vehicles();
            break;
        case 5:
            view_bookings(username);
            break;
        case 6:
            SET_BROWN;
            printf("\n\n\t\t\t\t\t\tLogging out...\n");
            RESET_COLOR;
            break;
        default:
            SET_RED;
            printf("\n\n\t\t\tInvalid choice. Please try again.\n");
            RESET_COLOR;
        }
        if (choice != 6)
        {
            SET_LIGHT_BLUE;
            printf("\n\n\n\t\t\tPress Enter to continue...");
            RESET_COLOR;
            while (getchar()!='\n');
            getchar();
        }
    }
    while (choice != 6);
}

void admin_menu()
{
    int choice;
    do
    {
        CLEAR_SCREEN;
        showRainbowBanner();
        SET_LIGHT_BLUE;
        printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Admin Menu '.<-|.<-|.<-|.<\n\n\n");
        RESET_COLOR;
        SET_YELLOW;
        printf("\t\t\t\t\t\t\t1. Add Vehicle\n");
        printf("\t\t\t\t\t\t\t2. Remove Vehicle\n");
        printf("\t\t\t\t\t\t\t3. Edit Vehicle\n");
        printf("\t\t\t\t\t\t\t4. View All Users\n");
        printf("\t\t\t\t\t\t\t5. Remove User\n");
        printf("\t\t\t\t\t\t\t6. View All Bookings\n");
        printf("\t\t\t\t\t\t\t7. Remove Booking\n");
        printf("\t\t\t\t\t\t\t8. Search User Bookings\n");
        printf("\t\t\t\t\t\t\t9. Revenue Report\n");
        printf("\t\t\t\t\t\t\t10. Manage Expenses\n");
        printf("\t\t\t\t\t\t\t11. Process Vehicle Return\n");
        printf("\t\t\t\t\t\t\t12. Back to Main Menu\n");
        RESET_COLOR;
        SET_PINK;
        printf("\n\n\t\t\t\tEnter your choice: ");
        RESET_COLOR;
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            add_vehicle();
            break;
        case 2:
            remove_vehicle();
            break;
        case 3:
            edit_vehicle();
            break;
        case 4:
            view_all_users();
            break;
        case 5:
            remove_user();
            break;
        case 6:
            view_all_bookings();
            break;
        case 7:
            remove_booking();
            break;
        case 8:
            search_user_bookings();
            break;
        case 9:
            revenue_report();
            break;
        case 10:
            manage_expenses_menu();
            break;
        case 11:
            process_vehicle_return();
            break;
        case 12:
            break;
        default:
            SET_RED;
            printf("\n\n\t\t\tInvalid choice. Please try again...\n");
            RESET_COLOR;
        }
        if (choice != 12)
        {
            SET_LIGHT_BLUE;
            printf("\n\n\n\t\t\tPress Enter to continue...");
            RESET_COLOR;
            while (getchar() != '\n');
            getchar();
        }
    }
    while (choice != 12);
}

void display_vehicles()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Available Vehicles '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_YELLOW;
    printf("\t\t\t\t   ID  Model             Type              \tDaily Rate                  Availability\n");
    RESET_COLOR;
    printf("\t\t\t\t  -----------        ------------        \t---------------               ---------------\n");

    int available_found = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].available == 1)
        {
            SET_BROWN;
            printf("\t\t\t\t    %-3d          %-18s           %-11s   $%-9.2f        Available\n",
                   vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate);
            RESET_COLOR;
            available_found = 1;
        }
    }
    if (!available_found)
    {
        printf("\n\n\t\t\tNo available vehicles found. Please check booking status or add new vehicles...\n");
    }
}

void display_vehicle_details()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Available Vehicles '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_YELLOW;
    printf("\t\t\t\t\t     ID  Model                  Type                 Daily Rate                 Availability\n");
    RESET_COLOR;
    printf("\t\t\t\t\t  ----------------        --------------         -----------------             ----------------\n");

    int available_found = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].available == 1)
        {
            printf("\t\t\t\t\t\t%-3d \t\t%-18s \t   %-11s $%-9.2f      Available\n",
                   vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate);
            available_found = 1;
        }
    }
    if (!available_found)
    {
        printf("\n\n\t\t\tNo available vehicles found..!!!\n");
        return;
    }

    int vehicle_id;
    printf("\n\n\t\t\t\tEnter Vehicle ID to view details: ");
    scanf("%d", &vehicle_id);

    int found = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == vehicle_id)
        {
            found = 1;
            SET_LIGHT_BLUE;
            printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Vehicle Details '.<-|.<-|.<-|.<\n\n\n");
            RESET_COLOR;
            SET_BROWN;
            printf("\t\t\t\t\t\tID: %d\n", vehicles[i].id);
            printf("\t\t\t\t\t\tModel: %s\n", vehicles[i].model);
            printf("\t\t\t\t\t\tType: %s\n", vehicles[i].type);
            printf("\t\t\t\t\t\tDaily Rate: $%.2f\n", vehicles[i].daily_rate);
            printf("\t\t\t\t\t\tEngine: %s\n", vehicles[i].engine);
            printf("\t\t\t\t\t\tMax Speed: %d mph\n", vehicles[i].max_speed);
            printf("\t\t\t\t\t\tAdvantages: %s\n", vehicles[i].advantages);
            printf("\t\t\t\t\t\tAvailability: %s\n", vehicles[i].available ? "Available" : "Booked");
            RESET_COLOR;
            break;
        }
    }
    if (!found)
    {
        SET_RED;
        printf("\n\n\t\t\tVehicle not found..!!!\n");
        RESET_COLOR;
    }
}

int calculate_age(const char *dob)
{
    int birth_year, birth_month, birth_day;
    sscanf(dob, "%d-%d-%d", &birth_year, &birth_month, &birth_day);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    int current_year = tm_info->tm_year + 1900;
    int current_month = tm_info->tm_mon + 1;
    int current_day = tm_info->tm_mday;

    int age = current_year - birth_year;
    if (current_month < birth_month || (current_month == birth_month && current_day < birth_day))
    {
        age--;
    }
    return age;
}

float calculate_damage_cost(int vehicle_id)
{
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == vehicle_id)
        {
            if (strcmp(vehicles[i].type, "Luxury") == 0) return 500.0;
            if (strcmp(vehicles[i].type, "SUV") == 0) return 300.0;
            if (strcmp(vehicles[i].type, "Truck") == 0) return 400.0;
            if (strcmp(vehicles[i].type, "Van") == 0) return 350.0;
            return 200.0;
        }
    }
    return 200.0;
}

void book_vehicle(char *username)
{
    CLEAR_SCREEN;
    showRainbowBanner();
    if (vehicle_count == 0)
    {
        SET_RED;
        printf("\n\n\t\t\tNo vehicles available for booking...\n");
        RESET_COLOR;
        return;
    }

    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            int age = calculate_age(users[i].dob);
            if (age < 18)
            {
                SET_RED;
                printf("\n\n\t\t\t\tSorry, you must be at least 18 years old to rent a car.\n");
                printf("\n\t\t\t\tYour age is %d based on DOB %s.\n", age, users[i].dob);
                RESET_COLOR;
                return;
            }
            strcpy(bookings[booking_count].license_number, users[i].license);
            break;
        }
    }

    display_vehicles();

    struct Booking new_booking;
    int vehicle_id;
    int found = 0;

    SET_PINK;
    printf("\n\n\t\t\t\tEnter Vehicle ID to book: ");
    RESET_COLOR;
    scanf("%d", &vehicle_id);

    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == vehicle_id && vehicles[i].available == 1)
        {
            found = 1;
            new_booking.vehicle_id = vehicle_id;
            strcpy(new_booking.username, username);
            break;
        }
    }

    if (!found)
    {
        SET_RED;
        printf("\n\n\t\t\tInvalid Vehicle ID or vehicle not available.\n");
        RESET_COLOR;
        return;
    }

    char today[11];
    get_current_date(today);

    do
    {
        SET_PINK;
        printf("\n\t\t\tEnter start date (YYYY-MM-DD): ");
        RESET_COLOR;
        scanf("%s", new_booking.start_date);
        if (!validate_date(new_booking.start_date))
        {
            SET_RED;
            printf("\n\n\t\t\t\tInvalid date format. Please use YYYY-MM-DD.\n");
            continue;
        }
        int days_before = date_diff(new_booking.start_date, today);
        if (days_before > 0)
        {
            printf("\n\t\t\t\tStart date cannot be in the past. Please choose today or a future date.\n");
            RESET_COLOR;
        }
    }
    while (!validate_date(new_booking.start_date) || date_diff(new_booking.start_date, today) > 0);

    do
    {
        SET_PINK;
        printf("\n\t\t\t\tEnter end date (YYYY-MM-DD): ");
        RESET_COLOR;
        scanf("%s", new_booking.end_date);
    }
    while (!validate_date(new_booking.end_date));

    if (date_diff(new_booking.start_date, new_booking.end_date) <= 0)
    {
        SET_RED;
        printf("\n\t\t\t\tEnd date must be after start date.\n");
        RESET_COLOR;
        return;
    }

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Damage Policy Agreement '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_BROWN;
    printf("\t\t\t\t\tBy renting this vehicle, you agree to:\n");
    printf("\t\t\t\t\t\t1.Be responsible for any damages during rental period\n");
    printf("\t\t\t\t\t\t2.Pay the following potential damage costs:\n");
    float base_damage = calculate_damage_cost(vehicle_id);
    printf("\n\t\t\t\t\t   - Tire damage: $%.2f\n", base_damage * 0.3);
    printf("\t\t\t\t\t   - Engine damage: $%.2f\n", base_damage * 0.5);
    printf("\t\t\t\t\t   - Windshield damage: $%.2f\n", base_damage * 0.2);
    printf("\t\t\t\t\t    -Maximum liability: $%.2f\n", base_damage);
    RESET_COLOR;
    char agree;
    SET_YELLOW;
    printf("\n\n\t\t\t\tDo you agree to these terms? (Y/N): ");
    RESET_COLOR;
    scanf(" %c", &agree);

    if (toupper(agree) != 'Y')
    {
        SET_RED;
        printf("\n\t\t\t\tBooking cancelled. You must agree to the damage policy.\n");
        RESET_COLOR;
        return;
    }
    new_booking.damage_agreement = 1;

    int payment_choice;
    SET_PINK;
    printf("\n\t\t\t\tSelect payment method:\n\n");
    RESET_COLOR;
    SET_YELLOW;
    printf("\t\t\t\t\t\t1. Bkash (Use 'ROADTRIP' for 15%% discount)\n");
    printf("\t\t\t\t\t\t2. Nagad (Use 'TRAVELWE' for 15%% discount)\n");
    printf("\t\t\t\t\t\t3. Hand Cash (No discount available)\n");
    RESET_COLOR;
    SET_PINK;
    printf("\n\t\t\t\tEnter choice: ");
    RESET_COLOR;
    scanf("%d", &payment_choice);

    switch(payment_choice)
    {
    case 1:
        strcpy(new_booking.payment_method, "Bkash");
        break;
    case 2:
        strcpy(new_booking.payment_method, "Nagad");
        break;
    case 3:
        strcpy(new_booking.payment_method, "Hand Cash");
        break;
    default:
        SET_RED;
        printf("\n\n\t\t\tInvalid choice. Defaulting to Hand Cash.\n");
        RESET_COLOR;
        strcpy(new_booking.payment_method, "Hand Cash");
    }

    SET_PINK;
    printf("\n\t\t\t\tEnter promo code (or 'none'): ");
    RESET_COLOR;
    scanf("%s", new_booking.promo_code);

    int days = date_diff(new_booking.start_date, new_booking.end_date);
    float daily_rate = 0;

    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == vehicle_id)
        {
            daily_rate = vehicles[i].daily_rate;
            vehicles[i].available = 0;
            break;
        }
    }

    new_booking.total_cost = days * daily_rate;
    new_booking.damage_cost = 0;
    apply_discounts(&new_booking);

    new_booking.booking_id = booking_count + 1000;
    generate_transaction_id(username, new_booking.transaction_id);
    new_booking.payment_completed = 1;

    bookings[booking_count++] = new_booking;

    SET_BROWN;
    printf("\n\n\t\t\t\tBooking Successful!\n\n");
    printf("\t\t\t\t\t\tBooking ID: %d\n", new_booking.booking_id);
    printf("\t\t\t\t\t\tTransaction ID: %s\n", new_booking.transaction_id);
    printf("\t\t\t\t\t\tTotal Cost: $%.2f\n", new_booking.total_cost);
    printf("\t\t\t\t\t\tPayment Method: %s\n", new_booking.payment_method);
    RESET_COLOR;
}

void search_vehicles()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    char search_type[20];
    SET_PINK;
    printf("\n\n\t\t\tEnter vehicle type to search (Economy, SUV, Luxury, etc.): ");
    RESET_COLOR;
    scanf("%s", search_type);

    SET_LIGHT_BLUE;
    printf("\n\n\t\t\t\t\t\t\t>.|->.|->.|->.' Search Results '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t   ID                         Model                 Type                Daily Rate              Availability\n");
    RESET_COLOR;
    printf("\n\t\t\t\t-----------               -------------          -----------         ------------------        --------------\n");

    int found = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (strcasecmp(vehicles[i].type, search_type) == 0 && vehicles[i].available == 1)
        {
            SET_BROWN;
            printf("\n\t\t\t\t     %-3d             %-18s        %-11s         $%-9.2f              Available\n",
                   vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate);
            RESET_COLOR;
            found = 1;
        }
    }

    if (!found)
    {
        SET_RED;
        printf("\n\t\t\t\t\tNo vehicles of type '%s' available.\n", search_type);
        RESET_COLOR;
    }
}

void view_bookings(char *username)
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Your Bookings '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t  Booking ID        Vehicle ID              Dates                 Total Cost          Payment Method       Transaction ID\n");
    RESET_COLOR;
    printf("\n\t\t-------------     --------------        ------------------      --------------       ----------------   ---------------------\n");

    int found = 0;
    for (int i = 0; i < booking_count; i++)
    {
        if (strcmp(bookings[i].username, username) == 0)
        {
            SET_BROWN;
            printf("\n\t\t%-11d       %-11d     %s to %s     $%-9.2f       %-14s         %s\n",
                   bookings[i].booking_id, bookings[i].vehicle_id,
                   bookings[i].start_date, bookings[i].end_date,
                   bookings[i].total_cost, bookings[i].payment_method,
                   bookings[i].transaction_id);
            RESET_COLOR;
            found = 1;
        }
    }

    if (!found)
    {
        SET_RED;
        printf("\n\n\t\t\t\tNo bookings found.\n");
        RESET_COLOR;
    }
}

void add_vehicle()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    if (vehicle_count >= MAX_VEHICLES)
    {
        SET_RED;
        printf("\n\n\t\t\t\t\tMaximum vehicle capacity reached.\n");
        RESET_COLOR;
        return;
    }

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Current Vehicles '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t    ID  Model                 Type              \t          Daily Rate                    Availability\n");
    RESET_COLOR;
    printf("\n\t\t\t\t  --------------         --------------          \t       ---------------                 --------------\n");

    int any_vehicles = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t\t    %-3d              %-18s%-11s                   $%-9.2f                  %s\n",
               vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate,
               vehicles[i].available ? "Available" : "Booked");
        RESET_COLOR;
        any_vehicles = 1;
    }

    if (!any_vehicles)
    {
        SET_RED;
        printf("\n\t\t\t\t\tNo vehicles currently in the system.\n");
        RESET_COLOR;
    }

    struct Vehicle new_vehicle;

    SET_PINK;
    printf("\n\n\t\t\tEnter vehicle details:\n");
    int new_id;
    do
    {
        printf("\n\n\t\t\tEnter new vehicle ID (must be unique): ");
        RESET_COLOR;
        scanf("%d", &new_id);
        int id_exists = 0;
        for (int i = 0; i < vehicle_count; i++)
        {
            if (vehicles[i].id == new_id)
            {
                id_exists = 1;
                SET_RED;
                printf("\n\t\t\t\t\t\tID %d is already taken. Please choose another.\n", new_id);
                RESET_COLOR;
                break;
            }
        }
        if (!id_exists) break;
    }
    while (1);

    new_vehicle.id = new_id;

    while (getchar() != '\n');
    SET_BROWN;
    printf("\n\n\t\t\t\t\t\t\tModel: ");
    scanf(" %[^\n]s", new_vehicle.model);
    printf("\n\t\t\t\t\t\t\tType (Economy/SUV/Luxury/etc.): ");
    scanf("%s", new_vehicle.type);
    printf("\n\t\t\t\t\t\t\tDaily rate: ");
    scanf("%f", &new_vehicle.daily_rate);

    int engine_choice;
    SET_YELLOW;
    printf("\n\n\t\t\t\tEngine type:\n\t\t\t\t1. Inline-4\n\t\t\t\t2. V6/V8\n");
    RESET_COLOR;
    printf("\n\n\t\t\t\tEnter choice: ");
    scanf("%d", &engine_choice);
    strcpy(new_vehicle.engine, engine_choice == 1 ? "Inline-4" : "V6/V8");

    printf("\n\n\t\t\t\t\t\tMax Speed: ");
    scanf("%d", &new_vehicle.max_speed);
    while (getchar() != '\n');
    printf("\n\t\t\t\t\t\tEnter advantages/features (max 200 chars): ");
    scanf(" %[^\n]s", new_vehicle.advantages);

    printf("\n\t\t\t\t\t\tService rate per day: ");
    scanf("%f", &new_vehicle.service_rate);
    printf("\\n\t\t\t\t\t\tMaintenance rate multiplier: ");
    scanf("%f", &new_vehicle.maintenance_rate);
    RESET_COLOR;
    new_vehicle.available = 1;

    vehicles[vehicle_count++] = new_vehicle;

    SET_YELLOW;
    printf("\n\n\t\t\t\t\t\t\tVehicle added successfully with ID %d.\n", new_vehicle.id);
    RESET_COLOR;
}

void remove_vehicle()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Current Vehicles '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t\t  ID                    Model                    Type                   Daily Rate              Availability\n");
    RESET_COLOR;
    printf("\n\t\t\t\t\t ----------        ---------------            -----------             -------------           --------------\n");

    int any_vehicles = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t\t\t     %-3d             %-18s               %-11s           $%-9.2f              %s\n",
               vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate,
               vehicles[i].available ? "Available" : "Booked");
        RESET_COLOR;
        any_vehicles = 1;
    }

    if (!any_vehicles)
    {
        SET_RED;
        printf("\n\t\t\t\t\tNo vehicles currently in the system.\n");
        RESET_COLOR;
        return;
    }

    int id;
    SET_PINK;
    printf("\n\n\t\t\t\tEnter Vehicle ID to remove: ");
    RESET_COLOR;
    scanf("%d", &id);

    int found = -1;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\t\t\t\t\t\tVehicle not found..!!!\n");
        return;
    }

    for (int i = 0; i < booking_count; i++)
    {
        if (bookings[i].vehicle_id == id)
        {
            printf("\n\t\t\t\tCannot remove vehicle with active bookings.\n");
            return;
        }
    }

    for (int i = found; i < vehicle_count - 1; i++)
    {
        vehicles[i] = vehicles[i + 1];
    }

    vehicle_count--;
    printf("\n\t\t\t\t\t\tVehicle removed successfully.\n");
    RESET_COLOR;
}

void edit_vehicle()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Current Vehicles '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t      ID  Model               Type                  Daily Rate                  Availability\n");
    RESET_COLOR;
    printf("\n\t\t\t\t   ---------------        -------------          -----------------             ---------------\n");

    int any_vehicles = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t\t %-3d               %-18s %-11s            $%-9.2f                 %s\n",
               vehicles[i].id, vehicles[i].model, vehicles[i].type, vehicles[i].daily_rate,
               vehicles[i].available ? "Available" : "Booked");
        RESET_COLOR;
        any_vehicles = 1;
    }

    if (!any_vehicles)
    {
        SET_RED;
        printf("\n\t\t\t\t\tNo vehicles currently in the system..!!!\n");
        RESET_COLOR;
        return;
    }

    int id;
    SET_PINK;
    printf("\n\n\t\t\t\tEnter Vehicle ID to edit: ");
    RESET_COLOR;
    scanf("%d", &id);

    int found = -1;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\t\t\t\t\tVehicle not found.\n");
        RESET_COLOR;
        return;
    }

    SET_BROWN;
    printf("\n\t\t\t\t\tCurrent Details:\n");
    printf("\n\t\t\t\t\t\tModel: %s\n", vehicles[found].model);
    printf("\t\t\t\t\t\tType: %s\n", vehicles[found].type);
    printf("\t\t\t\t\t\tDaily Rate: %.2f\n", vehicles[found].daily_rate);
    printf("\t\t\t\t\t\tEngine: %s\n", vehicles[found].engine);
    printf("\t\t\t\t\t\tMax Speed: %d\n", vehicles[found].max_speed);
    printf("\t\t\t\t\t\tAdvantages: %s\n", vehicles[found].advantages);
    printf("\t\t\t\t\t\tService Rate: %.2f\n", vehicles[found].service_rate);
    printf("\t\t\t\t\t\tMaintenance Rate: %.2f\n", vehicles[found].maintenance_rate);
    printf("\t\t\t\t\t\tAvailable: %s\n", vehicles[found].available ? "Yes" : "No");
    RESET_COLOR;

    SET_PINK;
    printf("\n\t\t\t\tEnter new details (press Enter to keep current):\n");
    RESET_COLOR;

    char line[200];
    while (getchar() != '\n');

    SET_BROWN;
    printf("\t\t\t\t\t\tModel: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(vehicles[found].model, line);
    }

    printf("\n\t\t\t\t\t\tType: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(vehicles[found].type, line);
    }

    printf("\n\t\t\t\t\t\tDaily Rate: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        vehicles[found].daily_rate = atof(line);
    }

    printf("\n\t\t\t\t\t\tEngine (1 for Inline-4, 2 for V6/V8): ");
    SET_YELLOW;
    printf("(1 for Inline-4, 2 for V6/V8)\n");
    RESET_COLOR;
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        int choice = atoi(line);
        strcpy(vehicles[found].engine, choice == 1 ? "Inline-4" : "V6/V8");
    }

    printf("\n\t\t\t\t\t\tMax Speed: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        vehicles[found].max_speed = atoi(line);
    }

    printf("\n\t\t\t\t\t\tAdvantages: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(vehicles[found].advantages, line);
    }

    printf("\n\t\t\t\t\t\tService Rate: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        vehicles[found].service_rate = atof(line);
    }

    printf("\n\t\t\t\t\t\tMaintenance Rate: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        vehicles[found].maintenance_rate = atof(line);
    }

    printf("\n\t\t\t\t\t\tAvailable (1 for yes, 0 for no): ");
    SET_YELLOW;
    printf("(1 for yes, 0 for no)\n");
    RESET_COLOR;
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        vehicles[found].available = atoi(line);
    }

    printf("\n\t\t\t\t\tVehicle details updated successfully.\n");
    RESET_COLOR;
}

int validate_date(const char *date)
{
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return 0;
    }

    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) return 0;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    int current_year = tm_info->tm_year + 1900;

    if (year < current_year || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return 0;
    if (month == 2)
    {
        int is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (day > (28 + is_leap)) return 0;
    }

    return 1;
}

int validate_dob(const char *dob)
{
    if (strlen(dob) != 10) return 0;
    if (dob[4] != '-' || dob[7] != '-') return 0;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7) continue;
        if (!isdigit(dob[i])) return 0;
    }

    int year, month, day;
    if (sscanf(dob, "%d-%d-%d", &year, &month, &day) != 3) return 0;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    int current_year = tm_info->tm_year + 1900;
    if (year < 1900 || year > current_year) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return 0;
    if (month == 2)
    {
        int is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (day > (28 + is_leap)) return 0;
    }

    return 1;
}

int date_diff(const char *start, const char *end)
{
    struct tm start_tm = {0};
    struct tm end_tm = {0};

    sscanf(start, "%d-%d-%d", &start_tm.tm_year, &start_tm.tm_mon, &start_tm.tm_mday);
    sscanf(end, "%d-%d-%d", &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday);

    start_tm.tm_year -= 1900;
    start_tm.tm_mon -= 1;
    end_tm.tm_year -= 1900;
    end_tm.tm_mon -= 1;

    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);

    if (start_time == -1 || end_time == -1)
    {
        return -1;
    }

    double diff = difftime(end_time, start_time) / (60 * 60 * 24);
    return (int)diff;
}

void apply_discounts(struct Booking *booking)
{
    int days = date_diff(booking->start_date, booking->end_date);
    float discount = 0;

    if (days >= 7)
    {
        discount += 0.10;
        SET_RED;
        printf("\t\t\t\tApplied 10%% discount for 7+ day rental\n");
        RESET_COLOR;
    }

    if (strcmp(booking->payment_method, "Bkash") == 0 && strcasecmp(booking->promo_code, "ROADTRIP") == 0)
    {
        discount += 0.15;
        printf("\t\t\t\t\tApplied 15%% discount for promo code ROADTRIP with Bkash\n");
    }
    else if (strcmp(booking->payment_method, "Nagad") == 0 && strcasecmp(booking->promo_code, "TRAVELWE") == 0)
    {
        discount += 0.15;
        printf("\t\t\t\t\tApplied 15%% discount for promo code TRAVELWE with Nagad\n");
    }

    if (discount > 0)
    {
        booking->total_cost *= (1 - discount);
    }
}

void initialize_sample_data()
{
    struct Vehicle sample_vehicles[] =
    {
        {
            1, "Toyota Corolla", "Economy", 50.0, 1, "Inline-4", 280,
            "Fuel efficient, comfortable ride, low maintenance costs", 5.0, 1.2
        },
        {
            2, "Honda Civic", "Economy", 55.0, 1, "Inline-4", 285,
            "Great handling, modern features, excellent safety ratings", 5.5, 1.2
        },
        {
            3, "Toyota RAV4", "SUV", 80.0, 1, "Inline-4", 270,
            "Spacious interior, all-wheel drive, great for families", 8.0, 1.5
        },
        {
            4, "Mazda CX-5", "SUV", 85.0, 1, "Inline-4", 275,
            "Luxury feel, sporty handling, premium interior", 8.5, 1.5
        },
        {
            5, "BMW 3 Series", "Luxury", 120.0, 1, "V6/V8", 300,
            "Sporty performance, cutting-edge tech, prestige brand", 15.0, 2.0
        },
        {
            6, "Mercedes C-Class", "Luxury", 130.0, 1, "V6/V8", 295,
            "Ultimate comfort, elegant design, advanced features", 16.0, 2.0
        },
        {
            7, "Ford Ranger", "Truck", 90.0, 1, "V6/V8", 260,
            "Powerful towing, rugged durability, versatile utility", 12.0, 1.8
        },
        {
            8, "Toyota Hiace", "Van", 100.0, 1, "Inline-4", 250,
            "Large capacity, reliable, perfect for groups", 10.0, 1.7
        }
    };

    for (int i = 0; i < 8; i++)
    {
        vehicles[vehicle_count] = sample_vehicles[i];
        vehicles[vehicle_count].available = 1;
        vehicle_count++;
    }

    SET_YELLOW;
    printf("\n\t\t\t\tInitialized with sample vehicle data. Total vehicles: %d\n", vehicle_count);
    RESET_COLOR;
}

void customer_registration()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    showRainbowBanner();
    if (user_count >= MAX_USERS)
    {
        SET_RED;
        printf("\n\t\t\tMaximum user capacity reached...!!!\n");
        RESET_COLOR;
        return;
    }

    struct User new_user;

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t>.|->.|->.|->.' Customer Registration '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;

    do
    {
        SET_PINK;
        printf("\t\t\t\t\t\t\tChoose a username: ");
        RESET_COLOR;
        scanf("%s", new_user.username);
        if (is_username_taken(new_user.username))
        {
            SET_RED;
            printf("\n\t\t\t\t\t\tUsername already taken. Please choose another.\n");
            RESET_COLOR;
        }
    }
    while (is_username_taken(new_user.username));

    SET_PINK;
    get_masked_password("\n\t\t\t\t\t\t\tChoose a password: ", new_user.password, sizeof(new_user.password));
    RESET_COLOR;

    while (getchar() != '\n');
    SET_PINK;
    printf("\n\t\t\t\t\t\t\t\tFull name: ");
    scanf(" %[^\n]s", new_user.name);

    printf("\n\t\t\t\t\t\t\t    Mobile number: ");
    scanf("%s", new_user.mobile);
    do
    {
        printf("\n\t\t\t\t\t\tDate of birth (YYYY-MM-DD): ");
        scanf("%s", new_user.dob);
        if (!validate_dob(new_user.dob))
        {
            SET_RED;
            printf("\n\t\t\t\t\t\tInvalid date of birth. Please enter a valid date.\n");
            RESET_COLOR;
        }
    }
    while (!validate_dob(new_user.dob));

    do
    {
        printf("\n\t\t\t\t\t\t\t\t    Email: ");
        scanf("%s", new_user.email);
        if (is_email_taken(new_user.email))
        {
            SET_RED;
            printf("\n\t\t\t\t\tEmail already registered. Please use another.\n");
            RESET_COLOR;
        }
    }
    while (is_email_taken(new_user.email));

    do
    {
        printf("\n\t\t\t\t\t\t\t   License number: ");
        scanf("%s", new_user.license);
        if (is_license_taken(new_user.license))
        {
            printf("\n\t\t\t\t\t\tLicense number already registered.\n");
            RESET_COLOR;
        }
    }
    while (is_license_taken(new_user.license));

    users[user_count++] = new_user;
    SET_YELLOW;
    printf("\n\t\t\tRegistration successful! You can now login with your username.\n");
    RESET_COLOR;
}

int customer_login(char *username, char *password)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0 &&
                strcmp(users[i].password, password) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void view_all_users()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t>.|->.|->.|->.' All Registered Users '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t   Username            Name              Mobile               Email                 DOB               License\n");
    RESET_COLOR;
    printf("\n\t\t\t  ----------        ----------        -------------        ------------          ----------         ----------\n");

    for (int i = 0; i < user_count; i++)
    {
        printf("\n\t\t\t%-9s             %-19s             %-12s                 %-22s                 %-10s                %s\n",
               users[i].username, users[i].name, users[i].mobile,
               users[i].email, users[i].dob, users[i].license);
    }
}

void remove_user()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    view_all_users();

    if (user_count == 0)
    {
        SET_RED;
        printf("\t\t\t\tNo users to remove.\n");
        RESET_COLOR;
        return;
    }

    char username[50];
    SET_PINK;
    printf("\n\n\t\t\tEnter username to remove: ");
    RESET_COLOR;
    scanf("%s", username);

    int found = -1;
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\t\t\t\t\tUser not found.\n");
        RESET_COLOR;
        return;
    }

    for (int i = 0; i < booking_count; i++)
    {
        if (strcmp(bookings[i].username, username) == 0)
        {
            SET_RED;
            printf("\n\t\t\t\tCannot remove user with active bookings.\n");
            RESET_COLOR;
            return;
        }
    }

    for (int i = found; i < user_count - 1; i++)
    {
        users[i] = users[i + 1];
    }

    user_count--;
    SET_YELLOW;
    printf("\n\t\t\t\t\tUser removed successfully.\n");
    RESET_COLOR;
}

void view_all_bookings()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' All Bookings '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t   Booking ID           Username         Vehicle ID               Dates                 Total Cost       Payment Method          Transaction ID\n");
    RESET_COLOR;
    printf("\n\t\t\t  -----------        --------------     -------------         -------------          --------------      ----------------          ---------------\n");

    for (int i = 0; i < booking_count; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t  %-11d            %-9s             %-11d         %s to %s             $%-9.2f            %-14s           %s\n",
               bookings[i].booking_id, bookings[i].username, bookings[i].vehicle_id,
               bookings[i].start_date, bookings[i].end_date,
               bookings[i].total_cost, bookings[i].payment_method,
               bookings[i].transaction_id);
        RESET_COLOR;
    }
}

void remove_booking()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    view_all_bookings();

    if (booking_count == 0)
    {
        SET_RED;
        printf("\n\n\t\t\tNo bookings to remove.\n");
        RESET_COLOR;
        return;
    }

    int booking_id;
    SET_PINK;
    printf("\n\t\t\t\t\t\tEnter Booking ID to remove: ");
    RESET_COLOR;
    scanf("%d", &booking_id);

    int found = -1;
    for (int i = 0; i < booking_count; i++)
    {
        if (bookings[i].booking_id == booking_id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\n\t\t\tBooking not found.\n");
        RESET_COLOR;
        return;
    }

    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == bookings[found].vehicle_id)
        {
            vehicles[i].available = 1;
            break;
        }
    }

    for (int i = found; i < booking_count - 1; i++)
    {
        bookings[i] = bookings[i + 1];
    }

    booking_count--;
    SET_YELLOW;
    printf("\n\n\t\t\tBooking removed successfully. Vehicle is now available.\n");
    RESET_COLOR;
}

void search_user_bookings()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Users with Bookings '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_PINK;
    printf("\n\t\t\t\t Username\n");
    RESET_COLOR;
    printf("\n\t\t\t\t----------\n");

    int found_users = 0;
    char displayed_users[MAX_USERS][50];
    int displayed_count = 0;

    for (int i = 0; i < booking_count; i++)
    {
        int already_displayed = 0;
        for (int j = 0; j < displayed_count; j++)
        {
            if (strcmp(bookings[i].username, displayed_users[j]) == 0)
            {
                already_displayed = 1;
                break;
            }
        }
        if (!already_displayed)
        {
            printf("\n\t\t\t\t  %s\n", bookings[i].username);
            strcpy(displayed_users[displayed_count++], bookings[i].username);
            found_users = 1;
        }
    }

    if (!found_users)
    {
        SET_RED;
        printf("\n\n\t\t\tNo users with bookings found.\n");
        RESET_COLOR;
    }

    char username[50];
    SET_PINK;
    printf("\n\t\t\t\t\tEnter username to view bookings: ");
    RESET_COLOR;
    scanf("%s", username);

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Bookings for %s '.<-|.<-|.<-|.<\n\n\n", username);
    printf("\n\t\t\t    Booking ID         Vehicle ID              Dates              Total Cost       Payment Method      Transaction ID\n");
    RESET_COLOR;
    printf("\n\t\t\t ----------------   -----------------      -------------       ---------------    ----------------    -----------------\n");

    int found = 0;
    for (int i = 0; i < booking_count; i++)
    {
        if (strcmp(bookings[i].username, username) == 0)
        {
            SET_BROWN;
            printf("\n\t\t\t    %-11d          %-11d        %s to %s        $%-9.2f         %-14s            %s\n",
                   bookings[i].booking_id, bookings[i].vehicle_id,
                   bookings[i].start_date, bookings[i].end_date,
                   bookings[i].total_cost, bookings[i].payment_method,
                   bookings[i].transaction_id);
            RESET_COLOR;
            found = 1;
        }
    }

    if (!found)
    {
        SET_RED;
        printf("\n\n\t\t\tNo bookings found for this user.\n");
        RESET_COLOR;
    }
}

void generate_transaction_id(char *username, char *transaction_id)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    sprintf(transaction_id, "%s%04d%02d%02d%02d%02d%02d",
            username,
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
}

int is_username_taken(char *username)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int is_email_taken(char *email)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].email, email) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int is_license_taken(char *license)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].license, license) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void revenue_report()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    float total_revenue = 0.0;
    float total_expenses = 0.0;
    float net_profit = 0.0;

    for (int i = 0; i < booking_count; i++)
    {
        total_revenue += bookings[i].total_cost;
    }

    for (int i = 0; i < expense_count; i++)
    {
        total_expenses += expenses[i].amount;
    }

    net_profit = total_revenue - total_expenses;

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Revenue Report '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_BROWN;
    printf("\t\t\t\t\tTotal Bookings: %d\n", booking_count);
    printf("\t\t\t\t\tTotal Revenue: $%.2f\n", total_revenue);
    printf("\t\t\t\t\tTotal Expenses: $%.2f\n", total_expenses);
    printf("\t\t\t\t\tNet Profit: $%.2f\n", net_profit);
    RESET_COLOR;
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Expense Breakdown '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t  Category                 Amount               Percentage\n");
    RESET_COLOR;
    printf("\n\t\t\t\t------------           --------------          -------------\n");

    const char *categories[] = {"Maintenance", "Service", "Admin", "Other"};
    float category_totals[4] = {0};

    for (int i = 0; i < expense_count; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (strcmp(expenses[i].category, categories[j]) == 0)
            {
                category_totals[j] += expenses[i].amount;
                break;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        float percentage = (total_expenses > 0) ? (category_totals[i] / total_expenses * 100) : 0;
        SET_BROWN;
        printf("\n\t\t\t\t%-15s                  $%-10.2f                %5.1f%%\n", categories[i], category_totals[i], percentage);
        RESET_COLOR;
    }

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Vehicle Utilization '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;
    SET_YELLOW;
    printf("\n\t\t\t\t\tTotal Vehicles: %d\n", vehicle_count);
    RESET_COLOR;

    int available_count = 0;
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].available) available_count++;
    }
    SET_BROWN;
    printf("\n\t\t\t\t\tAvailable Vehicles: %d\n", available_count);
    printf("\n\t\t\t\t\tBooked Vehicles: %d\n", vehicle_count - available_count);
    printf("\n\t\t\t\t\tUtilization Rate: ");
    RESET_COLOR;
    if (vehicle_count > 0)
    {
        printf("%.1f%%\n", (vehicle_count - available_count) * 100.0 / vehicle_count);
    }
    else
    {
        printf("0.0%%\n");
    }

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t>.|->.|->.|->.' Revenue by Vehicle Type '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t\t\t\t    Type          Bookings          Revenue\n");
    RESET_COLOR;
    printf("\n\t\t\t\t\t\t\t------------    ------------       --------\n");

    const char *types[] = {"Economy", "SUV", "Luxury", "Truck", "Van"};
    int type_bookings[5] = {0};
    float type_revenue[5] = {0};

    for (int i = 0; i < booking_count; i++)
    {
        for (int j = 0; j < vehicle_count; j++)
        {
            if (bookings[i].vehicle_id == vehicles[j].id)
            {
                for (int k = 0; k < 5; k++)
                {
                    if (strcmp(vehicles[j].type, types[k]) == 0)
                    {
                        type_bookings[k]++;
                        type_revenue[k] += bookings[i].total_cost;
                        break;
                    }
                }
                break;
            }
        }
    }

    for (int i = 0; i < 5; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t\t   %-12s          %-9d            $%.2f\n", types[i], type_bookings[i], type_revenue[i]);
        RESET_COLOR;
    }
}

void manage_expenses_menu()
{
    int choice;
    do
    {
        CLEAR_SCREEN;
        showRainbowBanner();
        SET_LIGHT_BLUE;
        printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Expense Management '.<-|.<-|.<-|.<\n\n\n");
        RESET_COLOR;
        SET_YELLOW;
        printf("\t\t\t\t\t\t1. Add Expense\n");
        printf("\t\t\t\t\t\t2. View Expenses\n");
        printf("\t\t\t\t\t\t3. Edit Expense\n");
        printf("\t\t\t\t\t\t4. Remove Expense\n");
        printf("\t\t\t\t\t\t5. Back to Admin Menu\n");
        RESET_COLOR;
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            add_expense();
            break;
        case 2:
            view_expenses();
            break;
        case 3:
            edit_expense();
            break;
        case 4:
            remove_expense();
            break;
        case 5:
            break;
        default:
            SET_RED;
            printf("\n\n\t\t\t\tInvalid choice. Please try again..\n");
            RESET_COLOR;
        }
        if (choice != 5)
        {
            SET_YELLOW;
            printf("\n\t\t\t\tPress Enter to continue...");
            RESET_COLOR;
            while (getchar() != '\n');
            getchar();
        }
    }
    while (choice != 5);
}

void add_expense()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    if (expense_count >= MAX_EXPENSES)
    {
        SET_RED;
        printf("\t\t\t\t\tMaximum expense capacity reached.\n");
        RESET_COLOR;
        return;
    }

    struct Expense new_expense;

    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' Add New Expense '.<-|.<-|.<-|.<\n\n\n");
    RESET_COLOR;

    do
    {
        SET_BROWN;
        printf("\n\t\t\t\t\tExpense date (YYYY-MM-DD): ");
        RESET_COLOR;
        scanf("%s", new_expense.date);
    }
    while (!validate_date(new_expense.date));

    while (getchar() != '\n');
    SET_PINK;
    printf("\n\t\t\t\t\tDescription: ");
    RESET_COLOR;
    scanf(" %[^\n]s", new_expense.description);

    SET_PINK;
    printf("\n\t\t\t\t\tAmount: ");
    RESET_COLOR;
    scanf("%f", &new_expense.amount);

    int category_choice;
    SET_PINK;
    printf("\n\t\t\t\tExpense Category:\n");
    RESET_COLOR;
    SET_YELLOW;
    printf("\t\t\t\t\t\t1. Maintenance\n");
    printf("\t\t\t\t\t\t2. Service\n");
    printf("\t\t\t\t\t\t3. Admin\n");
    printf("\t\t\t\t\t\t4. Other\n");
    RESET_COLOR;
    printf("\n\t\t\t\t\tEnter choice: ");
    scanf("%d", &category_choice);

    switch(category_choice)
    {
    case 1:
        strcpy(new_expense.category, "Maintenance");
        break;
    case 2:
        strcpy(new_expense.category, "Service");
        break;
    case 3:
        strcpy(new_expense.category, "Admin");
        break;
    default:
        strcpy(new_expense.category, "Other");
    }

    new_expense.id = expense_count + 1;
    expenses[expense_count++] = new_expense;

    printf("\n\t\t\t\t\t\tExpense added successfully with ID %d.\n", new_expense.id);
}

void view_expenses()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    SET_LIGHT_BLUE;
    printf("\n\t\t\t\t\t\t\t>.|->.|->.|->.' All Expenses '.<-|.<-|.<-|.<\n\n\n");
    printf("\n\t\t\t\t\t  ID           Date            Category           Amount             Description\n");
    RESET_COLOR;
    printf("\n\t\t\ts\t\t------      -----------      ------------       -----------         --------------\n");

    for (int i = 0; i < expense_count; i++)
    {
        SET_BROWN;
        printf("\n\t\t\t\t    %-3d          %-10s             %-15s            $%-8.2f              %s\n",
               expenses[i].id, expenses[i].date,
               expenses[i].category, expenses[i].amount,
               expenses[i].description);
        RESET_COLOR;
    }

    if (expense_count == 0)
    {
        SET_RED;
        printf("\n\t\t\t\t\tNo expenses recorded.\n");
        RESET_COLOR;
    }

    float total = 0;
    for (int i = 0; i < expense_count; i++)
    {
        total += expenses[i].amount;
    }
    SET_BROWN;
    printf("\n\t\t\t\tTotal Expenses: $%.2f\n", total);
    RESET_COLOR;
}

void edit_expense()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    int id;
    SET_PINK;
    printf("\n\n\t\t\tEnter Expense ID to edit: ");
    RESET_COLOR;
    scanf("%d", &id);

    int found = -1;
    for (int i = 0; i < expense_count; i++)
    {
        if (expenses[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\t\t\t\t\tExpense not found.\n");
        RESET_COLOR;
        return;
    }

    SET_PINK;
    printf("\n\t\t\t\tCurrent Details:\n");
    RESET_COLOR;
    SET_BROWN;
    printf("\n\t\t\t\t\tDate: %s\n", expenses[found].date);
    printf("\t\t\t\t\tDescription: %s\n", expenses[found].description);
    printf("\t\t\t\t\tAmount: %.2f\n", expenses[found].amount);
    printf("\t\t\t\t\tCategory: %s\n", expenses[found].category);
    RESET_COLOR;
    SET_PINK;
    printf("\n\t\t\t\t\tEnter new details (press Enter to keep current):\n");
    RESET_COLOR;

    char line[100];
    while (getchar() != '\n');

    SET_PINK;
    printf("\n\t\t\t\t\tDate (YYYY-MM-DD): ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        line[strcspn(line, "\n")] = '\0';
        while (!validate_date(line))
        {
            SET_RED;
            printf("\n\t\t\t\t\tInvalid date format. Use YYYY-MM-DD: ");
            fgets(line, sizeof(line), stdin);
            line[strcspn(line, "\n")] = '\0';
        }
        strcpy(expenses[found].date, line);
    }

    SET_BROWN;
    printf("\n\t\t\t\t\tDescription: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(expenses[found].description, line);
    }

    printf("\n\t\t\t\t\tAmount: ");
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        expenses[found].amount = atof(line);
    }
    RESET_COLOR;
    SET_PINK;
    printf("\n\t\t\t\t\tCategory (1=Maintenance, 2=Service, 3=Admin, 4=Other): ");
    RESET_COLOR;
    SET_YELLOW;
    printf("(\t\t\t\t1=Maintenance, 2=Service, 3=Admin, 4=Other)\n");
    RESET_COLOR;
    if (fgets(line, sizeof(line), stdin) && strlen(line) > 1)
    {
        int choice = atoi(line);
        switch(choice)
        {
        case 1:
            strcpy(expenses[found].category, "Maintenance");
            break;
        case 2:
            strcpy(expenses[found].category, "Service");
            break;
        case 3:
            strcpy(expenses[found].category, "Admin");
            break;
        default:
            strcpy(expenses[found].category, "Other");
        }
    }

    SET_YELLOW;
    printf("\n\t\t\t\t\tExpense updated successfully.\n");
    RESET_COLOR;
}

void remove_expense()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    int id;
    printf("\n\t\t\t\t\tEnter Expense ID to remove: ");
    scanf("%d", &id);

    int found = -1;
    for (int i = 0; i < expense_count; i++)
    {
        if (expenses[i].id == id)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\n\t\t\t\t\t\t\tExpense not found.\n");
        RESET_COLOR;
        return;
    }

    for (int i = found; i < expense_count - 1; i++)
    {
        expenses[i] = expenses[i + 1];
    }

    expense_count--;
    SET_YELLOW;
    printf("\n\t\t\t\t\t\t\t\tExpense removed successfully.\n");
    RESET_COLOR;
}

void get_current_date(char *current_date)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(current_date, 11, "%Y-%m-%d", tm_info);
}

void process_vehicle_return()
{
    CLEAR_SCREEN;
    showRainbowBanner();
    view_all_bookings();

    if (booking_count == 0)
    {
        SET_RED;
        printf("\n\t\t\t\t\t\t\tNo bookings available to process return.\n");
        RESET_COLOR;
        return;
    }

    int booking_id;
    SET_YELLOW;
    printf("\n\t\t\t\t\t\tEnter Booking ID to process return ");
    printf("\n\t\t\t\t\t\t    (or 0 to go back): ");
    RESET_COLOR;
    scanf("%d", &booking_id);

    if (booking_id == 0)
    {
        SET_BROWN;
        printf("\t\t\t\t\t\tReturning to admin menu...\n");
        RESET_COLOR;
        return;
    }

    int found = -1;
    int vehicle_id = -1;
    int days = 0;

    for (int i = 0; i < booking_count; i++)
    {
        if (bookings[i].booking_id == booking_id)
        {
            found = i;
            vehicle_id = bookings[i].vehicle_id;
            days = date_diff(bookings[i].start_date, bookings[i].end_date);
            break;
        }
    }

    if (found == -1)
    {
        SET_RED;
        printf("\t\t\t\t\t\tBooking not found!\n");
        RESET_COLOR;
        return;
    }

    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicles[i].id == vehicle_id)
        {
            float service_charge = vehicles[i].service_rate * days;
            float maintenance_cost = vehicles[i].daily_rate * 0.5 * vehicles[i].maintenance_rate;

            char damage_choice;
            SET_PINK;
            printf("\n\t\t\t\tIs there any damage to the vehicle? (Y/N): ");
            RESET_COLOR;
            scanf(" %c", &damage_choice);
            if (toupper(damage_choice) == 'Y')
            {
                float base_damage = calculate_damage_cost(vehicle_id);
                printf("\nSelect damage type:\n");
                SET_YELLOW;
                printf("\t\t\t\t\t\t1. Tire damage ($%.2f)\n", base_damage * 0.3);
                printf("\t\t\t\t\t\t2. Engine damage ($%.2f)\n", base_damage * 0.5);
                printf("\t\t\t\t\t\t3. Windshield damage ($%.2f)\n", base_damage * 0.2);
                printf("\t\t\t\t\t\t4. Other damage ($%.2f)\n", base_damage);
                RESET_COLOR;
                SET_PINK;
                printf("\n\t\t\t\t\t\tEnter choice: ");
                RESET_COLOR;
                int damage_type;
                scanf("%d", &damage_type);
                switch(damage_type)
                {
                case 1:
                    bookings[found].damage_cost = base_damage * 0.3;
                    break;
                case 2:
                    bookings[found].damage_cost = base_damage * 0.5;
                    break;
                case 3:
                    bookings[found].damage_cost = base_damage * 0.2;
                    break;
                case 4:
                    bookings[found].damage_cost = base_damage;
                    break;
                default:
                    bookings[found].damage_cost = 0;
                }
            }
            else
            {
                bookings[found].damage_cost = 0;
            }

            bookings[found].total_cost += service_charge + maintenance_cost + bookings[found].damage_cost;

            SET_LIGHT_BLUE;
            printf("\n\t\t\t\t\t>.|->.|->.|->.' Return Cost Calculation '.<-|.<-|.<-|.<\n\n\n");
            RESET_COLOR;
            SET_BROWN;
            printf("\t\t\t\t\tService Charge (%d days @ $%.2f/day): $%.2f\n",
                   days, vehicles[i].service_rate, service_charge);
            printf("\t\t\t\t\tMaintenance Cost: $%.2f\n", maintenance_cost);
            printf("\t\t\t\t\tDamage Cost: $%.2f\n", bookings[found].damage_cost);
            printf("\t\t\t\t\tTotal Additional Charges: $%.2f\n", service_charge + maintenance_cost + bookings[found].damage_cost);
            printf("\t\t\t\t\tUpdated Total Cost: $%.2f\n", bookings[found].total_cost);
            RESET_COLOR;
            vehicles[i].available = 1;

            for (int j = found; j < booking_count - 1; j++)
            {
                bookings[j] = bookings[j + 1];
            }
            booking_count--;

            SET_YELLOW;
            printf("\n\t\t\t\t\t\t\tVehicle returned successfully.\n");
            RESET_COLOR;
            break;
        }
    }
}

int main()
{
    srand(time(NULL));
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    load_data();

    if (vehicle_count == 0)
    {
        initialize_sample_data();
    }

    main_menu();
    save_data();
    return 0;
}
