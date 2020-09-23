/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 * This implementation is derived from the iMX8 clock driver in the Linux kernel.
 * The following copyright headers are present in the referenced sources-
 * Copyright 2018 NXP.
 * Copyright (C) 2017 Pengutronix, Lucas Stach <kernel@pengutronix.de>
 */

#pragma once
#include <imx8mq-clock.h>
#include <pebble/io.hpp>
#include <pm.hpp>

#define MAX_PARENTS 8U
#define CLOCK_DIV_UP(x, y) (((x) + (y)-1) / (y))

/* permissions mask */
enum : uint32 {
    CLOCK_FIXED = (1u << 0),              // no modification permitted
    CLOCK_CHANGE_PARENT = (1u << 1),      // change of parent permitted
    CLOCK_CHANGE_PARENT_RATE = (1u << 2), // change of parent rate permitted
    CLOCK_ENABLE_PARENT = (1u << 3),      // enable parent permitted
    CLOCK_DISABLE_PARENT = (1u << 4),     // disable parent permitted
    CLOCK_CHANGE_RATE = (1u << 5),        // change of rate permitted
    CLOCK_CHANGE_RATE_PARENT = (1u << 6), // change of rate by changing parent permitted
};

/*generic clock*/
class Clock {
public:
    virtual ~Clock() {}

    Clock() : _id(0), _rate(0), _enabled(false), _reg(0), _parent(nullptr), _flags(0) {}

    Clock(uint32 id, mword addr, Clock *parent, uint16 flags)
        : _id(id), _rate(0), _enabled(false), _reg(addr), _parent(parent), _flags(flags) {}

    Clock(uint32 id, Clock *parent, uint32 rate)
        : _id(id), _rate(rate), _enabled(false), _reg(0), _parent(parent), _flags(0) {}

    Clock(uint32 id, Clock *parent, mword addr)
        : _id(id), _rate(0), _enabled(false), _reg(addr), _parent(parent), _flags(0) {}

    Clock(uint32 id, uint32 rate, uint16 flags)
        : _id(id), _rate(rate), _enabled(true), _reg(0), _parent(nullptr), _flags(flags) {}

    virtual bool set_rate(uint32) = 0;
    virtual bool get_rate(uint32 &) = 0;
    virtual bool set_parent(Clock *) = 0;
    virtual bool get_parent(Clock **) = 0;
    virtual bool enable(void) = 0;
    virtual bool disable(void) = 0;
    virtual void init(void) = 0;
    virtual bool is_enabled(void) { return _enabled; }
    virtual bool describe_rate(Pm::clk_desc &) = 0;
    uint32 get_id() { return _id; }

protected:
    uint32 _id;
    uint32 _rate;
    bool _enabled;
    mword _reg;
    Clock *_parent;
    uint16 _flags;
};

/**
 * No operations, fixed rate. No gating.
 */
class Imx_fixed_clock : public Clock {
public:
    Imx_fixed_clock(uint32 id, uint32 rate) : Clock(id, rate, CLOCK_FIXED) {}

    bool set_rate(uint32) override { return false; }

    bool get_rate(uint32 &rate) override {
        rate = _rate;
        return true;
    }

    bool set_parent(Clock *) override { return false; }

    bool get_parent(Clock **) override { return false; }

    bool enable(void) override { return true; }

    bool disable(void) override { return false; }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        desc.min = _rate;
        return true;
    }

    void init(void) override { _enabled = true; }
};

/**
 * Rate is mul/div of parent.
 * Change of parent rate is allowed.
 */
class Imx_fixdiv_clock : public Clock {
public:
    Imx_fixdiv_clock(uint32 id, Clock *parent, uint8, uint8 div)
        : Clock(id, parent, static_cast<uint32>(0)), _div(div) {}

    bool set_rate(uint32 rate) override {
        uint32 prate = rate * _div;
        if (_parent->set_rate(prate)) {
            _parent->get_rate(_rate); // rate may be rounded-off
            return true;
        } else
            return false;
    }

    bool get_rate(uint32 &rate) override {
        if (_parent != nullptr)
            if (_parent->get_rate(rate)) {
                rate = rate / _div;
                _rate = rate;
                return true;
            }
        return false;
    }

    bool set_parent(Clock *) override { return false; } // no change of parent

    bool get_parent(Clock **parent) override {
        if (_parent != nullptr) {
            *parent = _parent;
            return true;
        } else
            return false;
    };

    bool enable(void) override {
        if (_parent->is_enabled()) {
            _enabled = true;
            return true;
        }
        return false;
    }

    bool disable(void) override { return false; };

    void init(void) override {
        if (_parent == nullptr) {
            _enabled = false;
            _rate = 0;
            return;
        }

        if (_parent->get_rate(_rate)) {
            _rate = _rate / _div;
        }
        _enabled = _parent->is_enabled();
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    uint8 _div;
};

/**
 * Can gate parent, can change parent.
 * Rate change by change of parent rate or explicit parent change.
 */
class Imx_clock_mux : public Clock {

public:
    // Default mux - don't change parent when requested to change rate.
    Imx_clock_mux(uint32 id, mword addr, uint8 shift, uint8 width, Clock *parents[],
                  uint8 num_parents)
        : Clock(id, addr, nullptr, 0), _shift(shift), _width(width), _num_parents(num_parents) {
        for (uint8 i = 0; i < MAX_PARENTS; i++)
            _parents[i] = (num_parents < MAX_PARENTS) ? parents[i] : nullptr;
    }

    Imx_clock_mux(uint32 id, mword addr, uint8 shift, uint8 width, Clock *parents[],
                  uint8 num_parents, uint16 flags)
        : Clock(id, addr, nullptr, flags), _shift(shift), _width(width), _num_parents(num_parents) {
        for (uint8 i = 0; i < MAX_PARENTS; i++)
            _parents[i] = (num_parents < MAX_PARENTS) ? parents[i] : nullptr;
    }

    bool set_rate(uint32 rate) override {
        if (_parent != nullptr)
            return _parent->set_rate(rate);
        else
            return false;
    }

    bool get_rate(uint32 &rate) override {
        if (_parent != nullptr)
            return _parent->get_rate(rate);
        else
            return false;
    }

    bool set_parent(Clock *parent) override {
        uint8 idx = MAX_PARENTS;

        for (uint8 i = 0; i < _num_parents; i++)
            if (_parents[i] == parent) idx = i;

        if (idx < _num_parents) {
            uint32 setmask = ((1u << _width) - 1) << _shift;
            uint32 clrmask = ~setmask;
            uint32 reg = ind(_reg);
            reg &= clrmask; // clear existing parent field
            reg |= static_cast<uint32>(idx) << _shift;
            outd(_reg, reg);
            _parent = parent;
            if (_flags & CLOCK_ENABLE_PARENT) _enabled = _parent->enable();
            _parent->get_rate(_rate);
            return true;
        } else {
            return false; // Not a valid parent for this clock
        }
    }

    bool get_parent(Clock **parent) override {
        if (_enabled) {
            *parent = _parent;
            return true;
        } else
            return false;
    }

    bool enable(void) override {
        if (_enabled)
            return true;
        else {
            init();
            return _enabled;
        }
    }

    bool disable(void) override {
        if (_flags & CLOCK_ENABLE_PARENT)
            if (_parent != nullptr)
                if (_parent->disable()) {
                    _enabled = false;
                    return true;
                }

        return false;
    }

    void init(void) override {
        uint32 reg = ind(_reg);

        uint32 mask = (1u << _width) - 1;
        uint32 idx = (reg >> _shift) & mask;

        if (_parents[idx] != nullptr) {
            _parent = _parents[idx];
            if (_flags & CLOCK_ENABLE_PARENT) {
                _enabled = _parent->enable();
            } else {
                _enabled = _parent->is_enabled();
            }
            _parent->get_rate(_rate);
        } else
            _enabled = false;
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    uint8 _shift;
    uint8 _width;
    uint8 _num_parents;
    Clock *_parents[MAX_PARENTS];
};

/**
 * Can change rate, can change parent rate, no change parent.
 * Can gate parent.
 */
class Imx_clock_div : public Clock {

public:
    Imx_clock_div(uint32 id, Clock *parent, mword addr, uint8 shift, uint8 width)
        : Clock(id, parent, addr), _shift(shift), _width(width) {}

    Imx_clock_div(uint32 id, Clock *parent, mword addr, uint8 shift, uint8 width, uint16 flags)
        : Clock(id, addr, parent, flags), _shift(shift), _width(width) {}

    // TODO: change parent rate if more accurate?
    bool set_rate(uint32 rate) override {
        if (_parent == nullptr) return false;

        uint32 prate;
        _parent->get_rate(prate);

        uint32 div = CLOCK_DIV_UP(prate, rate);
        div = div - 1;
        if (div <= ((1u << _width) - 1)) {
            uint32 reg = ind(_reg);
            reg &= ~((1u << _width) - 1);
            reg |= div << _shift;
            outd(_reg, reg);
        } else
            return false; // invalid divider

        return true;
    }

    bool get_rate(uint32 &rate) override {
        if (_parent != nullptr) {
            uint32 prate;
            if (_parent->get_rate(prate)) {
                uint32 div;
                uint32 reg = ind(_reg);
                div = (reg >> _shift) & ((1u << _width) - 1);
                rate = prate / div;
                _rate = rate;
                return true;
            } else {
                rate = _rate;
                return true;
            }
        } else
            return false;
    }

    bool set_parent(Clock *) override { return false; }

    bool get_parent(Clock **parent) override {
        *parent = _parent;
        return true;
    }

    bool enable(void) override {
        if (_enabled) return true;
        init();
        return _enabled;
    }

    bool disable(void) override {
        if (!_enabled) return false;
        if (_flags & CLOCK_ENABLE_PARENT) return _parent->disable();
        return false;
    }

    void init(void) override {
        if (_parent == nullptr) {
            _enabled = false;
            _rate = 0;
            return;
        }
        uint32 prate;
        if (_parent->get_rate(prate)) {
            uint32 div;
            uint32 reg = ind(_reg);
            div = (reg >> _shift) & ((1u << _width) - 1);
            _rate = prate / (div + 1);
            if (_flags & CLOCK_ENABLE_PARENT)
                _enabled = _parent->enable();
            else
                _enabled = _parent->is_enabled();
        } else {
            _enabled = false;
            // no parent rate, assume disabled
        }
    }

    /** TODO: clarify meaning of step-size/segment from
     * SCMI spec. Until then, use fixed rate.
        bool describe_rate(Pm::clk_desc &desc) {
            uint32 prate;
            if(_parent->get_rate(prate)) {
                desc.triplet = true;
                desc.max = prate;
                desc.min = prate / ((1u << _width) - 1);
                return true;
            }
            else {
                return false;
            }
        }
    */

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    uint8 _shift;
    uint8 _width;
};

/**
 * Only enable/disable
 */
class Imx_clock_gate : public Clock {

public:
    Imx_clock_gate(uint32 id, Clock *parent, mword addr, uint8 bit, uint8 en_val)
        : Clock(id, parent, addr), _bit(bit), _en_val(en_val) {}

    Imx_clock_gate(uint32 id, Clock *parent, mword addr, uint8 bit, uint8 en_val, uint16 flags)
        : Clock(id, addr, parent, flags), _bit(bit), _en_val(en_val) {}

    bool set_rate(uint32) override { return false; }

    bool get_rate(uint32 &rate) override {
        if (_parent != nullptr)
            return _parent->get_rate(rate);
        else
            return false;
    }

    bool set_parent(Clock *) override { return false; }

    bool get_parent(Clock **parent) override {
        *parent = _parent;
        return true;
    }

    bool enable(void) override {
        if (_parent == nullptr) return false;

        if (_flags & CLOCK_ENABLE_PARENT) {
            if (!_parent->enable()) return false;
        }

        uint32 reg = ind(_reg);
        reg |= static_cast<uint32>(_en_val) << _bit; // 0x1 for regular gate, 0x3 for ccm target
        outd(_reg, reg);
        _enabled = true;
        return true;
    }

    // disabling a gated clock always succeeds
    bool disable(void) override {
        uint32 reg = ind(_reg);
        reg &= ~(static_cast<uint32>(_en_val) << _bit);
        outd(_reg, reg);
        _enabled = false;
        return true;
    }

    void init(void) override {
        if (_parent == nullptr) {
            _rate = 0;
            _enabled = false;
            return;
        }

        if (_flags & CLOCK_ENABLE_PARENT) {
            if (!_parent->enable()) {
                _enabled = false;
                return;
            }
        }

        uint32 reg = ind(_reg);
        uint32 enabled = reg & (static_cast<uint32>(_en_val) << _bit);
        _enabled = (enabled == 0);
        _parent->get_rate(_rate);
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    uint8 _bit;
    uint8 _en_val;
};

/**
 * Cannot change parent, cannot gate. Can change/recalculate rate.
 * GPU, Audio, Video, VPU
 */
class Frac_pll : public Clock {
public:
    enum Cfg0 : uint32 {
        PLL_OUTPUT_DIV_VAL_SHIFT = (0),
        PLL_OUTPUT_DIV_VAL_MASK = (0x1fu << PLL_OUTPUT_DIV_VAL_SHIFT),

        PLL_REFCLK_DIV_VAL_SHIFT = (5),
        PLL_REFCLK_DIV_VAL_MASK = (0x3fu << PLL_REFCLK_DIV_VAL_SHIFT),

        PLL_NEWDIV_ACK = (0x1u << 11),
        PLL_NEWDIV_VAL = (0x1u << 12),

        PLL_COUNTCLK_25M_REF_CLK = (0x0u << 13),
        PLL_COUNTCLK_27M_REF_CLK = (0x1u << 13),

        PLL_BYPASS = (0x1u << 14),

        PLL_LOCK_SEL = (0x1u << 15),
        PLL_REFCLK_SHIFT = 16,
        PLL_REFCLK_MASK = (0x3u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_25M_REF_CLK = (0x0u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_27M_REF_CLK = (0x1u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_HDMI_PHY_27M_CLK = (0x2u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_CLK_P_N = (0x3u << PLL_REFCLK_SHIFT),

        PLL_PD_OVERRIDE = (0x1u << 18),
        PLL_PD = (0x1u << 19),

        PLL_OVERRIDE = (0x1u << 20),
        PLL_CLKE = (0x1u << 21),

        PLL_LOCK = (0x1u << 31)
    };

    static inline uint32 PLL_OUTPUT_DIV(uint32 div) {
        return ((div / 2 - 1) << Cfg0::PLL_OUTPUT_DIV_VAL_SHIFT) & Cfg0::PLL_OUTPUT_DIV_VAL_MASK;
    }
    static inline uint32 PLL_REFCLK_DIV(uint32 div) {
        return (div << Cfg0::PLL_REFCLK_DIV_VAL_SHIFT) & Cfg0::PLL_REFCLK_DIV_VAL_MASK;
    }

    enum Cfg1 : uint32 {
        PLL_INT_DIV_CTL_SHIFT = 0,
        PLL_INT_DIV_CTL_MASK = (0x7fu << PLL_INT_DIV_CTL_SHIFT),
        PLL_FRAC_DIV_CTL_SHIFT = 7,
        PLL_FRAC_DIV_CTL_MASK = (0xffffffu << PLL_FRAC_DIV_CTL_SHIFT),
    };
    static inline uint32 PLL_INT_DIV_CTL(uint32 div) {
        return (div << Cfg1::PLL_INT_DIV_CTL_SHIFT) & Cfg1::PLL_INT_DIV_CTL_MASK;
    }
    static inline uint32 PLL_FRAC_DIV_CTL(uint32 frac) {
        return (frac << Cfg1::PLL_FRAC_DIV_CTL_SHIFT) & Cfg1::PLL_FRAC_DIV_CTL_MASK;
    }

    Frac_pll(uint32 id, Clock *parent, mword addr) : Clock(id, parent, addr) {}

    bool set_rate(uint32 rate) override {
        uint32 prate;
        if (!_parent->get_rate(prate)) return false;
        uint32 cfg0, cfg1, divfrac, divint;
        uint64 tmp;
        uint64 prate64 = prate * 8;
        uint64 rate64 = rate * 2;
        divint = static_cast<uint32>(prate64 / rate64);

        tmp = prate64 * divint;
        tmp = rate64 - tmp;
        tmp = tmp * (1u << 24); // 2^24 factor
        divfrac = static_cast<uint32>(tmp / prate64);

        cfg1 = ind(_reg + 4);
        cfg1 &= ~(PLL_INT_DIV_CTL_MASK | PLL_FRAC_DIV_CTL_MASK);
        cfg1 |= (divfrac << PLL_FRAC_DIV_CTL_SHIFT);
        cfg1 |= (divint - 1);
        outd((_reg + 4), cfg1);

        cfg0 = ind(_reg);
        cfg0 &= ~PLL_OUTPUT_DIV_VAL_MASK;
        outd(_reg, cfg0);

        cfg0 = ind(_reg);
        cfg0 |= PLL_NEWDIV_VAL;
        outd(_reg, cfg0);

        if (!(cfg0 & (PLL_BYPASS | PLL_PD))) {
            do {
                cfg0 = ind(_reg);
                cfg0 &= PLL_NEWDIV_ACK;
            } while (cfg0 == 0);
        }

        cfg0 = ind(_reg);
        cfg0 &= ~PLL_NEWDIV_VAL;
        outd(_reg, cfg0);
        _rate = rate;
        return true;
    }

    bool get_rate(uint32 &rate) override {
        rate = _rate;
        return true;
    }

    bool set_parent(Clock *) override { return false; }

    bool get_parent(Clock **parent) override {
        if (_parent != nullptr) {
            *parent = _parent;
            return true;
        }
        return false;
    }

    bool enable(void) override {
        if (_enabled) return true;
        uint32 reg = ind(_reg);
        reg &= ~PLL_PD;
        outd(_reg, reg);

        do {
            reg = ind(_reg);
            reg &= PLL_LOCK;
        } while (reg == 0);

        _enabled = true;
        return true;
    }

    bool disable(void) override {
        if (!_enabled) return true;
        uint32 reg = ind(_reg);
        reg |= PLL_PD;
        outd(_reg, reg);
        _enabled = false;
        return true;
    }

    void init(void) override {
        if (_parent == nullptr) return;

        uint32 cfg0, cfg1, divfrac, divint, divout, prate;
        if (!_parent->get_rate(prate)) return;

        cfg1 = ind(_reg + 4);
        divfrac = ((cfg1 & PLL_FRAC_DIV_CTL_MASK) >> PLL_FRAC_DIV_CTL_SHIFT);
        divint = cfg1 & PLL_INT_DIV_CTL_MASK;

        cfg0 = ind(_reg);
        divout = (cfg0 & PLL_OUTPUT_DIV_VAL_MASK);
        divout = (divout + 1) * 2;

        uint64 tmp = prate * 8;
        tmp = tmp * divfrac;
        tmp = tmp / (1u << 24);

        uint64 tmprate = (prate * 8) * (divint + 1);
        tmprate = tmprate / divout;
        tmprate = tmprate + tmp;

        if (!(cfg0 & PLL_PD)) _enabled = true;

        if (cfg0 & PLL_BYPASS) {
            Clock *grandparent; // skip prediv when bypassed
            _parent->get_parent(&grandparent);
            grandparent->get_rate(_rate);
        } else
            _rate = static_cast<uint32>(tmprate);
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }
};

/**
 * Can change parent, can change rate
 * DRAM, Video2, ARM
 */
class Sccg_pll_clk : public Clock {
public:
    enum Cfg0 : uint32 {
        PLL_REFCLK_SHIFT = 0,
        PLL_REFCLK_MASK = (0x3u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_25M_REF_CLK = (0x0u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_27M_REF_CLK = (0x1u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_HDMI_PHY_27M_CLK = (0x2u << PLL_REFCLK_SHIFT),
        PLL_REFCLK_CLK_P_N = (0x3u << PLL_REFCLK_SHIFT),

        PLL_COUNTCLK_25M_REF_CLK = (0x0u << 2),
        PLL_COUNTCLK_27M_REF_CLK = (0x1u << 2),

        PLL_LOCK_SEL = (0x1u << 3),

        PLL_BYPASS2 = (0x1u << 4),
        PLL_BYPASS1 = (0x1u << 5),

        PLL_PD_OVERRIDE = (0x1u << 6),
        PLL_PD = (0x1u << 7),

        PLL_OVERRIDE = (0x1u << 8),
        PLL_CLKE = (0x1u << 9),

        PLL_LOCK = (0x1u << 31)
    };

    enum Cfg1 : uint32 {
        PLL_SSE = (1u << 0),

        PLL_SSMF_SHIFT = 1,
        PLL_SSMF_MASK = (0xfu << PLL_SSMF_SHIFT),

        PLL_SSMD_025 = (0u << 5),
        PLL_SSMD_05 = (1u << 5),
        PLL_SSMD_075 = (2u << 5),
        PLL_SSMD_100 = (3u << 5),
        PLL_SSMD_150 = (4u << 5),
        PLL_SSMD_200 = (5u << 5),
        PLL_SSMD_300 = (6u << 5),
        PLL_SSMD_400 = (7u << 5),

        PLL_SSDS = (1u << 8)
    };

    enum Cfg2 : uint32 {
        PLL_FILTER_RANGE = (1u << 0),

        PLL_OUTPUT_DIV_VAL_SHIFT = (1),
        PLL_OUTPUT_DIV_VAL_MASK = (0x3fu << PLL_OUTPUT_DIV_VAL_SHIFT),

        PLL_FEEDBACK_DIVF2_SHIFT = (7),
        PLL_FEEDBACK_DIVF2_MASK = (0x3fu << PLL_FEEDBACK_DIVF2_SHIFT),

        PLL_FEEDBACK_DIVF1_SHIFT = (13),
        PLL_FEEDBACK_DIVF1_MASK = (0x3fu << PLL_FEEDBACK_DIVF1_SHIFT),

        PLL_REF_DIVR2_SHIFT = (19),
        PLL_REF_DIVR2_MASK = (0x3fu << PLL_REF_DIVR2_SHIFT),

        PLL_REF_DIVR1_SHIFT = (25),
        PLL_REF_DIVR1_MASK = (0x7u << PLL_REF_DIVR1_SHIFT),
    };
    static inline uint32 PLL_OUTPUT_DIV_VAL(uint32 div) {
        return (div << Cfg2::PLL_OUTPUT_DIV_VAL_SHIFT) & Cfg2::PLL_OUTPUT_DIV_VAL_MASK;
    }
    static inline uint32 PLL_FEEDBACK_DIVF1_VAL(uint32 div) {
        return (div << Cfg2::PLL_FEEDBACK_DIVF1_SHIFT) & Cfg2::PLL_FEEDBACK_DIVF1_MASK;
    }
    static inline uint32 PLL_FEEDBACK_DIVF2_VAL(uint32 div) {
        return (div << Cfg2::PLL_FEEDBACK_DIVF2_SHIFT) & Cfg2::PLL_FEEDBACK_DIVF2_MASK;
    }
    static inline uint32 PLL_REF_DIVR1_VAL(uint32 div) {
        return (div << Cfg2::PLL_REF_DIVR1_SHIFT) & Cfg2::PLL_REF_DIVR1_MASK;
    }
    static inline uint32 PLL_REF_DIVR2_VAL(uint32 div) {
        return (div << Cfg2::PLL_REF_DIVR2_SHIFT) & Cfg2::PLL_REF_DIVR2_MASK;
    }

    Sccg_pll_clk(uint32 id, Clock *parent, mword addr, bool critical)
        : Clock(id, parent, addr), _is_critical(critical) {}

    bool set_rate(uint32) override { return true; }

    bool get_rate(uint32 &rate) override {
        if (_parent == nullptr) return false;

        uint32 prate;
        if (!_parent->get_rate(prate)) return false;

        uint32 cfg1 = ind(_reg + 0x4);
        if (cfg1 & PLL_SSE) {
            // TODO: spread spectrum mode not currently supported
            return false;
        }

        uint32 divr1, divr2, divf1, divf2;
        uint32 divout, cfg0, cfg2;
        uint64 tmp;

        cfg2 = ind(_reg + 0x8);
        divr1 = (cfg2 & PLL_REF_DIVR1_MASK) >> PLL_REF_DIVR1_SHIFT;
        divr2 = (cfg2 & PLL_REF_DIVR2_MASK) >> PLL_REF_DIVR2_SHIFT;
        divf1 = (cfg2 & PLL_FEEDBACK_DIVF1_MASK) >> PLL_FEEDBACK_DIVF1_SHIFT;
        divf2 = (cfg2 & PLL_FEEDBACK_DIVF2_MASK) >> PLL_FEEDBACK_DIVF2_SHIFT;
        divout = (cfg2 & PLL_OUTPUT_DIV_VAL_MASK) >> PLL_OUTPUT_DIV_VAL_SHIFT;

        cfg0 = ind(_reg);
        if (cfg0 & PLL_BYPASS2) {
            tmp = prate;
        } else if (cfg0 & PLL_BYPASS1) {
            tmp = (prate * divf2) / ((divr2 + 1) * (divout + 1));
        } else {
            tmp = (prate * 2) * (divf1 + 1) * (divf2 + 1);
            tmp = tmp / ((divr1 + 1) * (divr2 + 1) * (divout + 1));
        }

        _rate = static_cast<uint32>(tmp);
        rate = _rate;
        return true;
    }

    bool set_parent(Clock *) override { return false; }

    bool get_parent(Clock **parent) override {
        if (_parent != nullptr) {
            *parent = _parent;
            return true;
        }
        return false;
    }

    bool enable(void) override {
        if (_enabled) return true;

        uint32 cfg0 = ind(_reg);
        cfg0 &= ~PLL_PD;
        outd(_reg, cfg0);

        if (cfg0 & PLL_BYPASS2) {
            _enabled = true;
            return true;
        }

        do {
            cfg0 = ind(_reg);
            cfg0 &= PLL_LOCK;
        } while (cfg0 == 0);

        _enabled = true;
        return _enabled;
    }

    bool disable(void) override {
        if (_is_critical) return false;
        if (!_enabled) return true;

        uint32 cfg0 = ind(_reg);
        cfg0 |= PLL_PD;
        outd(_reg, cfg0);
        _enabled = false;
        return true;
    }

    /**
     *  Spec for PLL output -
     *  SSE=0: PLLOUT = REF/DIVR1 * 2 * DIVF1/DIVR2 * DIVF2/DIVQ
     *  PLL_BYPASS2=1: PLL_OUT = REF
     */
    void init(void) override {
        uint32 prate;
        if (_parent != nullptr)
            _parent->get_rate(prate);
        else
            return;

        uint32 cfg1 = ind(_reg + 0x4);
        if (cfg1 & PLL_SSE) {
            // TODO: spread spectrum mode not currently supported
            return;
        }

        uint32 divr1, divr2, divf1, divf2;
        uint32 divout, cfg0, cfg2;
        uint64 tmp;

        cfg2 = ind(_reg + 0x8);
        divr1 = (cfg2 & PLL_REF_DIVR1_MASK) >> PLL_REF_DIVR1_SHIFT;
        divr2 = (cfg2 & PLL_REF_DIVR2_MASK) >> PLL_REF_DIVR2_SHIFT;
        divf1 = (cfg2 & PLL_FEEDBACK_DIVF1_MASK) >> PLL_FEEDBACK_DIVF1_SHIFT;
        divf2 = (cfg2 & PLL_FEEDBACK_DIVF2_MASK) >> PLL_FEEDBACK_DIVF2_SHIFT;
        divout = (cfg2 & PLL_OUTPUT_DIV_VAL_MASK) >> PLL_OUTPUT_DIV_VAL_SHIFT;

        cfg0 = ind(_reg);
        if (cfg0 & PLL_BYPASS2) {
            tmp = prate;
        } else if (cfg0 & PLL_BYPASS1) {
            tmp = (prate * divf2) / ((divr2 + 1) * (divout + 1));
        } else {
            tmp = (prate * 2) * (divf1 + 1) * (divf2 + 1);
            tmp = tmp / ((divr1 + 1) * (divr2 + 1) * (divout + 1));
        }

        _rate = static_cast<uint32>(tmp);
        _enabled = ((cfg0 & PLL_PD) == 0);
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    bool _is_critical;
};

/**
 * CCM composite clock. Always 8 parents
 *
 */
class Imx_ccm_clk : public Clock {

public:
    enum CCM_TARGET_ROOT : uint32 {
        POST_PODF_SHIFT = (0),
        POST_DIV_WIDTH = (6),
        POST_DIV_MAX = ((1u << POST_DIV_WIDTH) - 1),
        POST_PODF_MASK = (POST_DIV_MAX << POST_PODF_SHIFT),

        PRE_PODF_SHIFT = (16),
        PRE_DIV_WIDTH = 3,
        PRE_DIV_MAX = ((1u << PRE_DIV_WIDTH) - 1),
        PRE_PODF_MASK = (PRE_DIV_MAX << PRE_PODF_SHIFT),

        MUX_SHIFT = (24),
        MUX_MASK = (0x7u << MUX_SHIFT),

        ENABLE = (0x1u << 28)
    };

    Imx_ccm_clk(uint32 id, Clock *parents[8], mword addr, bool critical)
        : Clock(id, addr, nullptr, 0), _is_critical(critical) {
        for (uint8 i = 0; i < 8u; i++)
            _parents[i] = parents[i];
    }

    Imx_ccm_clk(uint32 id, Clock *parents[8], mword addr)
        : Clock(id, addr, nullptr, 0), _is_critical(false) {
        for (uint8 i = 0; i < 8u; i++)
            _parents[i] = parents[i];
    }

    bool set_rate(uint32 rate) override {
        uint32 prate;
        if (!_parent->get_rate(prate)) return false;

        uint32 pre_div = 1, post_div = 1;
        uint32 delta1 = __UINT32_MAX__;

        for (uint32 pre = 1; pre <= (PRE_DIV_MAX + 1); pre++) {
            for (uint32 post = 1; post <= (POST_DIV_MAX + 1); post++) {
                uint32 delta2 = (((prate / pre) / post) - rate);

                if (((delta2 > delta1) && ((delta2 - delta1) < delta1))
                    || ((delta1 > delta2) && ((delta1 - delta2) < delta1))) {
                    pre_div = pre;
                    post_div = post;
                    delta1 = delta2;
                }
            }
        }

        uint32 reg = ind(_reg);
        reg &= ~(PRE_PODF_MASK | POST_PODF_MASK);
        reg |= (pre_div - 1) << PRE_PODF_SHIFT;
        reg |= (post_div - 1);
        outd(_reg, reg);

        _rate = CLOCK_DIV_UP(rate, pre_div);
        _rate = CLOCK_DIV_UP(_rate, post_div);

        return true;
    }

    bool get_rate(uint32 &rate) override {
        rate = _rate;
        return true;
    }

    bool set_parent(Clock *parent) override {
        uint8 idx = 9;
        for (uint8 i = 0; i < 8; i++)
            if (_parents[i] == parent) idx = i;

        if (idx < 8) {
            uint32 reg = ind(_reg);
            reg &= ~MUX_MASK;
            reg |= static_cast<uint32>(idx) << MUX_SHIFT;
            outd(_reg, reg);
            _parent = parent;

            uint32 rate;
            if (_parent->get_rate(rate)) {
                uint32 pre_div = ((reg & PRE_PODF_MASK) >> PRE_PODF_SHIFT);
                uint32 post_div = reg & POST_PODF_MASK;
                _rate = CLOCK_DIV_UP(rate, pre_div);
                _rate = CLOCK_DIV_UP(_rate, post_div);
            }
            return true;
        } else
            return false; // Not a valid parent for this clock
    }

    bool get_parent(Clock **parent) override {
        *parent = _parent;
        return true;
    }

    bool enable(void) override {
        if (!_parent->enable()) return false;

        uint32 reg = ind(_reg);
        reg |= ENABLE;
        outd(_reg, reg);
        _enabled = true;
        return true;
    }

    bool disable(void) override {
        if (_is_critical) return false;

        uint32 reg = ind(_reg);
        reg &= ~ENABLE;
        outd(_reg, reg);
        _enabled = false;
        return true;
    }

    void init(void) override {
        uint32 reg = ind(_reg);
        _enabled = ((reg & ENABLE) > 0);

        uint8 idx = ((reg & MUX_MASK) >> MUX_SHIFT);
        _parent = _parents[idx];

        uint32 rate;
        if (_parent->get_rate(rate)) {
            uint32 pre_div = (((reg & PRE_PODF_MASK) >> PRE_PODF_SHIFT) + 1);
            uint32 post_div = (reg & POST_PODF_MASK) + 1;
            _rate = CLOCK_DIV_UP(rate, pre_div);
            _rate = CLOCK_DIV_UP(_rate, post_div);
        }
    }

    bool describe_rate(Pm::clk_desc &desc) override {
        desc.triplet = false;
        return get_rate(desc.min);
    }

private:
    Clock *_parents[8];
    bool _is_critical;
};

/**
 * manage control ops for CCM instance
 */
class Imx_ClkCtrl {
public:
    Errno probe(void);

    Errno enable_clk(uint64 clk_id);

    Errno get_clkrate(uint64 clk_id, uint64 &value);

    Errno disable_clk(uint64 clk_id);

    Errno set_clkrate(uint64 clk_id, uint64 value);

    uint32 get_max_clkid(void);

    Errno describe_clkrate(uint64 clk_id, Pm::clk_desc &rate);

    bool is_enabled(uint64 clk_id);

    Imx_ClkCtrl(void) {
        for (uint16 i = 0; i < IMX8MQ_CLK_END; i++)
            _clks[i] = nullptr;
    }

    ~Imx_ClkCtrl() {}

private:
    Clock *_clks[IMX8MQ_CLK_END];
};