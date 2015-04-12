/*
 * PRU.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: User
 */

#include "PRU.h"

//static int mem_fd;			//only needed for DDR RAM access
//static void *ddrMem;
static void *sharedMem;
static unsigned int *sharedMem_int;

PRU::PRU() {
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	prussdrv_init (); 									//Initialize the PRU
	unsigned int ret;
	ret = prussdrv_open(PRU_EVTOUT_0);					//Open PRU Interrupt
	if (ret){
		std::cout << "prussdrv_open open failed" << std::endl;
	}
	prussdrv_pru_enable (PRU_NUM0);						//enable PRU0
	prussdrv_pru_enable (PRU_NUM1);						//enable PRU0
	prussdrv_pru_enable (1);							//enable PRU1
	prussdrv_pruintc_init(&pruss_intc_initdata);		//Get the interrupt initialized

	/* map the DDR memory */
	/*ddrMem = mmap(0, 0x0FFFFFFF, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, DDR_BASEADDR);
		if (ddrMem == NULL) {
			std::cout << "Failed to map the device " << std::strerror(errno) << std::endl;
			close(mem_fd);
		}*/

	//Allocate Shared PRU memory
	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &sharedMem);
	sharedMem_int = (unsigned int*) sharedMem;

	//initialize motion variables
	motion_vars.period = 50;
	motion_vars.lDir = FORWARD;
	motion_vars.rDir = FORWARD;
	motion_vars.lPWMDuty = 0;
	motion_vars.rPWMDuty = 0;

	sonar_vars.distance = 100;

	//set initial PWM's to 00
	sharedMem_int[OFFSET_SHAREDRAM+9] = 0;		//zero left PWM duty
	sharedMem_int[OFFSET_SHAREDRAM+10] = 0;		//zero Right PWM Duty
	sharedMem_int[OFFSET_SHAREDRAM+11] = 0;		//Zero Left Position
	sharedMem_int[OFFSET_SHAREDRAM+12] = 0;		//Zero Right Position


	prussdrv_exec_program (PRU_NUM0, "./Rover5_PRU0.bin");	//start PRU0 code
	prussdrv_exec_program (PRU_NUM1, "./Rover5_PRU1.bin");	//start PRU1 code
}

// returns a ping distance in cm
// calls close to each other my corrupted by echo returns, call at least 200ms apart
int PRU::GetPing(){
	unsigned int duration;
	unsigned short distance;
	int ping_ready;
	int ping_counter = 0;
	int timeout = 0;
	do{
		ping_ready = sharedMem_int[OFFSET_SHAREDRAM+5];		//see if there's ping data in shared RAM
		timeout++;
		if(timeout>1000){									//error check timeout
			std::cerr << "PING PRU TIMEOUT" << std::endl;
			return -1;
		}
	}while(!ping_ready);									//if there's no data ready stay in loop
	duration = sharedMem_int[OFFSET_SHAREDRAM+3];			//get ping duration from shared RAM
	distance = (duration*4*5*340.29*100)/(1000000000*2);	//calculate distance from duration cm's
	ping_counter = sharedMem_int[OFFSET_SHAREDRAM+4];		//get ping count
	sharedMem_int[OFFSET_SHAREDRAM+5] = 0;					//let .p know ready for another ping
	return distance;
}

int PRU::GetLeftPos(){
	int leftPos = sharedMem_int[OFFSET_SHAREDRAM+11];
	return leftPos;
}

int PRU::GetRightPos(){
	int rightPos = sharedMem_int[OFFSET_SHAREDRAM+12];
	return rightPos;
}

int PRU::GetMotionVars(motion_struct *new_vars){
	*new_vars = motion_vars;
	return 1;
}

int PRU::SetMotionVars(motion_struct *new_vars){
	motion_vars = *new_vars;
	int calcPeriod = (1.0/motion_vars.period)/0.000000150;		//35usec from PRU.asm cycles
	sharedMem_int[OFFSET_SHAREDRAM+6] = calcPeriod;
	sharedMem_int[OFFSET_SHAREDRAM+7] = motion_vars.lDir;
	sharedMem_int[OFFSET_SHAREDRAM+8] = motion_vars.rDir;
	if (motion_vars.lPWMDuty == 0){
		sharedMem_int[OFFSET_SHAREDRAM+9] = 0;
	}else {
		sharedMem_int[OFFSET_SHAREDRAM+9] = calcPeriod*(motion_vars.lPWMDuty/100.0);
	}
	if (motion_vars.rPWMDuty == 0){
		sharedMem_int[OFFSET_SHAREDRAM+10] = 0;
	}else {
		sharedMem_int[OFFSET_SHAREDRAM+10] = calcPeriod*(motion_vars.rPWMDuty/100.0);
	}
	return 1;
}

PRU::~PRU() {
	//set PWM's to 0
	sharedMem_int[OFFSET_SHAREDRAM+9] = 0;
	sharedMem_int[OFFSET_SHAREDRAM+10] = 0;

	/* Disable PRU and close memory mapping*/
	prussdrv_pru_clear_event (PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
	prussdrv_pru_disable(PRU_NUM0);
	prussdrv_pru_disable(PRU_NUM1);
	prussdrv_exit ();
	//munmap(ddrMem, 0x0FFFFFFF);			//only needed if using DDR RAM
	//close(mem_fd);
	std::cout << "PRU object terminated" << std::endl;
}

