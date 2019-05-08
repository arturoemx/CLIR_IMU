/*  Programa que realiza la inicialización de un IMU BNO055
 *  y realiza lecturas de cuaterniones. 
 *
 *  Dr. Arturo Espinosa Romero, Ing. Alex Antonio Turriza Suárez, 2019
 */

#include <cstdlib>
#include <iostream>
#include <BNO055-BBB_driver.h>
using namespace std;

	void BNO055::setAddress(unsigned char address)
	{
		if(ioctl(file, I2C_SLAVE, address) < 0)
		{
			cout << "Failed to acquire bus access and/or talk to slave." << endl;
			cout.flush();
			exit(-1);
		}
	}

	void BNO055::writeData(int n)
	{
		int wval;
		wval = write(file, data, n);
		if(wval != n)
		{
			cout << "Failed to write to i2c bus: " << wval << endl;
			strerror_r(errno, _buffer, 63);
			cout << _buffer << endl;
		}
		usleep(5000);
	}

	void BNO055::start(unsigned char quatadd)
	{
		x = y = z = w = 0;
		calGyro = calMag = calAcc = calSys = 0;
		imuAddress = quatadd;
		setAddress(imuAddress);

		/*Se envían parámetros de inicialización*/
		//Modo de configuración
		data[0] = BNO055_OPR_MODE_ADD;
		data[1] = OPERATION_MODE_CONFIG;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se envían parámetros de inicialización" << endl;
      cout.flush();
#endif

		//Explícitamente iniciamos en modo normal de uso
		data[0] = BNO055_PWR_MODE_ADD;
		data[1] = POWER_MODE_NORMAL;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Iniciamos en el modo normal de uso, en forma explicita." << endl;
      cout.flush();
#endif

		//Colocamos la página 0 del mapa de registros
		data[0] = BNO055_PAGE_ID_ADDR;
		data[1] = PAGE0;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se coloca la pagina 0 del amap de registros." << endl;
      cout.flush();
#endif

		//Reiniciamos
		data[0] = BNO055_SYS_TRIGGER_ADD;
		data[1] = RESET;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se reinicia el sensor." << endl;
      cout.flush();
#endif
		usleep(500000);

		//Fijamos las unidades de los sensores
		data[0] = BNO055_UNIT_SEL_ADDR;
		data[1] = UNIDADES_DEFAULT;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se definen las unidades de los sensores." << endl;
      cout.flush();
#endif

		//Reiniciamos el sensor
		data[0] = BNO055_SYS_TRIGGER_ADD;
		data[1] = 0x00;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se reinicia el sensor." << endl;
      cout.flush();
#endif

		//Modo de operación a utilizar
		data[0] = BNO055_OPR_MODE_ADD;
		data[1] = OPERATION_MODE_NDOF;
		writeData(2);
#ifdef __VERBOSE__
      cout << "Se define el modo de operacion a utilizar." << endl;
      cout.flush();
#endif

		usleep(20000);
#ifdef __VERBOSE__
      cout << "Termino la inicialización." << endl << endl;
      cout.flush();
#endif
	}

	BNO055::BNO055(char *filename)
	{
		if((file = open(filename, O_RDWR)) < 0)
		{
			perror("BNO055: failed to open i2c bus");
			exit(-2);
		}
		start();
		memset(data, 0, 16*sizeof(unsigned char));
	}

	BNO055::~BNO055()
	{
		close(file);
	}

	void BNO055::readCalibVals()
	{
		int rval;
		if(ioctl(file, I2C_SLAVE, BNO055_ADDRESS) < 0)
		{
			printf("Calib: Failed to acquire bus access and/or talk to slave");
			exit(-1);
		}
		data[0] = BNO055_CALIB_STAT_ADD;
		writeData(1);
		rval = read(file, data, 8);
		if(rval < 0)
		{
			printf("Calib: Failed to acquire bus access and/or talk to slave");
			strerror_r(errno, _buffer, 63);
			printf("%s\n\n", _buffer);
		}
		cout << "Raw data: " << data[0] << endl;
		calSys = int8_t ((data[0] >> 6) & 0x03);
		calGyro = int8_t ((data[0] >> 4) & 0x03);
		calAcc = int8_t ((data[0] >> 2) & 0x03);
		calMag = int8_t ((data[0]) & 0x03);
	}

	void BNO055::readQuatVals()
	{
		int cont, rval;
		cont = 0;
		if(ioctl(file, I2C_SLAVE, BNO055_ADDRESS) < 0)
		{
			printf("Quat: Failed to acquire bus access and/or talk to slave.\n");
			exit(-1);
		}
		data[0] = BNO055_QUATDATA_ADD;
		writeData(1);
		do
        	{
            		rval = read(file,data+cont,8);
            		if (rval < 0)
            		{
                		/* ERROR HANDLING: i2c transaction failed */
                		printf("Quat: Failed to read to the i2c bus.\n");
                		strerror_r(errno, _buffer, 63);
                		printf("%s\n\n", _buffer);
            		}
            		else
                		cont += rval;
        	} 
		while (cont < 8);
		w = (int16_t) ((data[1] << 8) | data[0]); //W
		x = (int16_t) ((data[3] << 8) | data[2]); //X
		y = (int16_t) ((data[5] << 8) | data[4]); //Y
		z = (int16_t) ((data[7] << 8) | data[6]); //Z
	}