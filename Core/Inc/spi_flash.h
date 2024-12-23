//
// Created by Aleksandr on 21.06.2024.
//

#ifndef SPI_FLASH_SPI_FLASH_H
#define SPI_FLASH_SPI_FLASH_H

#include "structures.h"
/*!@class Интерфэйсный класс для драйвера SPI flash
 */
class SPI_Flash{
public:
    /*!
    * @brief Инициализация микросхемы. Метод должен вызываться в потоке, после инициализации SPI.
    * @return 0 при успешной инициализации.
    */
    virtual int init() = 0;
    /*!
    * @brief Запись блоков во флеш память.
    * @param[in] buff - буфер, из которого забираются данные для записи.
    * @param[in] first_block - номер первого блока.
    * @param[in] num - количество записываемых блоков.
    * @return EOK при успешной записи.
    */
    virtual int write(const void* buff, uint32_t first_block, uint16_t num) = 0;
    /*!
    * @brief Чтение блоков из флеш памяти.
    * @param[out] buff - буфер, в который считываются данные.
    * @param[in] first_block - номер первого блока.
    * @param[in] num - количество считываемых блоков.
    * @return EOK при успешном чтение.
    */
    virtual int read(void *buff, uint32_t first_block, uint16_t num) = 0;
    /*!
    * @brief Очистка блоков во флеш памяти.
    * @param[in] first_block - номер первого блока. Блоки нумеруются от 0 до 32767.
    * @param[in] num - количество стираемых блоков.
    * @return EOK при успешном выполнении операции.
    */
    virtual int erase(uint32_t first_block, uint16_t num) = 0;
    /*!
    * @brief Возвращает размер блока.
    * @return Размер блока в байтах.
    */
    virtual uint16_t get_block_size() = 0;
    /*!
    * @brief Возвращает количество блоков.
    * @return Количество блоков
    */
    virtual uint16_t get_block_num() = 0;
};

#endif //SPI_FLASH_SPI_FLASH_H
