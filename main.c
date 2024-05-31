#include "raylib.h"
#include "math.h"


typedef struct Car {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    float speed;
    float maxSpeed;
    float accelerationRate;
    float brakingRate;
    float turnSpeed;
    float downforce;
    int drs;
    Model model;
} Car;

int main(void)
{
    const int screenWidth = 1920, screenHeight = 1200;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    ToggleFullscreen(); InitAudioDevice();

    Texture2D sprite = LoadTexture("Great_Britain_Circuit.png");
    Music music = LoadMusicStream("f1-151254.mp3");
    Sound sound = LoadSound("a.ogg");
    Texture2D texture = LoadTexture("Texture.png");
    Model model = LoadModel("untitled.obj");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    
    Mesh planeMesh = GenMeshPlane(40.0f, 40.0f, 1, 1);
    Model planeModel = LoadModelFromMesh(planeMesh);
    planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprite;

    Camera3D camera = { .position = {15.0f, 10.0f, 10.0f}, .target = {0.0f, 0.0f, 0.0f}, .up = {0.0f, 1.0f, 0.0f}, .fovy = 45.0f };
    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);
    SetTargetFPS(144);
    float spriteScale = 0.2f, posX = 0;

    double startTime = GetTime();
    while (!WindowShouldClose())
    {
        posX = fmodf(posX + GetFrameTime() * 100, GetScreenWidth());
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE)) { StopMusicStream(music); PlayMusicStream(music); }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Rectangle spriteRect = { posX, 10, sprite.width * spriteScale, sprite.height * spriteScale };
            if (CheckCollisionPointRec(GetMousePosition(), spriteRect)) PlaySound(sound);
        }
        
        double currentTime = GetTime();
        double elapsedTime = currentTime - startTime;

        BeginDrawing();
        ClearBackground(BLACK);
        
        BeginMode3D(camera);
        DrawModel(model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
        DrawBoundingBox(bounds, GREEN);

        Vector3 planePosition = {0.0f, -1.3f, 0.0f};
        Vector3 planeScale = {40.0f, 1.0f, 40.0f}; // plane scale
        DrawModelEx(planeModel, planePosition, (Vector3){1.0f, 0.0f, 0.0f}, 0.0f, planeScale, WHITE);
        
        DrawGrid(20, 1.0f);
        EndMode3D();
        
        
        DrawFPS(10, 10);
        DrawText(TextFormat("Speed: %.2f", 0), 10, 40, 20, RED);
        DrawText(TextFormat("Position: [%.2f, %.2f, %.2f]", 0, 0, 0), 10, 70, 20, RED);
        DrawText(TextFormat("Acceleration: [%.2f, %.2f, %.2f]", 0, 0, 0), 10, 100, 20, RED);
        DrawRectangle(posX, 0, 10, 10, BLUE);
        DrawTextureEx(sprite, (Vector2){1550, 10}, 0.0f, spriteScale, GRAY);
        //DrawCircle(posX, 10, 5, WHITE);
        DrawText(TextFormat("%.2f ", elapsedTime), 10,1100,20, RED);
        EndDrawing();
    }

    UnloadTexture(sprite); UnloadTexture(texture); UnloadModel(model);
    UnloadMusicStream(music); UnloadSound(sound); CloseAudioDevice(); CloseWindow();

    return 0;
}
