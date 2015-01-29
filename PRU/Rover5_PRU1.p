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
#define L_ENC		r11
#define R_ENC		r12
#define L_ENC_PREV	r13
#define R_ENC_PREV	r14
#define L_POS		r15
#define R_POS		r16
// *************************************************************************************

//	**	Clear position counts in shared RAM	**
	SBCO	0, CONST_PRUSHAREDRAM, 44, 4	//* save Left Position to shared ram	
	SBCO	0, CONST_PRUSHAREDRAM, 48, 4	//* save Right Position to shared ram
	
//	**	reset position counters	**
	MOV		L_POS, 0
	MOV		R_POS, 0	

//initialize encoder previous readings
	LSL  	r0, r31, 10    			// Put r31 bits 4&5 (pru pin r31.t4&5) in r0 bit 14&15
    LSR  	r0, r0.w0, 14 			// Zero extend r0 word 0, and shift right so bit 14 is bit 0	
    MOV		L_ENC_PREV, r0			// put reading into left encoder register
    
    LSL  	r0, r31, 8    			// Put r31 bits 6&7 (pru pin r31.t6&7) in r0 bit 14&15
    LSR  	r0, r0.w0, 14 			// Zero extend r0 word 0, and shift right so bit 14 is bit 0	
    MOV		R_ENC_PREV, r0			// put reading into right encoder register

LOOP1:
	LBCO 	PERIOD, CONST_PRUSHAREDRAM, 24, 4		//Load Period from shared ram	
	LBCO 	L_DIR, CONST_PRUSHAREDRAM, 28, 4		//Load Left Direction from shared ram
	LBCO 	R_DIR, CONST_PRUSHAREDRAM, 32, 4		//Load Right Direction from shared ram
	LBCO 	L_PWM_DUTY, CONST_PRUSHAREDRAM, 36, 4	//Load Left Duty from shared ram
	LBCO 	R_PWM_DUTY, CONST_PRUSHAREDRAM, 40, 4	//Load Right Duty from shared ram
	
// **	couple if-else statements to set direction commands	**
	QBNE	CLDIR, L_DIR, TRUE					//set direction for Left track
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
	
//	**	Run one period of PWM	**	
PWM:							
	SET     r30.t2								// Turn on Left PWM
    SET     r30.t3								// Turn on Right PWM
LOOP2:    
    QBNE    SKIP1, L_PWM_DUTY, 0                //* Compare each register with zero
    CLR     r30.t2                      		// If zero then turn off the corresponding channel
SKIP1:                                      	// Otherwise skip that channel (leaving it high)
    QBNE    SKIP2, R_PWM_DUTY, 0				//*
    CLR     r30.t3
SKIP2:	
	QBEQ    LOOP1, PERIOD, 0            		//* If cycle register is empty, restart
	SUB     PERIOD, PERIOD, 1					//* Subtract one from Period
    SUB     L_PWM_DUTY, L_PWM_DUTY, 1			//* Subtract one from each PWM register
    SUB     R_PWM_DUTY, R_PWM_DUTY, 1          	//*
    
//	**	Encoder reading	**
	LSL  	r0, r31, 10    			//* Put r31 bits 4&5 (pru pin r31.t4&5) in r0 bit 14&15
    LSR  	r0, r0.w0, 14 			//* Zero extend r0 word 0, and shift right so bit 14 is bit 0	
    MOV		L_ENC, r0				//* put reading into left encoder register
    
    LSL  	r0, r31, 8    			//* Put r31 bits 6&7 (pru pin r31.t6&7) in r0 bit 14&15
    LSR  	r0, r0.w0, 14 			//* Zero extend r0 word 0, and shift right so bit 14 is bit 0	
    MOV		R_ENC, r0				//* put reading into right encoder register
    
//	**	Process left encoder readings	**    
    QBEQ	L_ENC_0, L_ENC, 0		//* switch case conditional jump 
    QBEQ	L_ENC_1, L_ENC, 1		//*
    QBEQ	L_ENC_2, L_ENC, 2		//*	only counted every other period
    QBEQ	L_ENC_3, L_ENC, 3
    
//	**	Switch cases, jump to add or subtract based on previous value	**
L_ENC_0:
	QBEQ	L_POS_ADD, L_ENC_PREV, 2	//*
	QBEQ	L_POS_SUB, L_ENC_PREV, 1
	QBA		R_SWITCH				//*
L_ENC_1:
	QBEQ	L_POS_ADD, L_ENC_PREV, 0
	QBEQ	L_POS_SUB, L_ENC_PREV, 3
	QBA		R_SWITCH
L_ENC_2:
	QBEQ	L_POS_SUB, L_ENC_PREV, 0
	QBEQ	L_POS_ADD, L_ENC_PREV, 3	
	QBA		R_SWITCH
L_ENC_3:
	QBEQ	L_POS_SUB, L_ENC_PREV, 2
	QBEQ	L_POS_ADD, L_ENC_PREV, 1	
	QBA		R_SWITCH
	
L_POS_ADD:
	ADD		L_POS, L_POS, 1			//*
	QBA		R_SWITCH				//*
L_POS_SUB:
	SUB		L_POS, L_POS, 1	
	
R_SWITCH:	
//	**	Process right encoder readings	**	
	QBEQ	R_ENC_0, R_ENC, 0			//* switch case conditional jump 
    QBEQ	R_ENC_1, R_ENC, 1			//*
    QBEQ	R_ENC_2, R_ENC, 2			//*
    QBEQ	R_ENC_3, R_ENC, 3
    
//	**	Switch cases, jump to add or subtract based on previous value	**
R_ENC_0:
	QBEQ	R_POS_ADD, R_ENC_PREV, 2		//*
	QBEQ	R_POS_SUB, R_ENC_PREV, 1
	QBA		POS_ROLL					//*
R_ENC_1:
	QBEQ	R_POS_ADD, R_ENC_PREV, 0
	QBEQ	R_POS_SUB, R_ENC_PREV, 3
	QBA		POS_ROLL
R_ENC_2:
	QBEQ	R_POS_SUB, R_ENC_PREV, 0
	QBEQ	R_POS_ADD, R_ENC_PREV, 3	
	QBA		POS_ROLL
R_ENC_3:
	QBEQ	R_POS_SUB, R_ENC_PREV, 2
	QBEQ	R_POS_ADD, R_ENC_PREV, 1	
	QBA		POS_ROLL
	
R_POS_ADD:
	ADD		R_POS, R_POS, 1				//*
	QBA		POS_ROLL					//*
R_POS_SUB:
	SUB		R_POS, R_POS, 1

//	**	Position roll over protection	**
POS_ROLL:

	SBCO	L_POS, CONST_PRUSHAREDRAM, 44, 4	//* save Left Position to shared ram	
	SBCO	R_POS, CONST_PRUSHAREDRAM, 48, 4	//* save Right Position to shared ram
	
	MOV		L_ENC_PREV, L_ENC					//* copy Left encoder value into previous
	MOV		R_ENC_PREV, R_ENC					//* copy Right encoder value into previous

	QBA     LOOP2								//* Total of 30 statements per cycle, 5ns per instruction = 150ns
    											// Set Period register to Freq/clock ie. 50hz = 1/50/150ns = 133333

    // Halt the processor
    HALT


