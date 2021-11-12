#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>

// ********************************************************************
// PCI Registers - define registers for PCI board *********************

#define INTERRUPT	iobase[1] + 0	// Badr1 + 0 : also ADC register
#define	MUXCHAN		iobase[1] + 2	// Badr1 + 2
#define	TRIGGER		iobase[1] + 4	// Badr1 + 4
#define	AUTOCAL		iobase[1] + 6	// Badr1 + 6
#define DA_CTLREG	iobase[1] + 8	// Badr1 + 8

#define	AD_DATA		iobase[2] + 0	// Badr2 + 0
#define AD_FIFOCLR	iobase[2] + 2	// Badr2 + 2

#define	DIO_PORTA	iobase[3] + 4	// Badr3 + 4
#define	DIO_PORTB	iobase[3] + 5	// Badr3 + 5
#define	DIO_PORTC	iobase[3] + 6	// Badr3 + 6
#define	DIO_CTLREG	iobase[3] + 7	// Badr3 + 7

#define DA_Data		iobase[4] + 0	// Badr4 + 0
#define DA_FIFOCLR	iobase[4] + 2	// Badr4 + 2

#define	DEBUG		1

int badr[5];		// PCI 2.2 assigns 6 IO base addresses

void readswitch(){
    int switchval;
    switchval = in8(DIO_PORTA);
    printf("reading = %i\n",switchval)

}

void readpot(){
    unsigned int count;
    uint16_t adc_in;
    unsigned short chan;
    int potentioval;

    count = 0x00;
    chan = ((count & 0x0f)<<4) | (0x0f & count);
    out16(MUXCHAN,0x0D00|chan);
    delay(1);
    out16(AD_DATA,0);
    while(!(in16(MUXCHAN)&0x4000));
    potentioval = in16(AD_DATA); //read potentiometer value
    printf("potentiometer value =%i\n",potentioval);
}

int main() {

struct pci_dev_info info;
void *hdl;

uintptr_t iobase[6],dio_in;
uint16_t adc_in[2];
	
unsigned int i,j,count,read_data;
unsigned int stat,stat1,stat2, stat3;
unsigned short chan;

printf("\fSetting up connection to PCI-DAS 1602...\n\n");

memset(&info,0,sizeof(info));
if(pci_attach(0)<0) {
  perror("pci_attach");
  exit(EXIT_FAILURE);
  }
			
  // Vendor and Device ID
  info.VendorId=0x1307;
  info.DeviceId=0x01;

  if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {
    perror("pci_attach_device");
    exit(EXIT_FAILURE);
  }

  // Determine assigned BADRn IO addresses for PCI-DAS1602
  //printf("\nDAS 1602 Base addresses:\n\n");
  for(i=0;i<5;i++) {
    badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
    //printf("Badr[%d] : %x\n", i, badr[i]);
  }

  // Map I/O base address to user space
  //printf("\nReconfirm Iobase:\n");
  for(i=0;i<5;i++) {			// expect CpuBaseAddress to be the same as iobase for PC
    iobase[i]=mmap_device_io(0x0f,badr[i]);
    //printf("Index %d : Address : %x ", i,badr[i]);
    //printf("IOBASE  : %x \n",iobase[i]);
  }

  // Modify thread control privity
  if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
    perror("Thread Control");
    exit(1);
  }																		

//******************************************************************************
// Digital Port Functions - There are primary & secondary ports 
//			    A/B/C and 4 bits of primary port
// 
// out8(DIO_CTLREG,0x90);   Port A:In; Port B:Out, Port C(upper|lower):Out|Out	
//
// No need to configure the basic 4 bit I/O 
// Secondary port needs to be configured like in notes for PCI board
//******************************************************************************																																	

// printf("\nDIO Functions\n"); 

// out8(DIO_CTLREG,0x90);	// Port A : Input,  Port B : Output,  Port C (upper | lower) : Output | Output			

// for (i=0;i<16;i++) {		// write out 
//   out8(DIO_PORTB,(i & 0x0f));	// Light up LED lowe 4 bits
//   printf("Light up LED %2x\n", (i & 0x0f));
//   delay(500);			// Pause 500 ms
//   }
// Initialise Board
out16(INTERRUPT,0x60c0);				    // sets interrupts	 - Clears
out16(TRIGGER,0x2081);					// sets trigger control: 10MHz, clear, Burst off,SW trig. default:20a0
out16(AUTOCAL,0x007f);					// sets automatic calibration : default

out16(AD_FIFOCLR,0); 						// clear ADC buffer
out16(MUXCHAN,0x0D00);          // previous group select 0x0C10

printf("PCI board initialized!\n\n");

/* 


                                                INTIALIZATION ENDS HERE


*/
// printf("\nRead Toggle Swtich\n");		 //TOGGLE SWITCH


// for (i=0;i<16;i++) {		// write out 
//   read_data= in8(DIO_PORTA);	// read switch //READ SWITCH
//   printf("Data Read : %2x\n", read_data & 0x0f);
//   out8(DIO_PORTB, read_data);	// Light up LED lower 4 bits //OUTPUT LED COLOR
//   delay(500);			// Pause 2 seconds
//   }


// printf("\nWaiting for DAC start\n");
// getchar();

//******************************************************************************
// D/A Port Functions
// Set to 5V, Unipolar 16 bit offset map. 0V->0x0000 mid >0x7fff 5V->0xffff
// PCIe : 12 bit, PCI : 16 bit
//******************************************************************************
	

																																					
// for (j = 0; j<0x0f;j++) {  //OUTPUT A SQUARE WAVE
//   for(i=0; i<0xffff; i++) {
//     out16(DA_CTLREG,0x0a23);	// DA Enable, #0,#1,SW 5V unipolar		
//     out16(DA_FIFOCLR,0);	// Clear DA FIFO buffer 16 bit D/A
//     out16(DA_Data,(short) i & 0xffff);	 																																	
   	  
//     out16(DA_CTLREG,0x0a43);	// DA Enable, #1,#1,SW V unipolar		//OUTPUT SWUARE WAVE
//     out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
//     out16(DA_Data, (short)((i>0x8000)? 0 : 0xffff));																																		
//     }
//   }  

// printf("Output to Scope Ended\n");
// printf("\nWaiting for ADC start\n");
// getchar();

//******************************************************************************
// ADC Port Functions
//******************************************************************************
											

// for(i=0;i<0x40;i++){
//   for(count=0;count<2;count++){
  
//     out16(AD_DATA,0); 		// start ADC 
//     while(!(in16(MUXCHAN) & 0x4000));
//   	adc_in[count]=in16(AD_DATA); 
//   	}
  	
//   printf("Chan#0 : %04x Chan#1 : %04x\n",adc_in[0],adc_in[1]);
//   fflush( stdout );
//   delay(100);									
//   }

while(1){
  readswitch();
}

printf("\nDemo End\n");
				// End of program - orderly shutdown

pci_detach_device(hdl);
return(0);
}  