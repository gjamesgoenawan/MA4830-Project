// QNX DAS Registers
#if defined(__QNX__)
    #include <hw/pci.h>
    #include <hw/inout.h>
    #include <sys/neutrino.h>
    #include <sys/mman.h>															
    #define	INTERRUPT		iobase[1] + 0				// Badr1 + 0 : also ADC register
    #define	MUXCHAN			iobase[1] + 2				// Badr1 + 2
    #define	TRIGGER			iobase[1] + 4				// Badr1 + 4
    #define	AUTOCAL			iobase[1] + 6				// Badr1 + 6
    #define DA_CTLREG		iobase[1] + 8				// Badr1 + 8
    #define	AD_DATA			iobase[2] + 0				// Badr2 + 0
    #define	AD_FIFOCLR		iobase[2] + 2				// Badr2 + 2
    #define	TIMER0			iobase[3] + 0				// Badr3 + 0
    #define	TIMER1			iobase[3] + 1				// Badr3 + 1
    #define	TIMER2			iobase[3] + 2				// Badr3 + 2
    #define	COUNTCTL		iobase[3] + 3				// Badr3 + 3
    #define	DIO_PORTA		iobase[3] + 4				// Badr3 + 4
    #define	DIO_PORTB		iobase[3] + 5				// Badr3 + 5
    #define	DIO_PORTC		iobase[3] + 6				// Badr3 + 6
    #define	DIO_CTLREG		iobase[3] + 7				// Badr3 + 7
    #define	PACER1		    iobase[3] + 8				// Badr3 + 8
    #define	PACER2		    iobase[3] + 9				// Badr3 + 9
    #define	PACER3			iobase[3] + a				// Badr3 + a
    #define	PACERCTL		iobase[3] + b				// Badr3 + b
    #define DA_Data			iobase[4] + 0				// Badr4 + 0
    #define	DA_FIFOCLR		iobase[4] + 2				// Badr4 + 2
    uintptr_t iobase[6];

    #define	DEBUG		1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <process.h>
#include <unistd.h>
#include <time.h>
#include <strings.h>
#include <ncurses.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include "main.h"

int badr[5];															// PCI 2.2 assigns 6 IO base addresses

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// init mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable to store condition if there is an input from the keyboard
int freq_check = 0, amp_check = 0, specifier_given = 0;

int main(int argc, char *argv[]) {
    int configLoaded = 0;
    int daqEnable = 0;
//**********************************************************************************************
// Setting up PCI-DAS1602/16 Board
//**********************************************************************************************    
    void *hdl;
    unsigned int i;
    char **p_to_arg=&argv[1];
    #if defined(__QNX__)
		struct pci_dev_info info;
        memset(&info,0,sizeof(info));
        if(pci_attach(0)<0) {
            perror("pci_attach");
            exit(EXIT_FAILURE);}

        info.VendorId=0x1307;
        info.DeviceId=0x01;

        if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {
            perror("pci_attach_device");
            exit(EXIT_FAILURE);}	

        for(i=0;i<5;i++) {
            badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
            iobase[i]=mmap_device_io(0x0f,badr[i]);	}	
        
        if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
            perror("Thread Control");
            exit(1);}			
        
        // Initialise Board
        out16(INTERRUPT,0x60c0);				    // sets interrupts	 - Clears
        out16(TRIGGER,0x2081);					// sets trigger control: 10MHz, clear, Burst off,SW trig. default:20a0
        out16(AUTOCAL,0x007f);					// sets automatic calibration : default

        out16(AD_FIFOCLR,0); 						// clear ADC buffer
        out16(MUXCHAN,0x0D00);          // previous group select 0x0C10
    #endif

    //Initializing default settings	
    inputs.amp = DEFAULT_AMPLITUDE;  
    inputs.freq = DEFAULT_FREQUENCY;																							
   		
//**********************************************************************************************
// Parsing Comand Line Arguments
//**********************************************************************************************

    while(--argc && (*p_to_arg)[0]=='-'){
        if((*p_to_arg)[1]=='\0'){printf("invalid option1\n");exit(1);
            }
        switch((*p_to_arg)[1]){
            case '?': //Print help messages
                printf( 
                    "\n\nMA4830 CA 2: Multi Wave Generator\n"
                    "Author: Andhika Satriya Bhayangkara, Chan Yee Hang Caleb, Gabriel James Goenawan, Yap Choon Kai Jonathan \n"
                    "Usage:\n"
                    "-w sine|square|sawtooth|triangular -f [float] -a [float]\n\n"
                    "Functions:\n\n"
                    "-w Specific Type of Wave to be output.\n"
                    "-f Specify the Frequency of the wave in Hertz (Hz).\n"
                    "-a Specify the Amplitude of the wave in millivolt (mV).\n"
                    "-l Load previously saved parameters from config.tx\n"
                    "-d Enable input from potentiometer and toggle switches\n"
                    "-? Show this message\n\n");
                exit(0);
            case 'w': //Read wave type and store in a struct
                if(argc-1<=0){printf("Invalid Wave Type\n");exit(1);}
                else{
                    p_to_arg++;argc--;
                    if(strcasecmp(*p_to_arg, "SINE")==0){inputs.waveType=1;}
                    else if(strcasecmp(*p_to_arg, "SQUARE")==0){inputs.waveType=2;}
                    else if(strcasecmp(*p_to_arg, "SAWTOOTH")==0){inputs.waveType=3;}
                    else if(strcasecmp(*p_to_arg, "TRIANGULAR")==0){inputs.waveType=4;}
                    else{printf("Invalid Wave Type\n");exit(1);}
                    specifier_given = 1;
                    p_to_arg++;
                    continue; 
                }
            case 'f': //Read frequency and store in a struct
                if(argc-1<=0){printf("Invalid Frequency\n");exit(1);}
                else{
                    p_to_arg++;argc--;
                    if((*p_to_arg)[0] == '-'){printf("Invalid Frequency\n");exit(1); }
                    if(atof(*(p_to_arg)) <= 3.0){printf("Invalid Frequency\n");exit(1);}
                    inputs.freq = atof(*(p_to_arg));
                    specifier_given = 1;
                    p_to_arg++;
                    continue;  
                }
            case 'a': //Read amplitude and store in a struct
                if(argc-1<=0){printf("Invalid Amplitude\n");exit(1);}
                else{
                    p_to_arg++;argc--;
                    if((*p_to_arg)[0] == '-'){printf("Invalid Amplitude\n");exit(1); }
                    if(atof(*p_to_arg) <= 0.0){printf("Invalid Amplitude\n");exit(1);}
                    inputs.amp = atof(*p_to_arg);
                    specifier_given = 1;
                    p_to_arg++;
                    continue; 
                }
            case 'l': //Load previously saved config files
                read_config("config.txt", &inputs);
                configLoaded = 1;
                continue;
            case 'd': //Enable input from DAS Board
                daqEnable = 1;
                continue;
            default:printf("'%s' Invalid option. Use '-?' for help.\n", (*p_to_arg));exit(1); 
            }
        if (configLoaded == 1) break;
        p_to_arg++;
    }
    //
    if (specifier_given == 1) { //Print the parameters if specified in the arguments
        printf("Wave Type     : %d\nWave Frequency   : %f\nWave Amplitude: %f\n", inputs.waveType, inputs.freq, inputs.amp);
    }
    //Print the welcome message for default mode (keyboard input)
    if (specifier_given == 0 && daqEnable==0) {
        printf( 
        "\nMA4830 CA 2: Multi Wave Generator : Keyboard Input MODE\n"
        "Author: Andhika Satriya Bhayangkara, Chan Yee Hang Caleb, Gabriel James Goenawan, Yap Choon Kai Jonathan \n");
        printf(
        "\n\nW,A,S,D to move / change value\n"
        "Enter to select the current option\n\n\n");

    //Print the welcome message for DAQ Board input mode
    if (daqEnable==1){
        printf( 
        "\nMA4830 CA 2: Multi Wave Generator : DAQ Board Input MODE\n"
        "Author: Andhika Satriya Bhayangkara, Chan Yee Hang Caleb, Gabriel James Goenawan, Yap Choon Kai Jonathan \n");
        printf(
        "\n\nW,A,S,D to move / change frequency value\n"
        "Turn the potentiometer (AD 0) to change the amplitude\n"
        "Toggle the switches to change the wave type\n"
        "Triangular Wave\t Sawtooth Wave\t Square Wave\t Sine Wave\n\n"
        "Enter to select the current option\n\n\n");
    }

    //Initialize program start countdown
        for (i = start_program; i > 0; i--) {
            if (i == start_program) {
                printf("Program will start in \n");
            }
            printf("\b\b%is", i);
            fflush(stdout);
            sleep(1);
        }
    }
    
    //Thread creation
    if (daqEnable==1){ //Create DAQ Input Thread or Keyboard Input Thread depending on the input
         pthread_create(&t_id[0], NULL, &daqthread, NULL);}
    else {
        pthread_create(&t_id[0], NULL, &keyboardthread, NULL); 
    }
    pthread_create(&t_id[1], NULL, &outputthread, NULL); //Create output thread containing wave generator and graphical output (ncurses)

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}