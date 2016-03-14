#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE_OF(x) sizeof(x)/sizeof(x[0])

#define FULL_BLOCK "█"
#define SPACE " "
#define RIGHT_HALF_BLOCK "▐"
#define LEFT_HALF_BLOCK "▌"
#define UPPER_HALF_BLOCK "▀"
#define LOWER_HALF_BLOCK "▄"

#define DARK_SHADE "▓"
#define MEDIUM_SHADE "▒"
#define LIGHT_SHADE "░"

#define UPPER_LEFT "▛"
#define UPPER_RIGHT "▜"
#define LOWER_LEFT "▙"
#define LOWER_RIGHT "▟"

#define DOUBLE_DASH "═"

#define BOX_LENGTH 180


int amount = 0; //To store amount won

/*
Flag Field

attribute	  | bit_position
----------------------------
50/50	      |     0
Double Chance |     1
Hint	      |     2
*/
int lifeline = 1;

int done[10];
char CATEGORIES[5][16] = {"1. ", "2. ", "3. ", "4. ", "5. "};

struct Question {
    char question[32], option1[16], option2[16], option3[16], option4[16], hint[64];
    int answer;
} question;


void box();
void upperBorder();
void lowerBorder();
void welcome();
void renderInnerBorder(char[], int);
char* renderLines(int,char[]);
void nextQuestion();
void startGame();
void rules();
void highScores();
void invalidInput();
void showCategory(int);
void renderQuestion(struct Question);
struct Question selectQuestion(int);
int isDone(int);


char getch();
int width(char[]);

//bit methods
void setBit(int*, int);
void clearBit(int*, int);
void toggleBit(int*, int);
int isSet(int, int);

int main(int argc, char const *argv[]) {
    //initialise done to 0
    for(int i = 0; i < 10; i++) {
        done[i] = 0;
    }

    showCategory(1);
    return 0;
}

void welcome() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    renderLines(1, "WELCOME!");
    renderLines(1, "");
    renderLines(1, "ENTER YOUR NAME:-");
    char *name = renderLines(1, "getch");
    renderLines(3, "");
    renderLines(1, "1. START GAME");
    renderLines(1, "");
    renderLines(1, "2. RULES/HELP");
    renderLines(1, "");
    renderLines(1, "3. HIGH SCORES");
    renderLines(1, "");
    renderLines(1, "4. QUIT");
    renderLines(10, "");
    lowerBorder();
    int choice = 0;
    do {
        choice = (int) getch() - 48;
        switch (choice) {
            case 1:
                startGame();
                break;

            case 2:
                rules();
                break;

            case 3:
                highScores();
                break;

            case 4:
                return;
                break;
        }
    } while(choice < 1 || choice > 4);
}

void startGame() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    renderLines(1, "CATEGORIES");
    renderLines(2, "");
    renderLines(1, "1.");
    renderLines(1, "");
    renderLines(1, "2.");
    renderLines(1, "");
    renderLines(1, "3.");
    renderLines(1, "");
    renderLines(1, "4.");
    renderLines(1, "");
    renderLines(1, "5.");
    renderLines(1, "");
    lowerBorder();
    int choice = 0;
    while (1) {
        choice = (int) getch() - 48;
        if(choice > 0 && choice < 6) {
            break;
        }
    }
    showCategory(choice);
}

void showCategory(int category) {
    system("clear");
    upperBorder();
    renderLines(3, "");
    renderLines(1, CATEGORIES[category - 1]);
    renderLines(2, "");
    struct Question question = selectQuestion(category);
    renderQuestion(question);
    lowerBorder();
}

void renderQuestion(struct Question question) {
    renderInnerBorder(UPPER_HALF_BLOCK, 80);
    renderLines(1, question.question);
    renderInnerBorder(LOWER_HALF_BLOCK, 80);
    renderLines(2, "");

    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, "");
    renderLines(1, question.option1);
    renderLines(1, "");
    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, "");
    renderLines(1, question.option2);
    renderLines(1, "");
    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, "");
    renderLines(1, question.option3);
    renderLines(1, "");
    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, "");
    renderLines(1, question.option4);
    renderLines(1, "");
    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, "");

    //making lifeline box
    renderInnerBorder(DARK_SHADE, 12);
    renderInnerBorder("▓▓Lifeline▓▓", 1);
    renderInnerBorder(DARK_SHADE, 12);


    char lifelineStr[64] = "";
    strcat(lifelineStr, ((isSet(lifeline, 0)) ? "1̶.̶ ̶5̶0̶/̶5̶0̶     " : "1. 50/50     "));
    strcat(lifelineStr, ((isSet(lifeline, 1)) ? "2̶.̶ ̶D̶o̶u̶b̶l̶e̶ ̶C̶h̶a̶n̶c̶e̶     " : "2.Double Chance     "));
    strcat(lifelineStr, ((isSet(lifeline, 2)) ? "3̶.̶ ̶H̶i̶n̶t̶" : "3. Hint"));

    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, lifelineStr);
    renderInnerBorder(DOUBLE_DASH, 40);
}

struct Question selectQuestion(int category) {
    struct Question question;
    srand(time(NULL));
    int number;
    do {
        number = (rand() % 25) + 1;
    } while(!isDone(number));
    number = 1;//temporary

    //creating file path from category and random number
    char path[16];
    snprintf(path, 32, "questions/%d/%d.txt", category, number);

    //reading from file
    FILE *fp;
    fp = fopen (path,"r");
    char buff[255];
    if (fp!=NULL)
    {
        //scanning into struct object
        fscanf(fp,"%s%s%s%s%s%s%d", question.question, question.option1, question.option2, question.option3, question.option4, question.hint, &question.answer);
        fclose (fp);
    }
    return question;
}

//checks whether question is done or not
int isDone(int number) {
    int i;
    for(i = 0; done[i] != 0; i++) {
        if(done[i] == number) return 0;
    }
    done[i] = number;
    return 1;
}

void rules() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    lowerBorder();
}

void highScores() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    lowerBorder();
}

void invalidInput() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    lowerBorder();
}

//renderLines cannot render in unicode
void renderInnerBorder(char content[], int count) {
    int length = BOX_LENGTH;
    int contentLength = width(content);
    if(count == 1) count = contentLength;
    int middle = length / 2;
    int start = middle - (count / 2);
    int end = start + count;
    int i;

    printf(FULL_BLOCK);//left border
    for (i = 1; i < length - 1; i++) {
        if(i >= start && i < end) {
            printf("%s", content);
            if(contentLength != 1) i = end - 1;//For non-repeating content (eg. lifeline)
        }
        else printf(SPACE);
    }
    printf(FULL_BLOCK);//right border
    printf("\n");
}

char* renderLines(int lines, char contents[]) {
    int length = BOX_LENGTH;
    int contentsLength = strlen(contents);
    int middle = length / 2;
    int start = middle - (contentsLength / 2);
    int end = start + contentsLength;
    char *input;
    char temp[20];
    int i,j;

    for(j = 0; j < lines; j++) {
        printf(FULL_BLOCK);//left border
        for (i = 1; i < length - 1; i++) {

            if(i >= start && i < end) {
                if(contents == "getch") {
                    int count = 0;

                    while(1) {
                        char t = getch();
                        if(t == 10) break;
                        else if(t == 127) {
                            printf("\b \b");
                            count--;
                            continue;
                        } else if((t >= 65 && t <=90) || (t >= 97 && t <= 122) || (t == 32)) {
                            temp[count++] = t;
                            printf("%c", t);
                        }
                    }

                    input = &temp[0];
                    int inputLength = count;
                    length = length - inputLength + 1;
                    end = 0;
                } else {
                    printf("%c", contents[i - start]);
                }
            }
            else printf(SPACE);
        }
        printf(FULL_BLOCK);//right border
        printf("\n");
    }
    return input;
}

void upperBorder() {
    printf(FULL_BLOCK);
    for (int i = 1; i < (BOX_LENGTH - 1); i++) {
        printf(UPPER_HALF_BLOCK);
    }
    printf(FULL_BLOCK);
    printf("\n");
}

void lowerBorder() {
    printf(FULL_BLOCK);
    for (int i = 1; i < (BOX_LENGTH - 1); i++) {
        printf(LOWER_HALF_BLOCK);
    }
    printf(FULL_BLOCK);
    //printf("\n");
}










char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    //printf("%c\n",buf); No echo
    return buf;
}

int width(char contents[]) {
    int length = 0;
    int tempLength = strlen(contents);
    int temp = 0;
    for(int i = 0; i < tempLength; i++) {
        if(contents[i] < 0) {
            if(temp == 2) {
                length++;
                temp = 0;
            } else {
                temp++;
            }
        }
        else {
            length++;
        }
    }
    return length;
}

//Bit methods
void setBit(int * val, int bit_position) { * val = * val | (1 << bit_position); }

void clearBit(int * val, int bit_position) { * val = * val & ~(1 << bit_position); }

void toggleBit(int * val, int bit_position) { * val = * val ^ (1 << bit_position); }

int isSet(int val, int bit_position) { return (val & (1 << bit_position)); }
