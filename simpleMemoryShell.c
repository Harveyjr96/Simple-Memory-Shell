#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

 //Global Variables
char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
int rerun = 0; /* 0 if input comes from user, 1 if from histroy */
int historyTotal = -1; /* Tracks the total history */
FILE* fptr; /* File Pointer */


//HISTORY MANAGMENT SECTION START//////////////////////////////////////////////////////////////////////////////////////
void loadHistory() { //Creates/Loads History. Updates History Total
	if ((fptr = fopen("user.history", "a+")) == NULL) {
		printf("Error! opening file\n");
		exit(1);
	}
	else {
		char c;
		int i = 0;
		char s[100];
		while (c != EOF) //Read Whoe File (to get to total history)
		{
			c = fgetc(fptr);
			s[i] = c;
			i++;
		}
		int x = (int)s[i - 3] - '0'; //x will store the ones place of the history total
		historyTotal = s[i - 4] - '0'; //historyTotal will store the 10s place of the total
		historyTotal = historyTotal * 10 + x; //Calculate history total
		if (historyTotal < -1) { //No previous history or no 10s place
			historyTotal = -1;
			if (x >= 0) {//Ones place found but not 10s place
				historyTotal = x; //
			}
		}
		printf("History Loaded\n");
		fclose(fptr);
	}
}

void reworkHistory() { //Called to delete outdated history

	int i = 0;
	char s[100];
	if ((fptr = fopen("user.history", "r")) == NULL) {
		printf("Error! Could not save command to history");
		exit(1);
	}
	else {
		char c = fgetc(fptr);
		while (c != '\n') { //scan past oldest command (first line)
			c = fgetc(fptr);
		}

		while (c != EOF) { //Store the rest of the file in array
			c = fgetc(fptr);
			s[i] = c;
			i++;
		}
		fclose(fptr);
	}
	if ((fptr = fopen("user.history", "w+")) == NULL) {
		printf("Error! Could not save command to history");
		exit(1);
	}
	else {
		int n = i;
		i = 0;
		while (i < n - 1) { //Write stored array back to file
			fprintf(fptr, "%c", s[i]);
			i++;
		}
		fclose(fptr);
	}
}


void writeToHistory() { //Writes command to history file
	++historyTotal;
	if (historyTotal >= 100) { //history will reset to 0 after surpassing 100 (keeps load history algoirthm working)
		historyTotal = 0;
	}
	if ((fptr = fopen("user.history", "a")) == NULL) { //open in append (write at end)
		printf("Error! Could not save command to history");
		exit(1);
	}
	else {
		fprintf(fptr, "%s, %i\n", inputBuffer, historyTotal);
		fclose(fptr);
	}
	if (historyTotal > 9) { //Need to delete oldest command
		reworkHistory();
	}
}
//HISTORY MANAGMENT SECTION END//////////////////////////////////////////////////////////////////////////////////////

//RERUN HISTORY SECTION START////////////////////////////////////////////////////////////////////////////////////////
void returnCommand(int spot) {//Copy the selected command into inputBuffer
	char s[MAX_LINE]; //Middle man of file to inputBuffer
	int i = -1;
	if ((fptr = fopen("user.history", "r")) == NULL) {
		printf("Error! Could not search history");
		exit(1);
	}
	else {
		char c = 'c';
		for (i = 0; i < spot; i++) {//Scan to the spot of the command in the file
			c = fgetc(fptr);
		}
		i = 0;
		while (c != ',') {//Copy command from file (',' is a delimiter)
			c = fgetc(fptr);
			if (c != ',') {//do not copy the delimiter
				s[i] = c;
				i++;
			}
		}
		s[i] = '\n'; //Append a linebreak to the command
		fclose(fptr);
	}
	if (s[0] == 'l' && s[1] == 's') { //ls copying was buggy and would always add in a random @
		strcpy(s, "ls\n");
	}
	strcpy(inputBuffer, s); //Copy into inputBuffer (back to setup after this)
}

void searchHistory(char key) {//Find user requested command from selectHistory
	int index = -1;
	int latestFound = -1; //Stores the spot in the file of the most recent matching command
	int lastLineStart = 0;//Will store the spot in the file of the last command (for 'r' and unmatching 'r x')
	if ((fptr = fopen("user.history", "r")) == NULL) {
		printf("Error! Could not search history");
		exit(1);
	}
	else {
		char c = 'c'; //initalize
		while (c != EOF) //Read entire file
		{
			c = fgetc(fptr);
			index++;
			if (c != EOF) {
				lastLineStart = index; //stores every starting spot (will end up being last)
			}
			if (c == key) {//found matching command
				latestFound = index; //store spot found at
			}
			while (c != '\n' && c != EOF) { //skip to next line
				c = fgetc(fptr);
				index++;
			}
		}
		fclose(fptr);
	}
	if (latestFound == -1) { //no matching commands found
		returnCommand(lastLineStart); //will run the latest command
	}
	else {//match found
		returnCommand(latestFound); //will run latest matching command
	}
}

void selectHistory() {// Prompt user to select what to rerun
	bool invalid = true; //true if user types invalid input
	char key = '\0';
	printf("Select Option to run\n");
	while (invalid) { //Loop untill user selects types proper input
		static char selection[4] = { '\0', '\0', '\0', '\0' };
		gets(selection);
		if (selection[0] != 'r') {//user did not start with r
			//user does not type r to start
			printf("Rerun must start with r, Reenter\n");
			selection[2] = '\0';
		}
		else if (selection[2] == '\0') {//assumption user only types r
			printf("Rerunning last option......\n");
			invalid = false; //break while loop
			key = ')'; //use character that will not be at the start of a command 
			searchHistory(key); //searchHistory for lastLine
		}
		else {
			printf("rerunning specific option %c\n", selection[2]);
			invalid = false;
			key = selection[2]; //user given key for specific option
			searchHistory(key); //will search for the x in "r x"
		}
	}
}

void printHistory() { //print history
	char c;
	printf("printing history....\n");
	if ((fptr = fopen("user.history", "r")) == NULL) {
		printf("Error! History not loaded");
		exit(1);
	}
	else {
		//Read and print history
		c = fgetc(fptr);
		while (c != EOF) //Read to end
		{
			printf("%c", c); //Print file as we read it
			c = fgetc(fptr);
		}
		fclose(fptr);
	}
}
//RERUN HISTORY SECTION END//////////////////////////////////////////////////////////////////////////////////////////

/* the signal handler function */
void handle_SIGINT() {
	rerun = 1;  //Will change setup from user input to input from the history file
	printHistory();
	selectHistory();
}

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

void setup(char* args[], int* background)
{
	int length, /* # of characters in the command line */
		i,  	/* loop index for accessing inputBuffer array */
		start,  /* index where beginning of next command parameter is */
		ct; 	/* index of where to place the next parameter into args[] */

	ct = 0;

	/* read what the user enters on the command line */
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

	if (rerun == 1) { //user hit ctrl+c and is rerunning a command
		for (length = 0; inputBuffer[length] != '\0'; ++length); //set length
		printf("Rerun->\n"); //visual purposes
		printf("%s", inputBuffer); //visual purposes
		rerun = 0; //set rerun to 0 so user can continue typing commands after   	 
	}

	start = -1;
	if (length == 0)
		exit(1);
	if (length < 0) {
		perror("error reading the command");
		exit(-1);
	}

	/* examine every character in the inputBuffer */
	for (i = 0; i < length; i++) {
		switch (inputBuffer[i]) {
		case ' ':
		case '\t':           	/* argument separators */
			if (start != -1) {
				args[ct] = &inputBuffer[start];	/* set up pointer */
				ct++;
			}
			inputBuffer[i] = '\0'; /* add a null char; make a C string */
			start = -1;

			break;

		case '\n':             	/* should be the final char examined */
			if (start != -1) {
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL; /* no more arguments to this command */

			break;

		case '&':
			*background = 1;
			inputBuffer[i] = '\0';

			break;

		default:         	/* some other character */
			if (start == -1)
				start = i;
		}
	}
	args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void)
{
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	sigaction(SIGINT, &handler, NULL);

	loadHistory();

	int background;         	/* equals 1 if a command is followed by '&' */
	char* args[MAX_LINE / 2 + 1];   /* command line (of 80) has max of 40 arguments */

	while (1) {        	/* Program terminates normally inside setup */
		background = 0;
		printf("COMMAND->\n");

		setup(args, &background);   	/* get next command */
		writeToHistory();
		/* the steps are:
		(1) fork a child process using fork()
		(2) the child process will invoke execvp()
		(3) if background == 0, the parent will wait,
		otherwise returns to the setup() function. */
		pid_t pid;
		pid = fork(); //Fork the child process
		if (pid < 0) {
			//Process Failed to Create
		}
		else if (pid == 0) {        	//child process
			execvp(args[0], args);  	//Run the command given from the user
		}
		else {//Parent process
			if (background == 0) {  	//No "&" at the end of command
				wait(NULL);         	//Parent must wait for child to terminate
			}
			else {//background == 1
										//& was used so parent need not wait
			}
		}
	}
}
//end
