
#define UTAF_DEFINE_PER_LCORE(type, name)                       \
        __thread __typeof__(type) utaf_per_lcore_##name

extern int parse_cores_string(char *opt, int *ret, int cc);

int g_page_sz;

UTAF_DEFINE_PER_LCORE(int, lthrd_id);
UTAF_DEFINE_PER_LCORE(int, _lcore_id);

int g_device_id = 0;

int __calcu_cores_initialized = 0;
int __calcu_cores[2];

int parse_calc_cores(char *optarg)
{
    if ( parse_cores_string(optarg, __calcu_cores, 2) < 2 )
    {
        return -1;
    }

    __calcu_cores_initialized = 1;

    return 0;
}

