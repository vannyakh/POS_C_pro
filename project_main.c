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
void update_product();
void delete_product();
void view_product();
void add_customer();
void lis_costumer();
void update_costumer();
void seach_customer();

int main()
{
	char ch;
	 
	puts("\nView Pro");
	lis_costumer();
	puts("\nSearch Pro");
	seach_customer();
	puts("\nUpdate Pro");
	update_costumer();
	puts("\nView Pro");
	lis_costumer();


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
	printf("Costumer");



}
void add_costomer()
{
	struct addmin info;
	FILE *Fpl;
	Fpl = fopen("information_login.txt", "ab+");
	if(Fpl == NULL)
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
void lis_costumer(){
	struct addmin info;
	FILE *Fpl;
	int i=1;
	Fpl = fopen("information_login.txt", "rb+");
	if(Fpl == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	while (fread(&info,sizeof(info),1,Fpl)==1)
	{	
		printf("Roll: %d\n",i);
		printf("User Name: %s\n",info.employee.username);
		printf("User Password: %s\n",info.employee.pass);
		i++;
	}
	fclose(Fpl);

}
void seach_customer(){
	struct employee info;
	FILE *Fpl;
	char seach_name[50];
	printf("Search Name Costumer : "); gets(seach_name);
	Fpl = fopen("information_login.txt", "rb+");
	if(Fpl == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	while (fread(&info,sizeof(info),1,Fpl)==1)
	{	
		if (strcmp(info.username,seach_name)==0)
		{
			printf("User Name: %s\n",info.username);
			printf("User Password: %s\n",info.pass);
		}
	}
	fclose(Fpl);
}
void update_costumer(){
	struct employee info;
	FILE *Fpl;
	char seach_name[50];
	printf("Search Name Costumer : "); gets(seach_name);
	Fpl = fopen("information_login.txt", "rb+");
	if(Fpl == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	while (fread(&info,sizeof(info),1,Fpl)==1)
	{	
		if (strcmp(info.username,seach_name)==0)
		{
			printf("User Name: %s\n",info.username);
			printf("User Password: %s\n",info.pass);
			puts("\n_______________");
			printf("User Name: "); gets(info.username);
			printf("User Password: "); scanf("%s",&info.pass);
			fseek(Fpl ,sizeof(info),SEEK_CUR);
            fwrite(&info,sizeof(info),1,Fpl);
		}
	}
	fclose(Fpl);
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
	if(Fpl == NULL)
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

void add_product(){
	struct product pd;
	char ch;
	FILE *fp;
	fp=fopen("product_file.txt","ab+");
	if(fp == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	do
	{
	printf("Product Code  : ");scanf("%d",&pd.pd_code);
	printf("Product Name  : "); fflush(stdin); scanf("%s",&pd.pd_name);
	printf("Product QTY   : "); scanf("%d",&pd.pd_qty);
	printf("Product Price : "); scanf("%f",&pd.price);
	pd.amount = pd.price*pd.pd_qty;
	fwrite(&pd,sizeof(pd),1,fp);
	} while (ch=='y' || ch=='Y');
	fclose(fp);
}
void view_product(){
	struct product pd;
	FILE *fp;
	fp=fopen("product_file.txt","rb+");
	printf("PD-code\t\tPD-name\t\tPD-QTY\t\tPD-Price\t\tPD-Amoun\n\n");
	if(fp == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	while (fread(&pd,sizeof(pd),1,fp)==1)
	{
		/* code */
		printf("%d\t\t%s\t\t%d\t\t%.2f\t\t\t%.3f\n",pd.pd_code,pd.pd_name,pd.pd_qty,pd.price,pd.amount);
	}
	fclose(fp);
}

void gotoxy(short x, short y) 
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

