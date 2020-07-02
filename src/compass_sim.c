/**
 * @file compass_sim.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker Compass Simulation module implementation.  This
 * module shall provide a software abstraction of the HMC5983 device.  The
 * underlying data store shall contain the following data:
 *
 *        |   0   |   1   |   2   |   3   |
 * ========================================
 * 0x0000 |         heading (f32)         |
 * 0x0004
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * DATE      WHO DESCRIPTION
 * ----------------------------------------------------------------------------
 * 07/01/20  NH  Initial commit
 */
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass_sim.h"
#include <stdio.h>
#include <math.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Compass descriptor
 */
typedef struct Compass_Desc_
{
    FILE* pFile;    //!< File handle to the underlying data store
    char* path;     //!< String containing the path to the underlying data store
}Compass_Desc_t;

/**
 * Compass simulation file map.  This struct should have the same layout as the
 * underlying data store, and can be used if memory mapped files are avilable.
 */
typedef struct Compass_Sim_File_
{
    double heading;     //!< Simulated heading in degrees from Magnetic North
}Compass_Sim_File_t;
/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
/**
 * Singleton compass descriptor.
 */
static Compass_Desc_t compassDesc;
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initializes the Compass module
 * @param  pConfig Pointer to a Compass_Config_t struct with the desired 
 *                 configuration parameters
 * @return         1 if successful, 0 otherwise
 */
int Compass_Init(Compass_Config_t* pConfig)
{
    /*
     * This function should open the file at compassDesc.path and populate the
     * compassDesc.pFile variable.  It can safely ignore the parameters in
     * pConfig, as those will not be used in the simulator, but will be used
     * in the final hardware-specific module.
     *
     * If the file does not exist, it should be created and populated with
     * a random initial value.
     *
     * If the file exists, this function should check that it is populated.  If
     * not populated, the file should be populated with random values.
     */
    
    // FIXME
    return 1;
}

/**
 * Reads the current heading from the compass module.  This is the magnetic
 * field vector projected onto the XY plane, reported in +/- decimal degrees
 * from magnetic North.
 * @return  Magnetic heading in decimal degrees, range +/- 180.  NaN if not
 *                   initialized
 */
double Compass_Read(void)
{
    /*
     * This function should read out the double stored address 0 of the file in
     * pFile using standard I/O functions and return it.  It should return NaN
     * if the Compass_Init function has not yet been called.
     */
    
    // FIXME
    return NAN;
}

/**
 * Initializes the simulator aspects of the Compass module
 * @param  pConfig Pointer to the configuration struct
 * @return         1 if successful, otherwise 0
 */
int Compass_Sim_Init(Compass_Sim_Config_t* pConfig)
{
    /*
     * This function should set compassDesc.path to point to the path specified
     * in pConfig
     */
    
    // FIXME
    return 1;
}

/**
 * Deinitializes the Compass module
 */
void Compass_Sim_Deinit(void)
{
    /*
     * This function should close any open file descriptors, and zero out
     * compassDesc.
     */
    
    // FIXME
    return;
}