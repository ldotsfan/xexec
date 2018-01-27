
enum {
        ACI_NAM_Reset                     = 0x00,
        ACI_NAM_Master_Volume_Mute        = 0x02,
        ACI_NAM_Headphone_Volume_Mute     = 0x04,
        ACI_NAM_Master_Volume_Mono_Mute   = 0x06,
        ACI_NAM_Master_Tone_RL            = 0x08,
        ACI_NAM_PC_BEEP_Volume_Mute       = 0x0a,
        ACI_NAM_Phone_Volume_Mute         = 0x0c,
        ACI_NAM_Mic_Volume_Mute           = 0x0e,
        ACI_NAM_Line_In_Volume_Mute       = 0x10,
        ACI_NAM_CD_Volume_Mute            = 0x12,
        ACI_NAM_Video_Volume_Mute         = 0x14,
        ACI_NAM_Aux_Volume_Mute           = 0x16,
        ACI_NAM_PCM_Out_Volume_Mute       = 0x18,
        ACI_NAM_Record_Select             = 0x1a,
        ACI_NAM_Record_Gain_Mute          = 0x1c,
        ACI_NAM_Record_Gain_Mic_Mute      = 0x1e,
        ACI_NAM_General_Purpose           = 0x20,
        ACI_NAM_3D_Control                = 0x22,
        ACI_NAM_AC_97_RESERVED            = 0x24,
        ACI_NAM_Powerdown_Ctrl_Stat       = 0x26,
        ACI_NAM_Extended_Audio_ID         = 0x28,
        ACI_NAM_Extended_Audio_Ctrl_Stat  = 0x2a,
#define ACI_NAM_Extended_Audio_Ctrl_Stat_VRA    0x0001
#define ACI_NAM_Extended_Audio_Ctrl_Stat_VRM    0x0008
        ACI_NAM_PCM_Front_DAC_Rate        = 0x2c,
        ACI_NAM_PCM_Surround_DAC_Rate     = 0x2e,
        ACI_NAM_PCM_LFE_DAC_Rate          = 0x30,
        ACI_NAM_PCM_LR_ADC_Rate           = 0x32,
        ACI_NAM_MIC_ADC_Rate              = 0x34,
        ACI_NAM_6Ch_Vol_C_LFE_Mute        = 0x36,
        ACI_NAM_6Ch_Vol_L_R_Surround_Mute = 0x38,
        ACI_NAM_Vendor_Reserved           = 0x58,
        ACI_NAM_Sigmatel_Analog           = 0x6c,
        ACI_NAM_Sigmatel_Dac2Invert       = 0x6e,
        ACI_NAM_Vendor_ID1                = 0x7c,
        ACI_NAM_Vendor_ID2                = 0x7e,
};

typedef struct {
    uint16_t Reset;                         /* 0x00 */
    uint16_t Master_Volume_Mute;            /* 0x02 */
    uint16_t Headphone_Volume_Mute;         /* 0x04 */
    uint16_t Master_Volume_Mono_Mute;       /* 0x06 */
    uint16_t Master_Tone_RL;                /* 0x08 */
    uint16_t PC_BEEP_Volume_Mute;           /* 0x0a */
    uint16_t Phone_Volume_Mute;             /* 0x0c */
    uint16_t Mic_Volume_Mute;               /* 0x0e */
    uint16_t Line_In_Volume_Mute;           /* 0x10 */
    uint16_t CD_Volume_Mute;                /* 0x12 */
    uint16_t Video_Volume_Mute;             /* 0x14 */
    uint16_t Aux_Volume_Mute;               /* 0x16 */
    uint16_t PCM_Out_Volume_Mute;           /* 0x18 */
    uint16_t Record_Select;                 /* 0x1a */
    uint16_t Record_Gain_Mute;              /* 0x1c */
    uint16_t Record_Gain_Mic_Mute;          /* 0x1e */
    uint16_t General_Purpose;               /* 0x20 */
    uint16_t ThreeD_Control;                /* 0x22 */
    uint16_t AC_97_RESERVED;                /* 0x24 */
    uint16_t Powerdown_Ctrl_Stat;           /* 0x26 */
    uint16_t Extended_Audio_ID;             /* 0x28 */
    uint16_t Extended_Audio_Ctrl_Stat;      /* 0x2a */
    uint16_t PCM_Front_DAC_Rate;            /* 0x2c */
    uint16_t PCM_Surround_DAC_Rate;         /* 0x2e */
    uint16_t PCM_LFE_DAC_Rate;              /* 0x30 */
    uint16_t PCM_LR_ADC_Rate;               /* 0x32 */
    uint16_t MIC_ADC_Rate;                  /* 0x34 */
    uint16_t SixCh_Vol_C_LFE_Mute;          /* 0x36 */
    uint16_t SixCh_Vol_L_R_Surround_Mute;   /* 0x38 */
    uint8_t  pad1[0x18];                    /* 0x40 */
    uint16_t Vendor_Reserved;               /* 0x58 */
    uint8_t  pad2[0x12];                    /* 0x5a */
    uint16_t Sigmatel_Analog;               /* 0x6c */
    uint16_t Sigmatel_Dac2Invert;           /* 0x6e */
    uint8_t  pad3[0x0c];                    /* 0x70 */
    uint16_t Vendor_ID1;                    /* 0x7c */
    uint16_t Vendor_ID2;                    /* 0x7e */
} PACKED aci_nam;

static const char *const aci_nam_name[] = {
    NAME(ACI_NAM_Reset),
    NAME(ACI_NAM_Master_Volume_Mute),
    NAME(ACI_NAM_Headphone_Volume_Mute),
    NAME(ACI_NAM_Master_Volume_Mono_Mute),
    NAME(ACI_NAM_Master_Tone_RL),
    NAME(ACI_NAM_PC_BEEP_Volume_Mute),
    NAME(ACI_NAM_Phone_Volume_Mute),
    NAME(ACI_NAM_Mic_Volume_Mute),
    NAME(ACI_NAM_Line_In_Volume_Mute),
    NAME(ACI_NAM_CD_Volume_Mute),
    NAME(ACI_NAM_Video_Volume_Mute),
    NAME(ACI_NAM_Aux_Volume_Mute),
    NAME(ACI_NAM_PCM_Out_Volume_Mute),
    NAME(ACI_NAM_Record_Select),
    NAME(ACI_NAM_Record_Gain_Mute),
    NAME(ACI_NAM_Record_Gain_Mic_Mute),
    NAME(ACI_NAM_General_Purpose),
    NAME(ACI_NAM_3D_Control),
    NAME(ACI_NAM_AC_97_RESERVED),
    NAME(ACI_NAM_Powerdown_Ctrl_Stat),
    NAME(ACI_NAM_Extended_Audio_ID),
    NAME(ACI_NAM_Extended_Audio_Ctrl_Stat),
    NAME(ACI_NAM_PCM_Front_DAC_Rate),
    NAME(ACI_NAM_PCM_Surround_DAC_Rate),
    NAME(ACI_NAM_PCM_LFE_DAC_Rate),
    NAME(ACI_NAM_PCM_LR_ADC_Rate),
    NAME(ACI_NAM_MIC_ADC_Rate),
    NAME(ACI_NAM_6Ch_Vol_C_LFE_Mute),
    NAME(ACI_NAM_6Ch_Vol_L_R_Surround_Mute),
    NAME(ACI_NAM_Vendor_Reserved),
    NAME(ACI_NAM_Sigmatel_Analog),
    NAME(ACI_NAM_Sigmatel_Dac2Invert),
    NAME(ACI_NAM_Vendor_ID1),
    NAME(ACI_NAM_Vendor_ID2),
};

