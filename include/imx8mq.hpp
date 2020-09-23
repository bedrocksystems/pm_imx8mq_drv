/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#pragma once
#include <config.hpp>
#include <drv_ipc.hpp>
#include <imxclock.hpp>

/*get our devices mapped to these VAs*/
static constexpr uint32 ANATOP_VA = 0x40000000;
static constexpr uint32 ANATOP_SIZE = 0x10000;
static constexpr uint32 CCM_VA = (ANATOP_VA + ANATOP_SIZE);
static constexpr uint32 CCM_SIZE = 0x10000;
static constexpr uint32 DEV_MMIO_END = (CCM_VA + CCM_SIZE);

class Imx8mq {
public:
    Errno probe(Pbl::Utcb *utcb, const char *ccm, const char *anatop);

    Errno enable_clk(uint64 clk_id);

    Errno get_clkrate(uint64 clk_id, uint64 &value);

    Errno disable_clk(uint64 clk_id);

    Errno set_clkrate(uint64 clk_id, uint64 value);

    uint32 get_max_clkid(void);

    Errno describe_clkrate(uint64 clk_id, Pm::clk_desc &rate);

    bool is_clk_valid(uint64 clk_id);

    bool is_clk_enabled(uint64 clk_id);

private:
    Imx_ClkCtrl _ccm;
};
