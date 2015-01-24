// *
// * Rover5.p

.origin 0
.entrypoint MEMACCESS_DDR_PRUSHAREDRAM_PRU1

#include "Rover5.hp"

MEMACCESS_DDR_PRUSHAREDRAM_PRU1:

    // Enable OCP master port
    LBCO	r0, CONST_PRUCFG, 4, 4
    CLR		r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    SBCO	r0, CONST_PRUCFG, 4, 4

    // Configure the programmable pointer register for PRU0 by setting c28_pointer[15:0]
    // field to 0x0120.  This will make C28 point to 0x00012000 (PRU shared RAM).
    MOV		r0, SHARED_RAM
    MOV		r1, PRU1_CTRL + CTPPR0
    ST32	r0, r1   

// **  	define some variables to work with	**
// *************************************************************************************
#define TRUE		1
#define	FALSE		0
#define PERIOD		r6
#define L_DIR		r7
#define R_DIR		r8
#define L_PWM_DUTY	r9
#define R_PWM_DUTY	r10
// *************************************************************************************

LOOP1:
// some fake values to test
//	MOV		PERIOD, 571428
//	MOV		L_PWM_DUTY, 285714
//	MOV		R_PWM_DUTY, 142857
//	MOV		L_DIR, FALSE
//	MOV		R_DIR, TRUE
//	LBCO	r0, CONST_PRUSHAREDRAM, 24, 20			//load in registers from shared memory
	LBCO 	PERIOD, CONST_PRUSHAREDRAM, 24, 4		//save ping counter bit in shared ram	
	LBCO 	L_DIR, CONST_PRUSHAREDRAM, 28, 4		//save ping counter bit in shared ram
	LBCO 	R_DIR, CONST_PRUSHAREDRAM, 32, 4		//save ping counter bit in shared ram
	LBCO 	L_PWM_DUTY, CONST_PRUSHAREDRAM, 36, 4	//save ping counter bit in shared ram
	LBCO 	R_PWM_DUTY, CONST_PRUSHAREDRAM, 40, 4	//save ping counter bit in shared ram
	QBNE	CLDIR, L_DIR, TRUE						//set direction for Left track
	SET		r30.t0
	JMP		RDIR
CLDIR:
	CLR		r30.t0
RDIR:
	QBNE	CRDIR, R_DIR, TRUE					//set direction for Right track
	SET		r30.t1
	JMP		PWM
CRDIR:
	CLR		r30.t1
PWM:							
	SET     r30.t2								//Turn on Left PWM
    SET     r30.t3								//Turn on Right PWM
LOOP2:    
    QBNE    SKIP1, L_PWM_DUTY, 0                // Compare each register with zero
    CLR     r30.t2                      		// If zero then turn off the corresponding channel
SKIP1:                                      	// Otherwise skip that channel (leaving it high)
    QBNE    SKIP2, R_PWM_DUTY, 0
    CLR     r30.t3
SKIP2:	
	QBEQ    LOOP1, PERIOD, 0            		// If cycle register is empty, restart
	SUB     PERIOD, PERIOD, 1					// Subtract one from Period
    SUB     L_PWM_DUTY, L_PWM_DUTY, 1			// Subtract one from each PWM register
    SUB     R_PWM_DUTY, R_PWM_DUTY, 1          	
    QBA     LOOP2                   			// Total of 7 statements per cycle, 35ns
    											// Set Period register to 571428

    // Halt the processor
    HALT


