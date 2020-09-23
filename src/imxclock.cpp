/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 * This implementation is derived from the iMX8 clock driver in the Linux kernel.
 * The following copyright headers are present in the referenced sources -
 * Copyright 2018 NXP.
 * Copyright (C) 2017 Pengutronix, Lucas Stach <kernel@pengutronix.de>
 */

#include <imx8mq.hpp>
#include <imxclock.hpp>

/* Analog Clocks and PLLs */
Imx_fixed_clock imx_clk_dummy(IMX8MQ_CLK_DUMMY, 0);
Imx_fixed_clock imx_clk_32k(IMX8MQ_CLK_32K, 32768);
Imx_fixed_clock imx_clk_25m(IMX8MQ_CLK_25M, 25000000);
Imx_fixed_clock imx_clk_27m(IMX8MQ_CLK_27M, 27000000);
Imx_fixed_clock imx_clk_ext1(IMX8MQ_CLK_EXT1, 133000000);
Imx_fixed_clock imx_clk_ext2(IMX8MQ_CLK_EXT2, 133000000);
Imx_fixed_clock imx_clk_ext3(IMX8MQ_CLK_EXT3, 133000000);
Imx_fixed_clock imx_clk_ext4(IMX8MQ_CLK_EXT4, 133000000);

Clock* pll_ref_sels[4] = {&imx_clk_25m, &imx_clk_27m, nullptr, nullptr};

Imx_clock_mux imx_arm_pll_ref_sel(IMX8MQ_ARM_PLL_REF_SEL, (ANATOP_VA + 0x28), 16, 2, pll_ref_sels,
                                  4);
Imx_clock_mux imx_gpu_pll_ref_sel(IMX8MQ_GPU_PLL_REF_SEL, (ANATOP_VA + 0x18), 16, 2, pll_ref_sels,
                                  4);
Imx_clock_mux imx_vpu_pll_ref_sel(IMX8MQ_VPU_PLL_REF_SEL, (ANATOP_VA + 0x20), 16, 2, pll_ref_sels,
                                  4);
Imx_clock_mux imx_audio_pll1_ref_sel(IMX8MQ_AUDIO_PLL1_REF_SEL, (ANATOP_VA + 0x0), 16, 2,
                                     pll_ref_sels, 4);
Imx_clock_mux imx_audio_pll2_ref_sel(IMX8MQ_AUDIO_PLL2_REF_SEL, (ANATOP_VA + 0x8), 16, 2,
                                     pll_ref_sels, 4);
Imx_clock_mux imx_video_pll1_ref_sel(IMX8MQ_VIDEO_PLL1_REF_SEL, (ANATOP_VA + 0x10), 16, 2,
                                     pll_ref_sels, 4);
Imx_clock_mux imx_sys3_pll1_ref_sel(IMX8MQ_SYS3_PLL1_REF_SEL, (ANATOP_VA + 0x48), 0, 2,
                                    pll_ref_sels, 4);
Imx_clock_mux imx_dram_pll1_ref_sel(IMX8MQ_DRAM_PLL1_REF_SEL, (ANATOP_VA + 0x60), 0, 2,
                                    pll_ref_sels, 4);
Imx_clock_mux imx_video2_pll1_ref_sel(IMX8MQ_VIDEO2_PLL1_REF_SEL, (ANATOP_VA + 0x54), 0, 2,
                                      pll_ref_sels, 4);

Imx_clock_div imx_arm_pll_ref_div(IMX8MQ_ARM_PLL_REF_DIV, &imx_arm_pll_ref_sel, (ANATOP_VA + 0x28),
                                  5, 6);
Imx_clock_div imx_gpu_pll_ref_div(IMX8MQ_GPU_PLL_REF_DIV, &imx_gpu_pll_ref_sel, (ANATOP_VA + 0x18),
                                  5, 6);
Imx_clock_div imx_vpu_pll_ref_div(IMX8MQ_VPU_PLL_REF_DIV, &imx_vpu_pll_ref_sel, (ANATOP_VA + 0x20),
                                  5, 6);
Imx_clock_div imx_audio_pll1_ref_div(IMX8MQ_AUDIO_PLL1_REF_DIV, &imx_audio_pll1_ref_sel,
                                     (ANATOP_VA + 0x0), 5, 6);
Imx_clock_div imx_audio_pll2_ref_div(IMX8MQ_AUDIO_PLL2_REF_DIV, &imx_audio_pll2_ref_sel,
                                     (ANATOP_VA + 0x8), 5, 6);
Imx_clock_div imx_video_pll1_ref_div(IMX8MQ_VIDEO_PLL1_REF_DIV, &imx_video_pll1_ref_sel,
                                     (ANATOP_VA + 0x10), 5, 6);

Frac_pll imx_arm_pll(IMX8MQ_ARM_PLL, &imx_arm_pll_ref_div, (ANATOP_VA + 0x28));
Frac_pll imx_gpu_pll(IMX8MQ_GPU_PLL, &imx_gpu_pll_ref_div, (ANATOP_VA + 0x18));
Frac_pll imx_vpu_pll(IMX8MQ_VPU_PLL, &imx_vpu_pll_ref_div, (ANATOP_VA + 0x20));
Frac_pll imx_audio_pll1(IMX8MQ_AUDIO_PLL1, &imx_audio_pll1_ref_div, (ANATOP_VA + 0x0));
Frac_pll imx_audio_pll2(IMX8MQ_AUDIO_PLL2, &imx_audio_pll2_ref_div, (ANATOP_VA + 0x8));
Frac_pll imx_video_pll1(IMX8MQ_VIDEO_PLL1, &imx_video_pll1_ref_div, (ANATOP_VA + 0x10));

/* PLL bypass out */
Clock* arm_pll_bypass_sels[] = {&imx_arm_pll, &imx_arm_pll_ref_sel};
Imx_clock_mux imx_arm_pll_bypass(IMX8MQ_ARM_PLL_BYPASS, (ANATOP_VA + 0x28), 14, 1,
                                 arm_pll_bypass_sels, 2, (CLOCK_CHANGE_RATE_PARENT));

Clock* gpu_pll_bypass_sels[] = {&imx_gpu_pll, &imx_gpu_pll_ref_sel};
Imx_clock_mux imx_gpu_pll_bypass(IMX8MQ_GPU_PLL_BYPASS, (ANATOP_VA + 0x18), 14, 1,
                                 gpu_pll_bypass_sels, 2);

Clock* vpu_pll_bypass_sels[] = {&imx_vpu_pll, &imx_vpu_pll_ref_sel};
Imx_clock_mux imx_vpu_pll_bypass(IMX8MQ_VPU_PLL_BYPASS, (ANATOP_VA + 0x20), 14, 1,
                                 vpu_pll_bypass_sels, 2);

Clock* audio_pll1_bypass_sels[] = {&imx_audio_pll1, &imx_audio_pll1_ref_sel};
Imx_clock_mux imx_audio_pll1_bypass(IMX8MQ_AUDIO_PLL1_BYPASS, (ANATOP_VA + 0x0), 14, 1,
                                    audio_pll1_bypass_sels, 2);

Clock* audio_pll2_bypass_sels[] = {&imx_audio_pll2, &imx_audio_pll2_ref_sel};
Imx_clock_mux imx_audio_pll2_bypass(IMX8MQ_AUDIO_PLL2_BYPASS, (ANATOP_VA + 0x8), 14, 1,
                                    audio_pll2_bypass_sels, 2);

Clock* video_pll1_bypass_sels[] = {&imx_video_pll1, &imx_video_pll1_ref_sel};
Imx_clock_mux imx_video_pll1_bypass(IMX8MQ_VIDEO_PLL1_BYPASS, (ANATOP_VA + 0x10), 14, 1,
                                    video_pll1_bypass_sels, 2);

/* PLL OUT GATE */
Imx_clock_gate imx_arm_pll_out(IMX8MQ_ARM_PLL_OUT, &imx_arm_pll_bypass, (ANATOP_VA + 0x28), 21, 1);
Imx_clock_gate imx_gpu_pll_out(IMX8MQ_GPU_PLL_OUT, &imx_gpu_pll_bypass, (ANATOP_VA + 0x18), 21, 1);
Imx_clock_gate imx_vpu_pll_out(IMX8MQ_VPU_PLL_OUT, &imx_vpu_pll_bypass, (ANATOP_VA + 0x20), 21, 1);
Imx_clock_gate imx_audio_pll1_out(IMX8MQ_AUDIO_PLL1_OUT, &imx_audio_pll1_bypass, (ANATOP_VA + 0x0),
                                  21, 1);
Imx_clock_gate imx_audio_pll2_out(IMX8MQ_AUDIO_PLL2_OUT, &imx_audio_pll2_bypass, (ANATOP_VA + 0x8),
                                  21, 1);
Imx_clock_gate imx_video_pll1_out(IMX8MQ_VIDEO_PLL1_OUT, &imx_video_pll1_bypass, (ANATOP_VA + 0x10),
                                  21, 1);

Imx_fixed_clock imx_sys1_pll_out(IMX8MQ_SYS1_PLL_OUT, 800000000);
Imx_fixed_clock imx_sys2_pll_out(IMX8MQ_SYS2_PLL_OUT, 1000000000);

Sccg_pll_clk imx_sys3_pll_out(IMX8MQ_SYS3_PLL_OUT, &imx_sys3_pll1_ref_sel, (ANATOP_VA + 0x48),
                              true);
Sccg_pll_clk imx_dram_pll_out(IMX8MQ_DRAM_PLL_OUT, &imx_dram_pll1_ref_sel, (ANATOP_VA + 0x60),
                              true);
Sccg_pll_clk imx_video2_pll_out(IMX8MQ_VIDEO2_PLL_OUT, &imx_video2_pll1_ref_sel, (ANATOP_VA + 0x54),
                                false);

/* SYS PLL1 fixed output */
Imx_clock_gate imx_sys1_pll_40m_cg(IMX8MQ_SYS1_PLL_40M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30), 9,
                                   1);
Imx_clock_gate imx_sys1_pll_80m_cg(IMX8MQ_SYS1_PLL_80M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                   11, 1);
Imx_clock_gate imx_sys1_pll_100m_cg(IMX8MQ_SYS1_PLL_100M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    13, 1);
Imx_clock_gate imx_sys1_pll_133m_cg(IMX8MQ_SYS1_PLL_133M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    15, 1);
Imx_clock_gate imx_sys1_pll_160m_cg(IMX8MQ_SYS1_PLL_160M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    17, 1);
Imx_clock_gate imx_sys1_pll_200m_cg(IMX8MQ_SYS1_PLL_200M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    19, 1);
Imx_clock_gate imx_sys1_pll_266m_cg(IMX8MQ_SYS1_PLL_266M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    21, 1);
Imx_clock_gate imx_sys1_pll_400m_cg(IMX8MQ_SYS1_PLL_400M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    23, 1);
Imx_clock_gate imx_sys1_pll_800m_cg(IMX8MQ_SYS1_PLL_800M_CG, &imx_sys1_pll_out, (ANATOP_VA + 0x30),
                                    25, 1);

Imx_fixdiv_clock imx_sys1_pll_40m(IMX8MQ_SYS1_PLL_40M, &imx_sys1_pll_40m_cg, 1, 20);
Imx_fixdiv_clock imx_sys1_pll_80m(IMX8MQ_SYS1_PLL_80M, &imx_sys1_pll_80m_cg, 1, 10);
Imx_fixdiv_clock imx_sys1_pll_100m(IMX8MQ_SYS1_PLL_100M, &imx_sys1_pll_100m_cg, 1, 8);
Imx_fixdiv_clock imx_sys1_pll_133m(IMX8MQ_SYS1_PLL_133M, &imx_sys1_pll_133m_cg, 1, 6);
Imx_fixdiv_clock imx_sys1_pll_160m(IMX8MQ_SYS1_PLL_160M, &imx_sys1_pll_160m_cg, 1, 5);
Imx_fixdiv_clock imx_sys1_pll_200m(IMX8MQ_SYS1_PLL_200M, &imx_sys1_pll_200m_cg, 1, 4);
Imx_fixdiv_clock imx_sys1_pll_266m(IMX8MQ_SYS1_PLL_266M, &imx_sys1_pll_266m_cg, 1, 3);
Imx_fixdiv_clock imx_sys1_pll_400m(IMX8MQ_SYS1_PLL_400M, &imx_sys1_pll_400m_cg, 1, 2);
Imx_fixdiv_clock imx_sys1_pll_800m(IMX8MQ_SYS1_PLL_800M, &imx_sys1_pll_800m_cg, 1, 1);

/* SYS PLL2 fixed output */
Imx_clock_gate imx_sys2_pll_50m_cg(IMX8MQ_SYS2_PLL_50M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c), 9,
                                   1);
Imx_clock_gate imx_sys2_pll_100m_cg(IMX8MQ_SYS2_PLL_100M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    11, 1);
Imx_clock_gate imx_sys2_pll_125m_cg(IMX8MQ_SYS2_PLL_125M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    13, 1);
Imx_clock_gate imx_sys2_pll_166m_cg(IMX8MQ_SYS2_PLL_166M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    15, 1);
Imx_clock_gate imx_sys2_pll_200m_cg(IMX8MQ_SYS2_PLL_200M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    17, 1);
Imx_clock_gate imx_sys2_pll_250m_cg(IMX8MQ_SYS2_PLL_250M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    19, 1);
Imx_clock_gate imx_sys2_pll_333m_cg(IMX8MQ_SYS2_PLL_333M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    21, 1);
Imx_clock_gate imx_sys2_pll_500m_cg(IMX8MQ_SYS2_PLL_500M_CG, &imx_sys2_pll_out, (ANATOP_VA + 0x3c),
                                    23, 1);
Imx_clock_gate imx_sys2_pll_1000m_cg(IMX8MQ_SYS2_PLL_1000M_CG, &imx_sys2_pll_out,
                                     (ANATOP_VA + 0x3c), 25, 1);

Imx_fixdiv_clock imx_sys2_pll_50m(IMX8MQ_SYS2_PLL_50M, &imx_sys2_pll_50m_cg, 1, 20);
Imx_fixdiv_clock imx_sys2_pll_100m(IMX8MQ_SYS2_PLL_100M, &imx_sys2_pll_100m_cg, 1, 10);
Imx_fixdiv_clock imx_sys2_pll_125m(IMX8MQ_SYS2_PLL_125M, &imx_sys2_pll_125m_cg, 1, 8);
Imx_fixdiv_clock imx_sys2_pll_166m(IMX8MQ_SYS2_PLL_166M, &imx_sys2_pll_166m_cg, 1, 6);
Imx_fixdiv_clock imx_sys2_pll_200m(IMX8MQ_SYS2_PLL_200M, &imx_sys2_pll_200m_cg, 1, 5);
Imx_fixdiv_clock imx_sys2_pll_250m(IMX8MQ_SYS2_PLL_250M, &imx_sys2_pll_250m_cg, 1, 4);
Imx_fixdiv_clock imx_sys2_pll_333m(IMX8MQ_SYS2_PLL_333M, &imx_sys2_pll_333m_cg, 1, 3);
Imx_fixdiv_clock imx_sys2_pll_500m(IMX8MQ_SYS2_PLL_500M, &imx_sys2_pll_500m_cg, 1, 2);
Imx_fixdiv_clock imx_sys2_pll_1000m(IMX8MQ_SYS2_PLL_1000M, &imx_sys2_pll_1000m_cg, 1, 1);

/* CCM Clocks */

/* CORE */

Clock* imx8mq_a53_sels[]
    = {&imx_clk_25m,       &imx_arm_pll_out,   &imx_sys2_pll_500m,  &imx_sys2_pll_1000m,
       &imx_sys1_pll_800m, &imx_sys1_pll_400m, &imx_audio_pll1_out, &imx_sys3_pll_out};

Clock* imx8mq_arm_m4_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m,  &imx_sys2_pll_250m,  &imx_sys1_pll_266m,
    &imx_sys1_pll_800m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_sys3_pll_out,
};

Clock* imx8mq_vpu_sels[] = {
    &imx_clk_25m,       &imx_arm_pll_out,   &imx_sys3_pll_out,   &imx_sys2_pll_1000m,
    &imx_sys1_pll_800m, &imx_sys1_pll_400m, &imx_audio_pll1_out, &imx_vpu_pll_out,
};

Clock* imx8mq_gpu_core_sels[] = {
    &imx_clk_25m,        &imx_gpu_pll_out,    &imx_sys1_pll_800m,  &imx_sys3_pll_out,
    &imx_sys2_pll_1000m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};

Clock* imx8mq_gpu_shader_sels[] = {
    &imx_clk_25m,        &imx_gpu_pll_out,    &imx_sys1_pll_800m,  &imx_sys3_pll_out,
    &imx_sys2_pll_1000m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};

Imx_clock_mux imx_clk_a53_src(IMX8MQ_CLK_A53_SRC, (CCM_VA + 0x8000), 24, 3, imx8mq_a53_sels, 8,
                              CLOCK_ENABLE_PARENT);
Imx_clock_mux imx_clk_m4_src(IMX8MQ_CLK_M4_SRC, (CCM_VA + 0x8080), 24, 3, imx8mq_arm_m4_sels, 8,
                             CLOCK_ENABLE_PARENT);
Imx_clock_mux imx_clk_vpu_src(IMX8MQ_CLK_VPU_SRC, (CCM_VA + 0x8100), 24, 3, imx8mq_vpu_sels, 8,
                              CLOCK_ENABLE_PARENT);
Imx_clock_mux imx_clk_gpu_core_src(IMX8MQ_CLK_GPU_CORE_SRC, (CCM_VA + 0x8180), 24, 3,
                                   imx8mq_gpu_core_sels, 8, CLOCK_ENABLE_PARENT);
Imx_clock_mux imx_clk_gpu_shader_src(IMX8MQ_CLK_GPU_SHADER_SRC, (CCM_VA + 0x8200), 24, 3,
                                     imx8mq_gpu_shader_sels, 8, CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_a53_cg(IMX8MQ_CLK_A53_CG, &imx_clk_a53_src, (CCM_VA + 0x8000), 28, 1,
                              CLOCK_ENABLE_PARENT); // critical!
Imx_clock_gate imx_clk_m4_cg(IMX8MQ_CLK_M4_CG, &imx_clk_m4_src, (CCM_VA + 0x8080), 28, 1,
                             CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_vpu_cg(IMX8MQ_CLK_VPU_CG, &imx_clk_vpu_src, (CCM_VA + 0x8100), 28, 1,
                              CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpu_core_cg(IMX8MQ_CLK_GPU_CORE_CG, &imx_clk_gpu_core_src, (CCM_VA + 0x8180),
                                   28, 1, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpu_shader_cg(IMX8MQ_CLK_GPU_SHADER_CG, &imx_clk_gpu_core_src,
                                     (CCM_VA + 0x8200), 28, 1, CLOCK_ENABLE_PARENT);

Imx_clock_div imx_clk_a53_div(IMX8MQ_CLK_A53_DIV, &imx_clk_a53_cg, (CCM_VA + 0x8000), 0, 3,
                              CLOCK_ENABLE_PARENT);
Imx_clock_div imx_clk_m4_div(IMX8MQ_CLK_M4_DIV, &imx_clk_m4_cg, (CCM_VA + 0x8080), 0, 3,
                             CLOCK_ENABLE_PARENT);
Imx_clock_div imx_clk_vpu_div(IMX8MQ_CLK_VPU_DIV, &imx_clk_vpu_cg, (CCM_VA + 0x8100), 0, 3,
                              CLOCK_ENABLE_PARENT);
Imx_clock_div imx_clk_gpu_core_div(IMX8MQ_CLK_GPU_CORE_DIV, &imx_clk_gpu_core_cg, (CCM_VA + 0x8180),
                                   0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_div imx_clk_gpu_shader_div(IMX8MQ_CLK_GPU_SHADER_DIV, &imx_clk_gpu_shader_cg,
                                     (CCM_VA + 0x8200), 0, 3, CLOCK_ENABLE_PARENT);

/* BUS */
Clock* imx8mq_main_axi_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_333m,  &imx_sys1_pll_800m,  &imx_sys2_pll_250m,
    &imx_sys2_pll_1000m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_sys1_pll_100m,
};

Clock* imx8mq_enet_axi_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_266m,  &imx_sys1_pll_800m,  &imx_sys2_pll_250m,
    &imx_sys2_pll_200m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_sys3_pll_out,
};

Clock* imx8mq_nand_usdhc_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_266m, &imx_sys1_pll_800m, &imx_sys2_pll_200m,
    &imx_sys1_pll_133m, &imx_sys3_pll_out,  &imx_sys2_pll_250m, &imx_audio_pll1_out,
};

Clock* imx8mq_vpu_bus_sels[] = {
    &imx_clk_25m,      &imx_sys1_pll_800m,  &imx_vpu_pll_out,   &imx_audio_pll2_out,
    &imx_sys3_pll_out, &imx_sys2_pll_1000m, &imx_sys2_pll_200m, &imx_sys1_pll_100m,
};

Clock* imx8mq_disp_axi_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_125m,  &imx_sys1_pll_800m, &imx_sys3_pll_out,
    &imx_sys1_pll_400m, &imx_audio_pll2_out, &imx_clk_ext1,      &imx_clk_ext4,
};

Clock* imx8mq_disp_apb_sels[] = {
    &imx_clk_25m,      &imx_sys2_pll_125m,  &imx_sys1_pll_800m, &imx_sys3_pll_out,
    &imx_sys1_pll_40m, &imx_audio_pll2_out, &imx_clk_ext1,      &imx_clk_ext3,
};

Clock* imx8mq_disp_rtrm_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_800m,  &imx_sys2_pll_200m, &imx_sys1_pll_400m,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_clk_ext2,      &imx_clk_ext3,
};

Clock* imx8mq_usb_bus_sels[] = {
    &imx_clk_25m,       &imx_sys3_pll_out, &imx_sys1_pll_800m, &imx_sys2_pll_100m,
    &imx_sys2_pll_200m, &imx_clk_ext2,     &imx_clk_ext4,      &imx_audio_pll2_out,
};

Clock* imx8mq_gpu_axi_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_800m,  &imx_gpu_pll_out,    &imx_sys3_pll_out,
    &imx_sys2_pll_1000m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};

Clock* imx8mq_gpu_ahb_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_800m,  &imx_gpu_pll_out,    &imx_sys3_pll_out,
    &imx_sys2_pll_1000m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};

Clock* imx8mq_noc_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_800m,  &imx_sys3_pll_out,   &imx_sys2_pll_1000m,
    &imx_sys2_pll_500m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};

Clock* imx8mq_noc_apb_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_400m, &imx_sys3_pll_out,   &imx_sys2_pll_333m,
    &imx_sys2_pll_200m, &imx_sys1_pll_800m, &imx_audio_pll1_out, &imx_video_pll1_out,
};

Imx_ccm_clk imx_clk_main_axi(IMX8MQ_CLK_MAIN_AXI, imx8mq_main_axi_sels, (CCM_VA + 0x8800), true);
Imx_ccm_clk imx_clk_enet_axi(IMX8MQ_CLK_ENET_AXI, imx8mq_enet_axi_sels, (CCM_VA + 0x8880));
Imx_ccm_clk imx_clk_nand_usdhc_bus(IMX8MQ_CLK_NAND_USDHC_BUS, imx8mq_nand_usdhc_sels,
                                   (CCM_VA + 0x8900));
Imx_ccm_clk imx_clk_vpu_bus(IMX8MQ_CLK_VPU_BUS, imx8mq_vpu_bus_sels, (CCM_VA + 0x8980));
Imx_ccm_clk imx_clk_disp_axi(IMX8MQ_CLK_DISP_AXI, imx8mq_disp_axi_sels, (CCM_VA + 0x8a00));
Imx_ccm_clk imx_clk_disp_apb(IMX8MQ_CLK_DISP_APB, imx8mq_disp_apb_sels, (CCM_VA + 0x8a80));
Imx_ccm_clk imx_clk_disp_rtrm(IMX8MQ_CLK_DISP_RTRM, imx8mq_disp_rtrm_sels, (CCM_VA + 0x8b00));
Imx_ccm_clk imx_clk_usb_bus(IMX8MQ_CLK_USB_BUS, imx8mq_usb_bus_sels, (CCM_VA + 0x8b80));
Imx_ccm_clk imx_clk_gpu_axi(IMX8MQ_CLK_GPU_AXI, imx8mq_gpu_axi_sels, (CCM_VA + 0x8c00));
Imx_ccm_clk imx_clk_gpu_ahb(IMX8MQ_CLK_GPU_AHB, imx8mq_gpu_ahb_sels, (CCM_VA + 0x8c80));
Imx_ccm_clk imx_clk_noc(IMX8MQ_CLK_NOC, imx8mq_noc_sels, (CCM_VA + 0x8d00), true);
Imx_ccm_clk imx_clk_noc_apb(IMX8MQ_CLK_NOC_APB, imx8mq_noc_apb_sels, (CCM_VA + 0x8d80), true);

/* AHB */
Clock* imx8mq_ahb_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_133m, &imx_sys1_pll_800m,  &imx_sys1_pll_400m,
    &imx_sys2_pll_125m, &imx_sys3_pll_out,  &imx_audio_pll1_out, &imx_video_pll1_out,
};

Clock* imx8mq_audio_ahb_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_500m, &imx_sys1_pll_800m,  &imx_sys2_pll_1000m,
    &imx_sys2_pll_166m, &imx_sys3_pll_out,  &imx_audio_pll1_out, &imx_video_pll1_out,
};

/* AHB clock is used by the AHB bus therefore marked as critical */
Imx_ccm_clk imx_clk_ahb(IMX8MQ_CLK_AHB, imx8mq_ahb_sels, (CCM_VA + 0x9000), true);
Imx_ccm_clk imx_clk_audio_ahb(IMX8MQ_CLK_AUDIO_AHB, imx8mq_audio_ahb_sels, (CCM_VA + 0x9100));

/* IPG */
Imx_clock_div imx_clk_ipg_root(IMX8MQ_CLK_IPG_ROOT, &imx_clk_ahb, (CCM_VA + 0x9080), 0, 1,
                               CLOCK_ENABLE_PARENT);
Imx_clock_div imx_clk_ipg_audio_root(IMX8MQ_CLK_IPG_AUDIO_ROOT, &imx_clk_audio_ahb,
                                     (CCM_VA + 0x9180), 0, 1, CLOCK_ENABLE_PARENT);

Clock* imx8mq_dram_alt_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_800m, &imx_sys1_pll_100m,  &imx_sys2_pll_500m,
    &imx_sys2_pll_250m, &imx_sys1_pll_400m, &imx_audio_pll1_out, &imx_sys1_pll_266m,
};

Clock* imx8mq_dram_apb_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys1_pll_40m,  &imx_sys1_pll_160m,
    &imx_sys1_pll_800m, &imx_sys3_pll_out,  &imx_sys2_pll_250m, &imx_audio_pll2_out,
};

Imx_ccm_clk imx_clk_dram_alt(IMX8MQ_CLK_DRAM_ALT, imx8mq_dram_alt_sels, (CCM_VA + 0xa000));
Imx_ccm_clk imx_clk_dram_apb(IMX8MQ_CLK_DRAM_APB, imx8mq_dram_apb_sels, (CCM_VA + 0xa080), true);
Imx_fixdiv_clock imx_clk_dram_alt_root(IMX8MQ_CLK_DRAM_ALT_ROOT, &imx_clk_dram_alt, 1, 4);

Clock* imx8mq_dram_core_sels[] = {&imx_dram_pll_out, &imx_clk_dram_alt_root};

Imx_clock_mux imx_clk_dram_core(IMX8MQ_CLK_DRAM_CORE, (CCM_VA + 0x9800), 24, 1,
                                imx8mq_dram_core_sels, 2,
                                CLOCK_ENABLE_PARENT); // critical!

Clock* imx8mq_vpu_g1_sels[] = {
    &imx_clk_25m,       &imx_vpu_pll_out,   &imx_sys1_pll_800m, &imx_sys2_pll_1000m,
    &imx_sys1_pll_100m, &imx_sys2_pll_125m, &imx_sys3_pll_out,  &imx_audio_pll1_out,
};

Clock* imx8mq_vpu_g2_sels[] = {
    &imx_clk_25m,       &imx_vpu_pll_out,   &imx_sys1_pll_800m, &imx_sys2_pll_1000m,
    &imx_sys1_pll_100m, &imx_sys2_pll_125m, &imx_sys3_pll_out,  &imx_audio_pll1_out,
};

Imx_ccm_clk imx_clk_vpu_g1(IMX8MQ_CLK_VPU_G1, imx8mq_vpu_g1_sels, (CCM_VA + 0xa100));
Imx_ccm_clk imx_clk_vpu_g2(IMX8MQ_CLK_VPU_G2, imx8mq_vpu_g2_sels, (CCM_VA + 0xa180));

Clock* imx8mq_disp_dtrc_sels[] = {
    &imx_clk_25m,       &imx_vpu_pll_out,   &imx_sys1_pll_800m, &imx_sys2_pll_1000m,
    &imx_sys1_pll_160m, &imx_sys2_pll_100m, &imx_sys3_pll_out,  &imx_audio_pll2_out,
};

Clock* imx8mq_disp_dc8000_sels[] = {
    &imx_clk_25m,       &imx_vpu_pll_out,   &imx_sys1_pll_800m, &imx_sys2_pll_1000m,
    &imx_sys1_pll_160m, &imx_sys2_pll_100m, &imx_sys3_pll_out,  &imx_audio_pll2_out,
};

Imx_ccm_clk imx_clk_disp_dtrc(IMX8MQ_CLK_DISP_DTRC, imx8mq_disp_dtrc_sels, (CCM_VA + 0xa200));
Imx_ccm_clk imx_clk_disp_dc8000(IMX8MQ_CLK_DISP_DC8000, imx8mq_disp_dc8000_sels, (CCM_VA + 0xa280));

Clock* imx8mq_pcie1_ctrl_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_250m, &imx_sys2_pll_200m, &imx_sys1_pll_266m,
    &imx_sys1_pll_800m, &imx_sys2_pll_500m, &imx_sys2_pll_250m, &imx_sys3_pll_out,
};

Clock* imx8mq_pcie1_phy_sels[] = {
    &imx_clk_25m,  &imx_sys2_pll_100m, &imx_sys2_pll_500m, &imx_clk_ext1,
    &imx_clk_ext2, &imx_clk_ext3,      &imx_clk_ext4,
};

Clock* imx8mq_pcie1_aux_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys2_pll_500m, &imx_sys3_pll_out,
    &imx_sys2_pll_100m, &imx_sys1_pll_80m,  &imx_sys1_pll_160m, &imx_sys1_pll_200m,
};

Imx_ccm_clk imx_clk_pcie1_ctrl(IMX8MQ_CLK_PCIE1_CTRL, imx8mq_pcie1_ctrl_sels, (CCM_VA + 0xa300));
Imx_ccm_clk imx_clk_pcie1_phy(IMX8MQ_CLK_PCIE1_PHY, imx8mq_pcie1_phy_sels, (CCM_VA + 0xa380));
Imx_ccm_clk imx_clk_pcie1_aux(IMX8MQ_CLK_PCIE1_AUX, imx8mq_pcie1_aux_sels, (CCM_VA + 0xa400));

Clock* imx8mq_dc_pixel_sels[] = {
    &imx_clk_25m,       &imx_video_pll1_out, &imx_audio_pll2_out, &imx_audio_pll1_out,
    &imx_sys1_pll_800m, &imx_sys2_pll_1000m, &imx_sys3_pll_out,   &imx_clk_ext4,
};

Clock* imx8mq_lcdif_pixel_sels[] = {
    &imx_clk_25m,       &imx_video_pll1_out, &imx_audio_pll2_out, &imx_audio_pll1_out,
    &imx_sys1_pll_800m, &imx_sys2_pll_1000m, &imx_sys3_pll_out,   &imx_clk_ext4,
};

Imx_ccm_clk imx_clk_dc_pixel(IMX8MQ_CLK_DC_PIXEL, imx8mq_dc_pixel_sels, (CCM_VA + 0xa480));
Imx_ccm_clk imx_clk_lcdif_pixel(IMX8MQ_CLK_LCDIF_PIXEL, imx8mq_lcdif_pixel_sels, (CCM_VA + 0xa500));

Clock* imx8mq_sai1_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext1,       &imx_clk_ext2,
};

Clock* imx8mq_sai2_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext2,       &imx_clk_ext3,
};

Clock* imx8mq_sai3_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext3,       &imx_clk_ext4,
};

Clock* imx8mq_sai4_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext1,       &imx_clk_ext2,
};

Clock* imx8mq_sai5_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext2,       &imx_clk_ext3,
};

Clock* imx8mq_sai6_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext3,       &imx_clk_ext4,
};

Imx_ccm_clk imx_clk_sai1(IMX8MQ_CLK_SAI1, imx8mq_sai1_sels, (CCM_VA + 0xa580));
Imx_ccm_clk imx_clk_sai2(IMX8MQ_CLK_SAI2, imx8mq_sai2_sels, (CCM_VA + 0xa600));
Imx_ccm_clk imx_clk_sai3(IMX8MQ_CLK_SAI3, imx8mq_sai3_sels, (CCM_VA + 0xa680));
Imx_ccm_clk imx_clk_sai4(IMX8MQ_CLK_SAI4, imx8mq_sai4_sels, (CCM_VA + 0xa700));
Imx_ccm_clk imx_clk_sai5(IMX8MQ_CLK_SAI5, imx8mq_sai5_sels, (CCM_VA + 0xa780));
Imx_ccm_clk imx_clk_sai6(IMX8MQ_CLK_SAI6, imx8mq_sai6_sels, (CCM_VA + 0xa800));

Clock* imx8mq_spdif1_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext2,       &imx_clk_ext3,
};

Clock* imx8mq_spdif2_sels[] = {
    &imx_clk_25m,       &imx_audio_pll1_out, &imx_audio_pll2_out, &imx_video_pll1_out,
    &imx_sys1_pll_133m, &imx_clk_27m,        &imx_clk_ext3,       &imx_clk_ext4,
};

Imx_ccm_clk imx_clk_spdif1(IMX8MQ_CLK_SPDIF1, imx8mq_spdif1_sels, (CCM_VA + 0xa880));
Imx_ccm_clk imx_clk_spdif2(IMX8MQ_CLK_SPDIF2, imx8mq_spdif2_sels, (CCM_VA + 0xa900));

Clock* imx8mq_enet_ref_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_125m,  &imx_sys2_pll_500m,  &imx_sys2_pll_100m,
    &imx_sys1_pll_160m, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_clk_ext4,
};

Clock* imx8mq_enet_timer_sels[] = {
    &imx_clk_25m,  &imx_sys2_pll_100m, &imx_audio_pll1_out, &imx_clk_ext1,
    &imx_clk_ext2, &imx_clk_ext3,      &imx_clk_ext4,       &imx_video_pll1_out,
};

Clock* imx8mq_enet_phy_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_50m,   &imx_sys2_pll_125m,  &imx_sys2_pll_500m,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out,
};
Imx_ccm_clk imx_clk_enet_ref(IMX8MQ_CLK_ENET_REF, imx8mq_enet_ref_sels, (CCM_VA + 0xa980));
Imx_ccm_clk imx_clk_enet_timer(IMX8MQ_CLK_ENET_TIMER, imx8mq_enet_timer_sels, (CCM_VA + 0xaa00));
Imx_ccm_clk imx_clk_enet_phy_ref(IMX8MQ_CLK_ENET_PHY_REF, imx8mq_enet_phy_sels, (CCM_VA + 0xaa80));

Clock* imx8mq_nand_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_500m, &imx_audio_pll1_out, &imx_sys1_pll_400m,
    &imx_audio_pll2_out, &imx_sys3_pll_out,  &imx_sys2_pll_250m,  &imx_video_pll1_out,
};

Clock* imx8mq_qspi_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_400m, &imx_sys1_pll_800m, &imx_sys2_pll_500m,
    &imx_audio_pll2_out, &imx_sys1_pll_266m, &imx_sys3_pll_out,  &imx_sys1_pll_100m,
};
Imx_ccm_clk imx_clk_nand(IMX8MQ_CLK_NAND, imx8mq_nand_sels, (CCM_VA + 0xab00));
Imx_ccm_clk imx_clk_qspi(IMX8MQ_CLK_QSPI, imx8mq_qspi_sels, (CCM_VA + 0xab80));

Clock* imx8mq_usdhc1_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_400m, &imx_sys1_pll_800m, &imx_sys2_pll_500m,
    &imx_audio_pll2_out, &imx_sys1_pll_266m, &imx_sys3_pll_out,  &imx_sys1_pll_100m,
};

Clock* imx8mq_usdhc2_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_400m, &imx_sys1_pll_800m, &imx_sys2_pll_500m,
    &imx_audio_pll2_out, &imx_sys1_pll_266m, &imx_sys3_pll_out,  &imx_sys1_pll_100m,
};
Imx_ccm_clk imx_clk_usdhc1(IMX8MQ_CLK_USDHC1, imx8mq_usdhc1_sels, (CCM_VA + 0xac00));
Imx_ccm_clk imx_clk_usdhc2(IMX8MQ_CLK_USDHC2, imx8mq_usdhc2_sels, (CCM_VA + 0xac80));

Clock* imx8mq_i2c1_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_160m,  &imx_sys2_pll_50m,   &imx_sys3_pll_out,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out, &imx_sys1_pll_133m,
};

Clock* imx8mq_i2c2_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_160m,  &imx_sys2_pll_50m,   &imx_sys3_pll_out,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out, &imx_sys1_pll_133m,
};

Clock* imx8mq_i2c3_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_160m,  &imx_sys2_pll_50m,   &imx_sys3_pll_out,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out, &imx_sys1_pll_133m,
};

Clock* imx8mq_i2c4_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_160m,  &imx_sys2_pll_50m,   &imx_sys3_pll_out,
    &imx_audio_pll1_out, &imx_video_pll1_out, &imx_audio_pll2_out, &imx_sys1_pll_133m,
};
Imx_ccm_clk imx_clk_i2c1(IMX8MQ_CLK_I2C1, imx8mq_i2c1_sels, (CCM_VA + 0xad00));
Imx_ccm_clk imx_clk_i2c2(IMX8MQ_CLK_I2C2, imx8mq_i2c2_sels, (CCM_VA + 0xad80));
Imx_ccm_clk imx_clk_i2c3(IMX8MQ_CLK_I2C3, imx8mq_i2c3_sels, (CCM_VA + 0xae00));
Imx_ccm_clk imx_clk_i2c4(IMX8MQ_CLK_I2C4, imx8mq_i2c4_sels, (CCM_VA + 0xae80));

Clock* imx8mq_uart1_sels[] = {
    &imx_clk_25m,      &imx_sys1_pll_80m, &imx_sys2_pll_200m, &imx_sys2_pll_100m,
    &imx_sys3_pll_out, &imx_clk_ext2,     &imx_clk_ext4,      &imx_audio_pll2_out,
};

Clock* imx8mq_uart2_sels[] = {
    &imx_clk_25m,      &imx_sys1_pll_80m, &imx_sys2_pll_200m, &imx_sys2_pll_100m,
    &imx_sys3_pll_out, &imx_clk_ext2,     &imx_clk_ext3,      &imx_audio_pll2_out,
};

Clock* imx8mq_uart3_sels[] = {
    &imx_clk_25m,      &imx_sys1_pll_80m, &imx_sys2_pll_200m, &imx_sys2_pll_100m,
    &imx_sys3_pll_out, &imx_clk_ext2,     &imx_clk_ext4,      &imx_audio_pll2_out,
};

Clock* imx8mq_uart4_sels[] = {
    &imx_clk_25m,      &imx_sys1_pll_80m, &imx_sys2_pll_200m, &imx_sys2_pll_100m,
    &imx_sys3_pll_out, &imx_clk_ext2,     &imx_clk_ext3,      &imx_audio_pll2_out,
};
Imx_ccm_clk imx_clk_uart1(IMX8MQ_CLK_UART1, imx8mq_uart1_sels, (CCM_VA + 0xaf00));
Imx_ccm_clk imx_clk_uart2(IMX8MQ_CLK_UART2, imx8mq_uart2_sels, (CCM_VA + 0xaf80));
Imx_ccm_clk imx_clk_uart3(IMX8MQ_CLK_UART3, imx8mq_uart3_sels, (CCM_VA + 0xb000));
Imx_ccm_clk imx_clk_uart4(IMX8MQ_CLK_UART4, imx8mq_uart4_sels, (CCM_VA + 0xb080));

Clock* imx8mq_usb_core_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_100m, &imx_sys1_pll_40m, &imx_sys2_pll_100m,
    &imx_sys2_pll_200m, &imx_clk_ext2,      &imx_clk_ext3,     &imx_audio_pll2_out,
};

Clock* imx8mq_usb_phy_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_100m, &imx_sys1_pll_40m, &imx_sys2_pll_100m,
    &imx_sys2_pll_200m, &imx_clk_ext2,      &imx_clk_ext3,     &imx_audio_pll2_out,
};

Imx_ccm_clk imx_clk_usb_core_ref(IMX8MQ_CLK_USB_CORE_REF, imx8mq_usb_core_sels, (CCM_VA + 0xb100));
Imx_ccm_clk imx_clk_usb_phy_ref(IMX8MQ_CLK_USB_PHY_REF, imx8mq_usb_phy_sels, (CCM_VA + 0xb180));

Clock* imx8mq_gic_sels[]
    = {&imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys1_pll_40m, &imx_sys2_pll_100m,
       &imx_sys2_pll_200m, &imx_clk_ext2,      &imx_clk_ext3,     &imx_audio_pll2_out};

Imx_ccm_clk imx_clk_gic(IMX8MQ_CLK_GIC, imx8mq_gic_sels, (CCM_VA + 0xb200), true);

Clock* imx8mq_ecspi1_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys1_pll_40m,  &imx_sys1_pll_160m,
    &imx_sys1_pll_800m, &imx_sys3_pll_out,  &imx_sys2_pll_250m, &imx_audio_pll2_out,
};

Clock* imx8mq_ecspi2_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys1_pll_40m,  &imx_sys1_pll_160m,
    &imx_sys1_pll_800m, &imx_sys3_pll_out,  &imx_sys2_pll_250m, &imx_audio_pll2_out,
};
Imx_ccm_clk imx_clk_ecspi1(IMX8MQ_CLK_ECSPI1, imx8mq_ecspi1_sels, (CCM_VA + 0xb280));
Imx_ccm_clk imx_clk_ecspi2(IMX8MQ_CLK_ECSPI2, imx8mq_ecspi2_sels, (CCM_VA + 0xb300));

Clock* imx8mq_pwm1_sels[] = {
    &imx_clk_25m,      &imx_sys2_pll_100m, &imx_sys1_pll_160m, &imx_sys1_pll_40m,
    &imx_sys3_pll_out, &imx_clk_ext1,      &imx_sys1_pll_80m,  &imx_video_pll1_out,
};

Clock* imx8mq_pwm2_sels[] = {
    &imx_clk_25m,      &imx_sys2_pll_100m, &imx_sys1_pll_160m, &imx_sys1_pll_40m,
    &imx_sys3_pll_out, &imx_clk_ext1,      &imx_sys1_pll_80m,  &imx_video_pll1_out,
};

Clock* imx8mq_pwm3_sels[] = {
    &imx_clk_25m,      &imx_sys2_pll_100m, &imx_sys1_pll_160m, &imx_sys1_pll_40m,
    &imx_sys3_pll_out, &imx_clk_ext2,      &imx_sys1_pll_80m,  &imx_video_pll1_out,
};

Clock* imx8mq_pwm4_sels[] = {
    &imx_clk_25m,      &imx_sys2_pll_100m, &imx_sys1_pll_160m, &imx_sys1_pll_40m,
    &imx_sys3_pll_out, &imx_clk_ext2,      &imx_sys1_pll_80m,  &imx_video_pll1_out,
};
Imx_ccm_clk imx_clk_pwm1(IMX8MQ_CLK_PWM1, imx8mq_pwm1_sels, (CCM_VA + 0xb380));
Imx_ccm_clk imx_clk_pwm2(IMX8MQ_CLK_PWM2, imx8mq_pwm2_sels, (CCM_VA + 0xb400));
Imx_ccm_clk imx_clk_pwm3(IMX8MQ_CLK_PWM3, imx8mq_pwm3_sels, (CCM_VA + 0xb480));
Imx_ccm_clk imx_clk_pwm4(IMX8MQ_CLK_PWM4, imx8mq_pwm4_sels, (CCM_VA + 0xb500));

Clock* imx8mq_gpt1_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_100m, &imx_sys1_pll_400m,  &imx_sys1_pll_40m,
    &imx_video_pll1_out, &imx_sys1_pll_80m,  &imx_audio_pll1_out, &imx_clk_ext1,
};

Clock* imx8mq_wdog_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_133m, &imx_sys1_pll_160m, &imx_vpu_pll_out,
    &imx_sys2_pll_125m, &imx_sys3_pll_out,  &imx_sys1_pll_80m,  &imx_sys2_pll_166m,
};

Clock* imx8mq_wrclk_sels[] = {
    &imx_clk_25m,       &imx_sys1_pll_40m,  &imx_vpu_pll_out,   &imx_sys3_pll_out,
    &imx_sys2_pll_200m, &imx_sys1_pll_266m, &imx_sys2_pll_500m, &imx_sys1_pll_100m,
};

Clock* imx8mq_clko1_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_800m, &imx_clk_27m,     &imx_sys1_pll_200m,
    &imx_audio_pll2_out, &imx_sys2_pll_500m, &imx_vpu_pll_out, &imx_sys1_pll_80m,
};
Clock* imx8mq_clko2_sels[]
    = {&imx_clk_25m,      &imx_sys2_pll_200m,  &imx_sys1_pll_400m,  &imx_sys2_pll_166m,
       &imx_sys3_pll_out, &imx_audio_pll1_out, &imx_video_pll1_out, &imx_clk_32k};

Imx_ccm_clk imx_clk_gpt1(IMX8MQ_CLK_GPT1, imx8mq_gpt1_sels, (CCM_VA + 0xb580));
Imx_ccm_clk imx_clk_wdog(IMX8MQ_CLK_WDOG, imx8mq_wdog_sels, (CCM_VA + 0xb900));
Imx_ccm_clk imx_clk_wrclk(IMX8MQ_CLK_WRCLK, imx8mq_wrclk_sels, (CCM_VA + 0xb980));
Imx_ccm_clk imx_clk_clko1(IMX8MQ_CLK_CLKO1, imx8mq_clko1_sels, (CCM_VA + 0xba00));
Imx_ccm_clk imx_clk_clko2(IMX8MQ_CLK_CLKO2, imx8mq_clko2_sels, (CCM_VA + 0xba80));

Clock* imx8mq_dsi_core_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_266m, &imx_sys2_pll_250m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_dsi_phy_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_125m, &imx_sys2_pll_100m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_clk_ext2,      &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_dsi_dbi_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_266m, &imx_sys2_pll_100m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_dsi_esc_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_100m, &imx_sys1_pll_80m, &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_clk_ext3,     &imx_audio_pll2_out,
};
Clock* imx8mq_dsi_ahb_sels[]
    = {&imx_clk_25m,        &imx_sys2_pll_100m, &imx_sys1_pll_80m, &imx_sys1_pll_800m,
       &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_clk_ext3,     &imx_audio_pll2_out};

Imx_ccm_clk imx_clk_dsi_core(IMX8MQ_CLK_DSI_CORE, imx8mq_dsi_core_sels, (CCM_VA + 0xbb00));
Imx_ccm_clk imx_clk_dsi_phy_ref(IMX8MQ_CLK_DSI_PHY_REF, imx8mq_dsi_phy_sels, (CCM_VA + 0xbb80));
Imx_ccm_clk imx_clk_dsi_dbi(IMX8MQ_CLK_DSI_DBI, imx8mq_dsi_dbi_sels, (CCM_VA + 0xbc00));
Imx_ccm_clk imx_clk_dsi_esc(IMX8MQ_CLK_DSI_ESC, imx8mq_dsi_esc_sels, (CCM_VA + 0xbc80));
Imx_ccm_clk imx_clk_dsi_ahb(IMX8MQ_CLK_DSI_AHB, imx8mq_dsi_ahb_sels, (CCM_VA + 0x9200));
Imx_clock_div imx_clk_dsi_ipg_div(IMX8MQ_CLK_DSI_IPG_DIV, &imx_clk_dsi_ahb, (CCM_VA + 0x9280), 0, 6,
                                  CLOCK_ENABLE_PARENT);

Clock* imx8mq_csi1_core_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_266m, &imx_sys2_pll_250m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_csi1_phy_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_125m, &imx_sys2_pll_100m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_clk_ext2,      &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_csi1_esc_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_100m, &imx_sys1_pll_80m, &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_clk_ext3,     &imx_audio_pll2_out,
};

Clock* imx8mq_csi2_core_sels[] = {
    &imx_clk_25m,        &imx_sys1_pll_266m, &imx_sys2_pll_250m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_csi2_phy_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_125m, &imx_sys2_pll_100m,  &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_clk_ext2,      &imx_audio_pll2_out, &imx_video_pll1_out,
};

Clock* imx8mq_csi2_esc_sels[] = {
    &imx_clk_25m,        &imx_sys2_pll_100m, &imx_sys1_pll_80m, &imx_sys1_pll_800m,
    &imx_sys2_pll_1000m, &imx_sys3_pll_out,  &imx_clk_ext3,     &imx_audio_pll2_out,
};

Imx_ccm_clk imx_clk_csi1_core(IMX8MQ_CLK_CSI1_CORE, imx8mq_csi1_core_sels, (CCM_VA + 0xbd00));
Imx_ccm_clk imx_clk_csi1_phy_ref(IMX8MQ_CLK_CSI1_PHY_REF, imx8mq_csi1_phy_sels, (CCM_VA + 0xbd80));
Imx_ccm_clk imx_clk_csi1_esc(IMX8MQ_CLK_CSI1_ESC, imx8mq_csi1_esc_sels, (CCM_VA + 0xbe00));
Imx_ccm_clk imx_clk_csi2_core(IMX8MQ_CLK_CSI2_CORE, imx8mq_csi2_core_sels, (CCM_VA + 0xbe80));
Imx_ccm_clk imx_clk_csi2_phy_ref(IMX8MQ_CLK_CSI2_PHY_REF, imx8mq_csi2_phy_sels, (CCM_VA + 0xbf00));
Imx_ccm_clk imx_clk_csi2_esc(IMX8MQ_CLK_CSI2_ESC, imx8mq_csi2_esc_sels, (CCM_VA + 0xbf80));

Clock* imx8mq_pcie2_ctrl_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_250m, &imx_sys2_pll_200m, &imx_sys1_pll_266m,
    &imx_sys1_pll_800m, &imx_sys2_pll_500m, &imx_sys2_pll_333m, &imx_sys3_pll_out,
};

Clock* imx8mq_pcie2_phy_sels[] = {
    &imx_clk_25m,  &imx_sys2_pll_100m, &imx_sys2_pll_500m, &imx_clk_ext1,
    &imx_clk_ext2, &imx_clk_ext3,      &imx_clk_ext4,      &imx_sys1_pll_400m,
};

Clock* imx8mq_pcie2_aux_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys2_pll_50m,  &imx_sys3_pll_out,
    &imx_sys2_pll_100m, &imx_sys1_pll_80m,  &imx_sys1_pll_160m, &imx_sys1_pll_200m,
};

Imx_ccm_clk imx_clk_pcie2_ctrl(IMX8MQ_CLK_PCIE2_CTRL, imx8mq_pcie2_ctrl_sels, (CCM_VA + 0xc000));
Imx_ccm_clk imx_clk_pcie2_phy(IMX8MQ_CLK_PCIE2_PHY, imx8mq_pcie2_phy_sels, (CCM_VA + 0xc080));
Imx_ccm_clk imx_clk_pcie2_aux(IMX8MQ_CLK_PCIE2_AUX, imx8mq_pcie2_aux_sels, (CCM_VA + 0xc100));

Clock* imx8mq_ecspi3_sels[] = {
    &imx_clk_25m,       &imx_sys2_pll_200m, &imx_sys1_pll_40m,  &imx_sys1_pll_160m,
    &imx_sys1_pll_800m, &imx_sys3_pll_out,  &imx_sys2_pll_250m, &imx_audio_pll2_out,
};

Imx_ccm_clk imx_clk_ecspi3(IMX8MQ_CLK_ECSPI3, imx8mq_ecspi3_sels, (CCM_VA + 0xc180));

Imx_clock_gate imx_clk_ecspi1_root(IMX8MQ_CLK_ECSPI1_ROOT, &imx_clk_ecspi1, (CCM_VA + 0x4070), 0, 3,
                                   CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_ecspi2_root(IMX8MQ_CLK_ECSPI2_ROOT, &imx_clk_ecspi2, (CCM_VA + 0x4080), 0, 3,
                                   CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_ecspi3_root(IMX8MQ_CLK_ECSPI3_ROOT, &imx_clk_ecspi3, (CCM_VA + 0x4090), 0, 3,
                                   CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_enet1_root(IMX8MQ_CLK_ENET1_ROOT, &imx_clk_enet_axi, (CCM_VA + 0x40a0), 0, 3,
                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_gpio1_root(IMX8MQ_CLK_GPIO1_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x40b0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpio2_root(IMX8MQ_CLK_GPIO2_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x40c0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpio3_root(IMX8MQ_CLK_GPIO3_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x40d0), 0,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpio4_root(IMX8MQ_CLK_GPIO4_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x40e0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpio5_root(IMX8MQ_CLK_GPIO5_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x40f0), 0, 3,
                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_gpt1_root(IMX8MQ_CLK_GPT1_ROOT, &imx_clk_gpt1, (CCM_VA + 0x4100), 0, 3,
                                 CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_i2c1_root(IMX8MQ_CLK_I2C1_ROOT, &imx_clk_i2c1, (CCM_VA + 0x4170), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_i2c2_root(IMX8MQ_CLK_I2C2_ROOT, &imx_clk_i2c2, (CCM_VA + 0x4180), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_i2c3_root(IMX8MQ_CLK_I2C3_ROOT, &imx_clk_i2c3, (CCM_VA + 0x4190), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_i2c4_root(IMX8MQ_CLK_I2C4_ROOT, &imx_clk_i2c4, (CCM_VA + 0x41a0), 0, 3,
                                 CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_mu_root(IMX8MQ_CLK_MU_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x4210), 0, 3,
                               CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_ocotp_root(IMX8MQ_CLK_OCOTP_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x4220), 0, 3,
                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_pcie1_root(IMX8MQ_CLK_PCIE1_ROOT, &imx_clk_pcie1_ctrl, (CCM_VA + 0x4250), 0,
                                  3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_pcie2_root(IMX8MQ_CLK_PCIE2_ROOT, &imx_clk_pcie2_ctrl, (CCM_VA + 0x4640), 0,
                                  3, CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_pwm1_root(IMX8MQ_CLK_PWM1_ROOT, &imx_clk_pwm1, (CCM_VA + 0x4280), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_pwm2_root(IMX8MQ_CLK_PWM2_ROOT, &imx_clk_pwm2, (CCM_VA + 0x4290), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_pwm3_root(IMX8MQ_CLK_PWM3_ROOT, &imx_clk_pwm3, (CCM_VA + 0x42a0), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_pwm4_root(IMX8MQ_CLK_PWM4_ROOT, &imx_clk_pwm4, (CCM_VA + 0x42b0), 0, 3,
                                 CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_qspi_root(IMX8MQ_CLK_QSPI_ROOT, &imx_clk_qspi, (CCM_VA + 0x42f0), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_rawnand_root(IMX8MQ_CLK_RAWNAND_ROOT, &imx_clk_nand, (CCM_VA + 0x4300), 0, 3,
                                    CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_nand_usdhc_bus_rawnand_clk(IMX8MQ_CLK_NAND_USDHC_BUS_RAWNAND_CLK,
                                                  &imx_clk_nand_usdhc_bus, (CCM_VA + 0x4300), 0, 3,
                                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_sai1_root(IMX8MQ_CLK_SAI1_ROOT, &imx_clk_sai1, (CCM_VA + 0x4330), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai1_ipg(IMX8MQ_CLK_SAI1_IPG, &imx_clk_ipg_audio_root, (CCM_VA + 0x4330), 0,
                                3, CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_sai2_root(IMX8MQ_CLK_SAI2_ROOT, &imx_clk_sai2, (CCM_VA + 0x4340), 0, 3,
                                 CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_sai2_ipg(IMX8MQ_CLK_SAI2_IPG, &imx_clk_ipg_root, (CCM_VA + 0x4340), 0, 3,
                                CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai3_root(IMX8MQ_CLK_SAI3_ROOT, &imx_clk_sai3, (CCM_VA + 0x4350), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai3_ipg(IMX8MQ_CLK_SAI3_IPG, &imx_clk_ipg_root, (CCM_VA + 0x4350), 0, 3,
                                CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai4_root(IMX8MQ_CLK_SAI4_ROOT, &imx_clk_sai4, (CCM_VA + 0x4360), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai4_ipg(IMX8MQ_CLK_SAI4_IPG, &imx_clk_ipg_audio_root, (CCM_VA + 0x4360), 0,
                                3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai5_root(IMX8MQ_CLK_SAI5_ROOT, &imx_clk_sai5, (CCM_VA + 0x4370), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai5_ipg(IMX8MQ_CLK_SAI5_IPG, &imx_clk_ipg_audio_root, (CCM_VA + 0x4370), 0,
                                3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai6_root(IMX8MQ_CLK_SAI6_ROOT, &imx_clk_sai6, (CCM_VA + 0x4380), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sai6_ipg(IMX8MQ_CLK_SAI6_IPG, &imx_clk_ipg_audio_root, (CCM_VA + 0x4380), 0,
                                3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_snvs_root(IMX8MQ_CLK_SNVS_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x4470), 0, 3,
                                 CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_uart1_root(IMX8MQ_CLK_UART1_ROOT, &imx_clk_uart1, (CCM_VA + 0x4490), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_uart2_root(IMX8MQ_CLK_UART2_ROOT, &imx_clk_uart2, (CCM_VA + 0x44a0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_uart3_root(IMX8MQ_CLK_UART3_ROOT, &imx_clk_uart3, (CCM_VA + 0x44b0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_uart4_root(IMX8MQ_CLK_UART4_ROOT, &imx_clk_uart4, (CCM_VA + 0x44c0), 0, 3,
                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_usb1_ctrl_root(IMX8MQ_CLK_USB1_CTRL_ROOT, &imx_clk_usb_bus,
                                      (CCM_VA + 0x44d0), 0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_usb2_ctrl_root(IMX8MQ_CLK_USB2_CTRL_ROOT, &imx_clk_usb_bus,
                                      (CCM_VA + 0x44e0), 0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_usb1_phy_root(IMX8MQ_CLK_USB1_PHY_ROOT, &imx_clk_usb_phy_ref,
                                     (CCM_VA + 0x44f0), 0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_usb2_phy_root(IMX8MQ_CLK_USB2_PHY_ROOT, &imx_clk_usb_phy_ref,
                                     (CCM_VA + 0x4500), 0, 3, CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_usdhc1_root(IMX8MQ_CLK_USDHC1_ROOT, &imx_clk_usdhc1, (CCM_VA + 0x4510), 0, 3,
                                   CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_usdhc2_root(IMX8MQ_CLK_USDHC2_ROOT, &imx_clk_usdhc2, (CCM_VA + 0x4520), 0, 3,
                                   CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_wdog1_root(IMX8MQ_CLK_WDOG1_ROOT, &imx_clk_wdog, (CCM_VA + 0x4530), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_wdog2_root(IMX8MQ_CLK_WDOG2_ROOT, &imx_clk_wdog, (CCM_VA + 0x4540), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_wdog3_root(IMX8MQ_CLK_WDOG3_ROOT, &imx_clk_wdog, (CCM_VA + 0x4550), 0, 3,
                                  CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_vpu_g1_root(IMX8MQ_CLK_VPU_G1_ROOT, &imx_clk_vpu_g1, (CCM_VA + 0x4560), 0, 3,
                                   CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_gpu_root(IMX8MQ_CLK_GPU_ROOT, &imx_clk_gpu_core_div, (CCM_VA + 0x4570), 0, 3,
                                CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_vpu_g2_root(IMX8MQ_CLK_VPU_G2_ROOT, &imx_clk_vpu_g2, (CCM_VA + 0x45a0), 0, 3,
                                   CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_disp_root(IMX8MQ_CLK_DISP_ROOT, &imx_clk_disp_dc8000, (CCM_VA + 0x45d0), 0,
                                 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_disp_axi_root(IMX8MQ_CLK_DISP_AXI_ROOT, &imx_clk_disp_axi, (CCM_VA + 0x45d0),
                                     0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_disp_apb_root(IMX8MQ_CLK_DISP_APB_ROOT, &imx_clk_disp_apb, (CCM_VA + 0x45d0),
                                     0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_disp_rtrm_root(IMX8MQ_CLK_DISP_RTRM_ROOT, &imx_clk_disp_rtrm,
                                      (CCM_VA + 0x45d0), 0, 3, CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_tmu_root(IMX8MQ_CLK_TMU_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x4620), 0, 3,
                                CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_vpu_dec_root(IMX8MQ_CLK_VPU_DEC_ROOT, &imx_clk_vpu_bus, (CCM_VA + 0x4630), 0,
                                    3, CLOCK_ENABLE_PARENT);

Imx_clock_gate imx_clk_csi1_root(IMX8MQ_CLK_CSI1_ROOT, &imx_clk_csi1_core, (CCM_VA + 0x4650), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_csi2_root(IMX8MQ_CLK_CSI2_ROOT, &imx_clk_csi2_core, (CCM_VA + 0x4660), 0, 3,
                                 CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sdma1_root(IMX8MQ_CLK_SDMA1_ROOT, &imx_clk_ipg_root, (CCM_VA + 0x43a0), 0, 3,
                                  CLOCK_ENABLE_PARENT);
Imx_clock_gate imx_clk_sdma2_root(IMX8MQ_CLK_SDMA2_ROOT, &imx_clk_ipg_audio_root, (CCM_VA + 0x43b0),
                                  0, 3, CLOCK_ENABLE_PARENT);

Imx_fixdiv_clock imx_gpt_3m_clk(IMX8MQ_GPT_3M_CLK, &imx_clk_25m, 1, 8);

Errno
Imx_ClkCtrl::probe(void) {

    /* Analog Clocks and PLLs*/
    _clks[IMX8MQ_CLK_DUMMY] = &imx_clk_dummy;
    _clks[IMX8MQ_CLK_32K] = &imx_clk_32k;
    _clks[IMX8MQ_CLK_25M] = &imx_clk_25m;
    _clks[IMX8MQ_CLK_27M] = &imx_clk_27m;
    _clks[IMX8MQ_CLK_EXT1] = &imx_clk_ext1;
    _clks[IMX8MQ_CLK_EXT2] = &imx_clk_ext2;
    _clks[IMX8MQ_CLK_EXT3] = &imx_clk_ext3;
    _clks[IMX8MQ_CLK_EXT4] = &imx_clk_ext4;
    _clks[IMX8MQ_ARM_PLL_REF_SEL] = &imx_arm_pll_ref_sel;
    _clks[IMX8MQ_GPU_PLL_REF_SEL] = &imx_gpu_pll_ref_sel;
    _clks[IMX8MQ_VPU_PLL_REF_SEL] = &imx_vpu_pll_ref_sel;
    _clks[IMX8MQ_AUDIO_PLL1_REF_SEL] = &imx_audio_pll1_ref_sel;
    _clks[IMX8MQ_AUDIO_PLL2_REF_SEL] = &imx_audio_pll2_ref_sel;
    _clks[IMX8MQ_VIDEO_PLL1_REF_SEL] = &imx_video_pll1_ref_sel;
    _clks[IMX8MQ_SYS3_PLL1_REF_SEL] = &imx_sys3_pll1_ref_sel;
    _clks[IMX8MQ_DRAM_PLL1_REF_SEL] = &imx_dram_pll1_ref_sel;
    _clks[IMX8MQ_VIDEO2_PLL1_REF_SEL] = &imx_video2_pll1_ref_sel;
    _clks[IMX8MQ_ARM_PLL_REF_DIV] = &imx_arm_pll_ref_div;
    _clks[IMX8MQ_GPU_PLL_REF_DIV] = &imx_gpu_pll_ref_div;
    _clks[IMX8MQ_VPU_PLL_REF_DIV] = &imx_vpu_pll_ref_div;
    _clks[IMX8MQ_AUDIO_PLL1_REF_DIV] = &imx_audio_pll1_ref_div;
    _clks[IMX8MQ_AUDIO_PLL2_REF_DIV] = &imx_audio_pll2_ref_div;
    _clks[IMX8MQ_VIDEO_PLL1_REF_DIV] = &imx_video_pll1_ref_div;
    _clks[IMX8MQ_ARM_PLL] = &imx_arm_pll;
    _clks[IMX8MQ_GPU_PLL] = &imx_gpu_pll;
    _clks[IMX8MQ_VPU_PLL] = &imx_vpu_pll;
    _clks[IMX8MQ_AUDIO_PLL1] = &imx_audio_pll1;
    _clks[IMX8MQ_AUDIO_PLL2] = &imx_audio_pll2;
    _clks[IMX8MQ_VIDEO_PLL1] = &imx_video_pll1;
    _clks[IMX8MQ_ARM_PLL_BYPASS] = &imx_arm_pll_bypass;
    _clks[IMX8MQ_GPU_PLL_BYPASS] = &imx_gpu_pll_bypass;
    _clks[IMX8MQ_VPU_PLL_BYPASS] = &imx_vpu_pll_bypass;
    _clks[IMX8MQ_AUDIO_PLL1_BYPASS] = &imx_audio_pll1_bypass;
    _clks[IMX8MQ_AUDIO_PLL2_BYPASS] = &imx_audio_pll2_bypass;
    _clks[IMX8MQ_VIDEO_PLL1_BYPASS] = &imx_video_pll1_bypass;

    /* PLL OUT GATE */
    _clks[IMX8MQ_ARM_PLL_OUT] = &imx_arm_pll_out;
    _clks[IMX8MQ_GPU_PLL_OUT] = &imx_gpu_pll_out;
    _clks[IMX8MQ_VPU_PLL_OUT] = &imx_vpu_pll_out;
    _clks[IMX8MQ_AUDIO_PLL1_OUT] = &imx_audio_pll1_out;
    _clks[IMX8MQ_AUDIO_PLL2_OUT] = &imx_audio_pll2_out;
    _clks[IMX8MQ_VIDEO_PLL1_OUT] = &imx_video_pll1_out;
    _clks[IMX8MQ_SYS1_PLL_OUT] = &imx_sys1_pll_out;
    _clks[IMX8MQ_SYS2_PLL_OUT] = &imx_sys2_pll_out;
    _clks[IMX8MQ_SYS3_PLL_OUT] = &imx_sys3_pll_out;
    _clks[IMX8MQ_DRAM_PLL_OUT] = &imx_dram_pll_out;
    _clks[IMX8MQ_VIDEO2_PLL_OUT] = &imx_video2_pll_out;

    /* SYS PLL1 fixed output */
    _clks[IMX8MQ_SYS1_PLL_40M_CG] = &imx_sys1_pll_40m_cg;
    _clks[IMX8MQ_SYS1_PLL_80M_CG] = &imx_sys1_pll_80m_cg;
    _clks[IMX8MQ_SYS1_PLL_100M_CG] = &imx_sys1_pll_100m_cg;
    _clks[IMX8MQ_SYS1_PLL_133M_CG] = &imx_sys1_pll_133m_cg;
    _clks[IMX8MQ_SYS1_PLL_160M_CG] = &imx_sys1_pll_160m_cg;
    _clks[IMX8MQ_SYS1_PLL_200M_CG] = &imx_sys1_pll_200m_cg;
    _clks[IMX8MQ_SYS1_PLL_266M_CG] = &imx_sys1_pll_266m_cg;
    _clks[IMX8MQ_SYS1_PLL_400M_CG] = &imx_sys1_pll_400m_cg;
    _clks[IMX8MQ_SYS1_PLL_800M_CG] = &imx_sys1_pll_800m_cg;
    _clks[IMX8MQ_SYS1_PLL_40M] = &imx_sys1_pll_40m;
    _clks[IMX8MQ_SYS1_PLL_80M] = &imx_sys1_pll_80m;
    _clks[IMX8MQ_SYS1_PLL_100M] = &imx_sys1_pll_100m;
    _clks[IMX8MQ_SYS1_PLL_133M] = &imx_sys1_pll_133m;
    _clks[IMX8MQ_SYS1_PLL_160M] = &imx_sys1_pll_160m;
    _clks[IMX8MQ_SYS1_PLL_200M] = &imx_sys1_pll_200m;
    _clks[IMX8MQ_SYS1_PLL_266M] = &imx_sys1_pll_266m;
    _clks[IMX8MQ_SYS1_PLL_400M] = &imx_sys1_pll_400m;
    _clks[IMX8MQ_SYS1_PLL_800M] = &imx_sys1_pll_800m;

    /* SYS PLL2 fixed output */
    _clks[IMX8MQ_SYS2_PLL_50M_CG] = &imx_sys2_pll_50m_cg;
    _clks[IMX8MQ_SYS2_PLL_100M_CG] = &imx_sys2_pll_100m_cg;
    _clks[IMX8MQ_SYS2_PLL_125M_CG] = &imx_sys2_pll_125m_cg;
    _clks[IMX8MQ_SYS2_PLL_166M_CG] = &imx_sys2_pll_166m_cg;
    _clks[IMX8MQ_SYS2_PLL_200M_CG] = &imx_sys2_pll_200m_cg;
    _clks[IMX8MQ_SYS2_PLL_250M_CG] = &imx_sys2_pll_250m_cg;
    _clks[IMX8MQ_SYS2_PLL_333M_CG] = &imx_sys2_pll_333m_cg;
    _clks[IMX8MQ_SYS2_PLL_500M_CG] = &imx_sys2_pll_500m_cg;
    _clks[IMX8MQ_SYS2_PLL_1000M_CG] = &imx_sys2_pll_1000m_cg;
    _clks[IMX8MQ_SYS2_PLL_50M] = &imx_sys2_pll_50m;
    _clks[IMX8MQ_SYS2_PLL_100M] = &imx_sys2_pll_100m;
    _clks[IMX8MQ_SYS2_PLL_125M] = &imx_sys2_pll_125m;
    _clks[IMX8MQ_SYS2_PLL_166M] = &imx_sys2_pll_166m;
    _clks[IMX8MQ_SYS2_PLL_200M] = &imx_sys2_pll_200m;
    _clks[IMX8MQ_SYS2_PLL_250M] = &imx_sys2_pll_250m;
    _clks[IMX8MQ_SYS2_PLL_333M] = &imx_sys2_pll_333m;
    _clks[IMX8MQ_SYS2_PLL_500M] = &imx_sys2_pll_500m;
    _clks[IMX8MQ_SYS2_PLL_1000M] = &imx_sys2_pll_1000m;

    _clks[IMX8MQ_CLK_A53_SRC] = &imx_clk_a53_src;
    _clks[IMX8MQ_CLK_M4_SRC] = &imx_clk_m4_src;
    _clks[IMX8MQ_CLK_VPU_SRC] = &imx_clk_vpu_src;
    _clks[IMX8MQ_CLK_GPU_CORE_SRC] = &imx_clk_gpu_core_src;
    _clks[IMX8MQ_CLK_GPU_SHADER_SRC] = &imx_clk_gpu_shader_src;
    _clks[IMX8MQ_CLK_A53_CG] = &imx_clk_a53_cg;
    _clks[IMX8MQ_CLK_M4_CG] = &imx_clk_m4_cg;
    _clks[IMX8MQ_CLK_VPU_CG] = &imx_clk_vpu_cg;
    _clks[IMX8MQ_CLK_GPU_CORE_CG] = &imx_clk_gpu_core_cg;
    _clks[IMX8MQ_CLK_GPU_SHADER_CG] = &imx_clk_gpu_shader_cg;
    _clks[IMX8MQ_CLK_A53_DIV] = &imx_clk_a53_div;
    _clks[IMX8MQ_CLK_M4_DIV] = &imx_clk_m4_div;
    _clks[IMX8MQ_CLK_VPU_DIV] = &imx_clk_vpu_div;
    _clks[IMX8MQ_CLK_GPU_CORE_DIV] = &imx_clk_gpu_core_div;
    _clks[IMX8MQ_CLK_GPU_SHADER_DIV] = &imx_clk_gpu_shader_div;

    /*bus clocks*/
    _clks[IMX8MQ_CLK_MAIN_AXI] = &imx_clk_main_axi;
    _clks[IMX8MQ_CLK_ENET_AXI] = &imx_clk_enet_axi;
    _clks[IMX8MQ_CLK_NAND_USDHC_BUS] = &imx_clk_nand_usdhc_bus;
    _clks[IMX8MQ_CLK_VPU_BUS] = &imx_clk_vpu_bus;
    _clks[IMX8MQ_CLK_DISP_AXI] = &imx_clk_disp_axi;
    _clks[IMX8MQ_CLK_DISP_APB] = &imx_clk_disp_apb;
    _clks[IMX8MQ_CLK_DISP_RTRM] = &imx_clk_disp_rtrm;
    _clks[IMX8MQ_CLK_USB_BUS] = &imx_clk_usb_bus;
    _clks[IMX8MQ_CLK_GPU_AXI] = &imx_clk_gpu_axi;
    _clks[IMX8MQ_CLK_GPU_AHB] = &imx_clk_gpu_ahb;
    _clks[IMX8MQ_CLK_NOC] = &imx_clk_noc;
    _clks[IMX8MQ_CLK_NOC_APB] = &imx_clk_noc_apb;

    /* AHB clock is used by the AHB bus therefore marked as critical */
    _clks[IMX8MQ_CLK_AHB] = &imx_clk_ahb;
    _clks[IMX8MQ_CLK_AUDIO_AHB] = &imx_clk_audio_ahb;

    _clks[IMX8MQ_CLK_IPG_ROOT] = &imx_clk_ipg_root;
    _clks[IMX8MQ_CLK_IPG_AUDIO_ROOT] = &imx_clk_ipg_audio_root;
    _clks[IMX8MQ_CLK_DRAM_CORE] = &imx_clk_dram_core;
    _clks[IMX8MQ_CLK_DRAM_ALT] = &imx_clk_dram_alt;
    _clks[IMX8MQ_CLK_DRAM_APB] = &imx_clk_dram_apb;
    _clks[IMX8MQ_CLK_VPU_G1] = &imx_clk_vpu_g1;
    _clks[IMX8MQ_CLK_VPU_G2] = &imx_clk_vpu_g2;
    _clks[IMX8MQ_CLK_DISP_DTRC] = &imx_clk_disp_dtrc;
    _clks[IMX8MQ_CLK_DISP_DC8000] = &imx_clk_disp_dc8000;
    _clks[IMX8MQ_CLK_PCIE1_CTRL] = &imx_clk_pcie1_ctrl;
    _clks[IMX8MQ_CLK_PCIE1_PHY] = &imx_clk_pcie1_phy;
    _clks[IMX8MQ_CLK_PCIE1_AUX] = &imx_clk_pcie1_aux;
    _clks[IMX8MQ_CLK_DC_PIXEL] = &imx_clk_dc_pixel;
    _clks[IMX8MQ_CLK_LCDIF_PIXEL] = &imx_clk_lcdif_pixel;
    _clks[IMX8MQ_CLK_SAI1] = &imx_clk_sai1;
    _clks[IMX8MQ_CLK_SAI2] = &imx_clk_sai2;
    _clks[IMX8MQ_CLK_SAI3] = &imx_clk_sai3;
    _clks[IMX8MQ_CLK_SAI4] = &imx_clk_sai4;
    _clks[IMX8MQ_CLK_SAI5] = &imx_clk_sai5;
    _clks[IMX8MQ_CLK_SAI6] = &imx_clk_sai6;
    _clks[IMX8MQ_CLK_SPDIF1] = &imx_clk_spdif1;
    _clks[IMX8MQ_CLK_SPDIF2] = &imx_clk_spdif2;
    _clks[IMX8MQ_CLK_ENET_REF] = &imx_clk_enet_ref;
    _clks[IMX8MQ_CLK_ENET_TIMER] = &imx_clk_enet_timer;
    _clks[IMX8MQ_CLK_ENET_PHY_REF] = &imx_clk_enet_phy_ref;
    _clks[IMX8MQ_CLK_NAND] = &imx_clk_nand;
    _clks[IMX8MQ_CLK_QSPI] = &imx_clk_qspi;
    _clks[IMX8MQ_CLK_USDHC1] = &imx_clk_usdhc1;
    _clks[IMX8MQ_CLK_USDHC2] = &imx_clk_usdhc2;
    _clks[IMX8MQ_CLK_I2C1] = &imx_clk_i2c1;
    _clks[IMX8MQ_CLK_I2C2] = &imx_clk_i2c2;
    _clks[IMX8MQ_CLK_I2C3] = &imx_clk_i2c3;
    _clks[IMX8MQ_CLK_I2C4] = &imx_clk_i2c4;
    _clks[IMX8MQ_CLK_UART1] = &imx_clk_uart1;
    _clks[IMX8MQ_CLK_UART2] = &imx_clk_uart2;
    _clks[IMX8MQ_CLK_UART3] = &imx_clk_uart3;
    _clks[IMX8MQ_CLK_UART4] = &imx_clk_uart4;
    _clks[IMX8MQ_CLK_USB_CORE_REF] = &imx_clk_usb_core_ref;
    _clks[IMX8MQ_CLK_USB_PHY_REF] = &imx_clk_usb_phy_ref;
    _clks[IMX8MQ_CLK_GIC] = &imx_clk_gic;
    _clks[IMX8MQ_CLK_ECSPI1] = &imx_clk_ecspi1;
    _clks[IMX8MQ_CLK_ECSPI2] = &imx_clk_ecspi2;
    _clks[IMX8MQ_CLK_PWM1] = &imx_clk_pwm1;
    _clks[IMX8MQ_CLK_PWM2] = &imx_clk_pwm2;
    _clks[IMX8MQ_CLK_PWM3] = &imx_clk_pwm3;
    _clks[IMX8MQ_CLK_PWM4] = &imx_clk_pwm4;
    _clks[IMX8MQ_CLK_GPT1] = &imx_clk_gpt1;
    _clks[IMX8MQ_CLK_WDOG] = &imx_clk_wdog;
    _clks[IMX8MQ_CLK_WRCLK] = &imx_clk_wrclk;
    _clks[IMX8MQ_CLK_CLKO1] = &imx_clk_clko2;
    _clks[IMX8MQ_CLK_CLKO2] = &imx_clk_clko1;
    _clks[IMX8MQ_CLK_DSI_CORE] = &imx_clk_dsi_core;
    _clks[IMX8MQ_CLK_DSI_PHY_REF] = &imx_clk_dsi_phy_ref;
    _clks[IMX8MQ_CLK_DSI_DBI] = &imx_clk_dsi_dbi;
    _clks[IMX8MQ_CLK_DSI_ESC] = &imx_clk_dsi_esc;
    _clks[IMX8MQ_CLK_DSI_AHB] = &imx_clk_dsi_ahb;
    _clks[IMX8MQ_CLK_DSI_IPG_DIV] = &imx_clk_dsi_ipg_div;
    _clks[IMX8MQ_CLK_CSI1_CORE] = &imx_clk_csi1_core;
    _clks[IMX8MQ_CLK_CSI1_PHY_REF] = &imx_clk_csi1_phy_ref;
    _clks[IMX8MQ_CLK_CSI1_ESC] = &imx_clk_csi1_esc;
    _clks[IMX8MQ_CLK_CSI2_CORE] = &imx_clk_csi2_core;
    _clks[IMX8MQ_CLK_CSI2_PHY_REF] = &imx_clk_csi2_phy_ref;
    _clks[IMX8MQ_CLK_CSI2_ESC] = &imx_clk_csi2_esc;
    _clks[IMX8MQ_CLK_PCIE2_CTRL] = &imx_clk_pcie2_ctrl;
    _clks[IMX8MQ_CLK_PCIE2_PHY] = &imx_clk_pcie2_phy;
    _clks[IMX8MQ_CLK_PCIE2_AUX] = &imx_clk_pcie2_aux;
    _clks[IMX8MQ_CLK_ECSPI3] = &imx_clk_ecspi3;
    _clks[IMX8MQ_CLK_ECSPI1_ROOT] = &imx_clk_ecspi1_root;
    _clks[IMX8MQ_CLK_ECSPI2_ROOT] = &imx_clk_ecspi2_root;
    _clks[IMX8MQ_CLK_ECSPI3_ROOT] = &imx_clk_ecspi3_root;
    _clks[IMX8MQ_CLK_ENET1_ROOT] = &imx_clk_enet1_root;
    _clks[IMX8MQ_CLK_GPIO1_ROOT] = &imx_clk_gpio1_root;
    _clks[IMX8MQ_CLK_GPIO2_ROOT] = &imx_clk_gpio2_root;
    _clks[IMX8MQ_CLK_GPIO3_ROOT] = &imx_clk_gpio4_root;
    _clks[IMX8MQ_CLK_GPIO4_ROOT] = &imx_clk_gpio4_root;
    _clks[IMX8MQ_CLK_GPIO5_ROOT] = &imx_clk_gpio5_root;
    _clks[IMX8MQ_CLK_GPT1_ROOT] = &imx_clk_gpt1_root;
    _clks[IMX8MQ_CLK_I2C1_ROOT] = &imx_clk_i2c1_root;
    _clks[IMX8MQ_CLK_I2C2_ROOT] = &imx_clk_i2c2_root;
    _clks[IMX8MQ_CLK_I2C3_ROOT] = &imx_clk_i2c3_root;
    _clks[IMX8MQ_CLK_I2C4_ROOT] = &imx_clk_i2c4_root;
    _clks[IMX8MQ_CLK_MU_ROOT] = &imx_clk_mu_root;
    _clks[IMX8MQ_CLK_OCOTP_ROOT] = &imx_clk_ocotp_root;
    _clks[IMX8MQ_CLK_PCIE1_ROOT] = &imx_clk_pcie1_root;
    _clks[IMX8MQ_CLK_PCIE2_ROOT] = &imx_clk_pcie2_root;
    _clks[IMX8MQ_CLK_PWM1_ROOT] = &imx_clk_pwm1_root;
    _clks[IMX8MQ_CLK_PWM2_ROOT] = &imx_clk_pwm2_root;
    _clks[IMX8MQ_CLK_PWM3_ROOT] = &imx_clk_pwm3_root;
    _clks[IMX8MQ_CLK_PWM4_ROOT] = &imx_clk_pwm4_root;
    _clks[IMX8MQ_CLK_QSPI_ROOT] = &imx_clk_qspi_root;
    _clks[IMX8MQ_CLK_RAWNAND_ROOT] = &imx_clk_rawnand_root;
    _clks[IMX8MQ_CLK_NAND_USDHC_BUS_RAWNAND_CLK] = &imx_clk_nand_usdhc_bus_rawnand_clk;
    _clks[IMX8MQ_CLK_SAI1_ROOT] = &imx_clk_sai1_root;
    _clks[IMX8MQ_CLK_SAI1_IPG] = &imx_clk_sai1_ipg;
    _clks[IMX8MQ_CLK_SAI2_ROOT] = &imx_clk_sai2_root;
    _clks[IMX8MQ_CLK_SAI2_IPG] = &imx_clk_sai2_ipg;
    _clks[IMX8MQ_CLK_SAI3_ROOT] = &imx_clk_sai3_root;
    _clks[IMX8MQ_CLK_SAI3_IPG] = &imx_clk_sai3_ipg;
    _clks[IMX8MQ_CLK_SAI4_ROOT] = &imx_clk_sai4_root;
    _clks[IMX8MQ_CLK_SAI4_IPG] = &imx_clk_sai4_ipg;
    _clks[IMX8MQ_CLK_SAI5_ROOT] = &imx_clk_sai5_root;
    _clks[IMX8MQ_CLK_SAI5_IPG] = &imx_clk_sai5_ipg;
    _clks[IMX8MQ_CLK_SAI6_ROOT] = &imx_clk_sai6_root;
    _clks[IMX8MQ_CLK_SAI6_IPG] = &imx_clk_sai6_ipg;
    _clks[IMX8MQ_CLK_SNVS_ROOT] = &imx_clk_snvs_root;
    _clks[IMX8MQ_CLK_UART1_ROOT] = &imx_clk_uart1_root;
    _clks[IMX8MQ_CLK_UART2_ROOT] = &imx_clk_uart2_root;
    _clks[IMX8MQ_CLK_UART3_ROOT] = &imx_clk_uart3_root;
    _clks[IMX8MQ_CLK_UART4_ROOT] = &imx_clk_uart4_root;
    _clks[IMX8MQ_CLK_USB1_CTRL_ROOT] = &imx_clk_usb1_ctrl_root;
    _clks[IMX8MQ_CLK_USB2_CTRL_ROOT] = &imx_clk_usb2_ctrl_root;
    _clks[IMX8MQ_CLK_USB1_PHY_ROOT] = &imx_clk_usb1_phy_root;
    _clks[IMX8MQ_CLK_USB2_PHY_ROOT] = &imx_clk_usb2_phy_root;
    _clks[IMX8MQ_CLK_USDHC1_ROOT] = &imx_clk_usdhc1_root;
    _clks[IMX8MQ_CLK_USDHC2_ROOT] = &imx_clk_usdhc2_root;
    _clks[IMX8MQ_CLK_WDOG1_ROOT] = &imx_clk_wdog1_root;
    _clks[IMX8MQ_CLK_WDOG2_ROOT] = &imx_clk_wdog2_root;
    _clks[IMX8MQ_CLK_WDOG3_ROOT] = &imx_clk_wdog3_root;
    _clks[IMX8MQ_CLK_VPU_G1_ROOT] = &imx_clk_vpu_g1_root;
    _clks[IMX8MQ_CLK_GPU_ROOT] = &imx_clk_gpu_root;
    _clks[IMX8MQ_CLK_VPU_G2_ROOT] = &imx_clk_vpu_g2_root;
    _clks[IMX8MQ_CLK_DISP_ROOT] = &imx_clk_disp_root;
    _clks[IMX8MQ_CLK_DISP_AXI_ROOT] = &imx_clk_disp_axi_root;
    _clks[IMX8MQ_CLK_DISP_APB_ROOT] = &imx_clk_disp_apb_root;
    _clks[IMX8MQ_CLK_DISP_RTRM_ROOT] = &imx_clk_disp_rtrm_root;
    _clks[IMX8MQ_CLK_TMU_ROOT] = &imx_clk_tmu_root;
    _clks[IMX8MQ_CLK_VPU_DEC_ROOT] = &imx_clk_vpu_dec_root;
    _clks[IMX8MQ_CLK_CSI1_ROOT] = &imx_clk_csi1_root;
    _clks[IMX8MQ_CLK_CSI2_ROOT] = &imx_clk_csi2_root;
    _clks[IMX8MQ_CLK_SDMA1_ROOT] = &imx_clk_sdma1_root;
    _clks[IMX8MQ_CLK_SDMA2_ROOT] = &imx_clk_sdma2_root;
    _clks[IMX8MQ_GPT_3M_CLK] = &imx_gpt_3m_clk;
    _clks[IMX8MQ_CLK_DRAM_ALT_ROOT] = &imx_clk_dram_alt_root;

    _clks[IMX8MQ_CLK_ARM] = nullptr; /*ignore changes to core clock*/

    // initialize clocks- sync internal state with hw values
    for (uint16 i = 0; i < IMX8MQ_CLK_END; i++) {
        if (_clks[i] != nullptr) _clks[i]->init();
    }

    return Errno::ENONE;
}

Errno
Imx_ClkCtrl::enable_clk(uint64 clk_id) {
    if (clk_id > IMX8MQ_CLK_END) return Errno::EINVAL;
    if (_clks[clk_id] == nullptr) return Errno::ENOTSUP;

    return _clks[clk_id]->enable() ? Errno::ENONE : Errno::EINVAL;
}

Errno
Imx_ClkCtrl::get_clkrate(uint64 clk_id, uint64& value) {
    if (clk_id > IMX8MQ_CLK_END) return Errno::EINVAL;
    if (_clks[clk_id] == nullptr) return Errno::ENOTSUP;

    uint32 rate;
    if (_clks[clk_id]->get_rate(rate)) {
        value = static_cast<uint64>(rate);
        return Errno::ENONE;
    } else
        return Errno::EINVAL;
}

Errno
Imx_ClkCtrl::disable_clk(uint64 clk_id) {
    if (clk_id > IMX8MQ_CLK_END) return Errno::EINVAL;
    if (_clks[clk_id] == nullptr) return Errno::ENOTSUP;

    return _clks[clk_id]->disable() ? Errno::ENONE : Errno::EINVAL;
}

Errno
Imx_ClkCtrl::set_clkrate(uint64 clk_id, uint64 value) {
    if (clk_id > IMX8MQ_CLK_END) return Errno::EINVAL;
    if (_clks[clk_id] == nullptr) return Errno::ENOTSUP;

    uint32 rate = static_cast<uint32>(value);
    if (_clks[clk_id]->set_rate(rate))
        return Errno::ENONE;
    else
        return Errno::EINVAL;
}

uint32
Imx_ClkCtrl::get_max_clkid(void) {
    return IMX8MQ_CLK_END;
}

Errno
Imx_ClkCtrl::describe_clkrate(uint64 clk_id, Pm::clk_desc& rate) {
    if (clk_id > IMX8MQ_CLK_END) return Errno::EINVAL;
    if (_clks[clk_id] == nullptr) return Errno::ENOTSUP;

    return _clks[clk_id]->describe_rate(rate) ? Errno::ENONE : Errno::EINVAL;
}

bool
Imx_ClkCtrl::is_enabled(uint64 clk_id) {
    if ((clk_id > IMX8MQ_CLK_END) || (_clks[clk_id] == nullptr)) return false;
    return _clks[clk_id]->is_enabled();
}