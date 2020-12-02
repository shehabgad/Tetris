#include <iostream>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <queue>
#include <algorithm>
#include <random>
#include <thread>
#include <vector>
#include <iterator>
#include <deque>
using namespace std;

// initializing height and width of the window
const int WIDTH = 90;
const int HEIGHT = 35;

// declaring handlers
HANDLE wHnd;
HANDLE rHnd;

// clearing screen buffer function 
void ClearBuffer(CHAR_INFO *buffer, int bufferWidth = WIDTH, int bufferHeight = HEIGHT)
{
    for (int i = 0, size = WIDTH * HEIGHT; i < size; i++)
    {
        buffer[i].Char.UnicodeChar = L' ';
        buffer[i].Attributes = 15;
    }
}
// creating the container the player will play in 
void DrawContainer(CHAR_INFO* screenbuffer,wchar_t shapeX, wchar_t shapeY,const int width = WIDTH, const int height = HEIGHT)
{
    for (int x = 30; x < width - 30; x++)
    {
        screenbuffer[x + width * 5].Char.UnicodeChar = shapeX;
        screenbuffer[x + width * 5].Attributes = 15;
        screenbuffer[x + width * (height - 6)].Char.UnicodeChar = shapeX;
        screenbuffer[x + width * (height - 6)].Attributes = 15;
    }
    for (int y = 6; y < height - 6; y++)
    {
        screenbuffer[30 + width * y].Char.UnicodeChar = shapeY;
        screenbuffer[30 + width * y].Attributes = 15;

        screenbuffer[(width - 31) + width * y].Char.UnicodeChar = shapeY;
        screenbuffer[(width - 31) + width * y].Attributes = 15;
    }
}

//writing text on screen buffer 
void WriteText(CHAR_INFO *buffer, const wchar_t* text, COORD pos, int color)
{

    for (int i = pos.X, writingSize = wcslen(text) + pos.X, j = 0; i < writingSize; i++)
    {
        buffer[i + WIDTH * pos.Y].Char.UnicodeChar = text[j++];
        buffer[i + WIDTH * pos.Y].Attributes = color;

    }


}

// creating event data structure to hold the event buffer and its size 
struct EventData
{
    DWORD eventBufferSize;
    INPUT_RECORD* eventBuffer;
};

// function to create events 
EventData getEventBuffer() {
    DWORD EventsHappened = 0;
    GetNumberOfConsoleInputEvents(rHnd, &EventsHappened);
    DWORD EventsRead = 0;
    if (EventsHappened != 0)
    {
        INPUT_RECORD* eventBuffer = new INPUT_RECORD[EventsHappened];
        ReadConsoleInput(rHnd, eventBuffer, EventsHappened, &EventsRead);
        return EventData{ EventsRead, eventBuffer};
    }
    else {
        return EventData{ 0,0 };
    }
}

// creating Piece class that hold blocks
enum class Shapes {SHAPE_1,SHAPE_2,SHAPE_3,SHAPE_4,SHAPE_5,SHAPE_6};


class Piece {
    int Color;
    int rotate;
    static wchar_t drawingUnicode;
    void DrawBlocks(CHAR_INFO *screenBuffer, COORD POS,COORD shifts[])
    {

        for (int i = 0; i < 4; i++)
        {
        screenBuffer[(POS.X + shifts[i].X) + WIDTH * (POS.Y + shifts[i].Y)].Char.UnicodeChar = drawingUnicode;
        screenBuffer[(POS.X + shifts[i].X) + WIDTH * (POS.Y + shifts[i].Y)].Attributes = Color;
        Blocks[i].X = (POS.X + shifts[i].X);
        Blocks[i].Y = (POS.Y + shifts[i].Y);
        }
    }
    // this function will change the position we will right on if it reach the limits of the container
    
    // ***************************
    void changePos(COORD &POS,COORD newPosMax, COORD newPosMin)
    {
        if (Shape != Shapes::SHAPE_6 && Shape != Shapes::SHAPE_4)
        {
        if (POS.X >= 57)
            POS.X = newPosMax.X;
        else if (POS.X <= 31)
            POS.X = newPosMin.X;
        if (POS.Y >= 28)
            POS.Y = newPosMax.Y;
        else if (POS.Y <= 6)
            POS.Y = newPosMin.Y;
        }
        else if(Shape == Shapes::SHAPE_6)
        {
            if (POS.X >= 55)
                POS.X = newPosMax.X;
            else if (POS.X <= 31)
                POS.X = newPosMin.X;
            if (POS.Y >= 28)
                POS.Y = newPosMax.Y;
            else if (POS.Y <= 6)
                POS.Y = newPosMin.Y;
        }
        else if(Shape == Shapes::SHAPE_4){
            if (POS.X >= 58)
                POS.X = newPosMax.X;
            else if (POS.X <= 33)
                POS.X = newPosMin.X;
            if (POS.Y >= 28)
                POS.Y = newPosMax.Y;
            else if (POS.Y <= 6)
                POS.Y = newPosMin.Y;
        }
    }
public:
    COORD* Blocks;
    Shapes Shape;
    Piece(Shapes shape, int color) : Color{ color }, Shape{ shape }, Blocks{ new COORD[4] }, rotate{ 0 } { }
    Piece() : Piece(Shapes(0), 10) {}
    void setPieceCoordinates(COORD POS)
    {
        // ***************************

        if (Shape == Shapes::SHAPE_1 || Shape == Shapes::SHAPE_2 || Shape == Shapes::SHAPE_3)
            changePos(POS, { 56,27 }, { 31,6 });
        else if (Shape == Shapes::SHAPE_4)
            changePos(POS, { 58 ,27 }, { 33,6 });
        else if (Shape == Shapes::SHAPE_5)
            changePos(POS, { 57,27 }, { 31,6 });
        else if (Shape == Shapes::SHAPE_6)
            changePos(POS, { 55,28 }, { 31,6 });
        if (Shape == Shapes::SHAPE_1)
        {
            COORD shifts[] = { {0,0}, {0,1}, {1,1}, {2,1} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
        }
        else if (Shape == Shapes::SHAPE_2)
        {
            COORD shifts[] = { {0,0}, {1,0}, {2,0}, {1,1} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
        }
        else if (Shape == Shapes::SHAPE_3)
        {
            COORD shifts[] = { {0,0}, {1,0}, {1,1}, {2,1} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
        }
        else if (Shape == Shapes::SHAPE_4)
        {
            COORD shifts[] = { {0,0}, {0,1}, {-1,1}, {-2,1} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
           
        }
        else if (Shape == Shapes::SHAPE_5)
        {
            COORD shifts[] = { {0,0}, {1,0}, {0,1}, {1,1} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
          
        }
        else if (Shape == Shapes::SHAPE_6)
        {
            COORD shifts[] = { {0,0}, {1,0}, {2,0}, {3,0} };
            for (int i = 0; i < 4; i++)
            {
                Blocks[i].X = (POS.X + shifts[i].X);
                Blocks[i].Y = (POS.Y + shifts[i].Y);
            }
        }

    }
    void Draw(CHAR_INFO* screenBuffer, COORD POS, bool checkBoundries = true)
    {
        // ***************************
        if (checkBoundries)
        {
        if (Shape == Shapes::SHAPE_1 || Shape == Shapes::SHAPE_2 || Shape == Shapes::SHAPE_3)
            changePos(POS, { 56,27 }, { 31,6 });
        else if (Shape == Shapes::SHAPE_4)
            changePos(POS, { 58 ,27 }, { 33,6 });
        else if (Shape == Shapes::SHAPE_5)
            changePos(POS, { 57,27 }, { 31,6 });
        else if (Shape == Shapes::SHAPE_6)
            changePos(POS, { 55,28 }, { 31,6 });
        }
        if (Shape == Shapes::SHAPE_1)
        {
            COORD shifts[] = { {0,0}, {0,1}, {1,1}, {2,1} };
            DrawBlocks(screenBuffer, POS, shifts);
        }
        else if (Shape == Shapes::SHAPE_2)
        {
            COORD shifts[] = { {0,0}, {1,0}, {2,0}, {1,1} };
            DrawBlocks(screenBuffer, POS, shifts);
        }
        else if (Shape == Shapes::SHAPE_3)
        {
            COORD shifts[] = { {0,0}, {1,0}, {1,1}, {2,1} };
            DrawBlocks(screenBuffer, POS, shifts);
        }
        else if (Shape == Shapes::SHAPE_4)
        {
            COORD shifts[] = { {0,0}, {0,1}, {-1,1}, {-2,1} };
            DrawBlocks(screenBuffer, POS, shifts);
        }
        else if (Shape == Shapes::SHAPE_5)
        {
            COORD shifts[] = { {0,0}, {1,0}, {0,1}, {1,1} };
            DrawBlocks(screenBuffer, POS, shifts);
        }
        else if (Shape == Shapes::SHAPE_6)
        {
            COORD shifts[] = { {0,0}, {1,0}, {2,0}, {3,0} };
            DrawBlocks(screenBuffer, POS, shifts);
        }

    }

    void Rotate()
    {

    }
    COORD getCurrentPos()
    {
        return Blocks[0];
    }
    COORD HeighlightPiece(CHAR_INFO* screenBuffer) {
        COORD* MaxY = max_element(Blocks, Blocks + 4, [](COORD& a, COORD& b) {
            return a.Y < b.Y;
            });
        short maxY = MaxY->Y;
        COORD* MinY = min_element(Blocks, Blocks + 4, [](COORD& a, COORD& b) {
            return a.Y < b.Y;
            });
        short minY = MinY->Y;

        COORD* MaxX = max_element(Blocks, Blocks + 4, [](COORD& a, COORD& b) {
            return a.X < b.X;
            });
        short maxX = MaxX->X;
        COORD* MinX = min_element(Blocks, Blocks + 4, [](COORD& a, COORD& b) {
            return a.X < b.X;
            });
        short minX = MinX->X;
        short shifts = (maxY - minY) + 1;
        COORD dist;
        dist.X = Blocks[0].X;
        // ***************************
        for (short y = minY + 1; y < 30; y++)
        {
            if (y == 29)
                return { dist.X, 28 };
            Piece TrackedPiece(Shape,Color);
            TrackedPiece.setPieceCoordinates({ dist.X,y });;
            for (int i = 0; i < 4; i++)
            {
                if (screenBuffer[TrackedPiece.Blocks[i].X + WIDTH * TrackedPiece.Blocks[i].Y].Char.UnicodeChar != L' ')
                {
                    dist.Y = y - 1;
                    return dist;
                }
               
            }
        }
    }
    void GhostColor()
    {
        this->Color = 8;
    }
    int getColor()
    {
        return Color;
    }
    COORD DropPiece(CHAR_INFO *screenBuffer, COORD currentPos)
    {
     
        
    }

};
struct PieceData {
    COORD piecePos;
    int color;
};
wchar_t Piece::drawingUnicode = L'O';
int main()
{
    // // set the mode to write unicode character
    _setmode(_fileno(stdout), _O_U16TEXT);
    // setting up the window size 
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    rHnd = GetStdHandle(STD_INPUT_HANDLE);
    SMALL_RECT windowSize;
    windowSize = { 0, 0, WIDTH - 1, HEIGHT - 1 };
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
    // setting up the screen buffer
    COORD screenBufferSize = { WIDTH,HEIGHT };
    SetConsoleScreenBufferSize(wHnd, screenBufferSize);
    CHAR_INFO *screenBuffer = new CHAR_INFO[WIDTH * HEIGHT];
    ClearBuffer(screenBuffer);
    bool Menu = true;
    while (Menu)
    {
        for (int x = 7; x < WIDTH - 6; x++)
        {
            for (int y = 2; y < HEIGHT - 2; y++)
            {
                screenBuffer[x + WIDTH * y].Char.UnicodeChar = L' ';
                screenBuffer[x + WIDTH * y].Attributes = 223;
            }
        }

        WriteText(screenBuffer, L"    TETRIS GAME    ", COORD{ 37,8 }, 253);
        WriteText(screenBuffer, L"    MADE BY SHEHAB GAD    ", COORD{ 33,10 }, 253);
        WriteText(screenBuffer, L"    PRESS ENTER TO PLAY:    ", COORD{ 32,12 }, 253);
        WriteText(screenBuffer, L"    INSTRUCTIONS    ", COORD{ 37,16 }, 253);
        WriteText(screenBuffer, L"    PRESS \x2192 TO MOVE RIGHT    ", COORD{ 32,18 }, 253);
        WriteText(screenBuffer, L"    PRESS \x2190 TO MOVE LEFT     ", COORD{ 32,20 }, 253);
        WriteText(screenBuffer, L"    PRESS \x2193 TO DROP PIECE    ", COORD{ 32,22 }, 253);
        WriteText(screenBuffer, L"   PRESS [C] TO SWAP PIECE   ", COORD{ 32,24 }, 253);


        DWORD numberOfEventsHappend = 0;
        GetNumberOfConsoleInputEvents(rHnd, &numberOfEventsHappend);
        DWORD numberOfEventsRead = 0;
        if (numberOfEventsHappend != 0)
        {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[numberOfEventsHappend];
            ReadConsoleInput(rHnd, eventBuffer, numberOfEventsHappend, &numberOfEventsRead);
            for (int i = 0; i < numberOfEventsRead; i++)
            {
                if (eventBuffer[i].Event.KeyEvent.bKeyDown)
                {
                    if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                    {
                        Menu = false;
                        break;
                    }
                }
            }
        }
        WriteConsoleOutputW(wHnd, screenBuffer, screenBufferSize, COORD{ 0, 0 }, &windowSize);

    }
    bool Playing = true;
    bool GameRunning = true;
    while (Playing)
    {
    // game loop
    bool iscurrentPieceDropped = true;
    bool DroppingCurrent = false;
    queue<Piece> Pieces;
    random_device rd;
    random_device ra;
    mt19937 e2(rd());
    mt19937 e3(ra());
    EventData eventData{ 0,0 };
    uniform_int_distribution<> dist(0, 5);
    uniform_int_distribution<> dist2(9, 14);
    deque<Piece> NextPieces;
    Piece Piece1{ Shapes(dist(e2)), dist2(e3) };
    Piece Piece2{ Shapes(dist(e2)), dist2(e3) };
    Piece Piece3{ Shapes(dist(e2)), dist2(e3) };
    Piece Piece4{ Shapes(dist(e2)), dist2(e3) };
    Pieces.push(Piece1);
    Pieces.push(Piece2);
    Pieces.push(Piece3);
    Pieces.push(Piece4);
    NextPieces.push_back(Piece2);
    NextPieces.push_back(Piece3);
    NextPieces.push_back(Piece4);
    Piece currentPiece;
    // ***************************
    COORD currentPiecePOS{ 44, 6 };
    vector<PieceData> PiecesStorage;
    Piece GhostPiece;
    Piece Hold;
    bool firstHold = true;
    bool HoldEmpty = true;
    int lines = 0;
    int time = 0;
    int numberOfPieces = 0;
    int Score = 0;
    ClearBuffer(screenBuffer);
    while (GameRunning)
    {
            Sleep(10);
            time+=10;
            numberOfPieces = 0;
            eventData = getEventBuffer();
            if (eventData.eventBufferSize)
            {
                int Break = 0;
                for (int i = 0; i < eventData.eventBufferSize; i++)
                {
                    if (eventData.eventBuffer[i].Event.KeyEvent.bKeyDown)
                    {
                        if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT)
                        {
                         
                              currentPiecePOS.X--;
                              if (currentPiecePOS.X <= 33 && currentPiece.Shape == Shapes::SHAPE_4)
                                  currentPiecePOS.X = 33;
                              else if(currentPiecePOS.X <= 31)
                                  currentPiecePOS.X = 31;
                              currentPiece.setPieceCoordinates({ currentPiecePOS.X, currentPiecePOS.Y });
                              for (auto i = PiecesStorage.begin(); i != PiecesStorage.end(); i++)
                              {
                                  for (int j = 0; j < 4; j++)
                                  {
                                          if (currentPiece.Blocks[j].X == i->piecePos.X && currentPiece.Blocks[j].Y == i->piecePos.Y)
                                          {
                                              currentPiece.setPieceCoordinates({ currentPiecePOS.X, 6 });
                                              Score += 10;
                                              DroppingCurrent = true;
                                          }

                                  }

                              }

                        }
                        else if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT)
                        {
                         
                                currentPiecePOS.X++;
                                if (currentPiecePOS.X >= 57 && currentPiece.Shape != Shapes::SHAPE_6 && currentPiece.Shape != Shapes::SHAPE_4)
                                {
                                    if (currentPiece.Shape != Shapes::SHAPE_4 && currentPiece.Shape != Shapes::SHAPE_5)
                                    {
                                        currentPiecePOS.X = currentPiece.Blocks[0].X;

                                    }
                                    else
                                    {

                                        currentPiecePOS.X = 57;
                                    }
                                }
                                else if (currentPiecePOS.X >= 55 && currentPiece.Shape == Shapes::SHAPE_6)
                                    currentPiecePOS.X = 55;
                                else if (currentPiecePOS.X >= 58 && currentPiece.Shape == Shapes::SHAPE_4)
                                    currentPiecePOS.X = 58;
                                currentPiece.setPieceCoordinates({ currentPiecePOS.X, currentPiecePOS.Y });
                                for (auto i = PiecesStorage.begin(); i != PiecesStorage.end(); i++)
                                {
                                    for (int j = 0; j < 4; j++)
                                    {
                                        if (currentPiece.Blocks[j].X == i->piecePos.X && currentPiece.Blocks[j].Y == i->piecePos.Y)
                                        {
                                            currentPiece.setPieceCoordinates({ currentPiecePOS.X, 6 });
                                            Score += 10;
                                            DroppingCurrent = true;
                                        }

                                    }

                                }
                                
                        }
                        else if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
                        {
                          
                            DroppingCurrent = true;

                        }
                        else if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == 0x43)
                        {
                            if (firstHold)
                            {
                                firstHold = false;
                                Hold = currentPiece;
                                HoldEmpty = false;
                                currentPiece = NextPieces.front();
                                GhostPiece = currentPiece;
                                GhostPiece.GhostColor();
                                NextPieces.pop_front();
                                Piece newPiece{ Shapes(dist(e2)), dist2(e3) };
                                Pieces.push(newPiece);
                                NextPieces.push_back(newPiece);
                            }
                            else {
                                Piece temp = currentPiece;
                                currentPiece = Hold;
                                GhostPiece = Hold;
                                GhostPiece.GhostColor();
                                Hold = temp;
                            }
                   
                        }
                    }
                }
                if (Break)
                    break;
            }

        
        ClearBuffer(screenBuffer);
        DrawContainer(screenBuffer, L'\x25a0', L'\x2588');
        wchar_t NextText[20];
        swprintf_s(NextText, 20, L"  NEXT  ");
        WriteText(screenBuffer, NextText, COORD{ 72,3 }, 240);
        for (short i = 0, y = 5; i < NextPieces.size(); i++)
        {
            NextPieces[i].Draw(screenBuffer, COORD{74,y}, false);
            y += 3;
        }
        wchar_t HoldText[20];
        swprintf_s(HoldText, 20, L"  HOLD  ");
        WriteText(screenBuffer, HoldText, COORD{ 10,3 }, 240);
        if (!HoldEmpty)
        {
            Hold.Draw(screenBuffer, COORD{13,5}, false);
        }

        for (auto i = PiecesStorage.begin(); i != PiecesStorage.end(); i++)
        {
            screenBuffer[i->piecePos.X + WIDTH * i->piecePos.Y].Char.UnicodeChar = L'O';
            screenBuffer[i->piecePos.X + WIDTH * i->piecePos.Y].Attributes = i->color;
        }
        for (int y = 6; y < 29; y++)
        {
            numberOfPieces = 0;
            for (auto i = PiecesStorage.begin(); i != PiecesStorage.end(); i++)
            {
                if (i->piecePos.Y == y)
                    numberOfPieces++;
            }
            if (numberOfPieces == 28)
            {
                lines++;
                Score += 100;
                vector<PieceData> temp; 
                for (int i = 0; i < PiecesStorage.size(); i++)
                {
                    if (PiecesStorage.at(i).piecePos.Y < y)
                    {
                        PiecesStorage.at(i).piecePos.Y++;
                        temp.push_back(PiecesStorage.at(i));
                    }
                    else if (PiecesStorage.at(i).piecePos.Y > y)
                    {
                        temp.push_back(PiecesStorage.at(i));
                    }

                }
                PiecesStorage = temp;
                y= 6;
            }
        }
        if (iscurrentPieceDropped)
        {
            currentPiece = NextPieces.front();
            Pieces.pop();
            // ***************************
            currentPiecePOS.X = 44;
            currentPiecePOS.Y = 6;
            time = 0;
            if (currentPiece.Blocks[0].Y - currentPiece.Blocks[3].Y != 0)
            {
             if (screenBuffer[currentPiecePOS.X + WIDTH * (currentPiecePOS.Y )].Char.UnicodeChar != L' ')
             {
                GameRunning = false;
                break;
             }

            }
            else
            {
                if (screenBuffer[currentPiecePOS.X + WIDTH * currentPiecePOS.Y ].Char.UnicodeChar != L' ')
                {
                    GameRunning = false;
                    break;
                }
            }
            Piece newPiece{ Shapes(dist(e2)), dist2(e3) };
            Pieces.push(newPiece);
            NextPieces.push_back(newPiece);
            NextPieces.pop_front();
            GhostPiece = currentPiece;
            GhostPiece.GhostColor();
            iscurrentPieceDropped = false;
            DroppingCurrent = false;
        }
        if (DroppingCurrent)
        {
            COORD dest = currentPiece.HeighlightPiece(screenBuffer);
            Score+= 2* (dest.Y - currentPiecePOS.Y);
            currentPiece.Draw(screenBuffer, dest);
            int color = currentPiece.getColor();
            for (int i = 0; i < 4; i++)
            {
                PiecesStorage.push_back(PieceData{ currentPiece.Blocks[i],color });

            }
            iscurrentPieceDropped = true;
            DroppingCurrent = false;

        }
        else {
            COORD DIST = currentPiece.HeighlightPiece(screenBuffer);
            GhostPiece.Draw(screenBuffer, DIST);
            currentPiece.Draw(screenBuffer, { currentPiecePOS.X , currentPiecePOS.Y });
            if (time == 600)
            {
                currentPiecePOS.Y++;
                time = 0;
            }
            if (DIST.Y == currentPiecePOS.Y)
            {
                DroppingCurrent = true;
                Score += 10;
            }
            
        }
  
        wchar_t LinesText[20];
        swprintf_s(LinesText, 20, L"LINES : %d", lines);
        WriteText(screenBuffer, LinesText , COORD{ 30,3 }, 15);
        
        wchar_t ScoreText[20];
        swprintf_s(ScoreText, 20, L"SCORE : %d", Score);
        WriteText(screenBuffer, ScoreText, COORD{ 51,3 }, 15);
         
        
        // writing to screen buffer 
        WriteConsoleOutputW(wHnd, screenBuffer, screenBufferSize, COORD{0, 0}, &windowSize);
     }
     while (true)
     {
      wchar_t LinesText[20];
      swprintf_s(LinesText, 20, L"LINES : %d", lines);
      WriteText(screenBuffer, LinesText, COORD{ 30,3 }, 15);

      wchar_t ScoreText[20];
      swprintf_s(ScoreText, 20, L"SCORE : %d", Score);
      WriteText(screenBuffer, ScoreText, COORD{ 51,3 }, 15);

      wchar_t GameOverText[70];
      swprintf_s(GameOverText, 70, L"                  GAMEOVER                  ");
      WriteText(screenBuffer, GameOverText, COORD{ 25,1 }, 207);

      wchar_t GameOverInstruction[80];
      swprintf_s(GameOverInstruction, 80, L"  PRESS ENTER TO PLAY AGAIN OR ESC TO EXIT  ");
      WriteText(screenBuffer, GameOverInstruction, COORD{ 25,2 }, 207);
      WriteConsoleOutputW(wHnd, screenBuffer, screenBufferSize, COORD{ 0, 0 }, & windowSize);
       
      eventData = getEventBuffer();
      if (eventData.eventBufferSize)
      {
          for (int i = 0; i < eventData.eventBufferSize; i++)
          {
              if (eventData.eventBuffer[i].Event.KeyEvent.bKeyDown)
              {
                  if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                  {
                      GameRunning = true;
                      break;
                      
                  }
                  else if (eventData.eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
                  {
                      Playing = false;
                      break;
                  }
              }
          }
          if (!Playing || GameRunning)
              break;
      }

     }
    }



    // deleting screen buffer 
    delete[] screenBuffer;
}

