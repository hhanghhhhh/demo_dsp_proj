/*
 * Temporary DSP/FPGA XINTF hardware validation.
 * Delete this file and the include/calls in Main.c after validation.
 */

#define XINTF_TEST_REG_VALUE        0x12345AA5UL
#define XINTF_TEST_REG_EXPECTED     0xA55A5AA5UL
#define XINTF_TEST_RAM_WORDS        512U
#define XINTF_TEST_COPY_WAIT_LOOPS  32U
#define XINTF_TEST_REG_TIMEOUT      1000U

#define XINTF_TEST_STATE_REG_WAIT   1U
#define XINTF_TEST_STATE_RAM_WRITE  2U
#define XINTF_TEST_STATE_COPY_WAIT  3U
#define XINTF_TEST_STATE_RAM_READ   4U
#define XINTF_TEST_STATE_PASS       5U
#define XINTF_TEST_STATE_FAIL       6U

volatile Uint16 g_xintf_validation_state = 0U;
volatile Uint16 g_xintf_validation_pass = 0U;
volatile Uint16 g_xintf_validation_error = 0U;
volatile Uint16 g_xintf_validation_index = 0U;
volatile Uint32 g_xintf_validation_expected = 0UL;
volatile Uint32 g_xintf_validation_actual = 0UL;

static Uint16 s_xintf_validation_wait = 0U;

static Uint32 XintfValidationPattern(Uint16 index)
{
    Uint32 value = (Uint32)index;
    return 0x5AA50000UL ^ (value << 16) ^ value;
}

void XintfValidationInit(void)
{
    g_xintf_validation_state = XINTF_TEST_STATE_REG_WAIT;
    g_xintf_validation_pass = 0U;
    g_xintf_validation_error = 0U;
    g_xintf_validation_index = 0U;
    g_xintf_validation_expected = XINTF_TEST_REG_EXPECTED;
    g_xintf_validation_actual = 0UL;
    s_xintf_validation_wait = 0U;

    st_fpga_main_wr.ctrl_reg.all = XINTF_TEST_REG_VALUE;
}

void XintfValidationProcess(void)
{
    Uint32 value;

    switch(g_xintf_validation_state)
    {
        case XINTF_TEST_STATE_REG_WAIT:
            g_xintf_validation_actual = st_fpga_main_rd.fpga_info.all;
            if(g_xintf_validation_actual == XINTF_TEST_REG_EXPECTED)
            {
                g_xintf_validation_index = 0U;
                g_xintf_validation_state = XINTF_TEST_STATE_RAM_WRITE;
            }
            else if(++s_xintf_validation_wait >= XINTF_TEST_REG_TIMEOUT)
            {
                g_xintf_validation_error = 1U;
                g_xintf_validation_state = XINTF_TEST_STATE_FAIL;
            }
            break;

        case XINTF_TEST_STATE_RAM_WRITE:
            value = XintfValidationPattern(g_xintf_validation_index);
            if(FpgaRamWrite(g_xintf_validation_index, value) == 0U)
            {
                g_xintf_validation_error = 2U;
                g_xintf_validation_state = XINTF_TEST_STATE_FAIL;
            }
            else if(++g_xintf_validation_index >= XINTF_TEST_RAM_WORDS)
            {
                g_xintf_validation_index = 0U;
                s_xintf_validation_wait = 0U;
                g_xintf_validation_state = XINTF_TEST_STATE_COPY_WAIT;
            }
            break;

        case XINTF_TEST_STATE_COPY_WAIT:
            if(++s_xintf_validation_wait >= XINTF_TEST_COPY_WAIT_LOOPS)
            {
                g_xintf_validation_index = 0U;
                g_xintf_validation_state = XINTF_TEST_STATE_RAM_READ;
            }
            break;

        case XINTF_TEST_STATE_RAM_READ:
            g_xintf_validation_expected = XintfValidationPattern(g_xintf_validation_index);
            if(FpgaRamRead(g_xintf_validation_index, &value) == 0U)
            {
                g_xintf_validation_error = 3U;
                g_xintf_validation_state = XINTF_TEST_STATE_FAIL;
            }
            else
            {
                g_xintf_validation_actual = value;
                if(value != g_xintf_validation_expected)
                {
                    g_xintf_validation_error = 4U;
                    g_xintf_validation_state = XINTF_TEST_STATE_FAIL;
                }
                else if(++g_xintf_validation_index >= XINTF_TEST_RAM_WORDS)
                {
                    g_xintf_validation_pass = 1U;
                    g_xintf_validation_state = XINTF_TEST_STATE_PASS;
                }
            }
            break;

        case XINTF_TEST_STATE_PASS:
        case XINTF_TEST_STATE_FAIL:
        default:
            break;
    }
}
