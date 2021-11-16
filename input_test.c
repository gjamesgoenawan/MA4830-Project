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

int badr[5];		// PCI 2.2 assigns 6 IO base addresses
uintptr_t iobase[6]; 

void readswitch(input *paramsptr);
void readpot(input *paramsptr);

int main() {


struct pci_dev_info info;
void *hdl;

uintptr_t dio_in;
	
unsigned int i,j,read_data;
unsigned int stat,stat1,stat2, stat3;

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
  for(i=0;i<5;i++) {
    badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
  }

  // Map I/O base address to user space
  for(i=0;i<5;i++) {			// expect CpuBaseAddress to be the same as iobase for PC
    iobase[i]=mmap_device_io(0x0f,badr[i]);
  }

  // Modify thread control privity
  if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
    perror("Thread Control");
    exit(1);
  }																		

// Initialise Board
out16(INTERRUPT,0x60c0);				    // sets interrupts	 - Clears
out16(TRIGGER,0x2081);					// sets trigger control: 10MHz, clear, Burst off,SW trig. default:20a0
out16(AUTOCAL,0x007f);					// sets automatic calibration : default

out16(AD_FIFOCLR,0); 						// clear ADC buffer
out16(MUXCHAN,0x0D00);          // previous group select 0x0C10

printf("PCI board initialized!\n");
/* 
                                                INTIALIZATION ENDS HERE

*/
while(1){
  readpot(&inputs);
  readswitch(&inputs);
  printf("Amplitude : %.2f WaveType : %d\n",inputs.amp,inputs.waveType);
}

printf("\nDemo End\n");
				// End of program - orderly shutdown

pci_detach_device(hdl);
return(0);
}  

void readswitch(input *paramsptr){
  int switchval;

  switchval = in8(DIO_PORTA); //read the toggle switch state
  // printf("reading = %d\n",switchval);
  switch (switchval) //convert the value into corresponding waveType and store in a struct
  {
  case 240:
    paramsptr->waveType=NULL;
    break;
  case 241:
    paramsptr->waveType=1;
    break;
  case 242:
    paramsptr->waveType=2;
    break;
  case 244:
    paramsptr->waveType=3;
    break;
  case 248:
    paramsptr->waveType=4;
  default:
    break;
}

}

void readpot(input *paramsptr ){
  unsigned int count;
  uint16_t adc_in;
  unsigned short chan;
  int potentioval;

  count = 0x00; // Use ADC0
  chan = ((count & 0x0f)<<4) | (0x0f & count);
  out16(MUXCHAN,0x0D00|chan);
  delay(1);
  out16(AD_DATA,0);
  while(!(in16(MUXCHAN)&0x4000));
  potentioval = in16(AD_DATA); //read potentiometer value
  // printf("potentiometer value =%i\n",potentioval);
  paramsptr->amp=potentioval*2.5/65535; //scale the potentiometer value into amplitude and store the amplitude value in a struct
}