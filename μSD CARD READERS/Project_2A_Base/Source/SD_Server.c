/*
 *
 * Alex Dean, 2018
 * agdean@ncsu.edu
 *
 */ 

#include <MKL25Z4.h>
#include "sd_server.h"
#include "spi_io.h"
#include "sd_io.h"
#include "debug.h"

SDS_TD_T g_trans = {REQ_NONE, 0, (uint8_t * ) 0, 0, STAT_IDLE, SD_OK};

// Determines next state after S_IDLE based on request type
// Entries must be in order of declaration in SDSTD_T Request field
SDS_STATE_T Req_to_State[] = {S_IDLE, S_INIT, S_READ, S_WRITE};

void Update_Trans(SDS_TD_T * t, SDRESULTS res) {
	t->ErrorCode = res;
	t->Status = STAT_IDLE;
	t->Request = REQ_NONE; // Erase request code
}

void Task_SD_Server(void) {
	static SDS_STATE_T next_state = S_IDLE;
	// Local copy of transaction data, improves robustness
	static SDS_TD_T cur_trans;
	static SDRESULTS res;

	switch (next_state) {
		case S_IDLE:
				if (g_trans.Request != REQ_NONE) {
					cur_trans = g_trans; // Copy transaction request
					if (cur_trans.Request <= REQ_WRITE) {
						next_state = Req_to_State[cur_trans.Request];
						g_trans.Status = STAT_BUSY; 
					} else { // parameter error
						g_trans.Status = STAT_IDLE;
						g_trans.Request = REQ_NONE; // Erase request
						g_trans.ErrorCode = SD_PARERR; 
						next_state = S_IDLE; // Stay in idle state
					}
				}
			break;
		case S_INIT:
			DEBUG_START(DBG_4);
			res = SD_Init(cur_trans.Device);
			
		if(*instatus==1)
		{
			next_state=S_INIT;
		}
		else
		{	
		next_state = S_IDLE;
	  
		}	
		if(next_state == S_IDLE)
		{
		Update_Trans(&g_trans, res);
		}
		DEBUG_STOP(DBG_4);
			break;
		case S_READ:
			DEBUG_START(DBG_2);
			res = SD_Read(cur_trans.Device, cur_trans.Data, cur_trans.Sector, 0, 512);
			
		if(*rstatus==1)
		{
			next_state=S_READ;
		}
		else
		{
			next_state = S_IDLE;
			//Update_Trans(&g_trans, res);
		}
		if(next_state == S_IDLE)
		{
		Update_Trans(&g_trans, res);
		}
		DEBUG_STOP(DBG_2);
			break;
		case S_WRITE:
			DEBUG_START(DBG_3);
			res = SD_Write(cur_trans.Device, cur_trans.Data, cur_trans.Sector);
			 
		if(*status==1)
		{
			next_state=S_WRITE;
		}
		else
		{
			next_state = S_IDLE;
		//Update_Trans(&g_trans, res);	
		}
		if(next_state == S_IDLE)
		{
		Update_Trans(&g_trans, res);
		}
		DEBUG_STOP(DBG_3);
		break;
		case S_ERROR:
			while (1)
				;	// Optional: Add your code to handle the error here
			break;
		default:
			break;
	}
}
