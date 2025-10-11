#include <windows.h>
#include <stdio.h>
#include <conio.h>

/*
Пример патча для исполняемого файла игры Tomb Raider 3,
патч делает возможным запускать игру на широкоформатных
мониторах, игра не предназначена для запуска на широкоформатых
мониторах и картина на экране игры выглядит растянутой
*/
int main(void)
{
	FILE *fp;

	bool Patched = false;

	errno_t err;
	err  = fopen_s(&fp, "tomb3.exe", "r+b");
	
	if( err == 0 )
    {
		printf("Open exe!\n");
	}
	else
	{
		printf("This patch makes it possible to run the Tomb Raider 3 game in mode 60 FPS. Put this patch in your game folder and run it.\n");
		while( !_kbhit() ) {};
		return 0;
	}

	UINT n_seek_pos = 0;

	fseek(fp, 0, SEEK_END);
	
	DWORD dwFileSize = ftell(fp);

	rewind(fp);

	BYTE *Buff = new BYTE[dwFileSize];

	fread((BYTE*)Buff, dwFileSize, 1, fp);

	for ( UINT i = 0; i < dwFileSize - 10; i++ )
	{
		if ( Buff[i] == 0xAB && 
			Buff[i + 1] == 0xAA &&
			Buff[i + 2] == 0xAA &&
			Buff[i + 3] == 0x3F )
		{

			//задаем новый аспект т.е. соотношение
			//сторон экрана
			float Aspect = 16.0f / 9.0f;

			BYTE Data[4];

			//копируем float значение
			memcpy(Data, (void*)&Aspect, sizeof(float));
			
			//патчим ЕХЕ файл
			Buff[i] = Data[0];
			Buff[i + 1] = Data[1];
			Buff[i + 2] = Data[2];
			Buff[i + 3] = Data[3];

			Patched = true;
		}
	}

	if(Patched)
	{
		//записываем изменения в файл
		rewind(fp);
		fwrite(Buff, dwFileSize , 1, fp);

		printf("Done!\n");
	}
	else
	{
		printf("Not Patched!\n");
	}

	fclose(fp);

	while( !_kbhit() ) {};


	return 0;
}