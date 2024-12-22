//
// Created by Aleksandr on 22.06.2024.
//

#ifndef SPI_FLASH_STRUCTURES_H
#define SPI_FLASH_STRUCTURES_H
#include <cstdint>

/*!
* @brief Класс перечеслений описывающий варианты сообщений об ошибках
*/
enum class Err_msg{
    E_OK = 0,
    E_MODEL,                    //Неверная модель устройства
    E_DEVICE,                   //Не верное устройство
    E_BLOCK_CNT = 4,            //Выход за диапозон доступных блоков устройства
};

/*!
* @brief Структура описывающая флэш память
*/
typedef struct W25qxx{
    uint16_t PageSize;
    uint32_t PageCount;
    uint32_t SectorSize;
    uint32_t SectorCount;
    uint32_t BlockSize;
    uint32_t BlockCount;
    uint32_t CapacityInKiloByte;
}W25qxx_t;

#endif //SPI_FLASH_STRUCTURES_H
