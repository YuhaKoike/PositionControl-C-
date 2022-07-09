#include "pch.h"

//#include <iostream>

//add by YK
#include "lib_YK.h"

HANDLE		hDeviceHandle;		// Device handle
WORD		wSmplData[1024];	// Output data storage area
ULONG		ulSmplBufferSize;	// Output buffer size
DAFIFOREQ DaFifoConfig;			// Output conditions setting structure
LONGLONG		i;
int			nRet;

int da_out = 0;		//0: init, 1: cw, 2: ccw, 3: end
double voltage = 0.0, voltage_bef, vol_adj, max_voltage = 4.0, min_voltage = 0.35;
int freq = 300;

bool da_enable = false;
bool x_axis = false, y_axis = false, ccw = false;
bool init_vol = false;
int hov = -1;	//-1: init, 0: horizontal, 1: vertical
int da_auto = 0;	//0: init, 1: control, 2: manual

//PID
double Kp = 0.183, Ki = 7.32, Kd = 0.00114375;
double Kp_ver = 0, Ki_ver = 0, Kd_ver = 0;


void set_da() {

	// Open a device.
#ifdef _WIN64
	hDeviceHandle = DaOpen((LPCSTR)"FBIDA1");
#else
	hDeviceHandle = DaOpen((LPCTSTR)"FBIDA1");
#endif

	// Configure the output buffer size to 1024.
	ulSmplBufferSize = 512;
	nRet = DaSetBoardConfig(hDeviceHandle, ulSmplBufferSize, NULL, NULL, 0);

	// Retrieve the output conditions.
	nRet = DaGetFifoConfig(hDeviceHandle, &DaFifoConfig);

	// Configure the number of output channels to 2 channels.

	DaFifoConfig.ulChCount = 2;
	DaFifoConfig.SmplChReq[0].ulChNo = 1;			// CH1 is set to the first channel.
	DaFifoConfig.SmplChReq[0].ulRange = DA_0_5V;		// Voltage range of CH1: +5 V

	DaFifoConfig.SmplChReq[1].ulChNo = 2;				// CH2 is set to the second channel.
	DaFifoConfig.SmplChReq[1].ulRange = DA_0_5V;		// Voltage range of CH2: +/-5 V


	//DaFifoConfig.ulSmplNum = 10000;
	DaFifoConfig.ulSmplRepeat = 0; //1周期の繰り返し回数
	
}

WORD non_freq()
{
	for (i = 0; i < 512; i++)
	{
		wSmplData[i * 2] = 0x8000;
		wSmplData[i * 2 + 1] = 0x8000;
	}
	return 0;
}

WORD cw_freq(WORD wMin, WORD wMid, double vol_adj)
{
	for (i = 0; i < 512; i++)
	{
		wSmplData[i * 2] = (WORD)(wMin * cos(i * M_PI / 256) * vol_adj + wMid);
		wSmplData[i * 2 + 1] = (WORD)(wMin * sin(i * M_PI / 256) * vol_adj + wMid);
	}
	return 0;
}

WORD ccw_freq(WORD wMin, WORD wMid, double vol_adj)
{
	for (i = 0; i < 512; i++)
	{
		wSmplData[i * 2] = (WORD)(wMin * sin(i * M_PI / 256) * vol_adj + wMid);
		wSmplData[i * 2 + 1] = (WORD)(wMin * cos(i * M_PI / 256) * vol_adj + wMid);
	}
	return 0;
}

WORD x_freq(WORD wMin, WORD wMid, double vol_adj)
{
	for (i = 0; i < 512; i++)
	{
		wSmplData[i * 2] = (WORD)(wMin * cos(i * M_PI / 256) * vol_adj + wMid);
		wSmplData[i * 2 + 1] = 0x8000;
	}
	return 0;
}

WORD y_freq(WORD wMin, WORD wMid, double vol_adj)
{
	for (i = 0; i < 512; i++)
	{
		wSmplData[i * 2] = 0x8000;
		wSmplData[i * 2 + 1] = (WORD)(wMin * sin(i * M_PI / 256) * vol_adj + wMid);
	}
	return 0;
}

void da_output()
{
	//chrono::system_clock::time_point start, t_end;
	set_da();

	int freq_rate = 500;
	int freq_bef = freq;
	DaFifoConfig.fSmplFreq = freq * freq_rate;

	nRet = DaSetFifoConfig(hDeviceHandle, &DaFifoConfig);

	// Create the output data.
	WORD wMax; //最大値
	WORD wMin; //最小値
	WORD wMid; //中間値
	
	//分解能 16 ビット
	voltage = 0.0;
	voltage_bef = voltage;
	
	float et = 0.0, et_bef = 0.0, et_bef2 = 0.0;
	float voltage_delta = 0.0;

	

	while (true)
	{
		if (sw == 2) 
		{
			break;
		}

		if (freq_bef != freq)
		{
			DaFifoConfig.fSmplFreq = freq * freq_rate;

			nRet = DaSetFifoConfig(hDeviceHandle, &DaFifoConfig);
			freq_bef = freq;
		}

		if (da_enable == true)
		{
			if (da_auto == 1)
			{
				et = re_angle[hov];
				if (init_vol == false)
				{
					et_bef = 0.0;
					et_bef2 = 0.0;
					init_vol = true;
				}

				//voltage calculation start
				csv_write = false;

				if (hov == 0)
				{
					voltage_delta = Kp * (et - et_bef) + Ki * et + Kd * (et - 2.0 * et_bef + et_bef2);
					voltage = voltage_bef + voltage_delta;
					et_bef2 = et_bef;
					et_bef = et;
				}
				else if (hov == 1)
				{
					if (Kp_ver < 0.0 || Ki_ver < 0.0 || Kd_ver < 0.0)
					{
						voltage = max_voltage;
					}
					else
					{
						voltage_delta = Kp_ver * (et - et_bef) + Ki_ver * et + Kd_ver * (et - 2.0 * et_bef + et_bef2);
						voltage = voltage_bef + voltage_delta;
						et_bef2 = et_bef;
						et_bef = et;
					}
				}

				if (voltage > max_voltage)
				{
					voltage = max_voltage;
				}

				if (voltage < min_voltage)
				{
					voltage = min_voltage;
				}

				csv_write = true;
				//voltage calculation end

				if (hov == 0)
				{


					if (da_out == 1)
					{
						if (voltage_bef != voltage)
						{
							vol_adj = voltage / 10;
							wMax = 0xC000 + (0x4000 * voltage / 5);
							wMin = 0x4000;
							wMid = (wMax + wMin) / 2;

							nRet = DaStopSampling(hDeviceHandle);

							cw_freq(wMin, wMid, vol_adj);

							//nRet = DaStopSampling(hDeviceHandle);
							nRet = DaClearSamplingData(hDeviceHandle);
							nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
							nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
							voltage_bef = voltage;
						}

					}
					else if (da_out == 2)
					{
						if (voltage_bef != voltage)
						{
							vol_adj = voltage / 10;
							wMax = 0xC000 + (0x4000 * voltage / 5);
							wMin = 0x4000;
							wMid = (wMax + wMin) / 2;

							nRet = DaStopSampling(hDeviceHandle);

							ccw_freq(wMin, wMid, vol_adj);

							//nRet = DaStopSampling(hDeviceHandle);
							nRet = DaClearSamplingData(hDeviceHandle);
							nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
							nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
							voltage_bef = voltage;
						}

					}
					else if (da_out == 3)
					{
						voltage = 0.0;
						if (voltage_bef != voltage)
						{
							wMax = 0xC000 + (0x4000 * voltage / 5);
							wMin = 0x4000;
							wMid = (wMax + wMin) / 2;

							nRet = DaStopSampling(hDeviceHandle);

							non_freq();

							//nRet = DaStopSampling(hDeviceHandle);
							nRet = DaClearSamplingData(hDeviceHandle);
							nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
							nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
							voltage_bef = voltage;
						}

					}
				}
				else if (hov == 1)
				{


					if (da_out == 3)
					{
						if (Kp_ver < 0.0 || Ki_ver < 0.0 || Kd_ver < 0.0)
						{
							da_enable = false;
						}
						da_enable = false;
						voltage = 0.0;
						if (voltage_bef != voltage)
						{
							wMax = 0xC000 + (0x4000 * voltage / 5);
							wMin = 0x4000;
							wMid = (wMax + wMin) / 2;

							nRet = DaStopSampling(hDeviceHandle);

							non_freq();

							//nRet = DaStopSampling(hDeviceHandle);
							nRet = DaClearSamplingData(hDeviceHandle);
							nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
							nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
							voltage_bef = voltage;
						}

					}
					else
					{
						if (voltage_bef != voltage)
						{
							vol_adj = voltage / 10;
							wMax = 0xC000 + (0x4000 * voltage / 5);
							wMin = 0x4000;
							wMid = (wMax + wMin) / 2;

							nRet = DaStopSampling(hDeviceHandle);

							x_freq(wMin, wMid, vol_adj);

							//nRet = DaStopSampling(hDeviceHandle);
							nRet = DaClearSamplingData(hDeviceHandle);
							nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
							nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
							voltage_bef = voltage;

						}
					}
				}
			}
			else if (da_auto == 2)
			{
				voltage = max_voltage;

				if (x_axis == true && y_axis == true)
				{
					if (voltage_bef != voltage)
					{
						vol_adj = voltage / 10;
						wMax = 0xC000 + (0x4000 * voltage / 5);
						wMin = 0x4000;
						wMid = (wMax + wMin) / 2;

						nRet = DaStopSampling(hDeviceHandle);

						if (ccw == false)
						{
							cw_freq(wMin, wMid, vol_adj);
						}
						else if (ccw == true)
						{
							ccw_freq(wMin, wMid, vol_adj);
						}

						//nRet = DaStopSampling(hDeviceHandle);
						nRet = DaClearSamplingData(hDeviceHandle);
						nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
						nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
						voltage_bef = voltage;
					}
				}
				else if (x_axis == true && y_axis == false)
				{
					if (voltage_bef != voltage)
					{
						vol_adj = voltage / 10;
						wMax = 0xC000 + (0x4000 * voltage / 5);
						wMin = 0x4000;
						wMid = (wMax + wMin) / 2;

						nRet = DaStopSampling(hDeviceHandle);

						x_freq(wMin, wMid, vol_adj);

						//nRet = DaStopSampling(hDeviceHandle);
						nRet = DaClearSamplingData(hDeviceHandle);
						nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
						nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
						voltage_bef = voltage;
					}
				}
				else if (x_axis == false && y_axis == true)
				{
					if (voltage_bef != voltage)
					{
						vol_adj = voltage / 10;
						wMax = 0xC000 + (0x4000 * voltage / 5);
						wMin = 0x4000;
						wMid = (wMax + wMin) / 2;

						nRet = DaStopSampling(hDeviceHandle);

						y_freq(wMin, wMid, vol_adj);

						//nRet = DaStopSampling(hDeviceHandle);
						nRet = DaClearSamplingData(hDeviceHandle);
						nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
						nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
						voltage_bef = voltage;
					}
				}
				else
				{
					voltage = 0.0;
					if (voltage_bef != voltage)
					{
						wMax = 0xC000 + (0x4000 * voltage / 5);
						wMin = 0x4000;
						wMid = (wMax + wMin) / 2;

						nRet = DaStopSampling(hDeviceHandle);

						non_freq();

						//nRet = DaStopSampling(hDeviceHandle);
						nRet = DaClearSamplingData(hDeviceHandle);
						nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
						nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
						voltage_bef = voltage;
					}
					continue;
				}
			}
			
		}
		else
		{
			voltage = 0.0;
			if (voltage_bef != voltage)
			{
				wMax = 0xC000 + (0x4000 * voltage / 5);
				wMin = 0x4000;
				wMid = (wMax + wMin) / 2;

				nRet = DaStopSampling(hDeviceHandle);

				non_freq();

				//nRet = DaStopSampling(hDeviceHandle);
				nRet = DaClearSamplingData(hDeviceHandle);
				nRet = DaSetSamplingData(hDeviceHandle, wSmplData, 512);
				nRet = DaStartSampling(hDeviceHandle, FLAG_ASYNC);
				voltage_bef = voltage;
			}
		}
	}

	// Close the device.
	DaClose(hDeviceHandle);

}

