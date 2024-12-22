//
// Created by Aleksandr on 26.06.2024.
//
#include "flash_w25116jv.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;
#define W25Q_SPI hspi1
//Макросы выбора устройства на шине SPI
#define csLOW() HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET)
#define csHIGH() HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET)

//Макросы комманд
#define CMD_JEDEC_ID            0x9F
#define CMD_WRITE_EN            0x06
#define CMD_PAGE_PROGRAM        0x02
#define CMD_READ_DATA           0x03
#define CMD_READ_STATUS_REG1    0x05
#define CMD_BLOCK64_ERASE       0xD8

int Flash_W25Q16JV::init() {
    const uint8_t manuf_id = 0xEF;
    const uint16_t device_id = 0x4015;
    uint32_t id = read_id();
    if((id >> 0x10) == manuf_id){           //Если Winbond Serial Flash
        if((id & 0xFFFF) == device_id){     //Если модель W25Q16JV
            w25q16jv.BlockCount = 32;
            w25q16jv.PageSize = 256;
            w25q16jv.SectorSize = 0x1000;
            w25q16jv.SectorCount = w25q16jv.BlockCount * 16;
            w25q16jv.PageCount = (w25q16jv.SectorCount * w25q16jv.SectorSize) / w25q16jv.PageSize;
            w25q16jv.BlockSize = w25q16jv.SectorSize * 16;
            w25q16jv.CapacityInKiloByte = (w25q16jv.SectorCount * w25q16jv.SectorSize) / 1024;

            return static_cast<int>(Err_msg::E_OK);          //Успешная инициализация
        }
        return static_cast<int>(Err_msg::E_MODEL);           //Ошибка модели устройства
    }

    return static_cast<int>(Err_msg::E_DEVICE);             //Ошибка устройства
}

int Flash_W25Q16JV::write(const void *buff, uint32_t first_block, uint16_t num) {
    //Проверка переменных на выход за пределы памяти
    if((first_block + num) > w25q16jv.BlockCount){
        return static_cast<int>(Err_msg::E_BLOCK_CNT);
    }

    for(uint16_t i = 0; i < num; i++){
        write_block(first_block + i, ((uint8_t*)buff + (i * w25q16jv.BlockSize)));
    }

    return static_cast<int>(Err_msg::E_OK);
}

void Flash_W25Q16JV::write_page(uint32_t page, uint8_t *data) {
    //Адрес текущей страницы
    uint32_t page_addr = page * w25q16jv.PageSize;
    uint8_t tData[4];
    tData[0] = CMD_PAGE_PROGRAM;                //Команда памяти
    //Адрес записи данных
    tData[1] = (page_addr >> 16) & 0xFF;
    tData[2] = (page_addr >> 8) & 0xFF;
    tData[3] = page_addr & 0xFF;
    //Ждём завершения всех операций
    waite_whilebusy();
    //Вклучаем режим записи
    write_enable();

    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, tData, sizeof(tData), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&W25Q_SPI, data, w25q16jv.PageSize, HAL_MAX_DELAY);
    csHIGH();
    //Ждём завершения всех операций
    waite_whilebusy();
}

void Flash_W25Q16JV::write_block(uint32_t block, uint8_t *data) {
    //Адрес на который указывает текущий блок
    uint32_t mem_addr = block * w25q16jv.BlockSize;
    //Номер страницы указывающий на текущий адрес
    uint32_t page = mem_addr / w25q16jv.PageSize;
    //Количество страниц в блоке
    uint16_t page_cnt = w25q16jv.BlockSize / w25q16jv.PageSize;
    for(uint16_t i = 0; i < page_cnt; i++){
        write_page(page + i, data + (i * w25q16jv.PageSize));
    }
}

int Flash_W25Q16JV::read(void *buff, uint32_t first_block, uint16_t num) {
    //Проверка переменных на выход за пределы памяти
    if((first_block + num) > w25q16jv.BlockCount){
        return static_cast<int>(Err_msg::E_BLOCK_CNT);
    }
    uint32_t page_addr = first_block * w25q16jv.BlockSize;
    uint8_t tData[4];
    tData[0] = CMD_READ_DATA;
    tData[1] = (page_addr >> 16) & 0xFF;
    tData[2] = (page_addr >> 8) & 0xFF;
    tData[3] = page_addr & 0xFF;
    //Ждём завершения всех операций
    waite_whilebusy();
    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, tData, sizeof(tData), 100);
    HAL_SPI_Receive(&W25Q_SPI, static_cast<uint8_t*>(buff), w25q16jv.PageSize, HAL_MAX_DELAY);
    csHIGH();
    return 0;
}

int Flash_W25Q16JV::erase(uint32_t first_block, uint16_t num) {
    //Адрес на который указывает текущий блок
    uint32_t page_addr = first_block * w25q16jv.BlockSize;
    uint8_t tData[4];
    tData[0] = CMD_BLOCK64_ERASE;           //Команда
    //Адрес
    tData[1] = (page_addr >> 16) & 0xFF;
    tData[2] = (page_addr >> 8) & 0xFF;
    tData[3] = page_addr & 0xFF;
    //Ждём завершения всех операций
    waite_whilebusy();
    //Вклучаем режим записи
    write_enable();
    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, tData, sizeof(tData), HAL_MAX_DELAY);
    csHIGH();
    //Ждём завершения всех операций
    waite_whilebusy();
    return 0;
}

uint16_t Flash_W25Q16JV::get_block_size() {
    return w25q16jv.BlockSize;
}

uint16_t Flash_W25Q16JV::get_block_num() {
    return w25q16jv.BlockCount;
}

uint32_t Flash_W25Q16JV::read_id() {
    uint8_t tData = CMD_JEDEC_ID;
    uint8_t rData[3]{};
    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, &tData, sizeof(tData), HAL_MAX_DELAY);
    HAL_SPI_Receive(&W25Q_SPI, rData, sizeof(rData),HAL_MAX_DELAY);
    csHIGH();

    return ((rData[0]<<16) | (rData[1]<<8) | rData[2]);              //Выдаём байты в обратном порядке
}


void Flash_W25Q16JV::write_enable() {
    uint8_t tData = CMD_WRITE_EN;
    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, &tData, sizeof(tData), HAL_MAX_DELAY);
    csHIGH();
}

void Flash_W25Q16JV::waite_whilebusy() {
    uint8_t status_reg1 = 0;
    uint8_t tData = CMD_READ_STATUS_REG1;
    csLOW();
    HAL_SPI_Transmit(&W25Q_SPI, &tData, sizeof(tData), HAL_MAX_DELAY);
    do{
        status_reg1 = HAL_SPI_Receive(&W25Q_SPI, &status_reg1, sizeof(status_reg1), HAL_MAX_DELAY);
    }while((status_reg1 & 0x01) == 0x01);               //Ждём пока бит BUSY установлен
    csHIGH();
}