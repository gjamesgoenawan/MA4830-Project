#include <pthread.h>

#define start_program 2
#define DEFAULT_AMPLITUDE 1
#define DEFAULT_FREQUENCY 5
#define DEFAULT_DELAY_MULTIPLIER 150
#define BASE_DELAY 100
#define MAXIMUM_AMPLITUDE 2.5
#define MAXIMUM_FREQUENCY 30
#define MINIMUM_AMPLITUDE 0
#define MINIMUM_FREQUENCY 3
//Color count
#define NUM_COLORS 6

// Variable to store the keyboard arrow input and current beats per minute
typedef struct {
	unsigned int waveType;
	float freq;
	float amp;
	int currentInput;
	int generateWave;
}input;

// input init
input inputs;

// Variable to store condition if there is an input from the keyboard
int freq_check;
int amp_check;
int specifier_given;

// init mutex
pthread_mutex_t mutex;

// threads id creation
pthread_t t_id[2];

int checkUserInput(char* parameter, int type);
char* getconfig(char* file,char* config);
void read_config(char *fileName, input *paramptr);
void save_config(char* fileName, input *paramsptr);
void *keyboardthread(void *arg);
void *daqthread(void *arg);
void *outputthread(void *arg);

typedef unsigned short ushort;

ushort sinWaveNcurses(double amplitude, double period,
    short phase_shift, ushort color_index);

ushort squareWaveNcurses(double amplitude, double period,
    short phase_shift, ushort color_index);

void startNcurses();

int sineWave(int i, int n, float a);
int squareWave(int i, int n, float a);
int sawtoothWave(int i, int n, float a);
int triangularWave(int i, int n, float a);


void readswitch(input *paramsptr);
void readpot(input *paramsptr );

