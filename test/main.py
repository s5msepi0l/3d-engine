import pygame
import math

pygame.init()

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FOV = math.pi / 3  
NUM_RAYS = 120     
MAX_DEPTH = 800    
BLOCK_SIZE = 64    
STEP_ANGLE = FOV / NUM_RAYS
SPEED = 5          
ROTATION_SPEED = 0.1

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (100, 100, 100)
DARK_GRAY = (50, 50, 50)

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Doom-Style Raycaster")

world_map = [
    [1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 1, 0, 0, 0, 1],
    [1, 0, 0, 1, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1],
]

player_x = BLOCK_SIZE + BLOCK_SIZE / 2
player_y = BLOCK_SIZE + BLOCK_SIZE / 2
player_angle = 0

def cast_ray(x, y, angle):
    sin_a = math.sin(angle)
    cos_a = math.cos(angle)

    for depth in range(1, MAX_DEPTH):
        target_x = x + depth * cos_a
        target_y = y + depth * sin_a

        grid_x = int(target_x / BLOCK_SIZE)
        grid_y = int(target_y / BLOCK_SIZE)

        if grid_x < 0 or grid_x >= len(world_map[0]) or grid_y < 0 or grid_y >= len(world_map):
            return MAX_DEPTH

        if world_map[grid_x][grid_y] == 1:
            return depth  # return the depth of the wall hit

    return MAX_DEPTH

running = True
clock = pygame.time.Clock()

while running:
    screen.fill(BLACK)
    
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Handle player movement
    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        player_angle -= ROTATION_SPEED
    if keys[pygame.K_RIGHT]:
        player_angle += ROTATION_SPEED
    if keys[pygame.K_UP]:
        player_x += SPEED * math.cos(player_angle)
        player_y += SPEED * math.sin(player_angle)
    if keys[pygame.K_DOWN]:
        player_x -= SPEED * math.cos(player_angle)
        player_y -= SPEED * math.sin(player_angle)

    for ray in range(NUM_RAYS):
        ray_angle = player_angle - FOV / 2 + ray * STEP_ANGLE
        
        depth = cast_ray(player_x, player_y, ray_angle)

        # Calculate the height of the wall based on the depth
        wall_height = min(800 / (depth + 0.0001), SCREEN_HEIGHT)

        # Choose a color based on the distance
        shade = 255 / (1 + depth * depth * 0.0001)
        color = (shade, shade, shade)

        # Draw the vertical wall slice
        pygame.draw.rect(screen, color, (ray * (SCREEN_WIDTH / NUM_RAYS), (SCREEN_HEIGHT - wall_height) // 2, SCREEN_WIDTH / NUM_RAYS, wall_height))

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
