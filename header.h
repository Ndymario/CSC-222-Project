// Declare a constant data size assumption to use with all
// unknown lenght arrays
const int SIZE = 128;

// Create variables to store user input
char command[SIZE];
char* commandPtr = command;

// Create variables to store the tokenized input
char tempCommand[SIZE];
char* tempCommandPtr;

// Create variables to store any arguments passed
char args[SIZE];
char* argsPtr;
char* argsString[SIZE];
char* argsStringFix[SIZE];

// Create variables to store the cwd for use with PWD
char cwd[SIZE];
char *cwdPtr;

// Create variables to store information from a file being read
char fileRead[SIZE];
char *fileString[SIZE];
char charIn;

// Create loop variables
int i;
int j;
int k;
int l;
int m = 0;
int n = 0;
int argNum = 0;
int fileIndex = 0;
int fileNum = 0;
int argIndex = 0;

// Create variables to assist with files
bool hasOut = false;
int outIndex = 0;

bool hasIn = false;
int inIndex = 0;

bool hasArgs;
bool oneArg;
bool err;

// Create the bool that runs the while loop
bool ready = true;