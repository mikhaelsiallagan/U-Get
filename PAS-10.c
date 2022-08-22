#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

//Proyek Akhir Semester 
//Kelompok 10
//Bisma Alif Alghifari_2106731402
//Mikhael Morris H. Siallagan_2106731491


//Deklarasi struct Item
typedef struct item{
    char barang[30];
    int hargaasli;
    char kodeitem[5];
} Item;

struct bidder{
    char nama[20];
    char kodeitem[5];
    int bid;
};

struct nodeitem {
    struct item item;
    struct nodeitem * next;
};

struct nodebidder {
    struct bidder bidder;
    struct nodebidder * next;
};

//function countjmlitem
void countjmlitem( int *jumlah){
    FILE *fptr;
    char tempc;
    
    fptr = fopen("dataLelang.txt", "r");
    
    if (fptr == NULL){
        printf("Error!");
        return;
    }
    
    //menghitung jumlah baris
    while( (tempc = fgetc(fptr)) != EOF){
        if (tempc == '\n')
            (*jumlah)++;
    }
  *jumlah = *jumlah/2;
    fclose(fptr);
}

//function printtofile
void printtofile (struct item barang){
	
  	FILE * fptr;
 	fptr = fopen("dataLelang.txt", "a");
 	
 	if (fptr == NULL){
    	printf("Error!");
	} else {	
		fprintf(fptr, "%s\n", barang.barang);
    fprintf(fptr, "%s %d\n", barang.kodeitem, barang.hargaasli);
	}
	
	fclose(fptr);
}

//function additem
void additem(struct nodeitem** head, struct item barang, int *jumlahitem, int donereadfile){
    struct nodeitem *ptr, *temp;
    ptr = *head;
    temp = malloc(sizeof(struct nodeitem));
    
    strcpy(temp->item.barang, barang.barang);
    strcpy(temp->item.kodeitem, barang.kodeitem);
    temp->item.hargaasli = barang.hargaasli;
    temp->next = NULL;
    
    if (ptr == NULL){
      *head = temp;
    
    } else {
      while (ptr->next != NULL){
      ptr = ptr->next;
      }
      ptr->next = temp;
  }
  if (donereadfile == 1){
  		(*jumlahitem)++;
  		printtofile(barang);
    }
}

//function transferdata
void transferdata(struct nodeitem ** headitem, int jumlah, int donereadfile){
    FILE* fptr;
    int i;
   
    struct item temp;
    fptr = fopen("dataLelang.txt", "r");
    if (fptr == NULL){
        printf("Error!");
        return;
    }
    #pragma omp parallel 
	{
    	#pragma omp for shared(fptr,temp,headitem)
    	for (i = 0; i < jumlah; i++){
        	if (!feof(fptr) && !ferror(fptr)){
           	 fscanf(fptr, " %[^\n]s", temp.barang);
           	 fscanf(fptr, "%s %d", temp.kodeitem, &temp.hargaasli);
          	  additem(headitem, temp, &jumlah, donereadfile);
        	}
   		 }
	}
    fclose(fptr);
}

//function addbidder
void addbidder(struct nodebidder ** head, struct bidder bidder, int *jumlahbidder){
    struct nodebidder *ptr, *temp;
    temp = malloc(sizeof(struct nodebidder));
    ptr = *head;

    strcpy(temp->bidder.nama, bidder.nama);
    strcpy(temp->bidder.kodeitem, bidder.kodeitem);
    temp->bidder.bid = bidder.bid;
    temp->next = NULL;

  if (ptr == NULL){
    *head = temp;
    
  } else {
     while (ptr->next != NULL){
      ptr = ptr->next;
    }
        ptr->next = temp;
  }
    (*jumlahbidder)++;
}

//function addcatalogue
void showcatalogue (struct nodeitem* headitem, struct nodebidder * headbidder, int jumlahitem){
    int i;
    struct nodeitem* tempitem = headitem;
    printf("\n\n---------- LIST BARANG LELANG ----------");
    printf("\n%-3s | %-5s | %-15s  ", "No.", "Kode", "Barang");
    printf("\n--------------------------------------------");

  i = 0;
    while (tempitem != NULL){
        printf("\n%-3d | %-5s | %-30s", i + 1 , tempitem->item.kodeitem, tempitem->item.barang);
        i++;
        tempitem = tempitem->next;
    }
}

//function showbidder
void showbidder (struct nodeitem* headitem, char kodeitem[5], int hargaasli, struct nodebidder* headbidder, int jumlahbidder){
    struct nodebidder* tempbidder = headbidder;
  
    printf("\n\n-----------------------------------------------\n\n");
    printf("Memperlihatkan lelang untuk kode item %s dengan harga awal %d\n",  kodeitem, hargaasli );
    printf("\n %-15s | %-10s", "Nama" ,"Bid");
    printf("\n--------------------------------------------");

    if (jumlahbidder == 0){
        printf("\nBelum ada bidder lainnya!");
    } else {
        while (tempbidder != NULL){
            if (strcmp( tempbidder->bidder.kodeitem, kodeitem) == 0){
                printf("\n %-15s | %-10d", tempbidder->bidder.nama , tempbidder->bidder.bid);
            }
            tempbidder = tempbidder->next;
        }
    }
}

//function findhighest
int findhighest (struct nodeitem* headitem, char kodeitem[5], struct nodebidder* headbidder, int jumlahbidder){
    struct nodebidder* tempbidder = headbidder;
    int highest = 0;
    
    #pragma omp parallel shared (jumlahbidder) 
	{
    	 if (jumlahbidder == 0){
      		return 0;
  		} else {
    		#pragma omp single private(highest) 
			{
    			while (tempbidder != NULL){
           	 		if (strcmp( tempbidder->bidder.kodeitem, kodeitem) == 0){
            			if (tempbidder->bidder.bid >= highest){
               		 		highest = tempbidder->bidder.bid;
               		 	}
          	 		}
        			tempbidder = tempbidder->next;
     			}	
			}
     	 return highest;
		}
	}   	
}

//Main function
int main(){
	//Deklarasi variabel
    int pilihan;
    int jumlahitem = 0;
    int jumlahbidder = 0;
    int firsttime = 0;
    int totalvalue;
    int totaluntung;
    char password[20];
    char passattempt[20];
    char delay[20];
    int donereadfile = 0;
    int clearweek = 0;
  	//Inisialisasi linked list
    struct nodeitem* headitem = NULL;
    struct nodebidder* headbidder = NULL;
    struct item barangtemp;
    countjmlitem( &jumlahitem); //memanggil function countjmlitem
    transferdata(&headitem, jumlahitem, donereadfile); //memanggil function transferdata
    donereadfile = 1;
	//Main menu
    while (1 > 0){
        printf("\n\n~~~~ Welcome to U-Get! ~~~~");
        printf("\n\nAnda disini sebagai:");
        printf("\n  1) Submit barang");
        printf("\n  2) Bidder");
        printf("\n  3) Help / FAQ");
        printf("\n  4) Administrator");
        printf("\n  5) Exit");
        printf("\n>>> ");
        scanf("%d", &pilihan);
        //Input barang
        if (pilihan == 1){
            Item barangtemp;
            printf("\n~~~~ Input Barang ~~~~");
            printf("\n\nMasukkan nama barang: ");
            scanf(" %[^\n]s", &barangtemp.barang);
            printf("Masukkan kode untuk barang (disarankan 3 huruf, all caps, ex: MBR): ");
            scanf(" %[^\n]s", barangtemp.kodeitem);
            printf("Masukkan value barang: ");
            scanf("%d", &barangtemp.hargaasli);
            //memanggil function additem, untuk menambah item
            additem(&headitem, barangtemp, &jumlahitem,donereadfile);
            
            
        } else if (pilihan == 2){
            int adakodeitem;
            int hargaasli;
            struct nodeitem * checker;
          
            struct bidder biddertemp;
          
            printf("\n\nMasukkan nama anda: ");
            scanf(" %[^\n]s", biddertemp.nama);
            showcatalogue(headitem, headbidder, jumlahitem); //memanggil function showcatalogue, untuk mrnsmpilksn data barang yang telah diinput
          
            do {
                checker = headitem;
                adakodeitem = 0;
                hargaasli = 0;
                printf("\n\nMasukkan kode barang yang diinginkan: ");
                scanf(" %[^\n]s", biddertemp.kodeitem);
            
                #pragma omp parallel for scheduled (static)
                while (checker != NULL){
                    if (strcmp(checker->item.kodeitem, biddertemp.kodeitem) == 0){
                        adakodeitem = 1;
                        hargaasli = checker->item.hargaasli;
                    }
                    checker = checker->next;
                }
                if (adakodeitem != 1)
                    printf("\nKode yang anda masukkan invalid silakan mengulangi\n");
            
            } while (adakodeitem != 1);
          
            showbidder(headitem, biddertemp.kodeitem, hargaasli, headbidder, jumlahbidder);
            printf("\n\nMasukkan bid anda: ");
            scanf("%d", &biddertemp.bid);
            
            addbidder(&headbidder, biddertemp, &jumlahbidder);
        } else if (pilihan == 3){ //pilihan 3, berisi pedoman
            printf("\n\n~~~~~~~~ Help ~~~~~~~~");
            printf("\nSebagai user:");
            printf("\n- Pilih 1 untuk menambahkan item untuk dilelang");
            printf("\n- Pilih 2 untuk mengikuti lelang sebagai peserta dan memberikan bid terbesar untuk barang yang tersedia");
            printf("\n\nSebagai administrator");
            printf("\n- Akan diperlihatkan jumlah uang yang digunakan untuk membeli barang yang dilelang");
            printf("\n- Akan menunjukkan jumlah uang hasil penjualan barang berdasarkan bid tertinggi");
            printf("\n- Akan menunjukkan total keuntungan dengan mengurangi hasil penjualan dengan uang untuk membeli");
            printf("\n- Terdapat pilihan untuk mengakhiri pekan dan mereset semua data barang yang telah tersimpan");
            printf("\n\nType any character to continue ... ");
            scanf("%s", delay);
          
          
        } else if (pilihan == 4) { //pilihan 4
          printf("\n\n------ Administrator -----");
          if (firsttime == 0){
            printf("\n Welcome! Silakan memasukkan password untuk keperluan selanjutnya (max 20 char): ");
            scanf("%s", password);
            firsttime = 1;
          } else {
            printf("\nWelcome! silakan masukkan password anda: ");
            scanf("%s", passattempt);
            
            if (strcmp(password, passattempt) == 0){
              printf("\nSelamat datang, Admin");
              totalvalue = 0;
              totaluntung= 0;
              
               struct nodeitem * tempitem = headitem;
              #pragma omp parallel
                {
               #pragma omp task (shared: totalvalue, tempitem) reduction (+: totalvalue) 
                    while (tempitem != NULL){
                        totalvalue += tempitem->item.hargaasli;
                        tempitem = tempitem->next;
                    }
                struct nodeitem * tempitem2 = headitem;
                #pragma omp taskwait (shared: totaluntung, tempitem2) reduction (+: totaluntung)
                    while(tempitem2 != NULL){
                        totaluntung += findhighest(headitem, tempitem2->item.kodeitem, headbidder, jumlahbidder);
                        tempitem2 = tempitem2->next;
                    }
          	  }
        
              printf("\nTotal value barang yang dibeli: ");
              printf("Rp. %d", totalvalue);
              printf("\nTotal penjualan: ");
              printf("Rp. %d", totaluntung);
              printf("\nTotal keuntungan yang didapatkan: ");
              totaluntung -= totalvalue;
              printf("Rp. %d", totaluntung);

              do {
                printf("\n\nApakah ingin mengakhiri hari? (dan mereset semua data lelang)");
                printf("\n1 = ya\n2 = tidak\n>>> ");
                scanf("%d", &clearweek);
                  if (clearweek  > 2 || clearweek < 1)
                    printf("\nPilihan invalid!");
              } while (clearweek > 2 || clearweek < 1);
              
              if (clearweek == 1){
                FILE *fptr;
                fptr = fopen("dataLelang.txt", "w");
                fclose(fptr);
              }
               
                 printf("\nType any character to continue ... ");
              scanf("%s", delay);
            } else {
              printf("\nPassword anda salah!");
            }
          }
        } else if (pilihan == 5){ //pilihan 5, sebagai penutup
          printf("\nTerima kasih sudah menggunakan jasa kami");
          return 0;
        } else { //error handling jika memilih selain 1-5
          printf("\nPilihan anda invalid!");
          printf("Type any character to continue ... ");
          scanf("%s", delay);
        }
    }
}
