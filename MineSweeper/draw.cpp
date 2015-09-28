#include <nds.h>

void drawSprite(uint8* data, int x, int y)
{
	for(int j=0; j<16; j++)
	{
		for(int i=0; i<8; i++)
		{
			int pos = x+i + (y+j)*SCREEN_WIDTH;
			
			if((data[2*j]>>(7-i))&1)
				VRAM_A[pos] = RGB15(31,31,31);
			else
				VRAM_A[pos] = 0;
			
			if((data[2*j+1]>>(7-i))&1)
				VRAM_A[pos+8] = RGB15(31,31,31);
			else
				VRAM_A[pos+8] = 0;
		}
	}
}

uint16 drawColor = RGB15(31, 31, 31);

void drawLine(int x1, int y1, int x2, int y2)
{
	int dx = x2-x1;
	int dy = y2-y1;
	int x, y, xincr, yincr, err;
	
	if(dx<0) dx=-dx;
	if(dy<0) dy=-dy;

	if(x1<x2)
		xincr = 1;
	else
		xincr = -1;

	if(y1<y2)
		yincr = 1;
	else
		yincr = -1;

	x = x1;
	y = y1;

	if(dx > dy)
    {
		err = dx/2;
		for(int i=0; i<dx; i++)
		{
			x += xincr;
			err += dy;
			if(err > dx)
			{
				err -= dx;
				y += yincr;
			}
			VRAM_A[x+y*SCREEN_WIDTH] = drawColor;
		}
	}
	else
    {
		err = dy/2;
		for(int i=0; i<dy; i++)
		{
			y += yincr;
			err += dx;
			if(err > dy)
			{
				err -= dy;
				x += xincr;
			}
			VRAM_A[x+y*SCREEN_WIDTH] = drawColor;
		}
    }
}

void drawRect(int x1, int y1, int x2, int y2)
{
	if(x1 < 0) x1 = 0;
	if(y1 < 0) y1 = 0;
	for(int i=x1; i<=x2 && i<=SCREEN_WIDTH-1; i++)
	for(int j=y1; j<=y2 && j<=SCREEN_HEIGHT-1; j++)
		VRAM_A[i+j*SCREEN_WIDTH] = drawColor;
}

void clearScreen(uint16 c)
{
	for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++)
		VRAM_A[i] = c;
}

