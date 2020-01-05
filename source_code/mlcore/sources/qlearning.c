#include <stdlib.h>
#include "private/mlcore_private.h"

// create q table, allocate the memory
struct q_table_t *q_table_create(uint32_t num_states, uint32_t num_actions, float gamma)
{
    struct q_table_t *out = (struct q_table_t *)malloc(sizeof(struct q_table_t));

    // fill the structure
    out->gamma = gamma;
    out->num_states = num_states;
    out->num_actions = num_actions;

    // allocate the q table
    out->qtable = (float **)calloc(out->num_states, sizeof(float *));

    uint32_t i;
    for (i = 0; i < out->num_states; i++)
    {
        out->qtable[i] = (float *)calloc(out->num_actions, sizeof(float));
    }

    // return the created table
    return out;
}

// deallocate the memory allocated by the q table
void q_table_free(struct q_table_t **in)
{
    // check that the memory is already freed
    if (in == NULL || in[0] == NULL)
    {
        return;
    }

    // free the actions for each state
    uint32_t i;
    for (i = 0; i < in[0]->num_states; i++)
    {
        free(in[0]->qtable[i]);
    }

    // free the table completely
    free(in[0]->qtable);

    // free the structure
    free(in[0]);

    // set the memory to NULL
    in[0] = NULL;
}

// Q[state, action] = Q[state, action] + alpha * (reward + gamma * max(Q [next_state, :]) — Q[state, action])
return_t q_table_update(struct q_table_t *Q, uint32_t state, uint32_t action, uint32_t next_state, float alpha, float reward)
{
    int cond1 = state < Q->num_states && next_state < Q->num_states && action < Q->num_actions;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "given state/action is outside the boundary");

    uint32_t a = 0;

    // find the maximum q in the next state
    float max_q = Q->qtable[next_state][0];
    for (a = 1; a < Q->num_actions; a++)
    {
        max_q = max(max_q, Q->qtable[next_state][a]);
    }

    // update the table
    Q->qtable[state][action] += alpha * (reward + Q->gamma * max_q - Q->qtable[state][action]);

    return SUCCESS;
}

// get the action proposed by the q learning algorithm
uint32_t q_table_get_action(struct q_table_t *Q, uint32_t state, float exploration)
{
    // pick a random action
    uint32_t max_a = random_int(0, Q->num_actions - 1);

    // if in exploatation mode, find the best move
    if (random_float(0, 1) > exploration)
    {
        // find the maximum q in the next state
        uint32_t a = 0;
        for (a = 0; a < Q->num_actions; a++)
        {
            if (Q->qtable[state][a] > Q->qtable[state][max_a])
            {
                max_a = a;
            }
        }
    }

    // return the action
    return max_a;
}

// create q table, allocate the memory
struct q_table_t *q_table_read(const char *filename)
{
    FILE *fin = fopen(filename, "rb");
    check_file(fin, NULL);

    // first read the constructor parameters
    float gamma;
    uint32_t num_states;
    uint32_t num_actions;

    fread(&num_states, sizeof(uint32_t), 1, fin);
    fread(&num_actions, sizeof(uint32_t), 1, fin);
    fread(&gamma, sizeof(float), 1, fin);

    // construct the q table
    struct q_table_t *out = q_table_create(num_states, num_actions, gamma);

    // read each rows by turn
    uint32_t i;
    for (i = 0; i < out->num_states; i++)
    {
        size_t read = fread(out->qtable[i], sizeof(float), out->num_actions, fin);
    }
    // end of file
    fclose(fin);

    // return the created table
    return out;
}

// write the given qtable with the given name
return_t q_table_write(struct q_table_t *src, const char *filename)
{
    check_null(src, ERROR_NULL_TYPE);

    FILE *fout = fopen(filename, "wb");
    check_file(fout, ERROR_UNABLE_TO_OPEN);

    // first read the constructor parameters
    fwrite(&src->num_states, sizeof(uint32_t), 1, fout);
    fwrite(&src->num_actions, sizeof(uint32_t), 1, fout);
    fwrite(&src->gamma, sizeof(float), 1, fout);

    // write each rows by turn
    uint32_t i;
    for (i = 0; i < src->num_states; i++)
    {
        fwrite(src->qtable[i], sizeof(float), src->num_actions, fout);
    }
    // end of file
    fclose(fout);

    // return the created table
    return SUCCESS;
}