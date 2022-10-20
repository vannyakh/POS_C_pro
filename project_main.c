#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include <windows.h>

int n, i, j, k, l;
struct product
{
	char pd_name[20];
	int pd_qty;
	int pd_code;
	float price;
	float amount;
};

struct costumer
{
};
struct employee
{
	char username[20], pass[20];
};
struct addmin
{
	struct employee employee;
};

void gotoxy(short x, short y); // here we declare the gotoxy function//
void admin();
void costomer();
void add_costomer();
void login();
void add_product();
void view_product();
void search_product();
void update_product();
void delete_product();
void add_customer();
void lis_costumer();
void update_costumer();
void seach_customer();
void delete_customer();
void logout();
void menu_costumer();
void menu_stoke_costumer();
void menu_admmin();

int main()
{
	char ch;
	costomer();
	menu_stoke_costumer();
	puts("\n____________________");
	getch();
	return 0;
}

void admin()
{
	int x = 30;
	int ch;
	int Customers = 0, Total_Products = 0;
	float Total_Amount = 0;
ag:
	system("Cls");
	gotoxy(x, 3);
	puts("================================\"[Admin]\"===================================");
	gotoxy(x, 4);
	puts("----------------------------------------------------------------------------");
	gotoxy(x, 5);
	puts("|                                                                          |");
	gotoxy(x, 6);
	puts("|                                                                          |");
	gotoxy(x, 7);
	puts("|                                                                          |");
	gotoxy(x, 8);
	puts("|                                                                          |");
	gotoxy(x, 9);
	puts("|                                                                          |");
	gotoxy(x, 10);
	puts("|                                                                          |");
	gotoxy(x, 11);
	puts("|                                                                          |");
	gotoxy(x, 12);
	puts("|                                                                          |");
	gotoxy(x, 13);
	puts("|                                                                          |");
	gotoxy(x, 14);
	puts("|                                                                          |");
	gotoxy(x, 15);
	puts("----------------------------------------------------------------------------");
	gotoxy(44, 8);
	printf("( %d )", Customers);
	gotoxy(40, 10);
	printf("Customers[1]");
	gotoxy(64, 8);
	printf("( %d )", Total_Products);
	gotoxy(60, 10);
	printf("Total Products[2]");
	gotoxy(85, 8);
	printf("( %.2f )", Total_Amount);
	gotoxy(85, 10);
	printf("Total Amount[3]");

	gotoxy(x, 17);
	printf("Option Control [1],[2],[3]: ");
	scanf("%d", &ch);

	switch (ch)
	{
	case 1:
		system("Cls");
		printf("Customers");
		getch();
		goto ag;
	case 2:
		system("Cls");
		printf("Total Products");
		getch();
		goto ag;
		break;
	case 3:
		system("Cls");
		printf("Total Amount");
		getch();
		goto ag;
		break;
	case 0:

		getch();
		exit(0);
		break;
	default:
		system("Cls");
		printf("Not Fount");
		break;
	}
}
void costomer()
{
	menu_costumer();
}
void menu_costumer()
{
	int choice;
	while (choice != 0)
	{

		printf("\t\t\t=====POS SYSTEM=====");
		printf("\n\n\n\t\t\t\t     1. Stoke\n");
		printf("\t\t\t\t     2. Sell\n");
		printf("\t\t\t\t     0. Exit\n");
		printf("\t\t\t\t    _____________________\n");
		printf("\t\t\t\t     ");
		scanf("\t\t\t\t    %d", &choice);

		switch (choice)
		{
		case 1:
			system("cls");
			menu_stoke_costumer();
			system("cls");
			break;
		case 2:
			system("cls");

			printf("\t\t\t\t  press any key to exit..... \n");
			getch();
			system("cls");

			break;
		case 0:
			system("cls");
			;
			printf("\n\t\t\t\tThank you, for used this software.\n\n");
			exit(0);
			break;

		default:
			system("cls");
			getch();
			printf("\n\t\t\t\t\tEnter a valid number\n\n");
			printf("\t\t\t\tPress any key to continue.......");
			getch();
			system("cls");
			break;
		}
	}

	getch();
}
void menu_stoke_costumer()
{
	int choice;
	do
	{

		printf("\t\t\t=====POS SYSTEM=====");
		printf("\n\n\n\t\t\t\t     1. add product\n");
		printf("\t\t\t\t     2. View All product\n");
		printf("\t\t\t\t     3. Update product\n");
		printf("\t\t\t\t     4. Delete product\n");
		printf("\t\t\t\t     99. back\n");

		printf("\t\t\t\t     0. Exit\n");
		printf("\t\t\t\t    _____________________\n");
		printf("\t\t\t\t     ");
		scanf("\t\t\t\t    %d", &choice);

		switch (choice)
		{
		case 1:
			system("cls");
			add_product();
			system("cls");
			break;
		case 2:
			system("cls");
			view_product();
			printf("\t\t\t\t  press any key to exit..... \n");
			getch();
			system("cls");
			break;
		case 3:
			system("cls");
			update_product();
			printf("\t\t\t\t  press any key to exit..... \n");
			getch();
			system("cls");
			break;
		case 4:
			system("cls");
			delete_product();
			printf("\t\t\t\t  press any key to exit..... \n");
			getch();
			system("cls");
			break;
		case 99:
			system("cls");

			printf("\t\t\t\t  press any key to exit..... \n");
			getch();
			system("cls");
			break;
		case 0:
			system("cls");
			;
			printf("\n\t\t\t\tThank you, for used this software.\n\n");
			exit(0);
			break;

		default:
			system("cls");
			getch();
			printf("\n\t\t\t\t\tEnter a valid number\n\n");
			printf("\t\t\t\tPress any key to continue.......");
			getch();
			system("cls");
			break;
		}
	} while (choice != 0);

	getch();
}
void add_costomer()
{
	struct addmin info;
	FILE *Fpl;
	Fpl = fopen("information_login.txt", "ab+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	printf("Enter User Name: ");
	gets(info.employee.username);
	printf("Enter Password : ");
	scanf("%s", &info.employee.pass);
	fwrite(&info, sizeof(info), 1, Fpl);
	fclose(Fpl);
}
void lis_costumer()
{

	struct addmin info;

	FILE *Fpl;
	int i = 1;
	Fpl = fopen("information_login.txt", "rb+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&info, sizeof(info), 1, Fpl) == 1)
	{

		printf("\nRoll: %d\n", i);
		printf("User Name: %s\n", info.employee.username);
		printf("User Password: %s\n", info.employee.pass);

		printf("Roll: %d", i);
		printf("User Name: %s\n", info.employee.username);
		printf("User Password: %s\n", info.employee.pass);

		i++;
	}
	fclose(Fpl);
}
void seach_customer()
{
	struct employee info;
	FILE *Fpl;
	char seach_name[50];
	printf("Search Name Costumer : ");
	fflush(stdin);
	gets(seach_name);
	Fpl = fopen("information_login.txt", "rb+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&info, sizeof(info), 1, Fpl) == 1)
	{
		if (strcmp(info.username, seach_name) == 0)
		{
			printf("User Name: %s\n", info.username);
			printf("User Password: %s\n", info.pass);
		}
	}
	fclose(Fpl);
}
void update_costumer()
{
	struct employee info;
	FILE *Fpl;
	char seach_name[50];
	printf("Search Name Costumer : ");
	fflush(stdin);
	gets(seach_name);
	Fpl = fopen("information_login.txt", "rb+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&info, sizeof(info), 1, Fpl) == 1)
	{
		if (strcmp(info.username, seach_name) == 0)
		{
			printf("User Name: %s\n", info.username);
			printf("User Password: %s\n", info.pass);
			puts("\n_______________");
			printf("User Name: ");
			gets(info.username);
			printf("User Password: ");
			scanf("%s", &info.pass);

			fseek(Fpl, sizeof(info), SEEK_CUR);
			fwrite(&info, sizeof(info), 1, Fpl);
			break;
		}
	}
	fclose(Fpl);
}
void delete_customer()
{
	struct employee info;
	FILE *Fpl, *Fpl1;
	char seach_name[50];
	int found = 0;
	printf("Search Name Costumer : ");
	fflush(stdin);
	gets(seach_name);
	Fpl = fopen("information_login.txt", "rb+");
	Fpl1 = fopen("temp.txt", "wb+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&info, sizeof(info), 1, Fpl) == 1)
	{
		if (strcmp(info.username, seach_name) == 0)
		{

			found = 1;
		}
		else
		{
			fwrite(&info, sizeof(info), 1, Fpl1);
		}
	}
	fclose(Fpl);
	fclose(Fpl1);

	if (!found)
	{
		printf("\n\t\t\t\tRecord not found\n");
	}
	if (found)
	{
		remove("information_login.txt");
		rename("temp.txt", "information_login.txt");

		printf("\n\t\t\t\tRecord deleted succesfully\n");
	}
}

void login()
{
	FILE *Fpl;
	struct employee info;
	printf("Enter User Name: ");
	gets(info.username);
	printf("Enter Password : ");
	scanf("%s", &info.pass);
	Fpl = fopen("information_login.txt", "rb+");
	if (Fpl == NULL)
	{
		printf("Error!");
		exit(1);
	}
	if (strcmp(info.username, "admin") == 0 && strcmp(info.pass, "admin") == 0)
	{
		fclose(Fpl);
		admin();
	}
	else if (strcmp(info.username, "user") == 0 && strcmp(info.pass, "user") == 0)
	{
		fclose(Fpl);
		costomer();
	}
	else
	{
		fclose(Fpl);
		printf("Login Fail");
	}
}

void add_product()
{
	struct product pd;
	char ch;
	FILE *fp;
	fp = fopen("product_file.txt", "ab+");
	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}
	do
	{
		printf("Product Code  : ");
		scanf("%d", &pd.pd_code);
		printf("Product Name  : ");
		fflush(stdin);
		scanf("%s", &pd.pd_name);
		printf("Product QTY   : ");
		scanf("%d", &pd.pd_qty);
		printf("Product Price : ");
		scanf("%f", &pd.price);
		pd.amount = pd.price * pd.pd_qty;
		fwrite(&pd, sizeof(pd), 1, fp);
	} while (ch == 'y' || ch == 'Y');
	fclose(fp);
}
void view_product()
{
	struct product pd;
	FILE *fp;
	fp = fopen("product_file.txt", "rb+");
	printf("PD-code\t\tPD-name\t\tPD-QTY\t\tPD-Price\t\tPD-Amoun\n\n");
	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&pd, sizeof(pd), 1, fp) == 1)
	{
		/* code */
		printf("%d\t\t%s\t\t%d\t\t%.2f\t\t\t%.3f\n", pd.pd_code, pd.pd_name, pd.pd_qty, pd.price, pd.amount);
	}
	fclose(fp);
}
void search_product()
{
	struct product pd;
	FILE *fp;
	char name_product[30];
	fp = fopen("product_file.txt", "rb+");
	printf("Search product name: ");
	fflush(stdin);
	gets(name_product);
	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}
	printf("PD-code\t\tPD-name\t\tPD-QTY\t\tPD-Price\t\tPD-Amoun\n\n");
	while (fread(&pd, sizeof(pd), 1, fp) == 1)
	{
		if (strcmp(pd.pd_name, name_product) == 0)
		{
			printf("%d\t\t%s\t\t%d\t\t%.2f\t\t\t%.3f\n", pd.pd_code, pd.pd_name, pd.pd_qty, pd.price, pd.amount);
		}
	}
	fclose(fp);
}
void update_product()
{
	struct product pd;
	FILE *fp;
	char name_product[30];
	fp = fopen("product_file.txt", "ab+");
	printf("Search product name: ");
	fflush(stdin);
	gets(name_product);
	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}
	printf("PD-code\t\tPD-name\t\tPD-QTY\t\tPD-Price\t\tPD-Amoun\n\n");
	while (fread(&pd, sizeof(pd), 1, fp) == 1)
	{
		if (strcmp(pd.pd_name, name_product) == 0)
		{
			printf("%d\t\t%s\t\t%d\t\t%.2f\t\t\t%.3f\n", pd.pd_code, pd.pd_name, pd.pd_qty, pd.price, pd.amount);
			puts("\n\n________________Update_____________________");
			printf("Product Code  : ");
			scanf("%d", &pd.pd_code);
			printf("Product Name  : ");
			fflush(stdin);
			scanf("%s", &pd.pd_name);
			printf("Product QTY   : ");
			scanf("%d", &pd.pd_qty);
			printf("Product Price : ");
			scanf("%f", &pd.price);
			pd.amount = pd.price * pd.pd_qty;
			fseek(fp, sizeof(pd), SEEK_CUR);
			fwrite(&pd, sizeof(pd), 1, fp);
			fclose(fp);
		}
	}
}
void delete_product()
{
	struct product pd;
	FILE *fp, *fp1;
	char seach_name[50];
	int found = 0;
	printf("Search Name Costumer : ");
	fflush(stdin);
	gets(seach_name);
	fp = fopen("product_file.txt", "rb+");
	fp1 = fopen("temp.txt", "wb+");
	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}
	while (fread(&pd, sizeof(pd), 1, fp) == 1)
	{
		if (strcmp(pd.pd_name, seach_name) == 0)
		{
			found = 1;
		}
		else
		{
			fwrite(&pd, sizeof(pd), 1, fp1);
		}
	}
	fclose(fp);
	fclose(fp1);

	if (!found)
	{
		printf("\n\t\t\t\tRecord not found\n");
	}
	if (found)
	{
		remove("product_file.txt");
		rename("temp.txt", "product_file.txt");

		printf("\n\t\t\t\tRecord deleted succesfully\n");
	}
}

void gotoxy(short x, short y)
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
