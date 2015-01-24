// *
// * Rover5.p

.origin 0
.entrypoint MEMACCESS_DDR_PRUSHAREDRAM_PRU0

#include "Rover5.hp"

MEMACCESS_DDR_PRUSHAREDRAM_PRU0:

    // Enable OCP master port
    LBCO	r0, CONST_PRUCFG, 4, 4
    CLR		r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    SBCO	r0, CONST_PRUCFG, 4, 4

    // Configure the programmable pointer register for PRU0 by setting c28_pointer[15:0]
    // field to 0x0120.  This will make C28 point to 0x00012000 (PRU shared RAM).
    MOV		r0, SHARED_RAM
    MOV		r1, PRU0_CTRL + CTPPR0
    ST32	r0, r1

    // Configure the programmable pointer register for PRU0 by setting c31_pointer[15:0]
    // field to 0x0010.  This will make C31 point to 0x80001000 (DDR memory).
    MOV		r0, DDR_RAM
    MOV		r1, PRU0_CTRL + CTPPR1
    ST32	r0, r1   //ST32 is custom macro defined in .hp

    //Load values from external DDR Memory into Registers R0/R1/R2
    LBCO	r0, CONST_DDR, 0, 12

    //Store values from read from the DDR memory into PRU shared RAM
    SBCO	r0, CONST_PRUSHAREDRAM, 0, 12

// **  	define some variables to work with	**
// *************************************************************************************
#define TRUE		1
#define	FALSE		0
#define PING_CNT	r4
#define PING_RDY	r5
#define PING_DUR	r6
#define PING_PIN	r30.t5
// *************************************************************************************
    
// **	initialize variables in shared memory	**
// *************************************************************************************
	MOV		PING_CNT, FALSE						//Clear/initialize ping counter
	SBCO 	PING_CNT, CONST_PRUSHAREDRAM, 16, 4	//save ping counter bit in shared ram
	
	MOV 	PING_RDY, FALSE						//ping was returned, let c program know
	SBCO	PING_RDY, CONST_PRUSHAREDRAM, 20, 4	//save ping ready to shared ram
// *************************************************************************************

LOOP0:
	LBCO	PING_RDY, CONST_PRUSHAREDRAM, 20, 4	//check to see if c program is ready for another ping
	QBNE	LOOP0, PING_RDY, FALSE				//if PING_RDY is TRUE then there is ping data that hasn't 
												//been retrieved yet so wait here LOOP0		

LOOP1:
	SET 	PING_PIN		//Turn on ping pin
    
    MOV 	r0, 2000 		//set delay for 20us
DEL1:
    SUB 	r0, r0, 1		//decrement delay counter
    QBNE 	DEL1, r0, 0		//if delay counter not 0 then return to DEL1
    
    CLR 	PING_PIN  		//turn off ping pin
    
// ** 	Wait for ping return
LOOP3: 
    LSR  	r0, r31, 1    			// Put r31 bit 16 (pru pin r31.t16) in r0 bit 15   
    LSR  	r0, r0.w0, 15 			// Zero extend r0 word 0, and shift right so bit 15 is bit 0
    QBNE	LOOP3, r0, TRUE			//if ping pin is low, wait here for ping return to start
    
 // **	Count ping duration
    MOV 	PING_DUR, 0 			//clear ping duration counter
LOOP2: 
    LSR  	r0, r31, 1     			// Put r31 (input register) bit 16 in r0 bit 15   
    LSR  	r0, r0.w0, 15  			// Zero extend r0 word 0, and shift right so bit 15 is bit 0
    ADD 	PING_DUR, PING_DUR, 1	//increment counter 	
	QBNE 	LOOP2, r0, FALSE  		//if pin is not 0 then loop	
	
	ADD		PING_CNT, PING_CNT, 1				//increment ping counter 
	SBCO 	PING_CNT, CONST_PRUSHAREDRAM, 16, 4	//save ping counter to shared ram	
	
	SBCO 	PING_DUR, CONST_PRUSHAREDRAM, 12, 4	//move counter value into shared ram
    											
    MOV 	PING_RDY, TRUE						//ping was returned, let c program know data ready
	SBCO	PING_RDY, CONST_PRUSHAREDRAM, 20, 4	//save ping ready to shared ram	
    
    JMP 	LOOP0		//jump back to beginning to restart ping procedure

    // Send notification to Host for program completion
    MOV       r31.b0, PRU0_ARM_INTERRUPT+16

    // Halt the processor
    HALT


