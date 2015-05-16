//-------------------------------------------------------------------------------------------------
// Wyúwietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym bez odczytu flagi zajÍtoúci
// z dowolnym przypisaniem sygna≥Ûw sterujπcych
// Plik : HD44780.c	
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autor : Rados≥aw KwiecieÒ
// èrÛd≥o : http://radzio.dxp.pl/hd44780/
// Data : 24.03.2007
//-------------------------------------------------------------------------------------------------

#include "HD44780.h"
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiajπca pÛ≥bajt na magistralÍ danych
//
//-------------------------------------------------------------------------------------------------
void _LCD_OutNibble(unsigned char nibbleToWrite)
{

if(nibbleToWrite & 0x01)
	LCD_DB4_PORT |= LCD_DB4;
else
	LCD_DB4_PORT  &= ~LCD_DB4;

if(nibbleToWrite & 0x02)
	LCD_DB5_PORT |= LCD_DB5;
else
	LCD_DB5_PORT  &= ~LCD_DB5;

if(nibbleToWrite & 0x04)
	LCD_DB6_PORT |= LCD_DB6;
else
	LCD_DB6_PORT  &= ~LCD_DB6;

if(nibbleToWrite & 0x08)
	LCD_DB7_PORT |= LCD_DB7;
else
	LCD_DB7_PORT  &= ~LCD_DB7;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiajπca pÛ≥bajt na magistralÍ danych
//
//-------------------------------------------------------------------------------------------------
unsigned char _LCD_InNibble(void)
{
unsigned char tmp = 0;

if(LCD_DB4_PIN & LCD_DB4)
	tmp |= (1 << 0);
if(LCD_DB5_PIN & LCD_DB5)
	tmp |= (1 << 1);
if(LCD_DB6_PIN & LCD_DB6)
	tmp |= (1 << 2);
if(LCD_DB7_PIN & LCD_DB7)
	tmp |= (1 << 3);
return tmp;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wyúwietacza (bez rozrÛønienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write(unsigned char dataToWrite)
{
LCD_DB4_DIR |= LCD_DB4;
LCD_DB5_DIR |= LCD_DB5;
LCD_DB6_DIR |= LCD_DB6;
LCD_DB7_DIR |= LCD_DB7;

LCD_RW_PORT &= ~LCD_RW;
LCD_E_PORT |= LCD_E;
_LCD_OutNibble(dataToWrite >> 4);
LCD_E_PORT &= ~LCD_E;
LCD_E_PORT |= LCD_E;
_LCD_OutNibble(dataToWrite);
LCD_E_PORT &= ~LCD_E;
while(LCD_ReadStatus()&0x80);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu z wyúwietacza (bez rozrÛønienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------

unsigned char _LCD_Read(void)
{
unsigned char tmp = 0;
LCD_DB4_DIR &= ~LCD_DB4;
LCD_DB5_DIR &= ~LCD_DB5;
LCD_DB6_DIR &= ~LCD_DB6;
LCD_DB7_DIR &= ~LCD_DB7;

LCD_RW_PORT |= LCD_RW;
LCD_E_PORT |= LCD_E;
tmp |= (_LCD_InNibble() << 4);
LCD_E_PORT &= ~LCD_E;
LCD_E_PORT |= LCD_E;
tmp |= _LCD_InNibble();
LCD_E_PORT &= ~LCD_E;
return tmp;
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wyúwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char commandToWrite)
{
LCD_RS_PORT &= ~LCD_RS;
_LCD_Write(commandToWrite);
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu statusowego
//
//-------------------------------------------------------------------------------------------------
unsigned char LCD_ReadStatus(void)
{
LCD_RS_PORT &= ~LCD_RS;
return _LCD_Read();
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pamiÍci wyúwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData(unsigned char dataToWrite)
{
LCD_RS_PORT |= LCD_RS;
_LCD_Write(dataToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu danych z pamiÍci wyúwietlacza
//
//-------------------------------------------------------------------------------------------------
unsigned char LCD_ReadData(void)
{
LCD_RS_PORT |= LCD_RS;
return _LCD_Read();
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyúwietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText(char * text)
{
while(*text)
  LCD_WriteData(*text++);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wspÛ≥rzÍdnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo(unsigned char x, unsigned char y)
{
LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wyúwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear(void)
{
LCD_WriteCommand(HD44780_CLEAR);
_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywrÛcenia poczπtkowych wspÛ≥rzÍdnych wyúwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home(void)
{
LCD_WriteCommand(HD44780_HOME);
_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize(void)
{
unsigned char i;
LCD_DB4_DIR |= LCD_DB4; // Konfiguracja kierunku pracy wyprowadzeÒ
LCD_DB5_DIR |= LCD_DB5; //
LCD_DB6_DIR |= LCD_DB6; //
LCD_DB7_DIR |= LCD_DB7; //
LCD_E_DIR 	|= LCD_E;   //
LCD_RS_DIR 	|= LCD_RS;  //
LCD_RW_DIR 	|= LCD_RW;  //
_delay_ms(15); // oczekiwanie na ustalibizowanie siÍ napiecia zasilajacego
LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
LCD_E_PORT &= ~LCD_E;  // wyzerowanie linii E
LCD_RW_PORT &= ~LCD_RW;
for(i = 0; i < 3; i++) // trzykrotne powtÛrzenie bloku instrukcji
  {
  LCD_E_PORT |= LCD_E; //  E = 1
  _LCD_OutNibble(0x03); // tryb 8-bitowy
  LCD_E_PORT &= ~LCD_E; // E = 0
  _delay_ms(5); // czekaj 5ms
  }

LCD_E_PORT |= LCD_E; // E = 1
_LCD_OutNibble(0x02); // tryb 4-bitowy
LCD_E_PORT &= ~LCD_E; // E = 0

_delay_ms(1); // czekaj 1ms 
LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy≥πczenie wyswietlacza
LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartosÊi pamieci DDRAM
LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementaja adresu i przesuwanie kursora
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // w≥πcz LCD, bez kursora i mrugania
}

//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
