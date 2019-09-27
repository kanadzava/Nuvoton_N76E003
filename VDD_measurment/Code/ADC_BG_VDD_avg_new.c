/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2016 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Sep/27/2019
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E003 ADC demo code
//***********************************************************************************************************

#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"


bit BIT_TMP;
UINT16 READ_BANDGAP();
void ADC_Bypass (void);
float get_battery_voltage();
		
		
/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void main (void) 
{
	UINT16 a;
	
	while (1)
	{
		a = get_battery_voltage();
	}
	
}


float get_battery_voltage()
{
	static UINT16 Bandgap_Voltage = 0;
	UINT16 bgvalue;
	float VDD_Voltage;
	
	if (Bandgap_Voltage == 0)
		Bandgap_Voltage = READ_BANDGAP(); 
	
	CKDIV = 2;													//4 Mhz clock needed?
	Enable_ADC_BandGap; 
	ADC_Bypass(); 
	clr_ADCF; 
	set_ADCS; 
	while(ADCF == 0);
	CKDIV = 0x00;
	bgvalue = (ADCRH<<4) + ADCRL;
	Disable_ADC;
	
	VDD_Voltage = (4095.0/bgvalue)*Bandgap_Voltage;
	return VDD_Voltage;
}

UINT16 READ_BANDGAP() 
{ 
	UINT8 BandgapLow, BandgapMark, BandgapHigh;
	UINT16 Bandgap_Value, Bandgap_Voltage;
	
	set_IAPEN;
	IAPCN = READ_UID;
	IAPAL = 0x0d;
	IAPAH = 0x00;
	set_IAPGO;
	BandgapLow = IAPFD;
	BandgapMark = BandgapLow&0xF0;
		
	if (BandgapMark==0x80)
	{
			BandgapLow = BandgapLow&0x0F;
			IAPAL = 0x0C;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage = Bandgap_Value*3/4;
			Bandgap_Voltage = Bandgap_Voltage - 33;			//the actually banggap voltage value is similar this value.
	}
	if (BandgapMark==0x00)
	{
			BandgapLow = BandgapLow&0x0F;
			IAPAL = 0x0C;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage= Bandgap_Value*3/4;
	}
	if (BandgapMark==0x90)
	{
			IAPAL = 0x0E;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			IAPAL = 0x0F;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapLow = IAPFD;
			BandgapLow = BandgapLow&0x0F;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage= Bandgap_Value*3/4;
	}
	clr_IAPEN;
	return Bandgap_Voltage;
}

void ADC_Bypass (void) // The first three times convert should be bypass 
{ 
	UINT8 ozc; 
	for (ozc=0; ozc < 3; ozc++) 
	{ 
		clr_ADCF; 
		set_ADCS; 
		while(ADCF == 0); 
	}
}

