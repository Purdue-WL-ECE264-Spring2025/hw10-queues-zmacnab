#include "queue.h"
#include "tile_game.h"

void enqueue(struct queue *q, struct game_state state) {
    size_t encoded = serialize(state);
    insert_at_tail(&q->data, encoded);
}

struct game_state dequeue(struct queue *q) { 
    size_t encoded = remove_from_head(&q->data);
    return deserialize(encoded);
}

int number_of_moves(struct game_state start) {
    struct queue q = {.data = {.head = NULL}};
    struct linked_list visited = {.head = NULL};
  
    enqueue(&q, start);
    insert_at_tail(&visited, serialize(start));
  
    while (q.data.head != NULL) {
      struct game_state current = dequeue(&q);
  
      // Check if current state is goal
      int expected = 1;
      int correct = 1;
      for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
          if (r == 3 && c == 3) {
            if (current.tiles[r][c] != 0) correct = 0;
          } else {
            if (current.tiles[r][c] != expected++) correct = 0;
          }
        }
      }
      if (correct) {
        free_list(q.data);
        free_list(visited);
        return current.num_steps;
      }
  
      // Try all 4 moves
      for (int dir = 0; dir < 4; dir++) {
        struct game_state next = current;
  
        // Make move
        if (dir == 0 && next.empty_row < 3) {
          next.tiles[next.empty_row][next.empty_col] =
            next.tiles[next.empty_row + 1][next.empty_col];
          next.tiles[next.empty_row + 1][next.empty_col] = 0;
          next.empty_row++;
          next.num_steps++;
        } else if (dir == 1 && next.empty_row > 0) {
          next.tiles[next.empty_row][next.empty_col] =
            next.tiles[next.empty_row - 1][next.empty_col];
          next.tiles[next.empty_row - 1][next.empty_col] = 0;
          next.empty_row--;
          next.num_steps++;
        } else if (dir == 2 && next.empty_col < 3) {
          next.tiles[next.empty_row][next.empty_col] =
            next.tiles[next.empty_row][next.empty_col + 1];
          next.tiles[next.empty_row][next.empty_col + 1] = 0;
          next.empty_col++;
          next.num_steps++;
        } else if (dir == 3 && next.empty_col > 0) {
          next.tiles[next.empty_row][next.empty_col] =
            next.tiles[next.empty_row][next.empty_col - 1];
          next.tiles[next.empty_row][next.empty_col - 1] = 0;
          next.empty_col--;
          next.num_steps++;
        } else {
          continue; // invalid move
        }
  
        // Serialize and check if already visited
        size_t next_serial = serialize(next);
        int seen = 0;
        for (struct list_node *n = visited.head; n != NULL; n = n->next) {
          if (n->value == next_serial) {
            seen = 1;
            break;
          }
        }
  
        if (!seen) {
          insert_at_tail(&visited, next_serial);
          enqueue(&q, next);
        }
      }
    }
  
    free_list(q.data);
    free_list(visited);
    return -1; // No solution found 
}
