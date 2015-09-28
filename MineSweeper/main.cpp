#include <nds.h>
#include <stdio.h>
#include <time.h>

void drawSprite(uint8* data, int x, int y);
extern uint8 sprite[13][32];

class Game
{
	static const uint8 MINE = 1<<4;
	static const uint8 HIDE = 1<<5;
	static const uint8 FLAG = 1<<6;
	// 1bit:hide 1bit:mine 4bits:numbers
	uint8 grid[16][12]; // xxHMNNNN
	
	public:
	Game(int n);
	void clear();
	void generate(int x, int y);
	void placeMine(int x, int y);
	void incrCounter(int x, int y);
	void open(int x, int y);
	void setFlag(int x, int y);
	void draw();
	bool win();
	
	uint16 minesNumber, flags;
};

Game::Game(int n) {
	minesNumber = n;
	clear();
}

void Game::clear()
{
	for(int i=0; i<16*12; i++)
		grid[i%16][i/16]=HIDE;
	flags = 0;
}

void Game::generate(int cx, int cy)
{
	clear();
	int n = minesNumber;
	while(n > 0) {
		int x = rand()%16, y = rand()%12;
		
		if((x==cx && y==cy) || grid[x][y]&MINE)
			continue;
		
		placeMine(x, y);
		n--;
	}
}

void Game::placeMine(int x, int y)
{
	grid[x][y] |= MINE;
	for(int i=-1; i<2; i++)
	for(int j=-1; j<2; j++)
	{
		incrCounter(x+i, y+j);
	}
}

void Game::incrCounter(int x, int y)
{
	if(x<0 || y<0 || x>=16 || y>=12)
		return;
	int count = grid[x][y] & 0xF;
	count++;
	grid[x][y] &= 0xF0;
	grid[x][y] |= count & 0xF;
}

void Game::setFlag(int x, int y)
{
	if(x<0 || y<0 || x>=16 || y>=12)
		return;
	if(grid[x][y]&HIDE)
	{
		grid[x][y] ^= FLAG;
		
		if(grid[x][y]&FLAG) flags++;
		else flags--;
	}
}

void Game::draw()
{
	for(int i=0; i<16; i++)
	for(int j=0; j<12; j++)
	{
		if(grid[i][j] & FLAG)
			drawSprite(sprite[9], i*16, j*16);
		else
		if(grid[i][j] & HIDE)
			drawSprite(sprite[11], i*16, j*16);
		else
		if(grid[i][j] & MINE)
			drawSprite(sprite[12], i*16, j*16);
		else
			drawSprite(sprite[grid[i][j]&0xF], i*16, j*16);
	}
}

void Game::open(int x, int y)
{
	if(x<0 || y<0 || x>=16 || y>=12)
		return;
	
	if( !(grid[x][y] & HIDE) || grid[x][y]&FLAG )
		return;
		
	grid[x][y] &= ~HIDE;
	
	if(grid[x][y]&MINE) {
		iprintf("Vous avez perdu...\n");
		return;
	}
	
	if(grid[x][y]&0xF) {
		if(x>0 && !(grid[x-1][y]&0xF)) open(x-1, y);
		if(y>0 && !(grid[x][y-1]&0xF)) open(x, y-1);
		if(x<15 && !(grid[x+1][y]&0xF)) open(x+1, y);
		if(y<11 && !(grid[x][y+1]&0xF)) open(x, y+1);
		return;
	}
	
	open(x-1, y);
	open(x+1, y);
	open(x, y-1);
	open(x, y+1);
	
	open(x-1, y-1);
	open(x+1, y-1);
	open(x-1, y+1);
	open(x+1, y+1);
}

bool Game::win()
{
	for(int i=0; i<16; i++)
	for(int j=0; j<12; j++)
	{
		if( !(grid[i][j]&MINE) xor !(grid[i][j]&HIDE) )
			return false;
	}
	return true;
}

int main(void)
{
	videoSetMode(MODE_FB0);
	vramSetBankA(VRAM_A_LCD);
	lcdMainOnBottom();
	consoleDemoInit();
	
	srand(time(NULL));
	
	touchPosition touch;
	int mines = 30;
	Game game(mines);
	bool start = false;
	
	iprintf("Bienvenue dans le demineur.\n");
	iprintf("Appuyez sur R pour afficher\nl'aide.\n");
	iprintf("\nLe jeu commence avec %d mines!\n", game.minesNumber);
	
	while(1)
	{
		scanKeys();
		touchRead(&touch);
		
		if(keysDown() & KEY_TOUCH)
		{
			int x = touch.px/16;
			int y = touch.py/16;
			
			if(!start) {
				game.generate(x, y);
				game.open(x, y);
				start = true;
			}
			else
			{
				if(keysHeld() & KEY_L)
					game.setFlag(x, y);
				else
					game.open(x, y);
			}
			
			if(game.win())
				iprintf("Bravo, vous avez gagne!\n");
		}
		
		// Afficher l'aide
		if(keysDown() & KEY_R)
		{
			iprintf("\n         *** AIDE ***\n");
			iprintf("-X pour commencer une nouvelle\n partie\n");
			iprintf("-L et clic pour poser un drapeau");
			iprintf("-Haut pour augmenter le nombre\n de mines de la prochaine partie");
			iprintf("-Bas pour diminuer le nombre de\n mines de la prochaine partie\n");
			iprintf("-Gauche pour afficher le nombre\n de mines a trouver\n");
			iprintf("-Droite pour afficher le nombre\n de mines dans la partie\n\n");
		}
		
		// Faire une nouvelle partie
		if(keysDown() & KEY_X)
		{
			game.clear();
			start = false;
			game.minesNumber = mines;
			iprintf("Le jeu commence avec %d mines!\n", mines);
		}
		// Changer le nombre de mines
		if(keysDown() & KEY_DOWN)
		{
			mines--;
			if(mines < 1)
				mines = 1;
		}
		if(keysDown() & KEY_UP)
		{
			mines++;
			if(mines >= 16*12)
				mines = 16*12-1;
		}
		
		// Infos
		if(keysDown() & KEY_LEFT)
		{
			iprintf("Mines decouvertes: %d\n", game.flags);
		}
		if(keysDown() & KEY_RIGHT)
		{
			iprintf("Mines: %d\n", game.minesNumber);
		}
		
		game.draw();
		swiWaitForVBlank();
	}
 
	return 0;
}
