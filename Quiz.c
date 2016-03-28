#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SIZE_OF(x) sizeof(x)/sizeof(x[0])

// Unicode Characters
#define FULL_BLOCK "█"
#define SPACE " "
#define RIGHT_HALF_BLOCK "▐"
#define LEFT_HALF_BLOCK "▌"
#define UPPER_HALF_BLOCK "▀"
#define LOWER_HALF_BLOCK "▄"
#define PIPE "╼"
#define DARK_SHADE "▓"
#define MEDIUM_SHADE "▒"
#define LIGHT_SHADE "░"
#define DOUBLE_DASH "═"

#define BOX_LENGTH 180











int score = 1; // because at least 1 :P

/*
Flag Field

attribute	  | bit_position
----------------------------
50/50	      |     0   (50/50 lifeline used if 1)
Double Chance |     1   (Double Chance lifeline used if 1)
Hint	      |     2   (Hint lifeline used if 1)
Show Hint     |     3   (Show Hint for this question if 1)
isFirstChance |     4   (First Chance for this question if 1)
isSecondChance|     5   (Second Chance for this question if 1)
*/
int lifeline = 0;

int done[10];
char CATEGORIES[5][16] = {"1. Anime", "2. Bollywood","3. Geography", "4. History", "5. Sports"};

struct Question {
    char question[64], option1[64], option2[64], option3[64], option4[64], hint[64];
    char answer;
} question;

struct HighScore {
    char name[16];
    int score;
} highscores [10];

char name[16] = ""; //default














void box();
void upperBorder();
void lowerBorder();
void welcome();
void renderInnerBorder(char[], int);
char * renderLines(int,char[]);
void nextQuestion();
void startGame();
void rules();
void highScores();
void loadHighScores();
int insertHighScore(struct HighScore);
void invalidInput();
void showCategory(int);
void renderQuestion(struct Question);
struct Question selectQuestion(int);
void wrongAnswer();
int isDone(int);


char getch();
int width(char[]);

//bit methods
void setBit(int *, int);
void clearBit(int *, int);
void toggleBit(int *, int);
int isSet(int, int);

int main(int argc, char const *argv[]) {
    //initialise done to 0
    for(int i = 0; i < 10; i++) {
        done[i] = 0;
    }
    //showCategory(1);
    //highScores();
    welcome();
    return 0;
}

void welcome() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    renderLines(1, "WELCOME!");
    renderLines(1, "");
    renderLines(1, "ENTER YOUR NAME:-");

    //saving name
    char * temp = renderLines(1, "getch");
    for(int i = 0; i < width(temp); i++) { name[i] = *(temp+i); }

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
                system("clear");
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
    renderLines(1, CATEGORIES[0]);
    renderLines(1, "");
    renderLines(1, CATEGORIES[1]);
    renderLines(1, "");
    renderLines(1, CATEGORIES[2]);
    renderLines(1, "");
    renderLines(1, CATEGORIES[3]);
    renderLines(1, "");
    renderLines(1, CATEGORIES[4]);
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
    do {
        struct Question question = selectQuestion(category);
        char choice;

        start:
        system("clear");
        char tempScore[16];
        snprintf(tempScore, 16, "Score - %d", score);

        upperBorder();
        renderLines(3, "");
        renderLines(1, tempScore);
        renderLines(2, "");
        renderLines(1, CATEGORIES[category - 1]);
        renderLines(2, "");
        renderQuestion(question);
        lowerBorder();

        again:
        choice = getch();
        choice = toupper(choice);
        switch(choice) {
            case 'A': case 'B': case 'C': case 'D':
            if(choice == question.answer) {
                score *=  2;
                clearBit(&lifeline, 3); //Turning off hint
                clearBit(&lifeline, 4);
                clearBit(&lifeline, 5);
                continue;
            }
            else {
                if(isSet(lifeline, 4)) {
                    clearBit(&lifeline, 4); // Turning off 1st chance
                    setBit(&lifeline, 5); // Turning on 2nd chance
                        goto start;
                    }
                else
                    wrongAnswer();
                    return;
            }
            break;

            //Hint
            case '3':
            if(isSet(lifeline, 2)) goto again;
            setBit(&lifeline, 2);
            setBit(&lifeline, 3);
            goto start;
            break;

            //Double Chance
            case '2':
            if(isSet(lifeline, 1)) goto again;
            setBit(&lifeline, 1);
            setBit(&lifeline, 4);
            goto start;
            break;

            //50/50
            case '1':
            if(isSet(lifeline, 0)) goto again;
            setBit(&lifeline, 0);
            srand(time(NULL));

            int one, two;
            while((one = (rand() % 4) + 1) == (question.answer - 64));
            while((two = (rand() % 4) + 1) == one || two == (question.answer - 64));

            if (one == 1 || two == 1) question.option1[0] = '\0';
            if (one == 2 || two == 2) question.option2[0] = '\0';
            if (one == 3 || two == 3) question.option3[0] = '\0';
            if (one == 4 || two == 4) question.option4[0] = '\0';
            goto start;
            break;

            default:
            goto again;
            break;
        }
    } while(1);
}

void renderQuestion(struct Question question) {
    renderInnerBorder(UPPER_HALF_BLOCK, 80);
    renderLines(1, question.question);
    renderInnerBorder(LOWER_HALF_BLOCK, 80);
    renderLines(2, "");

    //Show Hint
    if(isSet(lifeline, 3)) {
        char hint[] = "Hint: ";
        strcat(hint, question.hint);
        renderLines(1, hint);
        renderLines(2, "");
    }
    if(isSet(lifeline, 4)) {
        renderLines(1, "First chance");
        renderLines(2, "");
    }
    if(isSet(lifeline, 5)) {
        renderLines(1, "Second chance");
        renderLines(2, "");
        toggleBit(&lifeline, 5);
    }

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
    strcat(lifelineStr, ((isSet(lifeline, 1)) ? "2̶.̶ ̶D̶o̶u̶b̶l̶e̶ ̶C̶h̶a̶n̶c̶e̶    " : "2. Double Chance    "));
    strcat(lifelineStr, ((isSet(lifeline, 2)) ? "3̶.̶ ̶H̶i̶n̶t̶" : "3. Hint"));

    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(1, lifelineStr);
    renderInnerBorder(DOUBLE_DASH, 40);
    renderLines(2, "");
}

struct Question selectQuestion(int category) {
    struct Question question;
    srand(time(NULL));
    int number;
    do {
        number = (rand() % 25) + 1;
    } while(!isDone(number));
    number = (rand() % 2) + 1;//temporary

    //creating file path from category and random number
    char path[16];
    snprintf(path, 32, "questions/%d/%d.txt", category, number);

    //reading from file
    FILE *fp;
    fp = fopen (path,"r");
    if (fp!=NULL)
    {
        //scanning into struct object
        fscanf(fp,"%s%s%s%s%s%s%s", question.question, question.option1, question.option2, question.option3, question.option4, question.hint, &question.answer);
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
    renderLines(2, "");
    renderInnerBorder(DOUBLE_DASH, 12);
    renderLines(1, "HIGHSCORES");
    renderInnerBorder(DOUBLE_DASH, 12);
    renderLines(2, "");
    loadHighScores();
    for (int i = 0; highscores[i].score != 0; i++) {
        renderInnerBorder(PIPE, 16);
        char temp[32];
        snprintf(temp, 32, "%s %d", highscores[i].name, highscores[i].score);
        renderLines(1, temp);

    }
    renderInnerBorder(PIPE, 16);
    renderLines(4, "");
    renderLines(1, "Press any key to continue...");
    lowerBorder();
    getch();
    welcome();
}

void loadHighScores() {
    int i = 0;
    FILE *fp;
    fp = fopen ("res/highscores.txt","r");
    if (fp!=NULL)
    {
        struct HighScore temp;
        while (fscanf(fp, "%s%d", temp.name, &temp.score) != EOF) {
            highscores[i++] = temp;
        }
        fclose(fp);
    }
}

void invalidInput() {
    system("clear");
    upperBorder();
    renderLines(5, "");
    lowerBorder();
}



int insertHighScore(struct HighScore hs) {
    loadHighScores();

    int i = 9;
    while (i > 0 && highscores[i-1].score < hs.score) {
        highscores[i] = highscores[i-1];
        i--;
    }
    highscores[i] = hs;

    //writing
    FILE *fp;
    fp = fopen ("res/highscores.txt","w");
    if (fp!=NULL)
    {
        i = 0;
        while(highscores[i].score != 0) {
            fprintf(fp, "%s %d\n", highscores[i].name, highscores[i].score);
            i++;
        }
        fclose (fp);
    }
}








void wrongAnswer() {
    system("clear");

    char tempName[32];
    snprintf(tempName, 32, "Name - %s", name);
    char tempScore[32];
    snprintf(tempScore, 32, "Score - %d", score);

    struct HighScore tempHS;
    strcpy(tempHS.name, name);
    tempHS.score = score;
    int flag = insertHighScore(tempHS);

    upperBorder();
    renderLines(5, "");
    renderInnerBorder(DARK_SHADE, 16);
    renderInnerBorder("▓▓ GAME OVER! ▓▓", 1);
    renderInnerBorder(DARK_SHADE, 16);
    renderLines(2, "");
    if(1) {
        renderInnerBorder(LIGHT_SHADE, 16);
        renderInnerBorder("░░ HIGH SCORE ░░", 1);
        renderInnerBorder(LIGHT_SHADE, 16);
        renderLines(2, "");
    }
    renderInnerBorder(PIPE, 16);
    renderLines(1, "");
    renderLines(1, tempName);
    renderLines(1, "");
    renderInnerBorder(PIPE, 16);
    renderLines(1, "");
    renderLines(1, tempScore);
    renderLines(1, "");
    renderInnerBorder(PIPE, 16);
    renderLines(3, "");
    renderLines(1, "Press any key to continue...");
    lowerBorder();

    getch();
    welcome();
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
    int contentsLength = width(contents);
    int middle = length / 2;
    int start = middle - (contentsLength / 2);
    int end = start + contentsLength;
    char *input;
    char temp[16];
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
                    printf("%s", contents);
                    i = end - 1;
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

int width(char s[]) {
    int i = 0, j = 0;
    while (s[i]) {
        int n;
       if      ((s[i] & 0x80) == 0)    n = 1;
       else if ((s[i] & 0xE0) == 0xC0) n = 2;
       else if ((s[i] & 0xF0) == 0xE0) n = 3;
       else if ((s[i] & 0xF8) == 0xF0) n = 4;

       //strike-through specific
       if(s[i+1] == -52 && s[i+2] == -74) n = 3;
      j++;
      i += n;
    }
    return j;
}






//Bit methods
void setBit(int * val, int bit_position) { * val = * val | (1 << bit_position); }

void clearBit(int * val, int bit_position) { * val = * val & ~(1 << bit_position); }

void toggleBit(int * val, int bit_position) { * val = * val ^ (1 << bit_position); }

int isSet(int val, int bit_position) { return (val & (1 << bit_position)); }
