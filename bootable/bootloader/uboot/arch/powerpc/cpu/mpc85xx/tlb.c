/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*
 * Copyright 2008-2009 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#ifdef CONFIG_ADDR_MAP
#include <addr_map.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

void invalidate_tlb(u8 tlb)
{
	if (tlb == 0)
		mtspr(MMUCSR0, 0x4);
	if (tlb == 1)
		mtspr(MMUCSR0, 0x2);
}

void init_tlbs(void)
{
	int i;

	for (i = 0; i < num_tlb_entries; i++) {
		write_tlb(tlb_table[i].mas0,
			  tlb_table[i].mas1,
			  tlb_table[i].mas2,
			  tlb_table[i].mas3,
			  tlb_table[i].mas7);
	}

	return ;
}

#ifndef CONFIG_NAND_SPL
static inline void use_tlb_cam(u8 idx)
{
	int i = idx / 32;
	int bit = idx % 32;

	gd->used_tlb_cams[i] |= (1 << bit);
}

static inline void free_tlb_cam(u8 idx)
{
	int i = idx / 32;
	int bit = idx % 32;

	gd->used_tlb_cams[i] &= ~(1 << bit);
}

void init_used_tlb_cams(void)
{
	int i;
	unsigned int num_cam = mfspr(SPRN_TLB1CFG) & 0xfff;

	for (i = 0; i < ((CONFIG_SYS_NUM_TLBCAMS+31)/32); i++)
		gd->used_tlb_cams[i] = 0;

	/* walk all the entries */
	for (i = 0; i < num_cam; i++) {
		u32 _mas1;

		mtspr(MAS0, FSL_BOOKE_MAS0(1, i, 0));

		asm volatile("tlbre;isync");
		_mas1 = mfspr(MAS1);

		/* if the entry isn't valid skip it */
		if ((_mas1 & MAS1_VALID))
			use_tlb_cam(i);
	}
}

int find_free_tlbcam(void)
{
	int i;
	u32 idx;

	for (i = 0; i < ((CONFIG_SYS_NUM_TLBCAMS+31)/32); i++) {
		idx = ffz(gd->used_tlb_cams[i]);

		if (idx != 32)
			break;
	}

	idx += i * 32;

	if (idx >= CONFIG_SYS_NUM_TLBCAMS)
		return -1;

	return idx;
}

void set_tlb(u8 tlb, u32 epn, u64 rpn,
	     u8 perms, u8 wimge,
	     u8 ts, u8 esel, u8 tsize, u8 iprot)
{
	u32 _mas0, _mas1, _mas2, _mas3, _mas7;

	if (tlb == 1)
		use_tlb_cam(esel);

	_mas0 = FSL_BOOKE_MAS0(tlb, esel, 0);
	_mas1 = FSL_BOOKE_MAS1(1, iprot, 0, ts, tsize);
	_mas2 = FSL_BOOKE_MAS2(epn, wimge);
	_mas3 = FSL_BOOKE_MAS3(rpn, 0, perms);
	_mas7 = FSL_BOOKE_MAS7(rpn);

	write_tlb(_mas0, _mas1, _mas2, _mas3, _mas7);

#ifdef CONFIG_ADDR_MAP
	if ((tlb == 1) && (gd->flags & GD_FLG_RELOC))
		addrmap_set_entry(epn, rpn, (1UL << ((tsize * 2) + 10)), esel);
#endif
}

void disable_tlb(u8 esel)
{
	u32 _mas0, _mas1, _mas2, _mas3, _mas7;

	free_tlb_cam(esel);

	_mas0 = FSL_BOOKE_MAS0(1, esel, 0);
	_mas1 = 0;
	_mas2 = 0;
	_mas3 = 0;
	_mas7 = 0;

	mtspr(MAS0, _mas0);
	mtspr(MAS1, _mas1);
	mtspr(MAS2, _mas2);
	mtspr(MAS3, _mas3);
#ifdef CONFIG_ENABLE_36BIT_PHYS
	mtspr(MAS7, _mas7);
#endif
	asm volatile("isync;msync;tlbwe;isync");

#ifdef CONFIG_ADDR_MAP
	if (gd->flags & GD_FLG_RELOC)
		addrmap_set_entry(0, 0, 0, esel);
#endif
}

static void tlbsx (const volatile unsigned *addr)
{
	__asm__ __volatile__ ("tlbsx 0,%0" : : "r" (addr), "m" (*addr));
}

/* return -1 if we didn't find anything */
int find_tlb_idx(void *addr, u8 tlbsel)
{
	u32 _mas0, _mas1;

	/* zero out Search PID, AS */
	mtspr(MAS6, 0);

	tlbsx(addr);

	_mas0 = mfspr(MAS0);
	_mas1 = mfspr(MAS1);

	/* we found something, and its in the TLB we expect */
	if ((MAS1_VALID & _mas1) &&
		(MAS0_TLBSEL(tlbsel) == (_mas0 & MAS0_TLBSEL_MSK))) {
		return ((_mas0 & MAS0_ESEL_MSK) >> 16);
	}

	return -1;
}

#ifdef CONFIG_ADDR_MAP
void init_addr_map(void)
{
	int i;
	unsigned int num_cam = mfspr(SPRN_TLB1CFG) & 0xfff;

	/* walk all the entries */
	for (i = 0; i < num_cam; i++) {
		unsigned long epn;
		u32 tsize, _mas1;
		phys_addr_t rpn;

		mtspr(MAS0, FSL_BOOKE_MAS0(1, i, 0));

		asm volatile("tlbre;isync");
		_mas1 = mfspr(MAS1);

		/* if the entry isn't valid skip it */
		if (!(_mas1 & MAS1_VALID))
			continue;

		tsize = (_mas1 >> 8) & 0xf;
		epn = mfspr(MAS2) & MAS2_EPN;
		rpn = mfspr(MAS3) & MAS3_RPN;
#ifdef CONFIG_ENABLE_36BIT_PHYS
		rpn |= ((phys_addr_t)mfspr(MAS7)) << 32;
#endif

		addrmap_set_entry(epn, rpn, (1UL << ((tsize * 2) + 10)), i);
	}

	return ;
}
#endif

unsigned int setup_ddr_tlbs(unsigned int memsize_in_meg)
{
	int i;
	unsigned int tlb_size;
	unsigned int ram_tlb_address = (unsigned int)CONFIG_SYS_DDR_SDRAM_BASE;
	unsigned int max_cam = (mfspr(SPRN_TLB1CFG) >> 16) & 0xf;
	u64 size, memsize = (u64)memsize_in_meg << 20;

	size = min(memsize, CONFIG_MAX_MEM_MAPPED);

	/* Convert (4^max) kB to (2^max) bytes */
	max_cam = max_cam * 2 + 10;

	for (i = 0; size && i < 8; i++) {
		int ram_tlb_index = find_free_tlbcam();
		u32 camsize = __ilog2_u64(size) & ~1U;
		u32 align = __ilog2(ram_tlb_address) & ~1U;

		if (ram_tlb_index == -1)
			break;

		if (align == -2) align = max_cam;
		if (camsize > align)
			camsize = align;

		if (camsize > max_cam)
			camsize = max_cam;

		tlb_size = (camsize - 10) / 2;

		set_tlb(1, ram_tlb_address, ram_tlb_address,
			MAS3_SX|MAS3_SW|MAS3_SR, 0,
			0, ram_tlb_index, tlb_size, 1);

		size -= 1ULL << camsize;
		memsize -= 1ULL << camsize;
		ram_tlb_address += 1UL << camsize;
	}

	if (memsize)
		print_size(memsize, " left unmapped\n");

	/*
	 * Confirm that the requested amount of memory was mapped.
	 */
	return memsize_in_meg;
}
#endif /* !CONFIG_NAND_SPL */