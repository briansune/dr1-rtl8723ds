/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
/******************************************************************************
 *
 *
 * Module:	rtl8192c_rf6052.c	( Source C File)
 *
 * Note:	Provide RF 6052 series relative API.
 *
 * Function:
 *
 * Export:
 *
 * Abbrev:
 *
 * History:
 * Data			Who		Remark
 *
 * 09/25/2008	MHC		Create initial version.
 * 11/05/2008	MHC		Add API for tw power setting.
 *
 *
******************************************************************************/

#include <rtl8723d_hal.h>

/*---------------------------Define Local Constant---------------------------*/
/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/
/*------------------------Define global variable-----------------------------*/


/*------------------------Define local variable------------------------------*/
#ifdef CONFIG_RF_SHADOW_RW
/* 2008/11/20 MH For Debug only, RF
 * static	RF_SHADOW_T	RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG] = {0}; */
static	RF_SHADOW_T	RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG];
#endif /*CONFIG_RF_SHADOW_RW*/
/*------------------------Define local variable------------------------------*/

/*-----------------------------------------------------------------------------
 * Function:    PHY_RF6052SetBandwidth()
 *
 * Overview:    This function is called by SetBWModeCallback8190Pci() only
 *
 * Input:       PADAPTER				Adapter
 *			WIRELESS_BANDWIDTH_E	Bandwidth
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		For RF type 0222D
 *---------------------------------------------------------------------------*/
void
PHY_RF6052SetBandwidth8723D(
		PADAPTER padapter,
		enum channel_width Bandwidth)	/* 20M or 40M */
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	char alpha2[3] = {0};

	if (adapter_to_rfctl(padapter)->country_ent->alpha2)
	{
		_rtw_memcpy(alpha2, adapter_to_rfctl(padapter)->country_ent->alpha2, 2);
	}

	switch (Bandwidth) {
	case CHANNEL_WIDTH_20:
		/*
		RF_A_reg 0x18[11:10]=2'b11
		RF_A_reg 0x18[9:0]
		*/
		pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff3ff) | BIT(10) | BIT(11));
		phy_set_rf_reg(padapter, RF_PATH_A, 0x18, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]); /* RF TRX_BW */

		/* SRRC */
		if (_rtw_memcmp(alpha2, "CN", 2) == _TRUE
			&& (phy_is_tx_power_limit_needed(padapter)
				#ifdef CONFIG_MP_INCLUDED
				|| rtw_mp_mode_check(padapter)
				#endif
			)
		) {
			//SRRC certification for band edge to be more strict
			if (pHalData->ant_path == RF_PATH_A)
			{
				//setting BB reg only for BW=20M
				phy_set_bb_reg(padapter, rOFDM0_TxFlexCoeff0, bMaskDWord, 0X80043187);
				phy_set_bb_reg(padapter, rOFDM0_TxFlexCoeff5, bMaskDWord, 0X00000FC0);

				//setting RF reg for BW=20M/40M
				phy_set_rf_reg(padapter, RF_PATH_A, RF_0xde, BIT(1), 1);
				phy_set_rf_reg(padapter, RF_PATH_A, RF_RX_BB1, BIT(5)| BIT(4)| BIT(3)|BIT(2)|BIT(1)|BIT(0), 0x27);
			}

			if (pHalData->current_channel == 13)
			{
				phy_set_bb_reg(padapter, R_0xa20, bMaskDWord, 0xf8fe0001);
				phy_set_bb_reg(padapter, R_0xa24, bMaskDWord, 0x64b80c1c);
				phy_set_bb_reg(padapter, R_0xa28, bMaskDWord, 0x8810);
				phy_set_bb_reg(padapter, R_0xaac, bMaskDWord, 0x01235667);
			}
		}
		break;
	case CHANNEL_WIDTH_40:
		/*
		RF_A_reg 0x18[11:10]=2'b01
		RF_A_reg 0x18[9:0]
		*/
		pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff3ff) | BIT(10));
		phy_set_rf_reg(padapter, RF_PATH_A, 0x18, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]); /* RF TRX_BW */

		/* SRRC */
		if (_rtw_memcmp(alpha2, "CN", 2) == _TRUE
			&& (phy_is_tx_power_limit_needed(padapter)
				#ifdef CONFIG_MP_INCLUDED
				|| rtw_mp_mode_check(padapter)
				#endif
			)
		) {
			//SRRC certification for band edge to be more strict
			if (pHalData->ant_path == RF_PATH_A)
			{
				//setting BB reg val to default value which is from array_mp_8723d_phy_reg[] only for BW=40M
				phy_set_bb_reg(padapter, rOFDM0_TxFlexCoeff0, bMaskDWord, 0x0010A3D0);
				phy_set_bb_reg(padapter, rOFDM0_TxFlexCoeff5, bMaskDWord, 0x00000F7D);

				//setting RF reg for BW=20M/40M
				phy_set_rf_reg(padapter, RF_PATH_A, RF_0xde, BIT(1), 1);
				phy_set_rf_reg(padapter, RF_PATH_A, RF_RX_BB1, BIT(5)| BIT(4)| BIT(3)|BIT(2)|BIT(1)|BIT(0), 0x27);
			}

			if (pHalData->current_channel == 13)
			{
				phy_set_bb_reg(padapter, R_0xa20, bMaskDWord, 0xf8fe0001);
				phy_set_bb_reg(padapter, R_0xa24, bMaskDWord, 0x64b80c1c);
				phy_set_bb_reg(padapter, R_0xa28, bMaskDWord, 0x8810);
				phy_set_bb_reg(padapter, R_0xaac, bMaskDWord, 0x01235667);
			}
		}
		break;
	default:
		break;
	}
}

static int
phy_RF6052_Config_ParaFile(
		PADAPTER		Adapter
)
{
	u32					u4RegValue = 0;
	enum rf_path			eRFPath;
	BB_REGISTER_DEFINITION_T	*pPhyReg;

	int					rtStatus = _SUCCESS;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(Adapter);

	/* 3//----------------------------------------------------------------- */
	/* 3// <2> Initialize RF */
	/* 3//----------------------------------------------------------------- */
	for (eRFPath = RF_PATH_A; eRFPath < hal_spec->rf_reg_path_num; eRFPath++) {

		pPhyReg = &pHalData->PHYRegDef[eRFPath];

		/*----Store original RFENV control type----*/
		switch (eRFPath) {
		case RF_PATH_A:
		case RF_PATH_C:
			u4RegValue = phy_query_bb_reg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV);
			break;
		case RF_PATH_B:
		case RF_PATH_D:
			u4RegValue = phy_query_bb_reg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV << 16);
			break;
		default:
			RTW_ERR("Invalid rf_path:%d\n", eRFPath);
			break;
		}

		/*----Set RF_ENV enable----*/
		phy_set_bb_reg(Adapter, pPhyReg->rfintfe, bRFSI_RFENV << 16, 0x1);
		rtw_udelay_os(1);/* PlatformStallExecution(1); */

		/*----Set RF_ENV output high----*/
		phy_set_bb_reg(Adapter, pPhyReg->rfintfo, bRFSI_RFENV, 0x1);
		rtw_udelay_os(1);/* PlatformStallExecution(1); */

		/* Set bit number of Address and Data for RF register */
		phy_set_bb_reg(Adapter, pPhyReg->rfHSSIPara2, b3WireAddressLength, 0x0);	/* Set 1 to 4 bits for 8255 */
		rtw_udelay_os(1);/* PlatformStallExecution(1); */

		phy_set_bb_reg(Adapter, pPhyReg->rfHSSIPara2, b3WireDataLength, 0x0);	/* Set 0 to 12  bits for 8255 */
		rtw_udelay_os(1);/* PlatformStallExecution(1); */

		/*----Initialize RF fom connfiguration file----*/
		switch (eRFPath) {
		case RF_PATH_A:
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
			if (PHY_ConfigRFWithParaFile(Adapter, PHY_FILE_RADIO_A, eRFPath) == _FAIL)
#endif
			{
#ifdef CONFIG_EMBEDDED_FWIMG
				if (odm_config_rf_with_header_file(&pHalData->odmpriv, CONFIG_RF_RADIO, eRFPath) == HAL_STATUS_FAILURE)
					rtStatus = _FAIL;
#endif
			}
			break;
		case RF_PATH_B:
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
			if (PHY_ConfigRFWithParaFile(Adapter, PHY_FILE_RADIO_B, eRFPath) == _FAIL)
#endif
			{
#ifdef CONFIG_EMBEDDED_FWIMG
				if (odm_config_rf_with_header_file(&pHalData->odmpriv, CONFIG_RF_RADIO, eRFPath) == HAL_STATUS_FAILURE)
					rtStatus = _FAIL;
#endif
			}
			break;
		case RF_PATH_C:
			break;
		case RF_PATH_D:
			break;
		default:
			RTW_ERR("Invalid rf_path:%d\n", eRFPath);
			break;
		}

		/*----Restore RFENV control type----*/;
		switch (eRFPath) {
		case RF_PATH_A:
		case RF_PATH_C:
			phy_set_bb_reg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV, u4RegValue);
			break;
		case RF_PATH_B:
		case RF_PATH_D:
			phy_set_bb_reg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV << 16, u4RegValue);
			break;
		default:
			RTW_ERR("Invalid rf_path:%d\n", eRFPath);
			break;
		}

		if (rtStatus != _SUCCESS) {
			goto phy_RF6052_Config_ParaFile_Fail;
		}

	}

	/* 3 ----------------------------------------------------------------- */
	/* 3 Configuration of Tx Power Tracking */
	/* 3 ----------------------------------------------------------------- */

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (PHY_ConfigRFWithTxPwrTrackParaFile(Adapter, PHY_FILE_TXPWR_TRACK) == _FAIL)
#endif
	{
#ifdef CONFIG_EMBEDDED_FWIMG
		odm_config_rf_with_tx_pwr_track_header_file(&pHalData->odmpriv);
#endif
	}

	return rtStatus;

phy_RF6052_Config_ParaFile_Fail:
	return rtStatus;
}


int
PHY_RF6052_Config8723D(
		PADAPTER		Adapter)
{
	int					rtStatus = _SUCCESS;

	/* */
	/* Config BB and RF */
	/* */
	rtStatus = phy_RF6052_Config_ParaFile(Adapter);
	return rtStatus;
}

/* End of HalRf6052.c */
