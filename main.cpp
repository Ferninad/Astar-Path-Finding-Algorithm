#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void DrawPath();
void DrawGrid();
void DrawObstacles();
void Astart(int sx, int sy, int ex, int ey);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

double screenWidth = 500;
double screenHeight = 500;
double gridSize = 5;
int sx, sy, ex, ey;

vector<vector<vector<int>>> grid;
vector<vector<int>> obs;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    
    for(int i = 0; i < screenWidth/gridSize; i++){
        vector<vector<int>> temp;
        for(int j = 0; j < screenHeight/gridSize; j++){
            int ob = 0;
            double ran = static_cast<double>(rand())/RAND_MAX;
            if(ran < .2 && !(i == 0 && j == 0) && !(i == screenWidth/gridSize - 1 && j == screenHeight/gridSize - 1))
                ob = 1;
            temp.push_back({0, 0, 0, ob, 0, 0}); // f g h (1 if obstacle) prevx prevy
        }
        grid.push_back(temp);
    }
    sx = 0;
    sy = 0;
    ex = grid.size() - 1;
    ey = grid[0].size() - 1;
    grid[ex - 1][ey - 1][3] = 1;
    Astart(sx, sy, ex, ey);
    while (gameLoop)
    {   
        //DrawGrid();
        DrawObstacles();
        DrawPath();
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Astart(int sx, int sy, int ex, int ey){
    vector<vector<int>> openSet;
    vector<vector<int>> closedSet;
    openSet.push_back({sx, sy});

    while(openSet.size() != 0){
        vector<int> current;
        double minF = grid[openSet[0][0]][openSet[0][1]][0];
        double minFIndex = 0;
        for(int i = 0; i < openSet.size(); i++){
            if(grid[openSet[i][0]][openSet[i][1]][0] < minF){
                minF = grid[openSet[i][0]][openSet[i][1]][0];
                minFIndex = i;
            }
        }
        current = openSet[minFIndex];
        if(current[0] == ex && current[1] == ey){
            cout << "done" << endl;
        }
        openSet.erase(openSet.begin()+minFIndex);
        closedSet.push_back(current);
        
        vector<vector<int>> neighbors;
        for(int x = -1; x < 2; x++){
            for(int y = -1; y < 2; y++){
                if(current[0] + x >= 0 && current[0] + x < grid.size() && current[1] + y >= 0 && current[1] + y < grid[0].size() && !(x==0 && y==0)){
                    if(grid[current[0] + x][current[1] + y][3] == 0)
                        neighbors.push_back({current[0] + x, current[1] + y});
                }
            }
        }
        for(int i = 0; i < neighbors.size(); i++){
            bool inClosedSet = false;
            bool inOpenSet = false;
            int tempG;
            for(int j = 0; j < closedSet.size(); j++){
                if(closedSet[j][0] == neighbors[i][0] && closedSet[j][1] == neighbors[i][1]){
                    inClosedSet = true;
                }
                if(inClosedSet)
                    break;
            }
            if(!inClosedSet){
                tempG = grid[current[0]][current[1]][1] + abs(sqrt(pow(neighbors[i][0] - current[0], 2) + pow(neighbors[i][1] - current[1], 2)));
                bool newPath = false;
                for(int k = 0; k < openSet.size(); k++){
                    if(openSet[k][0] == neighbors[i][0] && openSet[k][1] == neighbors[i][1]){
                        inOpenSet = true;
                        if(tempG < grid[neighbors[i][0]][neighbors[i][1]][1]){
                            newPath = true;
                            grid[neighbors[i][0]][neighbors[i][1]][1] = tempG;
                        }
                    }
                    if(inOpenSet)
                        break;
                }
                if(inOpenSet == false){
                    newPath = true;
                    grid[neighbors[i][0]][neighbors[i][1]][1] = tempG;
                    openSet.push_back({neighbors[i][0], neighbors[i][1]});
                }
                if(newPath){
                    grid[neighbors[i][0]][neighbors[i][1]][2] = abs(ex - neighbors[i][0]) + abs(ey - neighbors[i][1]);
                    //grid[neighbors[i][0]][neighbors[i][1]][2] = sqrt(pow(ex - neighbors[i][0], 2) + pow(ey - neighbors[i][1], 2));
                    grid[neighbors[i][0]][neighbors[i][1]][0] = grid[neighbors[i][0]][neighbors[i][1]][1] + grid[neighbors[i][0]][neighbors[i][1]][2];
                    grid[neighbors[i][0]][neighbors[i][1]][4] = current[0];
                    grid[neighbors[i][0]][neighbors[i][1]][5] = current[1];
                }
            }
        }
    }

}

void DrawObstacles(){
    for(int x = 0; x < grid.size(); x++){
        for(int y = 0; y < grid[0].size(); y++){
            if(grid[x][y][3] == 1){
                pos.x = x * gridSize;
                pos.y = y * gridSize;
                pos.w = gridSize;
                pos.h = gridSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
}

void DrawPath(){
    int x = ex;
    int y = ey;
    int tempx;
    int tempy;
    pos.x = x * gridSize;
    pos.y = y * gridSize;
    pos.w = gridSize;
    pos.h = gridSize;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &pos);
    while(!(x == sx && y == sy)){
        tempx = x;
        tempy = y;
        x = grid[tempx][tempy][4];
        y = grid[tempx][tempy][5];
        pos.x = x * gridSize;
        pos.y = y * gridSize;
        pos.w = gridSize;
        pos.h = gridSize;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
}

void DrawGrid(){
    for(int x = 0; x <= screenWidth; x+=gridSize){
        pos.x = x;
        pos.y = 0;
        pos.w = 1;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    for(int y = 0; y <= screenHeight; y+=gridSize){
        pos.x = 0;
        pos.y = y;
        pos.w = screenWidth;
        pos.h = 1;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
}