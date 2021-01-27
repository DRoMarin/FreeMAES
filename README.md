# FreeMAES
## MAES Framework FreeRTOS Implementation for Gomspace's Nanomind 3200 (AT32UC3C)


#### Instructions

1. Clone this repository:
```
git clone https://github.com/DRoMarin/MAES-FreeRTOS
```
2. Copy the MAES_Framework_Dev/MAES-Source directory into your own project root.
3. include `maes-rtos.h` in the project header.



#### Recomendations
If your compiler shows the error `undefined _cxa_pure_virtual` include the following code:
```
extern "C" void __cxa_pure_virtual() { while (1); }
```
This avoid undefined virtual functions.
