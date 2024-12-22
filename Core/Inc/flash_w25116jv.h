//
// Created by Aleksandr on 26.06.2024.
//
#ifndef SPI_FLASH_FLASH_W25116JV_H
#define SPI_FLASH_FLASH_W25116JV_H
#include "spi_flash.h"
/*!@class Класс Flash_W25Q16JV реализующий интерфейс SPI_Flash для модели флэш памяти W25Q16JV
 */
class Flash_W25Q16JV : SPI_Flash{
public:
    /*!
    * @brief Инициализация микросхемы.
    * @return 0 при успешной инициализации.
    */
    int init() override;
    /*!
    * @brief Запись блоков во флеш память.
    * @param[in] buff - буфер, из которого забираются данные для записи.
    * @param[in] first_block - номер первого блока.
    * @param[in] num - количество записываемых блоков.
    * @return EOK при успешной записи.
    */
    int write(const void* buff, uint32_t first_block, uint16_t num) override;
    /*!
    * @brief Чтение блоков из флеш памяти.
    * @param[out] buff - буфер, в который считываются данные.
    * @param[in] first_block - номер первого блока.
    * @param[in] num - количество считываемых блоков.
    * @return EOK при успешном чтении.
    */
    int read(void *buff, uint32_t first_block, uint16_t num) override;
    /*!
    * @brief Очистка блоков во флеш памяти.
    * @param[in] first_block - номер первого блока. Блоки нумеруются от 0 до 32767.
    * @param[in] num - количество стираемых блоков.
    * @return EOK при успешном выполнении операции.
    */
    int erase(uint32_t first_block, uint16_t num) override;
    /*!
    * @brief Возвращает размер блока.
    * @return Размер блока в байтах.
    */
    uint16_t get_block_size() override;
    /*!
    * @brief Возвращает количество блоков.
    * @return Количество блоков
    */
    uint16_t get_block_num() override;
private:
    //Структура описывающая флэш память
    W25qxx_t w25q16jv;
private:
    uint32_t read_id();
    void write_page(uint32_t page, uint8_t* data);
    void write_block(uint32_t block, uint8_t* data);
    void write_enable();
    void waite_whilebusy();
};

#endif //SPI_FLASH_FLASH_W25116JV_H
