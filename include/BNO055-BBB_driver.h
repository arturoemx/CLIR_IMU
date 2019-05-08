#ifndef __BNO055_DRIVER__
#define  __BNO055_DRIVER__

/*  Programa que realiza la inicialización de un IMU BNO055
 *  y realiza lecturas de cuaterniones. 
 *
 *  Dr. Arturo Espinosa Romero, Ing. Alex Antonio Turriza Suárez, 2019
 */

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

/*DIRECCIÓN DEL IMU*/
#define BNO055_ADDRESS 0x28

/*REGISTRO DE PÁGINA DE MAPA DE REGS*/
#define BNO055_PAGE_ID_ADDR 0x07
#define PAGE0 0x00
#define PAGE1 0x01

/*REGISTROS DE SELECCIÓN DE UNIDADES*/
#define BNO055_UNIT_SEL_ADDR 0x3B
#define BNO055_DATA_SELECT_ADDR 0x3C

/*UNIDADES A UTILIZAR*/
#define UNIDADES_DEFAULT 0x82 //1 <- Android orientation
			      //0 <- Reservado
			      //0 <- Reservado
			      //0 <- Grados celsius
			      //0 <- Reservado
			      //0 <- Grados
			      //1 <- Rps
			      //0 <- m/s² 

/*REGISTROS DE DATOS*/
#define BNO055_QUATDATA_ADD 0x20 // WLSB, WMSB, X, Y, Z [0x20 -> 0x27]

/*REGISTRO DE CALIBRACIÓN*/
#define BNO055_CALIB_STAT_ADD 0x35

/*REGISTROS DE OPERACIÓN*/
#define BNO055_OPR_MODE_ADD 0x3D
#define BNO055_PWR_MODE_ADD 0x3E
#define BNO055_SYS_TRIGGER_ADD 0x3D

/*OPERACIONES*/
#define RESET 0x20

/*MODOS DE OPERACIÓN*/
#define OPERATION_MODE_CONFIG 0x00
#define OPERATION_MODE_IMU 0x08
#define OPERATION_MODE_NDOF_FMC_OFF 0x0B
#define OPERATION_MODE_NDOF 0x0C

/*MODO DE ENERGÍA*/
#define POWER_MODE_NORMAL 0x00

using namespace std;

struct BNO055
{
	int file; //Descriptor
	unsigned char data[16];
	char _buffer[8];
	unsigned char imuAddress;
	int16_t x, y, z, w;
	int8_t calGyro, calMag, calAcc, calSys;
	const double scale = (1.0 / (1 << 14));

	void setAddress(unsigned char address);
	void writeData(int n);
	void start(unsigned char quatadd = BNO055_ADDRESS);
	BNO055(char *filename);
	~BNO055();
	void readCalibVals();
	void readQuatVals();
};
#endif

