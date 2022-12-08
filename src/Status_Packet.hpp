#ifndef __STATUS_PACKET__
#define __STATUS_PACKET__
/*! \file */
/**
 * GPS Subsystem state.
 */
typedef enum{
	GPS_INIT = 0,
	GPS_READY = 1,
	GPS_FAIL = 2,
	GPS_RETRY = 3
} GPSState;

/**
 * Storage Subsystem state.
 */
enum StorageState{
	STR_GET_OUTPUT_DIR = 0,
	STR_CHECK_OUTPUT_DIR = 1,
	STR_CHECK_SPACE = 2,
	STR_WAIT_RECYCLE = 3,
	STR_READY = 4,
	STR_FAIL = 5,
	STR__SIZE
};

/**
 * SDR Subsystem state.
 */
enum SDRState{
	SDR_FIND_DEVICES = 0,
	SDR_WAIT_RECYCLE = 1,
	SDR_USRP_PROBE = 2,
	SDR_READY = 3,
	SDR_FAIL = 4,
	SDR__SIZE
};

/**
 * Control subystem state.
 */
enum SystemState{
	SYS_INIT = 0,
	SYS_WAIT_INIT = 1,
	SYS_WAIT_START = 2,
	SYS_START = 3,
	SYS_WAIT_FOR_END = 4,
	SYS_FINISH = 5,
	SYS_FAIL = 6,
	SYS__SIZE
};

/**
 * System Status struct.  This structure contains the last known states of each
 * subsystem.
 */
typedef struct StatusPacket{
	StorageState storage;
	SDRState sdr;
	SystemState system;
	GPSState gps;
} StatusPacket;

#endif
