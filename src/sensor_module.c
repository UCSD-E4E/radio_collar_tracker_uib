#include "compass_sim.h"
#include "nmea.h"
#include "voltage_sim.h"
#include "sensor_encoder.h"
#include "sensor_module.h"
#include <time.h>

extern NMEA_Data_u NMEA_Data;
extern uint32_t encodeSensorPacket(DataSensorPacket_t* data, uint8_t* buf, uint32_t len);
extern NMEA_Message_e NMEA_Decode(char c);

int16_t readCompass(void)
{
    Compass_Sim_Config_t simConfig;
    Compass_Config_t compassConfig;
    int16_t compassValue;
    simConfig.path = "dataStore.bin";
    compassConfig.measMode = Compass_Mode_Continuous;

    Compass_Sim_Init(&simConfig);
    Compass_Init(&compassConfig);
    Compass_Read(&compassValue);
    Compass_Sim_Deinit();
    return compassValue;
}

int16_t readVoltage(void)
{
    Voltage_Sim_Config_t simConfig;
    uint16_t voltageValue;
    simConfig.path = "voltageSim.bin";

    Voltage_Sim_Init(&simConfig);
    Voltage_Init();
    Voltage_Read(&voltageValue);
    Voltage_Sim_Deinit();
    return voltageValue;
}

uint64_t changeTime(int *year, uint8_t *month, uint8_t *day, uint8_t hms[3])
{
    struct tm t;
    time_t t_of_day;

    t.tm_year = *year - 1900;      // Year - 1900
    t.tm_mon = *month - 1;           // Month, where 0 = january
    t.tm_mday = *day;            // Day of the month
    t.tm_hour = hms[0];
    t.tm_min = hms[1];
    t.tm_sec = hms[2];
    t.tm_isdst = -1;            // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);

    return t_of_day;
}

void setPacket(DataSensorPacket_t *packet)
{
    packet->sync_char1 = 0xE4;
    packet->sync_char2 = 0xEB;
    packet->packet_class = 0x04;
    packet->packet_id = 0x03;
    packet->payload.version = 0x01;
    packet->payload_length = sizeof(DataSensorInput_t);
    packet->payload.heading = readCompass(); //the number in dataStore.bin (pValue);
    packet->payload.voltage = readVoltage(); //the number in voltageSim.bin (pValue);
}

void encodePacket(DataSensorPacket_t *packet)
{
    uint8_t buffer[256];
    uint32_t length;
    length = encodeSensorPacket(packet, buffer, sizeof(buffer));
    for(int i = 0; i < length; i ++)
    {
        printf("%02x ", buffer[i]);
        if((i % 2) == 0)
        {
            printf("\b");
        }
    }
    printf("\n");
}

int sensorParse(char c)
{
    static int ymd_ready = 0; // year month day ready
    static int location_rdy = 0; // location ready
    static int alt_rdy = 0;
    static int type_set = 0;
    static int tmp_year;
    static uint8_t tmp_month;
    static uint8_t tmp_day;
    static int ZDA_ymd_set = 0;
    static DataSensorPacket_t packet;

    packet.payload.fixType = 1;

    switch(NMEA_Decode(c))
    {
        case NMEA_MESSAGE_GGA:
            {
                setPacket(&packet);

                packet.payload.fixType = NMEA_Data.GGA.fixQuality;
                type_set = 1;

                if(NMEA_Data.GGA.fixQuality != 0)
                {
                    packet.payload.latitude = (int32_t) (NMEA_Data.GGA.latitude * 1e7);
                    packet.payload.longitude = (int32_t) (NMEA_Data.GGA.longitude * 1e7);
                    packet.payload.altitude = (uint16_t) (NMEA_Data.GGA.altitude * 10);
                    location_rdy = 1;
                    alt_rdy = 1;

                    if(ZDA_ymd_set == 1)
                    {
                        packet.payload.time = changeTime(&tmp_year, &tmp_month, &tmp_day, NMEA_Data.GGA.fixTime);
                        ymd_ready = 1;
                    }
                }
            }
            break;
        case NMEA_MESSAGE_GLL:
            {
                setPacket(&packet);

                if(NMEA_Data.GLL.fixType != 'V')
                {
                    packet.payload.latitude = (int32_t) (NMEA_Data.GLL.latitude * 1e7);
                    packet.payload.longitude = (int32_t) (NMEA_Data.GLL.longitude * 1e7);
                    location_rdy = 1;

                    if(ZDA_ymd_set == 1)
                    {
                        packet.payload.time = changeTime(&tmp_year, &tmp_month, &tmp_day, NMEA_Data.GLL.fixTime);
                        ymd_ready = 1;
                    }
                }
                else
                {
                    packet.payload.fixType = 0;
                    type_set = 1;
                }
            }
            break;
        case NMEA_MESSAGE_ZDA:
            {
                setPacket(&packet);

                if(NMEA_Data.ZDA.fixTime[0] != 0xFF && NMEA_Data.ZDA.fixTime[1] != 0xFF && NMEA_Data.ZDA.fixTime[2] != 0xFF) //ZDA has no fixType like input
                {
                    packet.payload.time = changeTime(&NMEA_Data.ZDA.year, &NMEA_Data.ZDA.month, &NMEA_Data.ZDA.day, NMEA_Data.ZDA.fixTime);
                    tmp_year = NMEA_Data.ZDA.year;
                    tmp_month = NMEA_Data.ZDA.month;
                    tmp_day = NMEA_Data.ZDA.day;
                    ZDA_ymd_set = 1;
                    ymd_ready = 1;
                }
                else
                {
                    packet.payload.fixType = 0;
                    type_set = 1;
                }
            }
            break;
        case NMEA_MESSAGE_RMC:
            {
                // setPacket(&packet); // sets RMCpacket
                // ymd_ready = 1;
            }
        default:
            break;
    }
    if(ymd_ready == 1 && location_rdy == 1 && alt_rdy == 1 && type_set == 1)
    {
        encodePacket(&packet);
        memset(&packet, 0, sizeof(DataSensorPacket_t));
        ymd_ready = 0;
        alt_rdy = 0;
        location_rdy = 0;
        type_set = 0;
        return PARSE_COMPLETE;
    }
    return PARSE_NOT_COMPLETE;
}