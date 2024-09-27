#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 20
#define WORD_LEN 50
int BubbleSort(char arr[][WORD_LEN], int size, char target[]);

int main(void) {
   FILE* fp;
   char wordArr[SIZE][WORD_LEN] = {0};
   int index = 0;
   char word[WORD_LEN];
   char temp[WORD_LEN];
   char words[SIZE][WORD_LEN] = {0};
   FILE* file;

   file = fopen("input_4.dat", "r");

   while (fscanf(file, "%s", wordArr[index]) != EOF && index < SIZE)
   {
      index++;
   }
   printf("정렬 전\n-------------------------------------------------\n");
   for (int i = 0; i < index; i++)
   {
      printf("%s ", wordArr[i]);
   }


   fclose(file);

   fp = fopen("input.dat", "w");

   if (fp == NULL) {
      printf("파일을 열 수 없습니다.");
      exit(1);
   }

   for (int i = 0; i < index; i++) {
      strcpy(words[i], wordArr[i]);
      fprintf(fp, "%s\n", words[i]);
   }

   fclose(fp);

   for (int i = 0; i < index - 1; i++)
   {
      for (int j = 0; j < index - 1 - i; j++)
      {
         if (strcmp(wordArr[j], wordArr[j + 1]) < 0) {
            strcpy(temp, wordArr[j]);
            strcpy(wordArr[j], wordArr[j + 1]);
            strcpy(wordArr[j + 1], temp);
         }
      }
   }
   printf("\n\n");

   printf("Sort 후\n-------------------------------------------------\n ");
   for (int i = 0; i < SIZE; i++) {
      printf("%s ", wordArr[i]);
   }

   printf("\n");

   while (1) {
      printf("단어를 입력하세요:");
      scanf("%s", word);

      if (strcmp(word, "0") == 0) {
         break;
      }
      else {
         int result = BubbleSort(wordArr, index, word);
         if (result != -1) {
            printf("%s는 %d번째입니다.\n", word, result + 1);
         }
         else {
            printf("%s는 배열에 없습니다.\n", word);
         }
      }
   }

   return 0;
}


int BubbleSort(char arr[][WORD_LEN], int size, char target[]) {
   int low = 0;
   int high = size - 1;
   int mid;

   while (low <= high) {
      mid = (low + high) / 2;
      
      int cmp = strcmp(arr[mid], target);
      if (cmp == 0)
         return mid;
      else if (cmp > 0)
         low = mid + 1;
      else
         high = mid - 1;
   }
   return -1;
}
