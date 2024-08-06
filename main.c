#include "raylib.h"

#include <stdio.h> // For snprintf

#include "raymath.h"

typedef struct Car {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector2 steering;  // Steering vector
    Vector3 direction;
    float speed;
    float maxSpeed;
    float accelerationRate;
    float brakingRate;
    float turnSpeed;
    float downforce;
    float rotation; // Rotation around the Y-axis
    Model model;
} Car;

Vector3 Vector3Addc(Vector3 v1, Vector3 v2)
{
    return (Vector3){ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

float SinDeg(float degrees)
{
    return sinf(degrees * PI / 180.0f);
}

float CosDeg(float degrees)
{
    return cosf(degrees * PI / 180.0f);
}

float Sqrt(float value)
{
    return sqrtf(value);
}

void UpdateCarPhysics(Car *car, float deltaTime);

int main(void)
{
    Color customGray = { 90, 90, 90, 255 };
    Color customGreen = { 0, 255, 0, 255 };

    const int screenWidth = 1920, screenHeight = 1200;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    ToggleFullscreen();

    Camera3D camera = {
        .position = {0.0f, 10.0f, -10.0f}, // Camera behind the car
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 75.0f,
    };
    
    SetTargetFPS(144);

    



    const char msg[256] = "BILLYSHOOLIGANS";
    Font fontOtf = LoadFontEx("resources/goodtimesrg.otf", 50, 0, 250);
    Font fontTtf = LoadFontEx("resources/GeistMono-Regular.ttf", 32, 0, 250);

    Texture2D sprite = LoadTexture("resources/track.png");
    Mesh planeMesh = GenMeshPlane(40.0f, 40.0f, 1, 1);
    Model planeModel = LoadModelFromMesh(planeMesh);
    planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprite;

    Model model = LoadModel("resources/simplifiedcarbody.obj");

    Vector2 position = { -100.0f, -100.0f };

    Car car = {
        .position = {0.0f, 0.0f, 0.0f},
        .velocity = {0.0f, 0.0f, 0.0f},
        .acceleration = {0.0f, 0.0f, 0.0f},
        .steering = {0.0f, 1.0f}, // Initial steering direction facing forward
        .direction = {0.0f, 0.0f, 1.0f}, // Initial direction facing forward
        .speed = 0.0f,
        .maxSpeed = 200.0f,
        .accelerationRate = 50.0f,
        .brakingRate = 50.0f,
        .turnSpeed = 90.0f, // Degrees per second
        .downforce = 0.0f,
        .rotation = 0.0f,
        .model=model
    };

    while (!WindowShouldClose())
    {
        // DisableCursor();
        HideCursor();
        position = GetMousePosition();
        int k=position.x;
        int l=position.y;

        float deltaTime = GetFrameTime();
        
        Vector3 cameraOffset = {0.0f, 2.0f, -10.0f}; // Offset of the camera from the car
        camera.position = Vector3Addc(car.position, cameraOffset);
        camera.target = car.position;
        UpdateCamera(&camera, CAMERA_CUSTOM);
        
        UpdateCarPhysics(&car, deltaTime);

        char fpsText[10];
        snprintf(fpsText, sizeof(fpsText), "%d", GetFPS());

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
        // Draw the car as a red cube
        DrawCube(car.position, 1.0f, 0.5f, 2.0f, RED);
        DrawModel(car.model, car.position, 0.3f, GREEN);
        DrawCubeWires(car.position, 1.0f, 0.5f, 2.0f, DARKGRAY);

        Vector3 planePosition = {0.0f, -1.3f, 0.0f};
        Vector3 planeScale = {10.0f, 1.0f, 10.0f}; // Scale the plane to the desired size
        DrawModelEx(planeModel, planePosition, (Vector3){1.0f, 0.0f, 0.0f}, 0.0f, planeScale, WHITE);

        DrawGrid(20, 1.0f);
        EndMode3D();

        DrawRectangle(200,800,600,230,customGray);
        DrawRectangle((k/1920.0f)*600+200,(l/1080.0f)*230+800,7,7,customGreen);

        // DrawFPS(200, 820);
        // char ch[]=GetFPS();
        DrawTextEx(fontTtf,fpsText, (Vector2){ 200.0f, 830.0f }, (float)fontTtf.baseSize-10, 2, BLACK);

        DrawTextEx(fontTtf,TextFormat("Speed: %.2f", car.speed), (Vector2){ 200.0f, 850.0f }, (float)fontTtf.baseSize-8, 2, BLACK);
        DrawTextEx(fontTtf,TextFormat("Position: [%.2f, %.2f, %.2f]", car.position.x, car.position.y, car.position.z), (Vector2){ 200.0f, 870.0f }, (float)fontTtf.baseSize-8, 2, BLACK);
        DrawTextEx(fontTtf,TextFormat("Acceleration: [%.2f, %.2f, %.2f]", car.acceleration.x, car.acceleration.y, car.acceleration.z), (Vector2){ 200.0f, 890.0f }, (float)fontTtf.baseSize-8, 2, BLACK);
        
        DrawTextEx(fontOtf, msg, (Vector2){ 200.0f, 800.0f }, (float)fontOtf.baseSize-10, 2, customGreen);
        DrawTextEx(fontOtf, "6", (Vector2){ 700.0f, 830.0f }, (float)fontOtf.baseSize, 2, customGreen);

        
        // DrawRectangle(k,800,5,5,RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void UpdateCarPhysics(Car *car, float deltaTime)
{
    // Update acceleration based on input
    if (IsKeyDown(KEY_W)) car->acceleration.z = car->accelerationRate;
    else if (IsKeyDown(KEY_S)) car->acceleration.z = -car->brakingRate;
    else car->acceleration.z = 0;

    // Update steering based on input
    if (IsKeyDown(KEY_A)) car->rotation -= car->turnSpeed * deltaTime;
    if (IsKeyDown(KEY_D)) car->rotation += car->turnSpeed * deltaTime;

    // Clamp steering angle to +/- 15 degrees
    if (car->rotation > 15.0f) car->rotation = 15.0f;
    if (car->rotation < -15.0f) car->rotation = -15.0f;

    // Calculate the new direction vector based on rotation
    car->steering.x = SinDeg(car->rotation);
    car->steering.y = CosDeg(car->rotation);

    // Normalize the steering vector
    float length = Sqrt(car->steering.x * car->steering.x + car->steering.y * car->steering.y);
    car->steering.x /= length;
    car->steering.y /= length;

    // Update velocity and position based on acceleration and steering direction
    car->velocity.x = car->steering.x * car->acceleration.z * deltaTime;
    car->velocity.z = car->steering.y * car->acceleration.z * deltaTime;

    // Ensure the car speed is within max speed limits
    car->speed = Sqrt(car->velocity.x * car->velocity.x + car->velocity.z * car->velocity.z);
    if (car->speed > car->maxSpeed)
    {
        car->velocity.x *= car->maxSpeed / car->speed;
        car->velocity.z *= car->maxSpeed / car->speed;
        car->speed = car->maxSpeed;
    }

    // Update the car's position
    car->position.x += car->velocity.x * deltaTime;
    car->position.z += car->velocity.z * deltaTime;

    // Apply downforce (simplified)
    car->velocity.y -= car->downforce * deltaTime;
}
