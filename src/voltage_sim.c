/**
 * @file voltage_sim.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker Voltage Simulation module implementation.  This
 * module shall provide a software abstraction of the voltage monitoring ADC.
 * The underlying data store shall contain the following data:
 *
 *        |   0   |   1   |   2   |   3   |
 * ========================================
 * 0x0000 | voltage (U16) |
 * 0x0004
 *
 * where voltage is in mV.
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
 * 07/02/20  NH  Initial commit
 */
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "voltage_sim.h"
#include <stdio.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Compass descriptor
 */
typedef struct Voltage_Desc_
{
    FILE* pFile;    //!< File handle to the underlying data store
    char* path;     //!< String containing the path to the underlying data store
}Voltage_Desc_t;

/**
 * Compass simulation file map.  This struct should have the same layout as the
 * underlying data store, and can be used if memory mapped files are avilable.
 */
typedef struct Voltage_Sim_File_
{
    uint16_t heading;     //!< Simulated heading in degrees from Magnetic North
}Voltage_Sim_File_t;
/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
/**
 * Singleton compass descriptor.
 */
static Voltage_Desc_t voltageDesc;
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initializes the Voltage module
 * @return         1 if successful, 0 otherwise
 */
int Voltage_Init(void)
{
    /*
     * This function should open the file at voltageDesc.path and populate the
     * voltageDesc.pFile variable.
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
 * Reads the current voltage.  This is the voltage of the 5V rail as measured by
 * the onboard ADC, and should be output in mV.
 * @param   pValue  Pointer to an uint16_t variable to populate.
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Read(uint16_t *pValue)
{
    /*
     * This function should read out the double stored address 0 of the file in
     * pFile using standard I/O functions and return it in pValue.  It should 
     * return 0 if the Voltage_Init function has not yet been called.
     */
    
    // FIXME
    return 1;
}

/**
 * Initializes the simulator aspects of the Voltage module
 * @param  pConfig Pointer to the configuration struct
 * @return         1 if successful, otherwise 0
 */
int Voltage_Sim_Init(Voltage_Sim_Config_t* pConfig)
{
    /*
     * This function should set voltageDesc.path to point to the path specified
     * in pConfig
     */
    
    // FIXME
    return 1;
}

/**
 * Deinitializes the Voltage module
 */
void Voltage_Sim_Deinit(void)
{
    /*
     * This function should close any open file descriptors, and zero out
     * voltageDesc.
     */
    
    // FIXME
    return;
}