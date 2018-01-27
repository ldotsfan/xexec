
enum {
        NV_PAVS_VOICE_CFG_VBIN                      = 0x00000000,
#define NV_PAVS_VOICE_CFG_VBIN_V0BIN                                            0x0000001f
#define NV_PAVS_VOICE_CFG_VBIN_V0BIN__BITSHIFT                                  0
#define NV_PAVS_VOICE_CFG_VBIN_V1BIN                                            0x000003e0
#define NV_PAVS_VOICE_CFG_VBIN_V1BIN__BITSHIFT                                  5
#define NV_PAVS_VOICE_CFG_VBIN_V2BIN                                            0x00007c00
#define NV_PAVS_VOICE_CFG_VBIN_V2BIN__BITSHIFT                                  10
#define NV_PAVS_VOICE_CFG_VBIN_V3BIN                                            0x001f0000
#define NV_PAVS_VOICE_CFG_VBIN_V3BIN__BITSHIFT                                  16
#define NV_PAVS_VOICE_CFG_VBIN_V4BIN                                            0x03e00000
#define NV_PAVS_VOICE_CFG_VBIN_V4BIN__BITSHIFT                                  21
#define NV_PAVS_VOICE_CFG_VBIN_V5BIN                                            0x7c000000
#define NV_PAVS_VOICE_CFG_VBIN_V5BIN__BITSHIFT                                  26
        NV_PAVS_VOICE_CFG_FMT                       = 0x00000004,
#define NV_PAVS_VOICE_CFG_FMT_V6BIN                                             0x0000001f
#define NV_PAVS_VOICE_CFG_FMT_V6BIN__BITSHIFT                                   0
#define NV_PAVS_VOICE_CFG_FMT_V7BIN                                             0x000003e0
#define NV_PAVS_VOICE_CFG_FMT_V7BIN__BITSHIFT                                   5
#define NV_PAVS_VOICE_CFG_FMT_V6PHASE                                           0x00000400
#define NV_PAVS_VOICE_CFG_FMT_V6PHASE__BITSHIFT                                 10
#define NV_PAVS_VOICE_CFG_FMT_V6PHASE_POSITIVE                                  0
#define NV_PAVS_VOICE_CFG_FMT_V6PHASE_NEGATIVE                                  1
#define NV_PAVS_VOICE_CFG_FMT_V7PHASE                                           0x00000800
#define NV_PAVS_VOICE_CFG_FMT_V7PHASE__BITSHIFT                                 11
#define NV_PAVS_VOICE_CFG_FMT_V7PHASE_POSITIVE                                  0
#define NV_PAVS_VOICE_CFG_FMT_V7PHASE_NEGATIVE                                  1
#define NV_PAVS_VOICE_CFG_FMT_GAIN                                              0x00001000
#define NV_PAVS_VOICE_CFG_FMT_GAIN__BITSHIFT                                    12
#define NV_PAVS_VOICE_CFG_FMT_GAIN_X1                                           0
#define NV_PAVS_VOICE_CFG_FMT_GAIN_X2                                           1
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM                                          0x0000e000
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM__BITSHIFT                                13
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0                                0
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT0                                4
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT1                                5
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT2                                6
#define NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3                                7
#define NV_PAVS_VOICE_CFG_FMT_SAMPLES_PER_BLOCK                                 0x001f0000
#define NV_PAVS_VOICE_CFG_FMT_SAMPLES_PER_BLOCK__BITSHIFT                       16
#define NV_PAVS_VOICE_CFG_FMT_MULTIPASS                                         0x00200000
#define NV_PAVS_VOICE_CFG_FMT_MULTIPASS__BITSHIFT                               21
#define NV_PAVS_VOICE_CFG_FMT_MULTIPASS_OFF                                     0
#define NV_PAVS_VOICE_CFG_FMT_MULTIPASS_ON                                      1
#define NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE                                      0x00400000
#define NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE__BITSHIFT                            22
#define NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE_OFF                                  0
#define NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE_ON                                   1
#define NV_PAVS_VOICE_CFG_FMT_PERSIST                                           0x00800000
#define NV_PAVS_VOICE_CFG_FMT_PERSIST__BITSHIFT                                 23
#define NV_PAVS_VOICE_CFG_FMT_PERSIST_OFF                                       0
#define NV_PAVS_VOICE_CFG_FMT_PERSIST_ON                                        1
#define NV_PAVS_VOICE_CFG_FMT_DATA_TYPE                                         0x01000000
#define NV_PAVS_VOICE_CFG_FMT_DATA_TYPE__BITSHIFT                               24
#define NV_PAVS_VOICE_CFG_FMT_DATA_TYPE_BUFFER                                  0
#define NV_PAVS_VOICE_CFG_FMT_DATA_TYPE_STREAM                                  1
#define NV_PAVS_VOICE_CFG_FMT_LOOP                                              0x02000000
#define NV_PAVS_VOICE_CFG_FMT_LOOP__BITSHIFT                                    25
#define NV_PAVS_VOICE_CFG_FMT_LOOP_OFF                                          0
#define NV_PAVS_VOICE_CFG_FMT_LOOP_ON                                           1
#define NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX                                         0x04000000
#define NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX__BITSHIFT                               26
#define NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX_FALSE                                   0
#define NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX_TRUE                                    1
#define NV_PAVS_VOICE_CFG_FMT_STEREO                                            0x08000000
#define NV_PAVS_VOICE_CFG_FMT_STEREO__BITSHIFT                                  27
#define NV_PAVS_VOICE_CFG_FMT_STEREO_DISABLE                                    0
#define NV_PAVS_VOICE_CFG_FMT_STEREO_ENABLE                                     1
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE                                       0x30000000
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE__BITSHIFT                             28
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_U8                                    0
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S16                                   1
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S24                                   2
#define NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S32                                   3
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE                                    0xc0000000
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE__BITSHIFT                          30
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B8                                 0
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B16                                1
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM                              2
#define NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B32                                3
        NV_PAVS_VOICE_CFG_ENV0                      = 0x00000008,
#define NV_PAVS_VOICE_CFG_ENV0_EA_ATTACKRATE                                    0x00000fff
#define NV_PAVS_VOICE_CFG_ENV0_EA_ATTACKRATE__BITSHIFT                          0
#define NV_PAVS_VOICE_CFG_ENV0_EA_DELAYTIME                                     0x00fff000
#define NV_PAVS_VOICE_CFG_ENV0_EA_DELAYTIME__BITSHIFT                           12
#define NV_PAVS_VOICE_CFG_ENV0_EF_PITCHSCALE                                    0xff000000
#define NV_PAVS_VOICE_CFG_ENV0_EF_PITCHSCALE__BITSHIFT                          24
        NV_PAVS_VOICE_CFG_ENVA                      = 0x0000000c,
#define NV_PAVS_VOICE_CFG_ENVA_EA_DECAYRATE                                     0x00000fff
#define NV_PAVS_VOICE_CFG_ENVA_EA_DECAYRATE__BITSHIFT                           0
#define NV_PAVS_VOICE_CFG_ENVA_EA_HOLDTIME                                      0x00fff000
#define NV_PAVS_VOICE_CFG_ENVA_EA_HOLDTIME__BITSHIFT                            12
#define NV_PAVS_VOICE_CFG_ENVA_EA_SUSTAINLEVEL                                  0xff000000
#define NV_PAVS_VOICE_CFG_ENVA_EA_SUSTAINLEVEL__BITSHIFT                        24
        NV_PAVS_VOICE_CFG_ENV1                      = 0x00000010,
#define NV_PAVS_VOICE_CFG_ENV1_EF_ATTACKRATE                                    0x00000fff
#define NV_PAVS_VOICE_CFG_ENV1_EF_ATTACKRATE__BITSHIFT                          0
#define NV_PAVS_VOICE_CFG_ENV1_EF_DELAYTIME                                     0x00fff000
#define NV_PAVS_VOICE_CFG_ENV1_EF_DELAYTIME__BITSHIFT                           12
#define NV_PAVS_VOICE_CFG_ENV1_EF_FCSCALE                                       0xff000000
#define NV_PAVS_VOICE_CFG_ENV1_EF_FCSCALE__BITSHIFT                             24
        NV_PAVS_VOICE_CFG_ENVF                      = 0x00000014,
#define NV_PAVS_VOICE_CFG_ENVF_EF_DECAYRATE                                     0x00000fff
#define NV_PAVS_VOICE_CFG_ENVF_EF_DECAYRATE__BITSHIFT                           0
#define NV_PAVS_VOICE_CFG_ENVF_EF_HOLDTIME                                      0x00fff000
#define NV_PAVS_VOICE_CFG_ENVF_EF_HOLDTIME__BITSHIFT                            12
#define NV_PAVS_VOICE_CFG_ENVF_EF_SUSTAINLEVEL                                  0xff000000
#define NV_PAVS_VOICE_CFG_ENVF_EF_SUSTAINLEVEL__BITSHIFT                        24
        NV_PAVS_VOICE_CFG_MISC                      = 0x00000018,
#define NV_PAVS_VOICE_CFG_MISC_EF_RELEASERATE                                   0x00000fff
#define NV_PAVS_VOICE_CFG_MISC_EF_RELEASERATE__BITSHIFT                         0
#define NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE                                   0x00004000
#define NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE__BITSHIFT                         14
#define NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE_NORMAL                            0
#define NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE_DELAY                             1
#define NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE                                   0x00008000
#define NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE__BITSHIFT                         15
#define NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE_NORMAL                            0
#define NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE_DELAY                             1
#define NV_PAVS_VOICE_CFG_MISC_FMODE                                            0x00030000
#define NV_PAVS_VOICE_CFG_MISC_FMODE__BITSHIFT                                  16
#define NV_PAVS_VOICE_CFG_MISC_FMODE_BYPASS                                     0
#define NV_PAVS_VOICE_CFG_MISC_FMODE_STEREO_DLS2                                1
#define NV_PAVS_VOICE_CFG_MISC_FMODE_STEREO_P_EQ                                2
#define NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_DLS2                                  1
#define NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_P_EQ                                  2
#define NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_DLS2_P_EQ                             3
#define NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_DLS2_I3DL2                          1
#define NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_P_EQ_I3DL2                          2
#define NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_I3DL2                               3
#define NV_PAVS_VOICE_CFG_MISC_BPQ                                              0x001c0000
#define NV_PAVS_VOICE_CFG_MISC_BPQ__BITSHIFT                                    18
#define NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT                                 0x00800000
#define NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT__BITSHIFT                       23
#define NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT_DISABLE                         0
#define NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE                          1
#define NV_PAVS_VOICE_CFG_MISC_V_GSCNT                                          0xff000000
#define NV_PAVS_VOICE_CFG_MISC_V_GSCNT__BITSHIFT                                24
        NV_PAVS_VOICE_CFG_HRTF_TARGET               = 0x0000001c,
#define NV_PAVS_VOICE_CFG_HRTF_TARGET_HANDLE                                    0x0000ffff
#define NV_PAVS_VOICE_CFG_HRTF_TARGET_HANDLE__BITSHIFT                          0
        NV_PAVS_VOICE_CUR_PSL_START                 = 0x00000020,
#define NV_PAVS_VOICE_CUR_PSL_START_BA                                          0x00ffffff
#define NV_PAVS_VOICE_CUR_PSL_START_BA__BITSHIFT                                0
#define NV_PAVS_VOICE_CUR_PSL_START_SSLA_COUNT                                  0x000000ff
#define NV_PAVS_VOICE_CUR_PSL_START_SSLA_COUNT__BITSHIFT                        0
#define NV_PAVS_VOICE_CUR_PSL_START_SSLA_BASE                                   0x00ffff00
#define NV_PAVS_VOICE_CUR_PSL_START_SSLA_BASE__BITSHIFT                         8
#define NV_PAVS_VOICE_CUR_PSL_START_PS_7_0                                      0xff000000
#define NV_PAVS_VOICE_CUR_PSL_START_PS_7_0__BITSHIFT                            24
        NV_PAVS_VOICE_CUR_PSH_SAMPLE                = 0x00000024,
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_LBO                                        0x00ffffff
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_LBO__BITSHIFT                              0
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSI                                        0x000000ff
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSI__BITSHIFT                              0
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSSL                                       0x00ffff00
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSSL__BITSHIFT                             8
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_PS_15_8                                    0xff000000
#define NV_PAVS_VOICE_CUR_PSH_SAMPLE_PS_15_8__BITSHIFT                          24
        NV_PAVS_VOICE_CUR_VOLA                      = 0x00000028,
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME6_B3_0                                     0x0000000f
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME6_B3_0__BITSHIFT                           0
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME0                                          0x0000fff0
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME0__BITSHIFT                                4
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME7_B3_0                                     0x000f0000
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME7_B3_0__BITSHIFT                           16
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME1                                          0xfff00000
#define NV_PAVS_VOICE_CUR_VOLA_VOLUME1__BITSHIFT                                20
        NV_PAVS_VOICE_CUR_VOLB                      = 0x0000002c,
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME6_B7_4                                     0x0000000f
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME6_B7_4__BITSHIFT                           0
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME2                                          0x0000fff0
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME2__BITSHIFT                                4
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME7_B7_4                                     0x000f0000
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME7_B7_4__BITSHIFT                           16
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME3                                          0xfff00000
#define NV_PAVS_VOICE_CUR_VOLB_VOLUME3__BITSHIFT                                20
        NV_PAVS_VOICE_CUR_VOLC                      = 0x00000030,
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME6_B11_8                                    0x0000000f
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME6_B11_8__BITSHIFT                          0
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME4                                          0x0000fff0
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME4__BITSHIFT                                4
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME7_B11_8                                    0x000f0000
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME7_B11_8__BITSHIFT                          16
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME5                                          0xfff00000
#define NV_PAVS_VOICE_CUR_VOLC_VOLUME5__BITSHIFT                                20
        NV_PAVS_VOICE_CUR_ECNT                      = 0x00000034,
#define NV_PAVS_VOICE_CUR_ECNT_EACOUNT                                          0x0000ffff
#define NV_PAVS_VOICE_CUR_ECNT_EACOUNT__BITSHIFT                                0
#define NV_PAVS_VOICE_CUR_ECNT_EFCOUNT                                          0xffff0000
#define NV_PAVS_VOICE_CUR_ECNT_EFCOUNT__BITSHIFT                                16
        NV_PAVS_VOICE_CUR_PRD                       = 0x00000038,
        NV_PAVS_VOICE_CUR_FCA                       = 0x0000003c,
#define NV_PAVS_VOICE_CUR_FCA_FC0                                               0x0000ffff
#define NV_PAVS_VOICE_CUR_FCA_FC0__BITSHIFT                                     0
#define NV_PAVS_VOICE_CUR_FCA_FC1                                               0xffff0000
#define NV_PAVS_VOICE_CUR_FCA_FC1__BITSHIFT                                     16
        NV_PAVS_VOICE_CUR_FCB                       = 0x00000040,
#define NV_PAVS_VOICE_CUR_FCB_FC2                                               0x0000ffff
#define NV_PAVS_VOICE_CUR_FCB_FC2__BITSHIFT                                     0
#define NV_PAVS_VOICE_CUR_FCB_FC3                                               0xffff0000
#define NV_PAVS_VOICE_CUR_FCB_FC3__BITSHIFT                                     16
        NV_PAVS_VOICE_CUR_FSA                       = 0x00000044,
#define NV_PAVS_VOICE_CUR_FSA_FS0                                               0x00ffffff
#define NV_PAVS_VOICE_CUR_FSA_FS0__BITSHIFT                                     0
#define NV_PAVS_VOICE_CUR_FSA_FS3_B7_0                                          0xff000000
#define NV_PAVS_VOICE_CUR_FSA_FS3_B7_0__BITSHIFT                                24
        NV_PAVS_VOICE_CUR_FSB                       = 0x00000048,
#define NV_PAVS_VOICE_CUR_FSB_FS1                                               0x00ffffff
#define NV_PAVS_VOICE_CUR_FSB_FS1__BITSHIFT                                     0
#define NV_PAVS_VOICE_CUR_FSB_FS3_B15_8                                         0xff000000
#define NV_PAVS_VOICE_CUR_FSB_FS3_B15_8__BITSHIFT                               24
        NV_PAVS_VOICE_CUR_FSC                       = 0x0000004c,
#define NV_PAVS_VOICE_CUR_FSC_FS2                                               0x00ffffff
#define NV_PAVS_VOICE_CUR_FSC_FS2__BITSHIFT                                     0
#define NV_PAVS_VOICE_CUR_FSC_FS3_B23_16                                        0xff000000
#define NV_PAVS_VOICE_CUR_FSC_FS3_B23_16__BITSHIFT                              24
        NV_PAVS_VOICE_PAR_LFO                       = 0x00000050,
#define NV_PAVS_VOICE_PAR_LFO_LFOALVL                                           0x00007fff
#define NV_PAVS_VOICE_PAR_LFO_LFOALVL__BITSHIFT                                 0
#define NV_PAVS_VOICE_PAR_LFO_LFOADR                                            0x00008000
#define NV_PAVS_VOICE_PAR_LFO_LFOADR__BITSHIFT                                  15
#define NV_PAVS_VOICE_PAR_LFO_LFOFLVL                                           0x7fff0000
#define NV_PAVS_VOICE_PAR_LFO_LFOFLVL__BITSHIFT                                 16
#define NV_PAVS_VOICE_PAR_LFO_LFOFDR                                            0x80000000
#define NV_PAVS_VOICE_PAR_LFO_LFOFDR__BITSHIFT                                  31
        NV_PAVS_VOICE_PAR_STATE                     = 0x00000054,
#define NV_PAVS_VOICE_PAR_STATE_FRACTION                                        0x0000ffff
#define NV_PAVS_VOICE_PAR_STATE_FRACTION__BITSHIFT                              0
#define NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE                                  0x00010000
#define NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE__BITSHIFT                        16
#define NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE_NORMAL                           0
#define NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE_DELAY                            1
#define NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE                                  0x00020000
#define NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE__BITSHIFT                        17
#define NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE_NORMAL                           0
#define NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE_DELAY                            1
#define NV_PAVS_VOICE_PAR_STATE_PAUSED                                          0x00040000
#define NV_PAVS_VOICE_PAR_STATE_PAUSED__BITSHIFT                                18
#define NV_PAVS_VOICE_PAR_STATE_PAUSED_NO                                       0
#define NV_PAVS_VOICE_PAR_STATE_PAUSED_YES                                      1
#define NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR                                   0x00080000
#define NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR__BITSHIFT                         19
#define NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR_A                                 0
#define NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR_B                                 1
#define NV_PAVS_VOICE_PAR_STATE_NEW_VOICE                                       0x00100000
#define NV_PAVS_VOICE_PAR_STATE_NEW_VOICE__BITSHIFT                             20
#define NV_PAVS_VOICE_PAR_STATE_NEW_VOICE_NO                                    0
#define NV_PAVS_VOICE_PAR_STATE_NEW_VOICE_YES                                   1
#define NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE                                    0x00200000
#define NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE__BITSHIFT                          21
#define NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE_NO                                 0
#define NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE_YES                                1
#define NV_PAVS_VOICE_PAR_STATE_VOICE_START                                     0x00400000
#define NV_PAVS_VOICE_PAR_STATE_VOICE_START__BITSHIFT                           22
#define NV_PAVS_VOICE_PAR_STATE_VOICE_START_LOOPED                              0
#define NV_PAVS_VOICE_PAR_STATE_VOICE_START_FIRSTPASS                           1
#define NV_PAVS_VOICE_PAR_STATE_PRD_VALID                                       0x00800000
#define NV_PAVS_VOICE_PAR_STATE_PRD_VALID__BITSHIFT                             23
#define NV_PAVS_VOICE_PAR_STATE_PRD_VALID_NO                                    0
#define NV_PAVS_VOICE_PAR_STATE_PRD_VALID_YES                                   1
#define NV_PAVS_VOICE_PAR_STATE_EFCUR                                           0x0f000000
#define NV_PAVS_VOICE_PAR_STATE_EFCUR__BITSHIFT                                 24
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_OFF                                       0
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_DELAY                                     1
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_ATTACK                                    2
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_HOLD                                      3
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_DECAY                                     4
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_SUSTAIN                                   5
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_RELEASE                                   6
#define NV_PAVS_VOICE_PAR_STATE_EFCUR_FORCE_RELEASE                             7
#define NV_PAVS_VOICE_PAR_STATE_EACUR                                           0xf0000000
#define NV_PAVS_VOICE_PAR_STATE_EACUR__BITSHIFT                                 28
#define NV_PAVS_VOICE_PAR_STATE_EACUR_OFF                                       0
#define NV_PAVS_VOICE_PAR_STATE_EACUR_DELAY                                     1
#define NV_PAVS_VOICE_PAR_STATE_EACUR_ATTACK                                    2
#define NV_PAVS_VOICE_PAR_STATE_EACUR_HOLD                                      3
#define NV_PAVS_VOICE_PAR_STATE_EACUR_DECAY                                     4
#define NV_PAVS_VOICE_PAR_STATE_EACUR_SUSTAIN                                   5
#define NV_PAVS_VOICE_PAR_STATE_EACUR_RELEASE                                   6
#define NV_PAVS_VOICE_PAR_STATE_EACUR_FORCE_RELEASE                             7
        NV_PAVS_VOICE_PAR_OFFSET                    = 0x00000058,
#define NV_PAVS_VOICE_PAR_OFFSET_CBO                                            0x00ffffff
#define NV_PAVS_VOICE_PAR_OFFSET_CBO__BITSHIFT                                  0
#define NV_PAVS_VOICE_PAR_OFFSET_CSO                                            0x00ffffff
#define NV_PAVS_VOICE_PAR_OFFSET_CSO__BITSHIFT                                  0
#define NV_PAVS_VOICE_PAR_OFFSET_EALVL                                          0xff000000
#define NV_PAVS_VOICE_PAR_OFFSET_EALVL__BITSHIFT                                24
        NV_PAVS_VOICE_PAR_NEXT                      = 0x0000005c,
#define NV_PAVS_VOICE_PAR_NEXT_SSLB_COUNT                                       0x000000ff
#define NV_PAVS_VOICE_PAR_NEXT_SSLB_COUNT__BITSHIFT                             0
#define NV_PAVS_VOICE_PAR_NEXT_SSLB_BASE                                        0x00ffff00
#define NV_PAVS_VOICE_PAR_NEXT_SSLB_BASE__BITSHIFT                              8
#define NV_PAVS_VOICE_PAR_NEXT_EBO                                              0x00ffffff
#define NV_PAVS_VOICE_PAR_NEXT_EBO__BITSHIFT                                    0
#define NV_PAVS_VOICE_PAR_NEXT_EFLVL                                            0xff000000
#define NV_PAVS_VOICE_PAR_NEXT_EFLVL__BITSHIFT                                  24
        NV_PAVS_VOICE_TAR_VOLA                      = 0x00000060,
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME6_B3_0                                     0x0000000f
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME6_B3_0__BITSHIFT                           0
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME0                                          0x0000fff0
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME0__BITSHIFT                                4
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME7_B3_0                                     0x000f0000
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME7_B3_0__BITSHIFT                           16
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME1                                          0xfff00000
#define NV_PAVS_VOICE_TAR_VOLA_VOLUME1__BITSHIFT                                20
        NV_PAVS_VOICE_TAR_VOLB                      = 0x00000064,
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME6_B7_4                                     0x0000000f
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME6_B7_4__BITSHIFT                           0
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME2                                          0x0000fff0
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME2__BITSHIFT                                4
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME7_B7_4                                     0x000f0000
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME7_B7_4__BITSHIFT                           16
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME3                                          0xfff00000
#define NV_PAVS_VOICE_TAR_VOLB_VOLUME3__BITSHIFT                                20
        NV_PAVS_VOICE_TAR_VOLC                      = 0x00000068,
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME6_B11_8                                    0x0000000f
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME6_B11_8__BITSHIFT                          0
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME4                                          0x0000fff0
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME4__BITSHIFT                                4
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME7_B11_8                                    0x000f0000
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME7_B11_8__BITSHIFT                          16
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME5                                          0xfff00000
#define NV_PAVS_VOICE_TAR_VOLC_VOLUME5__BITSHIFT                                20
        NV_PAVS_VOICE_TAR_LFO_ENV                   = 0x0000006c,
#define NV_PAVS_VOICE_TAR_LFO_ENV_EA_RELEASERATE                                0x00000fff
#define NV_PAVS_VOICE_TAR_LFO_ENV_EA_RELEASERATE__BITSHIFT                      0
#define NV_PAVS_VOICE_TAR_LFO_ENV_LFOADLT                                       0x003ff000
#define NV_PAVS_VOICE_TAR_LFO_ENV_LFOADLT__BITSHIFT                             12
#define NV_PAVS_VOICE_TAR_LFO_ENV_LFOFDLT                                       0xffc00000
#define NV_PAVS_VOICE_TAR_LFO_ENV_LFOFDLT__BITSHIFT                             22
        NV_PAVS_VOICE_TAR_LFO_MOD                   = 0x00000070,
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAAM                                        0x000000ff
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAAM__BITSHIFT                              0
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFM                                        0x0000ff00
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFM__BITSHIFT                              8
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFC                                        0x00ff0000
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFC__BITSHIFT                              16
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOFFM                                        0xff000000
#define NV_PAVS_VOICE_TAR_LFO_MOD_LFOFFM__BITSHIFT                              24
        NV_PAVS_VOICE_TAR_FCA                       = 0x00000074,
#define NV_PAVS_VOICE_TAR_FCA_FC0                                               0x0000ffff
#define NV_PAVS_VOICE_TAR_FCA_FC0__BITSHIFT                                     0
#define NV_PAVS_VOICE_TAR_FCA_FC1                                               0xffff0000
#define NV_PAVS_VOICE_TAR_FCA_FC1__BITSHIFT                                     16
        NV_PAVS_VOICE_TAR_FCB                       = 0x00000078,
#define NV_PAVS_VOICE_TAR_FCB_FC2                                               0x0000ffff
#define NV_PAVS_VOICE_TAR_FCB_FC2__BITSHIFT                                     0
#define NV_PAVS_VOICE_TAR_FCB_FC3                                               0xffff0000
#define NV_PAVS_VOICE_TAR_FCB_FC3__BITSHIFT                                     16
        NV_PAVS_VOICE_TAR_PITCH_LINK                = 0x0000007c,
#define NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE                          0x0000ffff
#define NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE__BITSHIFT                0
#define NV_PAVS_VOICE_TAR_PITCH_LINK_PITCH                                      0xffff0000
#define NV_PAVS_VOICE_TAR_PITCH_LINK_PITCH__BITSHIFT                            16
};

static const char *const apu_pavs_name[] = {
    NAMER(NV_PAVS_VOICE_CFG_VBIN),
#if 0
NV_PAVS_VOICE_CFG_VBIN_V0BIN                                            0x0000001f
NV_PAVS_VOICE_CFG_VBIN_V0BIN__BITSHIFT                                  0
NV_PAVS_VOICE_CFG_VBIN_V1BIN                                            0x000003e0
NV_PAVS_VOICE_CFG_VBIN_V1BIN__BITSHIFT                                  5
NV_PAVS_VOICE_CFG_VBIN_V2BIN                                            0x00007c00
NV_PAVS_VOICE_CFG_VBIN_V2BIN__BITSHIFT                                  10
NV_PAVS_VOICE_CFG_VBIN_V3BIN                                            0x001f0000
NV_PAVS_VOICE_CFG_VBIN_V3BIN__BITSHIFT                                  16
NV_PAVS_VOICE_CFG_VBIN_V4BIN                                            0x03e00000
NV_PAVS_VOICE_CFG_VBIN_V4BIN__BITSHIFT                                  21
NV_PAVS_VOICE_CFG_VBIN_V5BIN                                            0x7c000000
NV_PAVS_VOICE_CFG_VBIN_V5BIN__BITSHIFT                                  26
#endif
    NAMER(NV_PAVS_VOICE_CFG_FMT),
#if 0
NV_PAVS_VOICE_CFG_FMT_V6BIN                                             0x0000001f
NV_PAVS_VOICE_CFG_FMT_V6BIN__BITSHIFT                                   0
NV_PAVS_VOICE_CFG_FMT_V7BIN                                             0x000003e0
NV_PAVS_VOICE_CFG_FMT_V7BIN__BITSHIFT                                   5
NV_PAVS_VOICE_CFG_FMT_V6PHASE                                           0x00000400
NV_PAVS_VOICE_CFG_FMT_V6PHASE__BITSHIFT                                 10
NV_PAVS_VOICE_CFG_FMT_V6PHASE_POSITIVE                                  0
NV_PAVS_VOICE_CFG_FMT_V6PHASE_NEGATIVE                                  1
NV_PAVS_VOICE_CFG_FMT_V7PHASE                                           0x00000800
NV_PAVS_VOICE_CFG_FMT_V7PHASE__BITSHIFT                                 11
NV_PAVS_VOICE_CFG_FMT_V7PHASE_POSITIVE                                  0
NV_PAVS_VOICE_CFG_FMT_V7PHASE_NEGATIVE                                  1
NV_PAVS_VOICE_CFG_FMT_GAIN                                              0x00001000
NV_PAVS_VOICE_CFG_FMT_GAIN__BITSHIFT                                    12
NV_PAVS_VOICE_CFG_FMT_GAIN_X1                                           0
NV_PAVS_VOICE_CFG_FMT_GAIN_X2                                           1
NV_PAVS_VOICE_CFG_FMT_HEADROOM                                          0x0000e000
NV_PAVS_VOICE_CFG_FMT_HEADROOM__BITSHIFT                                13
NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0                                0
NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT0                                4
NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT1                                5
NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT2                                6
NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3                                7
NV_PAVS_VOICE_CFG_FMT_SAMPLES_PER_BLOCK                                 0x001f0000
NV_PAVS_VOICE_CFG_FMT_SAMPLES_PER_BLOCK__BITSHIFT                       16
NV_PAVS_VOICE_CFG_FMT_MULTIPASS                                         0x00200000
NV_PAVS_VOICE_CFG_FMT_MULTIPASS__BITSHIFT                               21
NV_PAVS_VOICE_CFG_FMT_MULTIPASS_OFF                                     0
NV_PAVS_VOICE_CFG_FMT_MULTIPASS_ON                                      1
NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE                                      0x00400000
NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE__BITSHIFT                            22
NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE_OFF                                  0
NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE_ON                                   1
NV_PAVS_VOICE_CFG_FMT_PERSIST                                           0x00800000
NV_PAVS_VOICE_CFG_FMT_PERSIST__BITSHIFT                                 23
NV_PAVS_VOICE_CFG_FMT_PERSIST_OFF                                       0
NV_PAVS_VOICE_CFG_FMT_PERSIST_ON                                        1
NV_PAVS_VOICE_CFG_FMT_DATA_TYPE                                         0x01000000
NV_PAVS_VOICE_CFG_FMT_DATA_TYPE__BITSHIFT                               24
NV_PAVS_VOICE_CFG_FMT_DATA_TYPE_BUFFER                                  0
NV_PAVS_VOICE_CFG_FMT_DATA_TYPE_STREAM                                  1
NV_PAVS_VOICE_CFG_FMT_LOOP                                              0x02000000
NV_PAVS_VOICE_CFG_FMT_LOOP__BITSHIFT                                    25
NV_PAVS_VOICE_CFG_FMT_LOOP_OFF                                          0
NV_PAVS_VOICE_CFG_FMT_LOOP_ON                                           1
NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX                                         0x04000000
NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX__BITSHIFT                               26
NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX_FALSE                                   0
NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX_TRUE                                    1
NV_PAVS_VOICE_CFG_FMT_STEREO                                            0x08000000
NV_PAVS_VOICE_CFG_FMT_STEREO__BITSHIFT                                  27
NV_PAVS_VOICE_CFG_FMT_STEREO_DISABLE                                    0
NV_PAVS_VOICE_CFG_FMT_STEREO_ENABLE                                     1
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE                                       0x30000000
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE__BITSHIFT                             28
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_U8                                    0
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S16                                   1
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S24                                   2
NV_PAVS_VOICE_CFG_FMT_SAMPLE_SIZE_S32                                   3
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE                                    0xc0000000
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE__BITSHIFT                          30
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B8                                 0
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B16                                1
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM                              2
NV_PAVS_VOICE_CFG_FMT_CONTAINER_SIZE_B32                                3
#endif
    NAMER(NV_PAVS_VOICE_CFG_ENV0),
#if 0
NV_PAVS_VOICE_CFG_ENV0_EA_ATTACKRATE                                    0x00000fff
NV_PAVS_VOICE_CFG_ENV0_EA_ATTACKRATE__BITSHIFT                          0
NV_PAVS_VOICE_CFG_ENV0_EA_DELAYTIME                                     0x00fff000
NV_PAVS_VOICE_CFG_ENV0_EA_DELAYTIME__BITSHIFT                           12
NV_PAVS_VOICE_CFG_ENV0_EF_PITCHSCALE                                    0xff000000
NV_PAVS_VOICE_CFG_ENV0_EF_PITCHSCALE__BITSHIFT                          24
#endif
    NAMER(NV_PAVS_VOICE_CFG_ENVA),
#if 0
NV_PAVS_VOICE_CFG_ENVA_EA_DECAYRATE                                     0x00000fff
NV_PAVS_VOICE_CFG_ENVA_EA_DECAYRATE__BITSHIFT                           0
NV_PAVS_VOICE_CFG_ENVA_EA_HOLDTIME                                      0x00fff000
NV_PAVS_VOICE_CFG_ENVA_EA_HOLDTIME__BITSHIFT                            12
NV_PAVS_VOICE_CFG_ENVA_EA_SUSTAINLEVEL                                  0xff000000
NV_PAVS_VOICE_CFG_ENVA_EA_SUSTAINLEVEL__BITSHIFT                        24
#endif
    NAMER(NV_PAVS_VOICE_CFG_ENV1),
#if 0
NV_PAVS_VOICE_CFG_ENV1_EF_ATTACKRATE                                    0x00000fff
NV_PAVS_VOICE_CFG_ENV1_EF_ATTACKRATE__BITSHIFT                          0
NV_PAVS_VOICE_CFG_ENV1_EF_DELAYTIME                                     0x00fff000
NV_PAVS_VOICE_CFG_ENV1_EF_DELAYTIME__BITSHIFT                           12
NV_PAVS_VOICE_CFG_ENV1_EF_FCSCALE                                       0xff000000
NV_PAVS_VOICE_CFG_ENV1_EF_FCSCALE__BITSHIFT                             24
#endif
    NAMER(NV_PAVS_VOICE_CFG_ENVF),
#if 0
NV_PAVS_VOICE_CFG_ENVF_EF_DECAYRATE                                     0x00000fff
NV_PAVS_VOICE_CFG_ENVF_EF_DECAYRATE__BITSHIFT                           0
NV_PAVS_VOICE_CFG_ENVF_EF_HOLDTIME                                      0x00fff000
NV_PAVS_VOICE_CFG_ENVF_EF_HOLDTIME__BITSHIFT                            12
NV_PAVS_VOICE_CFG_ENVF_EF_SUSTAINLEVEL                                  0xff000000
NV_PAVS_VOICE_CFG_ENVF_EF_SUSTAINLEVEL__BITSHIFT                        24
#endif
    NAMER(NV_PAVS_VOICE_CFG_MISC),
#if 0
NV_PAVS_VOICE_CFG_MISC_EF_RELEASERATE                                   0x00000fff
NV_PAVS_VOICE_CFG_MISC_EF_RELEASERATE__BITSHIFT                         0
NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE                                   0x00004000
NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE__BITSHIFT                         14
NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE_NORMAL                            0
NV_PAVS_VOICE_CFG_MISC_LFOA_DELAYMODE_DELAY                             1
NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE                                   0x00008000
NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE__BITSHIFT                         15
NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE_NORMAL                            0
NV_PAVS_VOICE_CFG_MISC_LFOF_DELAYMODE_DELAY                             1
NV_PAVS_VOICE_CFG_MISC_FMODE                                            0x00030000
NV_PAVS_VOICE_CFG_MISC_FMODE__BITSHIFT                                  16
NV_PAVS_VOICE_CFG_MISC_FMODE_BYPASS                                     0
NV_PAVS_VOICE_CFG_MISC_FMODE_STEREO_DLS2                                1
NV_PAVS_VOICE_CFG_MISC_FMODE_STEREO_P_EQ                                2
NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_DLS2                                  1
NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_P_EQ                                  2
NV_PAVS_VOICE_CFG_MISC_FMODE_MONO_DLS2_P_EQ                             3
NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_DLS2_I3DL2                          1
NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_P_EQ_I3DL2                          2
NV_PAVS_VOICE_CFG_MISC_FMODE_THREED_I3DL2                               3
NV_PAVS_VOICE_CFG_MISC_BPQ                                              0x001c0000
NV_PAVS_VOICE_CFG_MISC_BPQ__BITSHIFT                                    18
NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT                                 0x00800000
NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT__BITSHIFT                       23
NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT_DISABLE                         0
NV_PAVS_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE                          1
NV_PAVS_VOICE_CFG_MISC_V_GSCNT                                          0xff000000
NV_PAVS_VOICE_CFG_MISC_V_GSCNT__BITSHIFT                                24
#endif
    NAMER(NV_PAVS_VOICE_CFG_HRTF_TARGET),
#if 0
NV_PAVS_VOICE_CFG_HRTF_TARGET_HANDLE                                    0x0000ffff
NV_PAVS_VOICE_CFG_HRTF_TARGET_HANDLE__BITSHIFT                          0
#endif
    NAMER(NV_PAVS_VOICE_CUR_PSL_START),
#if 0
NV_PAVS_VOICE_CUR_PSL_START_BA                                          0x00ffffff
NV_PAVS_VOICE_CUR_PSL_START_BA__BITSHIFT                                0
NV_PAVS_VOICE_CUR_PSL_START_SSLA_COUNT                                  0x000000ff
NV_PAVS_VOICE_CUR_PSL_START_SSLA_COUNT__BITSHIFT                        0
NV_PAVS_VOICE_CUR_PSL_START_SSLA_BASE                                   0x00ffff00
NV_PAVS_VOICE_CUR_PSL_START_SSLA_BASE__BITSHIFT                         8
NV_PAVS_VOICE_CUR_PSL_START_PS_7_0                                      0xff000000
NV_PAVS_VOICE_CUR_PSL_START_PS_7_0__BITSHIFT                            24
#endif
    NAMER(NV_PAVS_VOICE_CUR_PSH_SAMPLE),
#if 0
NV_PAVS_VOICE_CUR_PSH_SAMPLE_LBO                                        0x00ffffff
NV_PAVS_VOICE_CUR_PSH_SAMPLE_LBO__BITSHIFT                              0
NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSI                                        0x000000ff
NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSI__BITSHIFT                              0
NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSSL                                       0x00ffff00
NV_PAVS_VOICE_CUR_PSH_SAMPLE_CSSL__BITSHIFT                             8
NV_PAVS_VOICE_CUR_PSH_SAMPLE_PS_15_8                                    0xff000000
NV_PAVS_VOICE_CUR_PSH_SAMPLE_PS_15_8__BITSHIFT                          24
#endif
    NAMER(NV_PAVS_VOICE_CUR_VOLA),
#if 0
NV_PAVS_VOICE_CUR_VOLA_VOLUME6_B3_0                                     0x0000000f
NV_PAVS_VOICE_CUR_VOLA_VOLUME6_B3_0__BITSHIFT                           0
NV_PAVS_VOICE_CUR_VOLA_VOLUME0                                          0x0000fff0
NV_PAVS_VOICE_CUR_VOLA_VOLUME0__BITSHIFT                                4
NV_PAVS_VOICE_CUR_VOLA_VOLUME7_B3_0                                     0x000f0000
NV_PAVS_VOICE_CUR_VOLA_VOLUME7_B3_0__BITSHIFT                           16
NV_PAVS_VOICE_CUR_VOLA_VOLUME1                                          0xfff00000
NV_PAVS_VOICE_CUR_VOLA_VOLUME1__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_CUR_VOLB),
#if 0
NV_PAVS_VOICE_CUR_VOLB_VOLUME6_B7_4                                     0x0000000f
NV_PAVS_VOICE_CUR_VOLB_VOLUME6_B7_4__BITSHIFT                           0
NV_PAVS_VOICE_CUR_VOLB_VOLUME2                                          0x0000fff0
NV_PAVS_VOICE_CUR_VOLB_VOLUME2__BITSHIFT                                4
NV_PAVS_VOICE_CUR_VOLB_VOLUME7_B7_4                                     0x000f0000
NV_PAVS_VOICE_CUR_VOLB_VOLUME7_B7_4__BITSHIFT                           16
NV_PAVS_VOICE_CUR_VOLB_VOLUME3                                          0xfff00000
NV_PAVS_VOICE_CUR_VOLB_VOLUME3__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_CUR_VOLC),
#if 0
NV_PAVS_VOICE_CUR_VOLC_VOLUME6_B11_8                                    0x0000000f
NV_PAVS_VOICE_CUR_VOLC_VOLUME6_B11_8__BITSHIFT                          0
NV_PAVS_VOICE_CUR_VOLC_VOLUME4                                          0x0000fff0
NV_PAVS_VOICE_CUR_VOLC_VOLUME4__BITSHIFT                                4
NV_PAVS_VOICE_CUR_VOLC_VOLUME7_B11_8                                    0x000f0000
NV_PAVS_VOICE_CUR_VOLC_VOLUME7_B11_8__BITSHIFT                          16
NV_PAVS_VOICE_CUR_VOLC_VOLUME5                                          0xfff00000
NV_PAVS_VOICE_CUR_VOLC_VOLUME5__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_CUR_ECNT),
#if 0
NV_PAVS_VOICE_CUR_ECNT_EACOUNT                                          0x0000ffff
NV_PAVS_VOICE_CUR_ECNT_EACOUNT__BITSHIFT                                0
NV_PAVS_VOICE_CUR_ECNT_EFCOUNT                                          0xffff0000
NV_PAVS_VOICE_CUR_ECNT_EFCOUNT__BITSHIFT                                16
#endif
    NAMER(NV_PAVS_VOICE_CUR_PRD),
#if 0
#endif
    NAMER(NV_PAVS_VOICE_CUR_FCA),
#if 0
NV_PAVS_VOICE_CUR_FCA_FC0                                               0x0000ffff
NV_PAVS_VOICE_CUR_FCA_FC0__BITSHIFT                                     0
NV_PAVS_VOICE_CUR_FCA_FC1                                               0xffff0000
NV_PAVS_VOICE_CUR_FCA_FC1__BITSHIFT                                     16
#endif
    NAMER(NV_PAVS_VOICE_CUR_FCB),
#if 0
NV_PAVS_VOICE_CUR_FCB_FC2                                               0x0000ffff
NV_PAVS_VOICE_CUR_FCB_FC2__BITSHIFT                                     0
NV_PAVS_VOICE_CUR_FCB_FC3                                               0xffff0000
NV_PAVS_VOICE_CUR_FCB_FC3__BITSHIFT                                     16
#endif
    NAMER(NV_PAVS_VOICE_CUR_FSA),
#if 0
NV_PAVS_VOICE_CUR_FSA_FS0                                               0x00ffffff
NV_PAVS_VOICE_CUR_FSA_FS0__BITSHIFT                                     0
NV_PAVS_VOICE_CUR_FSA_FS3_B7_0                                          0xff000000
NV_PAVS_VOICE_CUR_FSA_FS3_B7_0__BITSHIFT                                24
#endif
    NAMER(NV_PAVS_VOICE_CUR_FSB),
#if 0
NV_PAVS_VOICE_CUR_FSB_FS1                                               0x00ffffff
NV_PAVS_VOICE_CUR_FSB_FS1__BITSHIFT                                     0
NV_PAVS_VOICE_CUR_FSB_FS3_B15_8                                         0xff000000
NV_PAVS_VOICE_CUR_FSB_FS3_B15_8__BITSHIFT                               24
#endif
    NAMER(NV_PAVS_VOICE_CUR_FSC),
#if 0
NV_PAVS_VOICE_CUR_FSC_FS2                                               0x00ffffff
NV_PAVS_VOICE_CUR_FSC_FS2__BITSHIFT                                     0
NV_PAVS_VOICE_CUR_FSC_FS3_B23_16                                        0xff000000
NV_PAVS_VOICE_CUR_FSC_FS3_B23_16__BITSHIFT                              24
#endif
    NAMER(NV_PAVS_VOICE_PAR_LFO),
#if 0
NV_PAVS_VOICE_PAR_LFO_LFOALVL                                           0x00007fff
NV_PAVS_VOICE_PAR_LFO_LFOALVL__BITSHIFT                                 0
NV_PAVS_VOICE_PAR_LFO_LFOADR                                            0x00008000
NV_PAVS_VOICE_PAR_LFO_LFOADR__BITSHIFT                                  15
NV_PAVS_VOICE_PAR_LFO_LFOFLVL                                           0x7fff0000
NV_PAVS_VOICE_PAR_LFO_LFOFLVL__BITSHIFT                                 16
NV_PAVS_VOICE_PAR_LFO_LFOFDR                                            0x80000000
NV_PAVS_VOICE_PAR_LFO_LFOFDR__BITSHIFT                                  31
#endif
    NAMER(NV_PAVS_VOICE_PAR_STATE),
#if 0
NV_PAVS_VOICE_PAR_STATE_FRACTION                                        0x0000ffff
NV_PAVS_VOICE_PAR_STATE_FRACTION__BITSHIFT                              0
NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE                                  0x00010000
NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE__BITSHIFT                        16
NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE_NORMAL                           0
NV_PAVS_VOICE_PAR_STATE_LFOA_DELAYMODE_DELAY                            1
NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE                                  0x00020000
NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE__BITSHIFT                        17
NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE_NORMAL                           0
NV_PAVS_VOICE_PAR_STATE_LFOF_DELAYMODE_DELAY                            1
NV_PAVS_VOICE_PAR_STATE_PAUSED                                          0x00040000
NV_PAVS_VOICE_PAR_STATE_PAUSED__BITSHIFT                                18
NV_PAVS_VOICE_PAR_STATE_PAUSED_NO                                       0
NV_PAVS_VOICE_PAR_STATE_PAUSED_YES                                      1
NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR                                   0x00080000
NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR__BITSHIFT                         19
NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR_A                                 0
NV_PAVS_VOICE_PAR_STATE_SSL_INDICATOR_B                                 1
NV_PAVS_VOICE_PAR_STATE_NEW_VOICE                                       0x00100000
NV_PAVS_VOICE_PAR_STATE_NEW_VOICE__BITSHIFT                             20
NV_PAVS_VOICE_PAR_STATE_NEW_VOICE_NO                                    0
NV_PAVS_VOICE_PAR_STATE_NEW_VOICE_YES                                   1
NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE                                    0x00200000
NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE__BITSHIFT                          21
NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE_NO                                 0
NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE_YES                                1
NV_PAVS_VOICE_PAR_STATE_VOICE_START                                     0x00400000
NV_PAVS_VOICE_PAR_STATE_VOICE_START__BITSHIFT                           22
NV_PAVS_VOICE_PAR_STATE_VOICE_START_LOOPED                              0
NV_PAVS_VOICE_PAR_STATE_VOICE_START_FIRSTPASS                           1
NV_PAVS_VOICE_PAR_STATE_PRD_VALID                                       0x00800000
NV_PAVS_VOICE_PAR_STATE_PRD_VALID__BITSHIFT                             23
NV_PAVS_VOICE_PAR_STATE_PRD_VALID_NO                                    0
NV_PAVS_VOICE_PAR_STATE_PRD_VALID_YES                                   1
NV_PAVS_VOICE_PAR_STATE_EFCUR                                           0x0f000000
NV_PAVS_VOICE_PAR_STATE_EFCUR__BITSHIFT                                 24
NV_PAVS_VOICE_PAR_STATE_EFCUR_OFF                                       0
NV_PAVS_VOICE_PAR_STATE_EFCUR_DELAY                                     1
NV_PAVS_VOICE_PAR_STATE_EFCUR_ATTACK                                    2
NV_PAVS_VOICE_PAR_STATE_EFCUR_HOLD                                      3
NV_PAVS_VOICE_PAR_STATE_EFCUR_DECAY                                     4
NV_PAVS_VOICE_PAR_STATE_EFCUR_SUSTAIN                                   5
NV_PAVS_VOICE_PAR_STATE_EFCUR_RELEASE                                   6
NV_PAVS_VOICE_PAR_STATE_EFCUR_FORCE_RELEASE                             7
NV_PAVS_VOICE_PAR_STATE_EACUR                                           0xf0000000
NV_PAVS_VOICE_PAR_STATE_EACUR__BITSHIFT                                 28
NV_PAVS_VOICE_PAR_STATE_EACUR_OFF                                       0
NV_PAVS_VOICE_PAR_STATE_EACUR_DELAY                                     1
NV_PAVS_VOICE_PAR_STATE_EACUR_ATTACK                                    2
NV_PAVS_VOICE_PAR_STATE_EACUR_HOLD                                      3
NV_PAVS_VOICE_PAR_STATE_EACUR_DECAY                                     4
NV_PAVS_VOICE_PAR_STATE_EACUR_SUSTAIN                                   5
NV_PAVS_VOICE_PAR_STATE_EACUR_RELEASE                                   6
NV_PAVS_VOICE_PAR_STATE_EACUR_FORCE_RELEASE                             7
#endif
    NAMER(NV_PAVS_VOICE_PAR_OFFSET),
#if 0
NV_PAVS_VOICE_PAR_OFFSET_CBO                                            0x00ffffff
NV_PAVS_VOICE_PAR_OFFSET_CBO__BITSHIFT                                  0
NV_PAVS_VOICE_PAR_OFFSET_CSO                                            0x00ffffff
NV_PAVS_VOICE_PAR_OFFSET_CSO__BITSHIFT                                  0
NV_PAVS_VOICE_PAR_OFFSET_EALVL                                          0xff000000
NV_PAVS_VOICE_PAR_OFFSET_EALVL__BITSHIFT                                24
#endif
    NAMER(NV_PAVS_VOICE_PAR_NEXT),
#if 0
NV_PAVS_VOICE_PAR_NEXT_SSLB_COUNT                                       0x000000ff
NV_PAVS_VOICE_PAR_NEXT_SSLB_COUNT__BITSHIFT                             0
NV_PAVS_VOICE_PAR_NEXT_SSLB_BASE                                        0x00ffff00
NV_PAVS_VOICE_PAR_NEXT_SSLB_BASE__BITSHIFT                              8
NV_PAVS_VOICE_PAR_NEXT_EBO                                              0x00ffffff
NV_PAVS_VOICE_PAR_NEXT_EBO__BITSHIFT                                    0
NV_PAVS_VOICE_PAR_NEXT_EFLVL                                            0xff000000
NV_PAVS_VOICE_PAR_NEXT_EFLVL__BITSHIFT                                  24
#endif
    NAMER(NV_PAVS_VOICE_TAR_VOLA),
#if 0
NV_PAVS_VOICE_TAR_VOLA_VOLUME6_B3_0                                     0x0000000f
NV_PAVS_VOICE_TAR_VOLA_VOLUME6_B3_0__BITSHIFT                           0
NV_PAVS_VOICE_TAR_VOLA_VOLUME0                                          0x0000fff0
NV_PAVS_VOICE_TAR_VOLA_VOLUME0__BITSHIFT                                4
NV_PAVS_VOICE_TAR_VOLA_VOLUME7_B3_0                                     0x000f0000
NV_PAVS_VOICE_TAR_VOLA_VOLUME7_B3_0__BITSHIFT                           16
NV_PAVS_VOICE_TAR_VOLA_VOLUME1                                          0xfff00000
NV_PAVS_VOICE_TAR_VOLA_VOLUME1__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_TAR_VOLB),
#if 0
NV_PAVS_VOICE_TAR_VOLB_VOLUME6_B7_4                                     0x0000000f
NV_PAVS_VOICE_TAR_VOLB_VOLUME6_B7_4__BITSHIFT                           0
NV_PAVS_VOICE_TAR_VOLB_VOLUME2                                          0x0000fff0
NV_PAVS_VOICE_TAR_VOLB_VOLUME2__BITSHIFT                                4
NV_PAVS_VOICE_TAR_VOLB_VOLUME7_B7_4                                     0x000f0000
NV_PAVS_VOICE_TAR_VOLB_VOLUME7_B7_4__BITSHIFT                           16
NV_PAVS_VOICE_TAR_VOLB_VOLUME3                                          0xfff00000
NV_PAVS_VOICE_TAR_VOLB_VOLUME3__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_TAR_VOLC),
#if 0
NV_PAVS_VOICE_TAR_VOLC_VOLUME6_B11_8                                    0x0000000f
NV_PAVS_VOICE_TAR_VOLC_VOLUME6_B11_8__BITSHIFT                          0
NV_PAVS_VOICE_TAR_VOLC_VOLUME4                                          0x0000fff0
NV_PAVS_VOICE_TAR_VOLC_VOLUME4__BITSHIFT                                4
NV_PAVS_VOICE_TAR_VOLC_VOLUME7_B11_8                                    0x000f0000
NV_PAVS_VOICE_TAR_VOLC_VOLUME7_B11_8__BITSHIFT                          16
NV_PAVS_VOICE_TAR_VOLC_VOLUME5                                          0xfff00000
NV_PAVS_VOICE_TAR_VOLC_VOLUME5__BITSHIFT                                20
#endif
    NAMER(NV_PAVS_VOICE_TAR_LFO_ENV),
#if 0
NV_PAVS_VOICE_TAR_LFO_ENV_EA_RELEASERATE                                0x00000fff
NV_PAVS_VOICE_TAR_LFO_ENV_EA_RELEASERATE__BITSHIFT                      0
NV_PAVS_VOICE_TAR_LFO_ENV_LFOADLT                                       0x003ff000
NV_PAVS_VOICE_TAR_LFO_ENV_LFOADLT__BITSHIFT                             12
NV_PAVS_VOICE_TAR_LFO_ENV_LFOFDLT                                       0xffc00000
NV_PAVS_VOICE_TAR_LFO_ENV_LFOFDLT__BITSHIFT                             22
#endif
    NAMER(NV_PAVS_VOICE_TAR_LFO_MOD),
#if 0
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAAM                                        0x000000ff
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAAM__BITSHIFT                              0
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFM                                        0x0000ff00
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFM__BITSHIFT                              8
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFC                                        0x00ff0000
NV_PAVS_VOICE_TAR_LFO_MOD_LFOAFC__BITSHIFT                              16
NV_PAVS_VOICE_TAR_LFO_MOD_LFOFFM                                        0xff000000
NV_PAVS_VOICE_TAR_LFO_MOD_LFOFFM__BITSHIFT                              24
#endif
    NAMER(NV_PAVS_VOICE_TAR_FCA),
#if 0
NV_PAVS_VOICE_TAR_FCA_FC0                                               0x0000ffff
NV_PAVS_VOICE_TAR_FCA_FC0__BITSHIFT                                     0
NV_PAVS_VOICE_TAR_FCA_FC1                                               0xffff0000
NV_PAVS_VOICE_TAR_FCA_FC1__BITSHIFT                                     16
#endif
    NAMER(NV_PAVS_VOICE_TAR_FCB),
#if 0
NV_PAVS_VOICE_TAR_FCB_FC2                                               0x0000ffff
NV_PAVS_VOICE_TAR_FCB_FC2__BITSHIFT                                     0
NV_PAVS_VOICE_TAR_FCB_FC3                                               0xffff0000
NV_PAVS_VOICE_TAR_FCB_FC3__BITSHIFT                                     16
#endif
    NAMER(NV_PAVS_VOICE_TAR_PITCH_LINK),
#if 0
NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE                          0x0000ffff
NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE__BITSHIFT                0
NV_PAVS_VOICE_TAR_PITCH_LINK_PITCH                                      0xffff0000
NV_PAVS_VOICE_TAR_PITCH_LINK_PITCH__BITSHIFT                            16
#endif
};

