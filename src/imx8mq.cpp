/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <imx8mq.hpp>

Errno
Imx8mq::probe(Pbl::Utcb *utcb, const char *ccm, const char *anatop) {

    Errno err = Pbl::API::acquire_resource(utcb, ccm, Pbl::API::RES_REG, 0, CCM_VA, 0, false);
    if (err != Errno::ENONE) return err;
    err = Pbl::API::acquire_resource(utcb, anatop, Pbl::API::RES_REG, 0, ANATOP_VA, 0, false);
    if (err != Errno::ENONE) return err;

    return _ccm.probe();
}

Errno
Imx8mq::enable_clk(uint64 clk_id) {
    return _ccm.enable_clk(clk_id);
}

Errno
Imx8mq::get_clkrate(uint64 clk_id, uint64 &value) {
    return _ccm.get_clkrate(clk_id, value);
}

Errno
Imx8mq::disable_clk(uint64 clk_id) {
    return _ccm.disable_clk(clk_id);
}

Errno
Imx8mq::set_clkrate(uint64 clk_id, uint64 value) {
    return _ccm.set_clkrate(clk_id, value);
}

uint32
Imx8mq::get_max_clkid(void) {
    return _ccm.get_max_clkid();
}

bool
Imx8mq::is_clk_enabled(uint64 clk_id) {
    return _ccm.is_enabled(clk_id);
}

bool
Imx8mq::is_clk_valid(uint64 clk_id) {
    if ((clk_id >= _ccm.get_max_clkid()) || (clk_id == IMX8MQ_CLK_DUMMY)) {
        return false;
    }
    return true;
}

Errno
Imx8mq::describe_clkrate(uint64 clk_id, Pm::clk_desc &rate) {
    return _ccm.describe_clkrate(clk_id, rate);
}
