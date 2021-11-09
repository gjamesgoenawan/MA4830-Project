// 09September 2005
//******************************************************************************************************
// Performs basic I/O for the Omega PCI-DAS1602 
//
// Demonstration routine to demonstrate pci hardware programming
// Demonstrated the most basic DIO and ADC and DAC functions
// - excludes FIFO and strobed operations 
//
// Note :
//
//			22 Sept 2016 : Restructured to demonstrate Sine wave to DA
//
// G.Seet - 26 August 2005
//******************************************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
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

int badr[5];															// PCI 2.2 assigns 6 IO base addresses

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int sineWave(int i, int n, float a){
    float delta=(2.0*3.142)/n;
    return((int) (((sinf((float)(i*delta))) + 1.0) * 0.5 * a * 0x8000));
}

int squareWave(int i, int n, float a){
    if(i<=(n/2)){
        return((int)(1.0 * a * 0x8000));
    }
    else{
        return((int)(0.0 * a * 0x8000));
    }
}

int sawtoothWave(int i, int n, float a){
    return((((float) i / (float) n) * a * 0.5 * ((float) 0x8000)));
}

int triangularWave(int i, int n, float a){
    if(i<=(n/2)){
        return((int)(((((float)i*2.0*((float)0x8000))/((float)n)))*((float) a)));
    }
    else{
        return((int)((((float) 0x8000)-((((float) 0x8000)*((2.0*((float) i))-((float) n)))/((float) n)))*((float) a)));
    } 
}

int main(int argc, char *argv[]) {
    void *hdl;
    unsigned int i,count, n=50, waveType=0;
    unsigned short chan;
    unsigned int data[100000];
    float delta,dummy,a=1.0;
    char **p_to_arg=&argv[1];
    #if defined(__QNX__)
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
   		
//**********************************************************************************************
// Parsing Arguments
//**********************************************************************************************

while(--argc && (*p_to_arg)[0]=='-'){
	if((*p_to_arg)[1]=='\0'){printf("invalid option1\n");exit(1);
		}
	switch((*p_to_arg)[1]){
        case '?':
            printf( 
                "MA4830 CA 2: Multi Wave Generator\n"
                "Author: Andhika Satriya Bhayangkara, Cahn Yee Hang Caleb, Gabriel James Goenawan, Yap Choon Kai Jonathan \n"
                "Usage:\n"
                "-w sine|square|sawtooth|triangular -f [float] -a [float]\n\n"
                "Functions:\n\n"
                "-w Specific Type of Wave to be output.\n"
                "-f Specify the Frequency of the wave in Hertz (Hz).\n"
                "-a Specify the Amplitude of the wave in millivolt (mV).\n"
                "-? Show this message\n\n");
			exit(0);


		case 'w':
			if(argc-1<=0){printf("Invalid Wave Type\n");exit(1);}
			else{
				p_to_arg++;argc--;
				if(strcasecmp(*p_to_arg, "SINE")==0){waveType=0;}
				else if(strcasecmp(*p_to_arg, "SQUARE")==0){waveType=1;}
				else if(strcasecmp(*p_to_arg, "SAWTOOTH")==0){waveType=2;}
				else if(strcasecmp(*p_to_arg, "TRIANGULAR")==0){waveType=3;}
				else{printf("Invalid Wave Type\n");exit(1);}
				p_to_arg++;
				continue; 
			}
		case 'f':
			if(argc-1<=0){printf("Invalid Frequency\n");exit(1);}
			else{
				p_to_arg++;argc--;
				if((*p_to_arg)[0] == '-'){printf("Invalid Frequency\n");exit(1); }
				n = (int) ((50/atof(*(p_to_arg)))/0.00057);
                if(n<=0){printf("Invalid Frequency\n");exit(1);}
                p_to_arg++;
				continue; 
				
			}
		case 'a':
			if(argc-1<=0){printf("Invalid Amplitude\n");exit(1);}
			else{
				p_to_arg++;argc--;
				if((*p_to_arg)[0] == '-'){printf("Invalid Amplitude\n");exit(1); }
				a = (atof(*p_to_arg)*2/25);
                if(a<=0){printf("Invalid Amplitude\n");exit(1);}
				p_to_arg++;
				continue; 
			}
		default:printf("'%s' Invalid option. Use '-?' for help.\n", (*p_to_arg));exit(1); 
		}
	p_to_arg++;
	}
//**********************************************************************************************
// Pregenerating wave
//**********************************************************************************************

	
    if(waveType == 0){
        for(i=0;i<n;i++) {
            dummy = sineWave(i, n, a);
            data[i]= (unsigned) dummy;
        }
    }
    else if(waveType==1){
        for(i=0;i<n;i++) {
            dummy = squareWave(i, n, a);
            data[i]= (unsigned) dummy;
        }
    }
    else if(waveType==2){
        for(i=0;i<n;i++) {
            dummy = sawtoothWave(i, n, a);
            data[i]= (unsigned) dummy;
        }
    }
    else{
        for(i=0;i<n;i++) {
            dummy = triangularWave(i, n, a);
            data[i]= (unsigned) dummy;
        }
    }
   	//for(i=0; i<n;i++){
		//printf("data %d: %d\n",i,data[i]);
	//}
    printf("Wave Type     : %d\nWave Period   : %d\nWave Amplitude: %f\n", waveType, n, a);
    
    #if defined(__QNX__)
//*********************************************************************************************
// Output wave
//*********************************************************************************************
        while(1) {
        for(i=0;i<n;i++) {
	        out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		2/6
   	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
   	        out16(DA_Data,(short) data[i]);																																		
   	        out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		2/6
  	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
	        out16(DA_Data,(short) data[i]);																																		
  	        }
        }
  	
//**********************************************************************************************
// Reset DAC to default : 5v
//**********************************************************************************************

        out16(DA_CTLREG,(short)0x0a23);	
        out16(DA_FIFOCLR,(short) 0);			
        out16(DA_Data, 0x8fff);																								
        out16(DA_CTLREG,(short)0x0a43);	
        out16(DA_FIFOCLR,(short) 0);			
        out16(DA_Data, 0x8fff);																																									
        pci_detach_device(hdl);

//**********************************************************************************************
    #endif
    
    exit(0);
}
