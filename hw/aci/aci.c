/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2006 InnoTek Systemberatung GmbH
 *  Copyright (c) 2017-2019 Michael Saga
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "aci.h"

#define DEBUG_ACI 2

#ifdef DEBUG_ACI
# if DEBUG_ACI == 1
#  define PRINT_ACI         PRINT
# elif DEBUG_ACI == 2
#  define PRINT_ACI         PRINT
#  define VARDUMP_ACI       VARDUMP
#  define VARDUMP2_ACI      VARDUMP2
#  define VARDUMP3_ACI      VARDUMP3
#  define VARDUMP4_ACI      VARDUMP4
# else
#  define ENTER_ACI         ENTER
#  define LEAVE_ACI         LEAVE
#  define PRINT_ACI         PRINT
#  define VARDUMP_ACI       VARDUMP
#  define VARDUMP2_ACI      VARDUMP2
#  define VARDUMP3_ACI      VARDUMP3
#  define VARDUMP4_ACI      VARDUMP4
# endif
#endif
#ifndef ENTER_ACI
# define ENTER_ACI
#endif
#ifndef LEAVE_ACI
# define LEAVE_ACI
#endif
#ifndef PRINT_ACI
# define PRINT_ACI(...)
#endif
#ifndef VARDUMP_ACI
# define VARDUMP_ACI(...)
#endif
#ifndef VARDUMP2_ACI
# define VARDUMP2_ACI(...)
#endif
#ifndef VARDUMP3_ACI
# define VARDUMP3_ACI(...)
#endif
#ifndef VARDUMP4_ACI
# define VARDUMP4_ACI(...)
#endif

static int
aci_offset(register uint32_t *addr) {
    register int ret;

    if ((ret = RANGE(aci->memreg_base, aci->memreg_size, *addr))) *addr -= aci->memreg_base;

    return !ret;
}

static const hw_block_t *
aci_block_lookup(register uint32_t addr, register const char **reg) {
    register const hw_block_t *b;
    register size_t i;
    ENTER_ACI;

    if (reg) *reg = NULL;

    for (i = 0; i <= ARRAY_SIZE(aci_block); ++i) {
        if (i >= ARRAY_SIZE(aci_block)) INT3;
        b = &aci_block[i];
        if (RANGE(b->offset, b->size, addr)) {
            if (reg) {
                i = addr - b->offset;
                switch (b->index) {
                case ACI_NAM:
                    ARRAY(*reg, aci_nam_name, i);
                    break;
                case ACI_NABM:
                    ARRAY(*reg, aci_nabm_name, i);
                    break;
                }
            }
            break;
        }
    }

    LEAVE_ACI;
    return b;
}

static void
aci_nam_volume_set(uint32_t r, uint16_t v) {
    register void *p;
    register const hw_block_t *b;
    register aci_nam *nam;
    ENTER_ACI;

    p   = aci->memreg;
    b   = &aci_block[ACI_NAM];
    nam = p + b->offset;

    switch (r) {
    case ACI_NAM_Master_Volume_Mute:
        v &= 0xbf3f;
        nam->Master_Volume_Mute = v;
//        update_combined_volume_out (s);
        break;
    case ACI_NAM_PCM_Out_Volume_Mute:
        v &= 0x9f1f;
        nam->PCM_Out_Volume_Mute = v;
//        update_combined_volume_out (s);
        break;
    case ACI_NAM_Record_Gain_Mute:
        v &= 0x8f0f;
        nam->Record_Gain_Mute = v;
//        update_volume_in (s);
        break;
    }

    LEAVE_ACI;
}

static void
aci_nam_record_select_set(uint16_t v) {
    register void *p;
    register const hw_block_t *b;
    register aci_nam *nam;
    ENTER_ACI;

    p   = aci->memreg;
    b   = &aci_block[ACI_NAM];
    nam = p + b->offset;

    nam->Record_Select = v & 0x0707;

    LEAVE_ACI;
}

static void
aci_nam_reset(void) {
    register void *p;
    register const hw_block_t *b;
    register aci_nam *nam;
    ENTER_ACI;

    p   = aci->memreg;
    b   = &aci_block[ACI_NAM];
    nam = p + b->offset;

    memset(nam, 0, b->size);
    nam->Powerdown_Ctrl_Stat      = 0x000f;
    nam->Vendor_ID1               = 0x10de;
    nam->Vendor_ID2               = 0x01b1;
    nam->Extended_Audio_ID        = 0x0809;
    nam->Extended_Audio_Ctrl_Stat = 0x0009;
    nam->PCM_Front_DAC_Rate       = 48000;
    nam->PCM_Surround_DAC_Rate    = 48000;
    nam->PCM_LFE_DAC_Rate         = 48000;
    nam->PCM_LR_ADC_Rate          = 48000;
    nam->MIC_ADC_Rate             = 48000;
    aci_nam_volume_set(ACI_NAM_Master_Volume_Mute, 0x8000);
    aci_nam_volume_set(ACI_NAM_PCM_Out_Volume_Mute, 0x8808);
    aci_nam_volume_set(ACI_NAM_Record_Gain_Mute, 0x8808);
//    reset_voices (s, active);

    LEAVE_ACI;
}

static aci_nabm *
aci_nabm_lookup(uint32_t r, uint32_t *index) {
    register void *p;
    register size_t i;
    register uint32_t pos;
    register aci_nabm *ret = NULL;
    ENTER_ACI;

    p = aci->memreg + ACI_BLOCK_OFFSET(ACI_NABM);

    for (i = 0; i < ARRAY_SIZE(aci_nabm_index); ++i) {
        pos = aci_nabm_index[i];
        if (r >= pos && r < pos + sizeof(*ret)) {
            ret = p + pos;
            if (index) *index = pos;
            break;
        }
    }

    LEAVE_ACI;
    return ret;
}

static void
aci_nabm_voice_active_set(uint32_t index, int on) {
    ENTER_ACI;

    switch (index) {
    case ACI_NABM_PI_INDEX:
//        AUD_set_active_in (s->voice_pi, on);
        break;
    case ACI_NABM_PO_INDEX:
//        AUD_set_active_out (s->voice_po, on);
        break;
    case ACI_NABM_MC_INDEX:
//        AUD_set_active_in (s->voice_mc, on);
        break;
    }

    LEAVE_ACI;
}

static void
aci_nabm_reset(const uint32_t *index) {
    register void *p;
    register const hw_block_t *b;
    register aci_nabm *nabm;
    register size_t i;
    ENTER_ACI;

    p = aci->memreg;
    b = &aci_block[ACI_NABM];

    for (i = 0; i < ARRAY_SIZE(aci_nabm_index); ++i) {
        if (index && *index != aci_nabm_index[i]) continue;
        nabm = p + b->offset + aci_nabm_index[i];
        nabm->bdbar = 0;
        nabm->civ   = 0;
        nabm->lvi   = 0;
        nabm->sr    = ACI_NABM_SR_DCH;
//        update_sr (s, r, SR_DCH);//TODO
        nabm->picb  = 0;
        nabm->piv   = 0;
        nabm->cr   &= ACI_NABM_CR_NCLEAR_MASK;
        aci_nabm_voice_active_set(aci_nabm_index[i], 0);
        if (index && *index == aci_nabm_index[i]) break;
    }

    LEAVE_ACI;
}

static int
aci_write(uint32_t addr, const void *val, size_t sz) {
    register const hw_block_t *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    const char *reg;
    register int ret = 0;
    ENTER_ACI;

    if (aci_offset(&addr)) {
        LEAVE_ACI;
        return ret;
    }

    b = aci_block_lookup(addr, &reg);
    n = aci->name;
    p = aci->memreg;
    r = addr - b->offset;

    switch (b->index) {
    case ACI_NAM:
        REG32(p + ACI_BLOCK_OFFSET(ACI_NABM) + ACI_NABM_CAS) = 0;
        break;
    case ACI_NABM:
        break;
    }

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(p + addr) = v;
        ret = 1;
        break;
    case 4:
        v = REG32(val);
        o = REG32(p + addr);
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.08x) <- 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(p + addr) = v;
        ret = 1;
        break;
    default:
        LEAVE_ACI;
        return ret;
    }

    switch (b->index) {
    case ACI_NAM:
        {
            register aci_nam *nam = p + b->offset;

            switch (r) {
            case ACI_NAM_Reset:
                aci_nam_reset();
                break;
            case ACI_NAM_Master_Volume_Mute:
            case ACI_NAM_PCM_Out_Volume_Mute:
            case ACI_NAM_Record_Gain_Mute:
                aci_nam_volume_set(r, v);
                break;
            case ACI_NAM_Record_Select:
                aci_nam_record_select_set(v);
                break;
            case ACI_NAM_Powerdown_Ctrl_Stat:
                v &= ~0x800f;
                v |= o & 0xf;
                REG16(p + addr) = v;
                break;
            case ACI_NAM_Extended_Audio_Ctrl_Stat:
                if (!(v & ACI_NAM_Extended_Audio_Ctrl_Stat_VRA)) {
                    nam->PCM_Front_DAC_Rate = 48000;
                    nam->PCM_LR_ADC_Rate = 48000;
//                    open_voice (s, PI_INDEX, 48000);
//                    open_voice (s, PO_INDEX, 48000);
                }
                if (!(v & ACI_NAM_Extended_Audio_Ctrl_Stat_VRM)) {
                    nam->MIC_ADC_Rate = 48000;
//                    open_voice (s, MC_INDEX, 48000);
                }
                break;
            case ACI_NAM_PCM_Front_DAC_Rate:
                if (nam->Extended_Audio_Ctrl_Stat & ACI_NAM_Extended_Audio_Ctrl_Stat_VRA) {
//                    open_voice (s, PO_INDEX, val);
                } else {
                    REG16(p + addr) = o;
                }
                break;
            case ACI_NAM_PCM_LR_ADC_Rate:
                if (nam->Extended_Audio_Ctrl_Stat & ACI_NAM_Extended_Audio_Ctrl_Stat_VRA) {
//                    open_voice (s, PI_INDEX, val);
                } else {
                    REG16(p + addr) = o;
                }
                break;
            case ACI_NAM_MIC_ADC_Rate:
                if (nam->Extended_Audio_Ctrl_Stat & ACI_NAM_Extended_Audio_Ctrl_Stat_VRM) {
//                    open_voice (s, MC_INDEX, val);
                } else {
                    REG16(p + addr) = o;
                }
                break;
            case ACI_NAM_Headphone_Volume_Mute:
            case ACI_NAM_Master_Volume_Mono_Mute:
            case ACI_NAM_Master_Tone_RL:
            case ACI_NAM_PC_BEEP_Volume_Mute:
            case ACI_NAM_Phone_Volume_Mute:
            case ACI_NAM_Mic_Volume_Mute:
            case ACI_NAM_Line_In_Volume_Mute:
            case ACI_NAM_CD_Volume_Mute:
            case ACI_NAM_Video_Volume_Mute:
            case ACI_NAM_Aux_Volume_Mute:
            case ACI_NAM_Record_Gain_Mic_Mute:
            case ACI_NAM_General_Purpose:
            case ACI_NAM_3D_Control:
            case ACI_NAM_Sigmatel_Analog:
            case ACI_NAM_Sigmatel_Dac2Invert:
                /* not emulated */
            case ACI_NAM_Extended_Audio_ID:
            case ACI_NAM_Vendor_ID1:
            case ACI_NAM_Vendor_ID2:
                REG16(p + addr) = o;
                break;
            }
        }
        break;
    case ACI_NABM:
        {
            uint32_t index;
            register aci_nabm *nabm = aci_nabm_lookup(r, &index);

            if (nabm) {
                switch (r) {
                case ACI_NABM_PI_BDBAR:
                case ACI_NABM_PO_BDBAR:
                case ACI_NABM_MC_BDBAR:
                case ACI_NABM_SO_BDBAR:
                    nabm->bdbar &= ~0x00000003;
                    break;
                case ACI_NABM_PI_LVI:
                case ACI_NABM_PO_LVI:
                case ACI_NABM_MC_LVI:
                case ACI_NABM_SO_LVI:
                    if (nabm->sr & ACI_NABM_SR_DCH &&
                        nabm->cr & ACI_NABM_CR_RPBM) {
                        nabm->sr  &= ~(ACI_NABM_SR_DCH | ACI_NABM_SR_CELV);
                        nabm->civ  = nabm->piv++;
                        nabm->piv %= 32;
//                        fetch_bd (s, r);
                    }
                    nabm->lvi %= 32;
                    break;
                case ACI_NABM_PI_SR:
                case ACI_NABM_PO_SR:
                case ACI_NABM_MC_SR:
                case ACI_NABM_SO_SR:
                    nabm->sr = o | (v & ~(ACI_NABM_SR_RO_MASK | ACI_NABM_SR_WCLEAR_MASK));
//                    update_sr (s, r, r->sr & ~(val & SR_WCLEAR_MASK));
                    break;
                case ACI_NABM_PI_CR:
                case ACI_NABM_PO_CR:
                case ACI_NABM_MC_CR:
                case ACI_NABM_SO_CR:
                    if (v & ACI_NABM_CR_RR) {
                        aci_nabm_reset(&index);
                    } else {
                        nabm->cr &= ACI_NABM_CR_MASK;
                        if (!(nabm->cr & ACI_NABM_CR_RPBM)) {
//                            voice_set_active (s, r - s->bm_regs, 0);
                            nabm->sr  |= ACI_NABM_SR_DCH;
                        } else {
                            nabm->civ  = nabm->piv++;
                            nabm->piv %= 32;
//                            fetch_bd (s, r);
                            nabm->sr  &= ~ACI_NABM_SR_DCH;
//                            voice_set_active (s, r - s->bm_regs, 1);
                        }
                    }
                    break;
                }
            } else {
                switch (r) {
                case ACI_NABM_GLOB_CNT:
//                    if (v & ACI_NABM_GLOB_CNT_WR) warm_reset();
//                    if (v & ACI_NABM_GLOB_CNT_CR) cold_reset();
                    if (!(v & (ACI_NABM_GLOB_CNT_CR | ACI_NABM_GLOB_CNT_WR))) {
                        REG32(p + addr) = v & ACI_NABM_GLOB_CNT_MASK;
                    } else {
                        REG32(p + addr) = o;
                    }
                    break;
                case ACI_NABM_GLOB_STA:
                    REG32(p + addr)  = o & ~(v & ACI_NABM_GLOB_STA_WCLEAR_MASK);
                    REG32(p + addr) |= (v & ~(ACI_NABM_GLOB_STA_WCLEAR_MASK | ACI_NABM_GLOB_STA_RO_MASK)) & ACI_NABM_GLOB_STA_MASK;
                    break;
                case ACI_NABM_CAS:
                    break;
                }
            }
        }
        break;
    }

    LEAVE_ACI;
    return ret;
}

static int
aci_read(uint32_t addr, void *val, size_t sz) {
    register const hw_block_t *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    const char *reg;
    register int ret = 0;
    ENTER_ACI;

    if (aci_offset(&addr)) {
        LEAVE_ACI;
        return ret;
    }

    b = aci_block_lookup(addr, &reg);
    n = aci->name;
    p = aci->memreg;
    r = addr - b->offset;
    v = REG32(p + addr);

    switch (b->index) {
    case ACI_NAM:
        REG32(p + ACI_BLOCK_OFFSET(ACI_NABM) + ACI_NABM_CAS) = 0;
        break;
    case ACI_NABM:
        {
            uint32_t index;
            register aci_nabm *nabm = aci_nabm_lookup(r, &index);

            if (nabm) {
#if 0
                switch (index) {
                case ACI_NABM_PI_INDEX:
                    break;
                case ACI_NABM_PO_INDEX:
                    break;
                case ACI_NABM_MC_INDEX:
                    break;
                case ACI_NABM_SO_INDEX:
                    break;
                }
#endif
            } else {
                switch (r) {
                case ACI_NABM_GLOB_CNT:
                    break;
                case ACI_NABM_GLOB_STA:
                    v |= ACI_NABM_GLOB_STA_S0CR;
                    break;
                case ACI_NABM_CAS:
                    REG32(p + addr) = 1;
                    break;
                }
            }
        }
        break;
    }

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINT_ACI(
            XEXEC_DBG_REG,
            "%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(val) = v;
        ret = 1;
        break;
    default:
        break;
    }

    LEAVE_ACI;
    return ret;
}

static void
aci_reset(void) {
    ENTER_ACI;

    aci_nabm_reset(NULL);
    aci_nam_reset();

    LEAVE_ACI;
}

static int
aci_irq(void) {
    return 1;//TODO
}

static void
aci_irq_raise(int mask) {
    (void)mask;//TODO
}

static void
aci_irq_restore(int mask) {
    (void)mask;//TODO
}

static int
aci_init(void) {
    return 1;//TODO
}

static int
aci_destroy(void) {
    return 1;//TODO
}

const hw_ops_t aci_op = {
    .init           = aci_init,
    .destroy        = aci_destroy,
    .reset          = aci_reset,
    .irq            = aci_irq,
    .irq_raise      = aci_irq_raise,
    .irq_restore    = aci_irq_restore,
    .write          = aci_write,
    .read           = aci_read,
};

