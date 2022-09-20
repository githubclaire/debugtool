#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;
unsigned char channel = 0;
unsigned int flag_1020 = 0;

// ct test function start

// get high bit bigger than d
unsigned int mux_cal_s(unsigned int r, unsigned int d)
{
    unsigned int q, count;
    q = 0;
    count = 30;
    do
    {
        count--;
        if ((r >> count) >= d)
        {
            r -= (d << count);
            q += (1 << count);
        }
    } while (count);
    return r;
}

unsigned int mux_cal(unsigned int r, unsigned int d)
{
    unsigned int q, count;
    q = 0;
    count = 30;
    do
    {
        count--;
        if ((r >> count) >= d)
        {
            r -= (d << count);
            q += (1 << count);
        }
    } while (count);
    return q;
}

void ct_pattern(unsigned int value, unsigned int *send_pattern, unsigned int *exp_data, unsigned int *exp_dqs)
{

    unsigned int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, bg0, act_n, ba0, ba1, cke, odt, par, alert, cs, reset, clk, clk_c, clk_t, dm0, dm1;

    unsigned int mt0, mt1, mt2, mt3, mt4, mt5, mt6, mt7, mt8, mt9;
    *exp_data = 0;
    *exp_dqs = 0;
    // it could corver the whole situation
    a1 = (value >> 0) & 0x1;
    a6 = (value >> 1) & 0x1;
    par = (value >> 2) & 0x1;

    // printf("value=%x, a1=%x, a6=%x, par=%x\n",value,a1,a6,par);

    a8 = (value >> 0) & 0x1;
    alert = (value >> 1) & 0x1;
    a9 = (value >> 2) & 0x1;

    a2 = (value >> 0) & 0x1;
    a5 = (value >> 1) & 0x1;
    a13 = (value >> 2) & 0x1;

    a0 = (value >> 0) & 0x1;
    a7 = (value >> 1) & 0x1;
    a11 = (value >> 2) & 0x1;

    clk_c = (value >> 0) & 0x1;
    odt = (value >> 1) & 0x1;
    a15 = (value >> 2) & 0x1;

    cke = (value >> 0) & 0x1;
    a16 = (value >> 1) & 0x1;
    a10 = (value >> 2) & 0x1;

    act_n = (value >> 0) & 0x1;
    a4 = (value >> 1) & 0x1;
    ba1 = (value >> 2) & 0x1;

    dm0 = (value >> 1) & 0x1;
    // bg1=(value>>0) & 0x1;
    dm1 = (value >> 2) & 0x1;
    if (clk_c == 0)
    {
        clk_t = 1;
    }
    else
    {
        clk_t = 0;
    }
    // mt=dqs_t
    a14 = (value >> 0) & 0x1;
    a12 = (value >> 1) & 0x1;
    ba0 = (value >> 2) & 0x1;
    // dqs_c
    value = (~value) & 0x7;
    bg0 = (value >> 0) & 0x1;
    a3 = (value >> 1) & 0x1;
    reset = (value >> 2) & 0x1;

    mt0 = a1 ^ a6 ^ par;
    mt1 = a8 ^ alert ^ a9;
    mt2 = a2 ^ a5 ^ a13;
    mt3 = a0 ^ a7 ^ a11;
    mt4 = clk_c ^ odt ^ a15;
    mt5 = cke ^ a16 ^ a10;
    mt6 = act_n ^ a4 ^ ba1;
    //  mt7=(dm1|bg1)^dm0^clk_t;
    mt7 = dm1 ^ dm0 ^ clk_t;
    mt8 = a14 ^ a12 ^ ba0;
    mt9 = bg0 ^ a3 ^ reset;

    *exp_data = mt0 + (mt1 << 1) + (mt2 << 2) + (mt3 << 3) + (mt4 << 4) + (mt5 << 5) + (mt6 << 6) + (mt7 << 7);
    // printf("*exp_data=%x\n",*exp_data);
    *exp_data = (~(*exp_data << 8) & 0xff00) | *exp_data;
    *exp_data = (*exp_data << 16) | *exp_data;
    *exp_dqs = (mt8 << 0) + (mt9 << 1) + (mt8 << 2) + (mt9 << 3) + (mt8 << 4) + (mt9 << 5) + (mt8 << 6) + (mt9 << 7);

    clk = clk_t;
    *send_pattern = (a0 << 0) + (a1 << 1) + (a2 << 2) + (a3 << 3) + (a4 << 4) + (a5 << 5) + (a6 << 6) + (a7 << 7) + (a8 << 8) + (a9 << 9) + (a10 << 10) + (a11 << 11) + (a12 << 12) + (a13 << 13) +
                    (bg0 << 14) + (act_n << 15) + (ba0 << 16) + (ba1 << 17) + (bg0 << 18) + (a16 << 19) + (a15 << 20) + (a14 << 21) + (cke << 22) + (odt << 23) + (par << 24) + (alert << 25) +
                    (cs << 26) + (reset << 27) + (clk << 28) + (dm0 << 29) + (dm1 << 30);
    //  printf("mt0=%d mt1=%d mt2=%d mt3=%d mt4=%d mt5=%d mt6=%d mt7=%d  mt8=%d mt9=%d\n",mt0,mt1,mt2,mt3,mt4,mt5,mt6,mt7,mt8,mt9 );
    // printf("data pattern= %0x\n",*send_pattern);
}

void ct_test_fail_position(unsigned int dq, unsigned int fail_val)
{
    // printf("enter ct_test_fail_position");
    unsigned int b = 0, mt = 0, byte_val = 0, i = 0;
    mt = mux_cal_s(dq, 8);
    byte_val = mux_cal(dq, 8);

    if (fail_val == 7)
    {
        printf("fail at byte%d, DQ%d ,or fail at below address: \n", byte_val, dq);
    }
    for (i = 0; i <= 2; i++)
    {
        if ((fail_val >> i) & 0x1 == 0x1)
        {
            b = i;
            switch (mt)
            {
            case 0:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> A1\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A6\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> PAR\n", byte_val);
                }
                break;
            case 1:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> A8\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> Alert\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> A9\n", byte_val);
                }
                break;

            case 2:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> A2\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A5\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> A13\n", byte_val);
                }
                break;
            case 3:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> A0\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A7\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> A11\n", byte_val);
                }
                break;
            case 4:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> CLK_C\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> ODT\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> A15\n", byte_val);
                }
                break;
            case 5:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> CKE\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A16\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> A10\n", byte_val);
                }
                break;
            case 6:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> Act_n\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A4\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> BA1\n", byte_val);
                }
                break;
            case 7:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> CLK_t\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> DM0\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> DM1\n", byte_val);
                }
                break;
            case 8:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> A14\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A12\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> BA0\n", byte_val);
                }
                break;
            case 9:
                if (b == 0)
                {
                    printf("fail at byte%d, address-> BG0\n", byte_val);
                }
                else if (b == 1)
                {
                    printf("fail at byte%d, address-> A3\n", byte_val);
                }
                else if (b == 2)
                {
                    printf("fail at byte%d, address-> Reset\n", byte_val);
                }
                break;
            }
        }
    }
}

void ct_test_start(unsigned int value, unsigned int *exp_dq, unsigned int *exp_dqs, unsigned int *dqs0_7, unsigned int *dq0_31, unsigned int *dq32_63, unsigned int channel)
{
    // unsigned int send_pattern=0,BASE_REG=0xd000;
    unsigned int send_pattern = 0, BASE_REG = 0xd000, tem_dq56_63 = 0, tem_dq48_55 = 0, tem_dqs6 = 0, tem_dqs7 = 0;

    // unsigned int temp = 0x00000000;
    switch (channel)
    {
    case 0: // BASE_REG=A_CHANNEL_BASE_REG;
        BASE_REG = 0xd000;
        break;
    case 1: // BASE_REG=B_CHANNEL_BASE_REG;
        BASE_REG = 0xe000;
        break;
    case 2: // BASE_REG=C_CHANNEL_BASE_REG;
        BASE_REG = 0xf000;
        break;
    default: // BASE_REG=A_CHANNEL_BASE_REG;
        BASE_REG = 0xd000;
        break;
    }
    ct_pattern(value, &send_pattern, exp_dq, exp_dqs);
    // printf("exp_dq = %x \n",*exp_dq);
    // printf("BASE_REG=%x  ,mapped=%x\n",BASE_REG,mapped_base);

    writel(0x022800ef, video_pci_prop.mapped_mmioBase + BASE_REG + 0x003c);
    // udelay(1000);
    writel(0x0000aa00, video_pci_prop.mapped_mmioBase + BASE_REG + 0x0310);
    // udelay(1000);
    writel(send_pattern, video_pci_prop.mapped_mmioBase + BASE_REG + 0x0314);
    udelay(1000);
    writel(0x180, video_pci_prop.mapped_mmioBase + BASE_REG + 0x0240);
    udelay(80000);

    // temp = readl(mapped_base+BASE_REG+0x0314);
    // printf("0x0314 data = %x\n",temp);
    *dqs0_7 = readl(video_pci_prop.mapped_mmioBase + BASE_REG + 0x02c8) & 0xff; // when D240[8]=1,D2C8[7:0]=DQS_ZI[7:0]
    *dq0_31 = readl(video_pci_prop.mapped_mmioBase + BASE_REG + 0x02cc);
    *dq32_63 = readl(video_pci_prop.mapped_mmioBase + BASE_REG + 0x02d0);

    if (video_pci_prop.DeviceId == 0x3d02){
        tem_dqs6 = ((*dqs0_7) & 0x80) >> 1;
        tem_dqs7 = ((*dqs0_7) & 0x40) << 1;
        *dqs0_7 = ((*dqs0_7) & 0x3f) | tem_dqs7 | tem_dqs6;

        // udelay(1000);
        // printf("dqs0_7 = %x \n",dqs0_7);

        // udelay(1000);
        // printf("dq0_31 = %x \n",*dq0_31);

        // udelay(1000);
        // printf("dq32_63 = %x \n",*dq32_63);
        tem_dq56_63 = ((*dq32_63) & 0x00ff0000) << 8;
        tem_dq48_55 = ((*dq32_63) & 0xff000000) >> 8;
        *dq32_63 = ((*dq32_63) & 0x0000ffff) | tem_dq48_55 | tem_dq56_63;
    }
    writel(0x080,video_pci_prop.mapped_mmioBase+BASE_REG+0x0240);
    // udelay(1000);
}

void ct_test_s(unsigned int channel)
{

    unsigned int exp_dq = 0, exp_dqs;
    unsigned int i = 0, x = 0, y = 0, dqs0_7, dq0_31, dq32_63;
    unsigned int dq_res_save_l = 0, dq_res_save_h = 0, dqs_res_save = 0;
    unsigned int fail_val = 0;
    unsigned int b = 0, status = 1, dqs_gold = 0, gold_find = 0;
    unsigned int val1 = 0, val2 = 0;

    for (i = 0; i <= 101; i++)
    {
        ct_test_start(i, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);

        // printf("exp_dqs=%x,dqs0_7=%x\n",exp_dqs,dqs0_7);
        // work around: delete the first data
        if (i >= 1)
        {
            //printf("exp_dq=%x, dq0_31=%x, dq32_63=%x, exp_dqs=%x, dqs0_7=%x\n", exp_dq, dq0_31, dq32_63, exp_dqs, dqs0_7);
            for (x = 0; x <= 31; x++)
            {
                if (((exp_dq >> x) & 0x1) != ((dq0_31 >> x) & 0x1))
                {
                    dq_res_save_l |= (1 << x); // record the position of error bit
                }

                if (((exp_dq >> x) & 0x1) != ((dq32_63 >> x) & 0x1))
                {
                    dq_res_save_h |= (1 << x); // record the position of error bit
                }
            }

            for (x = 0; x <= 7; x++)
            {
                if (((exp_dqs >> x) & 0x1) != ((dqs0_7 >> x) & 0x1))
                {
                    dqs_res_save |= (1 << x); // record the position of error bit
                }
            }
        }
        // printf("dqs data/mdw 0xd2c8=%0x\n",dqs0_7);
        // printf("dq 31 ~ 0/mdw 0xd2cc=%0x\n",dq0_31);
        // printf("dq 63 ~ 32/mdw 0xd2d0=%0x\n\n",dq32_63);
    }
    printf("dq_res_save_l = %x\n", dq_res_save_l);
    printf("dq_res_save_h = %x\n", dq_res_save_h);
    // printf("dqs_res_save = %x\n",dqs_res_save);

    if ((dq_res_save_l == 0) && (dq_res_save_h == 0) && (dqs_res_save == 0))
    {
        printf("####CT test  pass####\n");
    }
    else
    {
        printf("####CT test  FAIL####\n");
    }
    if ((dq_res_save_l == 0xffffffff) && (dq_res_save_h == 0xffffffff) && (dqs_res_save == 0xff))
    {
        printf("Maby power is not ready\n");
    }
    else
    {
        if (dqs_res_save != 0)
        {

            for (x = 0; x <= 7; x++)
            {
                if ((dqs_res_save >> x) & 0x1 == 0x1)
                { // get fail dq bit=x
                    fail_val = 0;
                    status = 1;
                    dqs_gold = 0;
                    gold_find = 0;
                    for (i = 0; i <= 7; i++)
                    {
                        if (((dqs_res_save >> i) & 0x1) == 0x1)
                        {
                            status &= 1;
                        }
                        else
                        {
                            status &= 0;
                        }
                    } // if dqs_res_save has bit value = 0,then status=0, means exit ddr ic write read correct ,means A14,A12,BA0,BG0,A3,Reset ok
                    // printf("dqs_res_save=%x, status=%x\n",dqs_res_save,status);
                    if (status == 0)
                    {
                        // find same address  pass on another dq bit , so chip side these ad is pass
                        printf("####find fail memory side : ");
                        for (y = 0; y <= 7; y++)
                        {
                            for (i = 0; i < 50; i++)
                            {
                                ct_test_start(0, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                                val2 = (dqs0_7 >> x) & 0x1;
                                val1 = (exp_dqs >> x) & 0x1;
                                if (val1 != val2)
                                {
                                    break;
                                }
                            }
                            if (i == 50)
                            {
                                dqs_gold = y;
                                gold_find = 1;
                                break;
                            }
                        }
                        if (gold_find == 1)
                        { // find dqs pass pattern
                            printf(" Byte %d DQS connect pass, maybe fail on address: ", x);
                            printf("-> A14,A12,BA0,BG0,A3,Reset\n");
                        }
                        else
                        { // all data pattern fail
                            printf(" Byte %d DQS_t maybe connect fail  \n", x);
                        }
                    }

                    // all the dqs data are not correct,which means the gpu side input signals are error
                    else
                    {
                        printf("####fail on GRAY chip address side : ");
                        printf("-> A14,A12,BA0,BG0,A3,Reset\n");
                        break;
                    }
                }
            }
        }
        if ((dq_res_save_l != 0) || (dq_res_save_h != 0))
        {

            for (x = 0; x <= 63; x++)
            {
                if (x <= 31)
                {
                    if ((dq_res_save_l >> x) & 0x1 == 0x1)
                    {                        // get fail dq bit=x
                        b = mux_cal_s(x, 8); // get error bit relative postion
                        fail_val = 0;
                        if (((((dq_res_save_l >> b) & 0x1) & ((dq_res_save_l >> (8 + b)) & 0x1) & ((dq_res_save_l >> (16 + b)) & 0x1) & ((dq_res_save_l >> (24 + b)) & 0x1) & ((dq_res_save_h >> b) & 0x1) & ((dq_res_save_h >> (8 + b)) & 0x1) & ((dq_res_save_h >> (16 + b)) & 0x1) & ((dq_res_save_h >> (24 + b)) & 0x1))) == 0)
                        {
                            // find same address  pass on another dq bit , so chip side these ad is pass
                            printf("####fail on memory side : ");
                        }
                        else
                        {
                            printf("####fail on GRAY chip side : \n");
                            for (i = 0; i <= 3; i++)
                            {
                                dq_res_save_l &= (~(1 << (b + 8 * i)));
                                dq_res_save_h &= (~(1 << (b + 8 * i)));
                            }
                            // printf("dq_res_save_l = %x\n",dq_res_save_l);
                            // printf("dq_res_save_h = %x\n",dq_res_save_h);
                        }
                        ct_test_start(0, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val1 = (dq0_31 >> x) & 0x1;

                        ct_test_start(1, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq0_31 >> x) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 0;

                        ct_test_start(2, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq0_31 >> x) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 1;

                        ct_test_start(4, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq0_31 >> x) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 2;
                        // printf("fail val l = %x\n",fail_val);
                        ct_test_fail_position(x, fail_val);
                    }
                }
                else
                {
                    if ((dq_res_save_h >> (x - 31)) & 0x1 == 0x1)
                    { // get fail dq bit=x
                        b = mux_cal_s(x, 8);
                        fail_val = 0;

                        printf("####fail on memory side : ");

                        ct_test_start(0, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val1 = (dq32_63 >> (x - 31)) & 0x1;

                        ct_test_start(1, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq32_63 >> (x - 31)) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 0;

                        ct_test_start(2, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq32_63 >> (x - 31)) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 1;

                        ct_test_start(4, &exp_dq, &exp_dqs, &dqs0_7, &dq0_31, &dq32_63, channel);
                        val2 = (dq32_63 >> (x - 31)) & 0x1;
                        if (val1 == val2)
                            fail_val += 1 << 2;
                        // printf("fail val h = %x\n",fail_val);
                        ct_test_fail_position(x, fail_val);
                    }
                }
            }
        }
    }
    printf("\n");
}

void cts_test(int argc, char *argv[])
{
    if ((argc == 2) && (strcmp(argv[1], "-d") == 0))
    {
        flag_1020 = 1; // for 1020 designer not change;
    }
    channel = readl(video_pci_prop.mapped_mmioBase + 0x49024);
    switch (channel)
    {
    case 0:
        channel = 3;
        break;
    case 4:
        channel = 2;
        break;
    case 6:
        channel = 1;
        break;
    }
    printf("MIU Channel Num: %d \n", channel);
    if (channel == 3)
    {
        printf("##############################  MIU0 CT test  ###########################\n");
        ct_test_s(0);
        printf("##############################  MIU1 CT test  ###########################\n");
        ct_test_s(1);
        printf("##############################  MIU2 CT test  ###########################\n");
        ct_test_s(2);
    }
    else if (channel == 2)
    {
        printf("##############################  MIU0 CT test  ###########################\n");
        ct_test_s(0);
        printf("##############################  MIU1 CT test  ###########################\n");
        ct_test_s(1);
    }
    else if (channel == 1)
    {
        printf("##############################  MIU0 CT test  ###########################\n");
        ct_test_s(0);
    }
    // return 0;
}
