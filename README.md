Алгоритм работы Устройства

При подаче питания переходит в режим "Снято с охраны" (statusOXP=false). В этом режиме ждет звонка со знакомого номера. 
В режиме "Снято с охраны" при поступлении звонка кладет трубку, ждет 1 секунду и переходит в режим "Охрана" (statusOXP=true), 
отправляет sms: "SET". 
При срабатывании датчика переходит в режим "Тревога", ждет 10 сек, включает сирену, отправляет sms: "ALARM"+номер датчика.
В режиме "Охрана" при поступлении звонка кладет струбку (сбрасывает), переходит в режим "Снято с охраны".

Светодиод режима работы устройства (наружний и встроеный)
"Снято с охраны" - не горит
"Охрана" - горит постоянно
"Тревога" - мигает 2 раза в секунду
