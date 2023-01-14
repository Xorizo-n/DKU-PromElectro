# DKU-PromElectro
# Проект: ПО для ДКУ-02

Программа для ДКУ-02 находится в репозитории https://github.com/Xorizo-n/DKU-PromElectro

## Как скачать проект?
Необходимо нажать на кнопку "Code" в репозитории, в контекстном меню нажать на "Download ZIP"

## Работа системы верхнего уровня:
Необходимо ввести значения для счётчика осей, скорости и нажать на кнопку "Старт".

## Работа эмулятора:
Для работы эмулятора необходимо подключить датчик к устройству и в эмуляторе выбрать необходимый COM-порт(смотрим в диспетчере устройств).
Затем программа будет определять факт прохода, направление движения, скорость колеса, осуществлять счёт количества осей с учётом направления движения, передавать эту информацию другим системам или оператору. Для чтения используется третья функция Modbus, для записи шестая функция Modbus.

## Описание регистровых переменных ДКУ:
  - 40025 (для записи) - Регистр записи индивидуального адреса
  - 40030 (для чтения) - 10-битный счетчик осей
  - 40031 (для чтения) - 16-битный регистр параметра скорости
  - 40032 (для чтения) - Регистр эксплуатационных признаков (младший байт), регистр отказов и диагностики (старший байт)
  - 40033 (для записи) - Регистр команд
  - 40034 (для чтения) - Младшие байты регистра меток времени
  - 40035 (для чтения) - Старшие байты регистра меток времени
  - 40036 (для чтения) - Младшие байты счетчика меток времени
  - 40037 (для чтения) - Старшие байты счетчика меток времени
