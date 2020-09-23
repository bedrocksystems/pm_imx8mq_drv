/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <imx8mq.hpp>
#include <pebble/pebble.hpp>

static Imx8mq drv;

/*get our UTCB mapped here*/
static mword UTCB_BASE = (DEV_MMIO_END + PAGE_SIZE);

PBL_PORTAL(imx8mq_srv, mword, Mtd, Pbl::Utcb *) {
    drv_ipc::header *hdr = reinterpret_cast<drv_ipc::header *>(UTCB_BASE);

    switch (hdr->id) {
    case drv_ipc::method::CLK_IS_ENABLED: {
        drv_ipc::clk_is_enabled_args *in
            = reinterpret_cast<drv_ipc::clk_is_enabled_args *>(UTCB_BASE);
        drv_ipc::clk_is_enabled_ret *out
            = reinterpret_cast<drv_ipc::clk_is_enabled_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        if (drv.is_clk_enabled(in->clk_id))
            out->enabled = 1;
        else
            out->enabled = 0;
        return out->size();
    }
    case drv_ipc::method::CLK_GET_MAX: {
        drv_ipc::clk_get_max_ret *out = reinterpret_cast<drv_ipc::clk_get_max_ret *>(UTCB_BASE);
        out->max_id = drv.get_max_clkid();
        out->errno = ENONE;
        return out->size();
    }
    case drv_ipc::method::CLK_ENABLE: {
        drv_ipc::clk_enable_args *in = reinterpret_cast<drv_ipc::clk_enable_args *>(UTCB_BASE);
        drv_ipc::clk_enable_ret *out = reinterpret_cast<drv_ipc::clk_enable_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        out->errno = drv.enable_clk(in->clk_id);
        return out->size();
    }
    case drv_ipc::method::CLK_DISABLE: {
        drv_ipc::clk_disable_args *in = reinterpret_cast<drv_ipc::clk_disable_args *>(UTCB_BASE);
        drv_ipc::clk_disable_ret *out = reinterpret_cast<drv_ipc::clk_disable_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        out->errno = drv.disable_clk(in->clk_id);
        return out->size();
    }
    case drv_ipc::method::CLK_GET_RATE: {
        drv_ipc::clk_get_rate_args *in = reinterpret_cast<drv_ipc::clk_get_rate_args *>(UTCB_BASE);
        drv_ipc::clk_get_rate_ret *out = reinterpret_cast<drv_ipc::clk_get_rate_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        out->errno = drv.get_clkrate(in->clk_id, out->rate);
        return out->size();
    }
    case drv_ipc::method::CLK_SET_RATE: {
        drv_ipc::clk_set_rate_args *in = reinterpret_cast<drv_ipc::clk_set_rate_args *>(UTCB_BASE);
        drv_ipc::clk_set_rate_ret *out = reinterpret_cast<drv_ipc::clk_set_rate_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        out->errno = drv.set_clkrate(in->clk_id, in->rate);
        return out->size();
    }
    case drv_ipc::method::CLK_DESCRIBE_RATE: {
        drv_ipc::clk_describe_rate_args *in
            = reinterpret_cast<drv_ipc::clk_describe_rate_args *>(UTCB_BASE);
        drv_ipc::clk_describe_rate_ret *out
            = reinterpret_cast<drv_ipc::clk_describe_rate_ret *>(UTCB_BASE);
        if (!drv.is_clk_valid(in->clk_id)) {
            out->errno = EINVAL;
            return out->size();
        }
        out->errno = drv.describe_clkrate(in->clk_id, out->desc);
        return out->size();
    }
    default:
        return 0;
    }
}
EXPORT_PORTAL(imx8mq_srv, mword);

static constexpr char const *anatop_id = "/anatop@30360000";
static constexpr char const *ccm_id = "/ccm@30380000";

extern "C" mword __ZIP[];

/** Heap layout
 *  ---------------------
 *  |  Service stack    |  (SRV_STACK_SIZE)
 *  +-------------------+
 *  |                   |
 */

static inline mword
srv_stack_va() {
    return Pbl::heap_start();
}

static inline mword
srv_sp_va() {
    return srv_stack_va() + SRV_STACK_SIZE;
}

/* 0x1f = all permissions */
static constexpr mword
NOVA_PT_CRD(Sel obj) {
    return ((obj << 12) | 0x1f);
}

void
pbl_main(Pbl::Utcb *utcb, Cpu cpu) {
    static Sel SELS_BASE = Pbl::sels_base();

    Errno err = drv.probe(utcb, ccm_id, anatop_id);
    ASSERT(err == Errno::ENONE);

    Sel ec_sel(SELS_BASE++);
    Sel evt_base(0);
    err = Pbl::create_local_ec(utcb, ec_sel, cpu, UTCB_BASE, srv_sp_va(), evt_base);
    ASSERT(err == Errno::ENONE);

    /*Get our UUID from the ZIP*/
    Uuid *my_uuid = reinterpret_cast<Uuid *>(__ZIP);

    /* allow PM connection */
    Sel pt_sel(SELS_BASE++);
    err = Pbl::API::srv_create(utcb, ec_sel, *my_uuid, NOVA_PT_CRD(pt_sel), 0,
                               reinterpret_cast<mword>(PT_ENTRY(imx8mq_srv)));
    ASSERT(err == Errno::ENONE);
}
