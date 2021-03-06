#include "MPU6050.h"
#include "i2c.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "filter.h"


#ifdef HIGH_FREQ_CTRL
#define IMU_SAMPLE_RATE 			200.0f
#else
#define IMU_SAMPLE_RATE 			100.0f	//1000.0f/(float)DMP_CALC_PRD
#endif

#define IMU_FILTER_CUTOFF_FREQ	30.0f


#define M_PI_F 3.1415926
#define CONSTANTS_ONE_G					9.80665f		/* m/s^2		*/
#define SENSOR_MAX_G 8.0f		//constant g		// tobe fixed to 8g. but IMU need to  correct at the same time
#define SENSOR_MAX_W 2000.0f	//deg/s
const float ACC_SCALE = (SENSOR_MAX_G/32768.0f) * CONSTANTS_ONE_G;
const float GYRO_SCALE = (SENSOR_MAX_W/32768.0f) * M_PI_F /180.f;


FILTER_HANDLE_T filter[6];

MPU6050_SENSOR_DATA_T sensorData = {0};

ErrorStatus MPU6050_WriteByte(uint8_t nAddress, uint8_t nData)
{
    return Hal_I2C_WriteRegister(&hi2c1, MPU6050_ADDRESS, nAddress, &nData, 1);
}
/******************************************************************
  * @函数说明：   读取设备ID，正常返回0X68
  * @输入参数：   无
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
uint8_t MPU6050_GetDeviceID(void)
{
    uint8_t data;
    ErrorStatus status;
    
    status = Hal_I2C_ReadRegister(&hi2c1, MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, &data, 1);
    
    if (status == SUCCESS)
    {
        return data;
    }
    else
    {
        return 0;
    }
}
/******************************************************************
  * @函数说明：  设置陀螺仪的范围
  * @输入参数：   uint8_t range 范围
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
void MPU6050_setFullScaleGyroRange(uint8_t range) 
{
    uint8_t data[MPU6050_ACONFIG_AFS_SEL_LENGTH];
    
    memset(data, range, MPU6050_ACONFIG_AFS_SEL_LENGTH);
    Hal_I2C_WriteRegister(&hi2c1, MPU6050_ADDRESS, MPU6050_ACONFIG_AFS_SEL_BIT, data, MPU6050_ACONFIG_AFS_SEL_LENGTH);
}





/******************************************************************
  * @函数说明：  读取MPU6050的测量数据
  * @输入参数：   无
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
MPU6050_SENSOR_DATA_T MPU6050_ReadSensor(void)
{
    
    
    static uint8_t data[12];
    float acc[3];
    float gyro[3];
    //data = osMalloc(12);
    Hal_I2C_ReadRegister(&hi2c1, MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, data, 12);   //读取数据
            
    acc[0] = (float)((int16_t)(data[0] << 8 | data[1]) *ACC_SCALE);  //转换
    acc[1] = (float)((int16_t)(data[2] << 8 | data[3]) *ACC_SCALE);
    acc[2] = (float)((int16_t)(data[4] << 8 | data[5]) *ACC_SCALE);
    
    gyro[0] = (float)((uint16_t)(data[6] << 8 | data[7]) * GYRO_SCALE) ;
    gyro[1] = (float)((uint16_t)(data[8] << 8 | data[9]) * GYRO_SCALE) ;
    gyro[2] = (float)((uint16_t)(data[10] << 8 | data[11]) * GYRO_SCALE);
    
    sensorData.accX = Filter_LPF2pApply(filter[0], acc[0]);
    sensorData.accY = Filter_LPF2pApply(filter[1], acc[1]);
    sensorData.accZ = Filter_LPF2pApply(filter[2], acc[2]);
    
    sensorData.gyroX = Filter_LPF2pApply(filter[3], gyro[0]) - sensorData.offsetGyroX;
    sensorData.gyroY = Filter_LPF2pApply(filter[4], gyro[1]) - sensorData.offsetGyroY;
    sensorData.gyroZ = Filter_LPF2pApply(filter[5], gyro[2]) - sensorData.offsetGyroZ;
    return sensorData;
    
}


/******************************************************************
  * @函数说明：  初始化MPU6050
  * @输入参数：   
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus MPU6050_initialize(void) 
{
    if (MPU6050_GetDeviceID() != 0x68)
    {
        return ERROR;
    }

	MPU6050_WriteByte(MPU6050_RA_PWR_MGMT_1, 0x80);      //PWR_MGMT_1    -- DEVICE_RESET 1
    osDelay(100);

    MPU6050_WriteByte(MPU6050_RA_SMPLRT_DIV, 0x00);      //SMPLRT_DIV    -- SMPLRT_DIV = 0  Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    MPU6050_WriteByte(MPU6050_RA_PWR_MGMT_1, 0x03);      //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
    MPU6050_WriteByte(MPU6050_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);  // INT_PIN_CFG   -- INT_LEVEL_HIGH, INT_OPEN_DIS, LATCH_INT_DIS, INT_RD_CLEAR_DIS, FSYNC_INT_LEVEL_HIGH, FSYNC_INT_DIS, I2C_BYPASS_EN, CLOCK_DIS
    MPU6050_WriteByte(MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);  //CONFIG        -- EXT_SYNC_SET 0 (disable input pin for data sync) ; default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
	
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
	//MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
    // Accel scale 8g (4096 LSB/g)
    MPU6050_WriteByte(MPU6050_RA_ACCEL_CONFIG, 2 << 3);
    
    uint8_t i;
    
    for (i = 0; i < 6; i++)
    {
        filter[i] = Filter_LPF2pCreate(IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
        if (filter[i] == NULL)
        {
            return ERROR;
        }
    }
    osDelay(500);
    float offsetSum[3] = {0.0f, 0.0f, 0.0f};
    
    for (i = 0; i < 10; i++)
    {
        osDelay(10);
        MPU6050_ReadSensor();
    }
    //MPU6050_SENSOR_DATA_T sensorData;
    for (i = 0; i < 32; i++)
    {
        osDelay(10);
        MPU6050_ReadSensor();
        offsetSum[0] += sensorData.gyroX;
        offsetSum[1] += sensorData.gyroY;
        offsetSum[2] += sensorData.gyroZ;
        
    }
    
    sensorData.offsetGyroX = offsetSum[0] / 32.0f;
    sensorData.offsetGyroY = offsetSum[1] / 32.0f;
    sensorData.offsetGyroZ = offsetSum[2] / 32.0f;
    
    return SUCCESS;
}



