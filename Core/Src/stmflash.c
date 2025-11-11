#include "stmflash.h"

/**
 * @brief       ��ָ����ַ��ȡһ���� (32λ����)
 * @param       faddr   : ��ȡ��ַ (�˵�ַ����Ϊ4����!!)
 * @retval      ��ȡ�������� (32λ)
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/**
 * @brief       ��ָ����ַ��ʼ����ָ�����ȵ�����
 * @param       raddr : ��ʼ��ַ
 * @param       pbuf  : ����ָ��
 * @param       length: Ҫ��ȡ����(32λ)��,��4���ֽڵ�������
 * @retval      ��
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr); /* ��ȡ4���ֽ� */
        raddr += 4;                          /* ƫ��4���ֽ� */
    }
}

/**
 * @brief       ��ȡĳ����ַ���ڵ�flash����
 * @param       addr: flash��ַ
 * @retval      ָ����ַ���ڵ�flash����
 */
static uint32_t stmflash_get_flash_sector(uint32_t addr)
{
    uint32_t sector = 0;

    if ((addr >= FLASH_BASE) && (addr < FLASH_BASE + FLASH_BANK_SIZE))
    {
        sector = (addr & ~FLASH_BASE) / FLASH_SECTOR_SIZE;
    }
    else if ((addr >= FLASH_BASE + FLASH_BANK_SIZE) && (addr < FLASH_BASE + FLASH_SIZE))
    {
        sector = ((addr & ~FLASH_BASE) - FLASH_BANK_SIZE) / FLASH_SECTOR_SIZE;
    }
    else
    {
        sector = 0xFFFFFFFF; /* ��ַ��� */
    }

    return sector;
}

/**
 * @brief       ��ȡĳ����ַ���ڵ�flash bank
 * @param       addr: flash��ַ
 * @retval      ����ֵ�ǣ�FLASH_BANK_1��FLASH_BANK_2��0xFFFFFFFF����ʾ��ַ�����
 */
static uint32_t stmflash_get_flash_bank(uint32_t addr)
{
    uint32_t bank = 0;

    if ((addr >= FLASH_BASE) && (addr < FLASH_BASE + FLASH_BANK_SIZE))
    {
        bank = FLASH_BANK_1;
    }
    else if ((addr >= FLASH_BASE + FLASH_BANK_SIZE) && (addr < FLASH_BASE + FLASH_SIZE))
    {
        bank = FLASH_BANK_2;
    }
    else
    {
        bank = 0xFFFFFFFF; /* ��ַ��� */
    }

    return bank;
}

/**
 * @brief       ��ָ����ַд��ָ�����ȵ�����
 * @param       waddr : ָ��д�����ݵ���ʼ��ַ
 * @param       pbuf  : ����д�����ݵ���ʼ��ַ
 * @param       length: ָ��д�����ݵĳ��ȣ���λ����
 * @retval      ��
 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t addrx;
    uint32_t endaddr;
    uint32_t first_sector = 0;
    uint32_t num_sectors = 0;
    uint32_t bank_number = 0;
    uint32_t erase_addr; /* �����������ֵΪ���������������ַ */
    FLASH_EraseInitTypeDef flash_erase_init = {0};
    HAL_StatusTypeDef status = HAL_OK;

    if ((waddr < STM32_FLASH_BASE) ||                      /* ָ����ַС��flash����ʼ��ַ */
        (waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)) || /* ָ����ַ����flash��ĩ��ַ */
        waddr % 4)                                         /* ָ����ַû�а�4�ֽڶ��� */
    {
        return; /* �Ƿ���ַ */
    }

    HAL_ICACHE_Disable(); /* ����ָ��� */
    HAL_FLASH_Unlock();   /* FLASH���� */

    addrx = waddr;                /* ����д�����ʼ��ַ */
    endaddr = waddr + length * 4; /* ����д��Ľ�����ַ */

    first_sector = stmflash_get_flash_sector(addrx);                     /* ��ȡҪ�����ĵ�һ������ */
    num_sectors = stmflash_get_flash_sector(endaddr) - first_sector + 1; /* ��ȡҪ������������ */
    bank_number = stmflash_get_flash_bank(addrx);                        /* ��ȡ������ַ��bank */

    if (addrx < 0x0800FFFF) /* ֻ�����洢��������Ҫ���в������� */
    {
        while (addrx < endaddr) /* ɨ��һ���ϰ�(�Է�FFFFFFFF�ĵط�,�Ȳ���) */
        {
            if ((uint32_t)stmflash_read_word(addrx) != 0xFFFFFFFF) /* ���ڷ�0xFFFFFFFF */
            {
                flash_erase_init.TypeErase = FLASH_TYPEERASE_SECTORS; /* �����������ķ�ʽ */
                flash_erase_init.Banks = bank_number;                 /* �������ڵ�bank */
                flash_erase_init.Sector = first_sector;               /* Ҫ�����ĵ�һ������ */
                flash_erase_init.NbSectors = num_sectors;             /* Ҫ������������ */
                status = HAL_FLASHEx_Erase(&flash_erase_init, &erase_addr);

                if (status == HAL_OK) /* �����ɹ� */
                {
                    break;
                }
            }
            else /* ������� */
            {
                addrx += 4;
            }
        }
    }

    if (status == HAL_OK) /* ��������û�д��� */
    {
        for (int i = 0; i < length; i++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, waddr, (uint32_t)&pbuf[i]);
            waddr += 4; /* ָ����һ������ */
        }
    }

    HAL_FLASH_Lock();    /* ���� */
    HAL_ICACHE_Enable(); /* ����ָ��� */
}

/***************************************************************************************************************************************/
/* �����ô��� */

/**
 * @brief       ����д����(д1����)
 * @param       waddr : ��ʼ��ַ
 * @param       wdata : Ҫд�������
 * @retval      ��ȡ��������
 */
void stmflash_test_write(uint32_t waddr, uint32_t wdata)
{
    stmflash_write(waddr, &wdata, 1); /* д��һ���� */
}
