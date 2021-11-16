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
#endif

int badr[5];	
/* Function to capture input from Keyboard */

void *keyboardthread(void *arg){
	int ch;
    inputs.generateWave = 0;
    while(1){
        // Take Mutex to update user inputs
        pthread_mutex_lock(&mutex);
        ch = getchar();
        if(ch == 115){      
            inputs.currentInput = (inputs.currentInput + 1) % 5;
        }
        else if(ch == 119){    
            if (inputs.currentInput == 0){
            	inputs.currentInput = 4;
            }
            else{
            	inputs.currentInput--;
            } 
        }
        else if(ch == 100 || ch == 97){
            if (inputs.currentInput==0){
                if(ch == 100){
                    inputs.waveType++;
                }
                else {
                    inputs.waveType--;
                }
                inputs.waveType = ((inputs.waveType - 1) % 4) + 1;
            }
            else if (inputs.currentInput==1){
                if(ch == 100 && inputs.freq<200){
                    inputs.freq += 1;
                } 

                else if(inputs.freq>3){
                    inputs.freq -= 1;
                }
            }
            else if (inputs.currentInput==2){
                if(ch == 100 && inputs.amp<50.0){
                    inputs.amp += 0.01;
                }
                else if(inputs.amp>0.01){
                    inputs.amp -= 0.01;
                }
            }
        }
        else if(ch==13 && inputs.currentInput == 3){
            inputs.currentInput = 0;    
            inputs.generateWave = 1;
        }
        else if(ch==13 && inputs.currentInput == 4){
            erase();
            save_config("config.txt", &inputs);
            endwin();
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
    } 	
}

void *outputthread(void *arg) {
    
    // Wave attributes
    unsigned int data[10000];
    unsigned int i, n;
    float dummy, a;
    int cnt = 0;
     #if defined(__QNX__)
        void *hdl;
        struct pci_dev_info info;
        uintptr_t iobase[6];
        uintptr_t dio_in;
        uint16_t adc_in;
        memset(&info,0,sizeof(info));
        if(pci_attach(0)<0) {perror("pci_attach");exit(EXIT_FAILURE);}
        info.VendorId=0x1307;
        info.DeviceId=0x01;
        if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {perror("pci_attach_device");exit(EXIT_FAILURE);}		
        for(i=0;i<5;i++) {badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);iobase[i]=mmap_device_io(0x0f,badr[i]);	}	
        if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {perror("Thread Control");exit(1);}			
    #endif	

    startNcurses();

    while(1) {
        erase();
        // printing of wave type
        if(inputs.currentInput==0){
            attron(A_STANDOUT);
            mvprintw(0,0,"> ");
            attroff(A_STANDOUT);
            mvprintw(1,0,"  ");
            mvprintw(2,0,"  ");
            mvprintw(4,0,"  ");
            mvprintw(6,0,"  ");
        }
        else if(inputs.currentInput==1){
            mvprintw(0,0,"  ");
            attron(A_STANDOUT);
            mvprintw(1,0,"> ");
            attroff(A_STANDOUT);
            mvprintw(2,0,"  ");
            mvprintw(4,0,"  ");
            mvprintw(6,0,"  ");
        }
        else if(inputs.currentInput==2){
            mvprintw(0,0,"  ");
            mvprintw(1,0,"  ");
            attron(A_STANDOUT);
            mvprintw(2,0,"> ");
            attroff(A_STANDOUT);
            mvprintw(4,0,"  ");
            mvprintw(6,0,"  ");
        }
        else if(inputs.currentInput==3){
            mvprintw(0,0,"  ");
            mvprintw(1,0,"  ");
            mvprintw(2,0,"  ");
            attron(A_STANDOUT);
            mvprintw(4,0,"> ");
            attroff(A_STANDOUT);
            mvprintw(6,0,"  ");
        }
        else{
            mvprintw(0,0,"  ");
            mvprintw(1,0,"  ");
            mvprintw(2,0,"  ");
            mvprintw(4,0,"  ");
            attron(A_STANDOUT);
            mvprintw(6,0,"> ");
            attroff(A_STANDOUT);
        }
        if (inputs.currentInput == 0){attron(A_STANDOUT);}
        mvprintw(0, 2, "WAVE TYPE :  %s", "   ");
        switch(inputs.waveType) {
            case 1: 
                mvprintw(0, 2, "WAVE TYPE :  %s", "Sine");
                break;
            case 2:
                mvprintw(0, 2, "WAVE TYPE :  %s", "Square");
                break;
            case 3:
                mvprintw(0, 2, "WAVE TYPE :  %s", "Sawtooth");
                break;
            case 4:
                mvprintw(0, 2, "WAVE TYPE :  %s", "Triangular");
                break;
            case -1:
                mvprintw(0, 2, "WAVE TYPE :  %s", "Invalid input for WAVETYPE! Re-enter!");
                break;
        }
        attroff(A_STANDOUT);
        // printing of frequency
        if (inputs.currentInput == 1){attron(A_STANDOUT);}
        move(1,12);
        clrtoeol();
        mvprintw(1, 2, "FREQUENCY :  %.2lf Hz", inputs.freq);
        attroff(A_STANDOUT);
        // printing of amplitude
        if (inputs.currentInput == 2){attron(A_STANDOUT);}
        move(2,12);
        clrtoeol();
        mvprintw(2, 2, "AMPLITUDE :  %.2lf V", inputs.amp);
        attroff(A_STANDOUT);

        // printing button
        if (inputs.currentInput == 3){attron(A_STANDOUT);}
        if (inputs.generateWave == 0){
            move(4,12);
            clrtoeol();
            mvprintw(4, 2, "Generate Wave");
            //mvprintw(5,0,"%d, %d\n",n, cnt);
        }
        else{
            a = (inputs.amp*20/26);
            if(inputs.waveType == 1){
           		n = (int) ((100 * (1/inputs.freq))/0.077);
                for(i=0;i<n;i++) {
                    dummy = sineWave(i, n, a);
                    data[i]= (unsigned) dummy;
                }
            }
            else if(inputs.waveType ==2){
            	n = (int) ((100 * (1/inputs.freq))/0.004);
                for(i=0;i<n;i++) {
                    dummy = squareWave(i, n, a);
                    data[i]= (unsigned) dummy;
                }
            }
            else if(inputs.waveType ==3){
            	n = (int) ((100 * (1/inputs.freq))/0.004);
                for(i=0;i<n;i++) {
                    dummy = sawtoothWave(i, n, a);
                    data[i]= (unsigned) dummy;
                }
            }
            else if(inputs.waveType ==4){
            	n = (int) ((100 * (1/inputs.freq))/0.0045);
                for(i=0;i<n;i++) {
                    dummy = triangularWave(i, n, a);
                    data[i]= (unsigned) dummy;
                }
            }
            cnt = 0;
            inputs.generateWave = 0;           
        }
        attroff(A_STANDOUT);

        if(inputs.currentInput == 4){attron(A_STANDOUT);}
        mvprintw(6, 2, "Save & Exit");
        attroff(A_STANDOUT);

        refresh();

        #if defined (__QNX__)
	      	out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		2/6
   	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
   	        out16(DA_Data,(short) data[cnt]);																																		
   	        out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		2/6
  	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
	        out16(DA_Data,(short) data[cnt]);	
	        cnt++;
	        if (cnt>=n){
	        	cnt = 0;
	        }																															
        #endif
    }
    endwin();
}