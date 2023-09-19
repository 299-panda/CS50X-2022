#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // FOR DEBUG
    for (int i = 0; i < candidate_count; i++)
    {
        printf("argv[%i]: %s\n", i, argv[i + 1]);
    }


    // Query for votes 投票者ごとにループ
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference ranks[i]は投票者の優先順位
        int ranks[candidate_count];

        // Query for each rank 投票者の好選を設定
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        // FOR DEBUG
        printf("voter: %i\n", i);
        for (int j = 0; j < candidate_count; j++)
        {
            printf("ranks[%i]: %i\n", j, ranks[j]);
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    // FOR DEBUG
    printf("pair_count: %i\n", pair_count);
    for (int i = 0; i < pair_count; i++)
    {
        printf("pairs[%i] winner:%i loser:%i preferences: %i\n", i, pairs[i].winner, pairs[i].loser, preferences[pairs[i].winner][pairs[i].loser]);
    }

    sort_pairs();
    // FOR DEBUG
    printf("--- after sort ---\n");
    for (int i = 0; i < pair_count; i++)
    {
        printf("pairs[%i] winner:%i loser:%i preferences: %i\n", i, pairs[i].winner, pairs[i].loser, preferences[pairs[i].winner][pairs[i].loser]);
    }

    lock_pairs();
    // FOR DEBUG
    printf("--- after lock ---\n");
    for (int i = 0; i < pair_count; i++)
    {
        printf("pairs[%i] winner:%i loser:%i preferences:%i locked:%i\n", i, pairs[i].winner, pairs[i].loser, preferences[pairs[i].winner][pairs[i].loser], locked[pairs[i].winner][pairs[i].loser]);
    }

    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // printf("--- vote start ---\n");

    for (int i = 0; i < candidate_count; i++)
    {
        printf("name: %s\n", name);
        printf("candidates[%i]: %s\n", i, candidates[i]);
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[i] = rank;
            // printf("ranks[%i]: %i\n", i, ranks[i]);
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // printf("--- record_preferences start ---\n");

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (ranks[i] < ranks[j])
            {
                preferences[i][j]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] != 0)
            {
                // printf("preferences[%i][%i]: %i\n", i, j, preferences[i][j]); // FOR DEBUG
                pair_count++;
                pairs[pair_count - 1].winner = i;
                pairs[pair_count - 1].loser = j;
            }
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair pairs_tmp[pair_count];

    for (int i = 0; i < pair_count; i++)
    {
        for (int j = i + 1; j < pair_count; j++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[j].winner][pairs[j].loser])
            {
                pairs_tmp[0] = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = pairs_tmp[0];
            }
        }

    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (i == candidate_count - 1)
        {
            int lock_flg = 0;

            for (int j = 0; j < candidate_count; j++)
            {
                if (pairs[i].loser == pairs[j].winner)
                {
                    lock_flg++;
                }
            }

            if (lock_flg == 0)
            {
                locked[pairs[i].winner][pairs[i].loser] = true;
            }
        }
        else
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // ロックされた配列の数を確認
    int locked_count = 0;

    for (int i = 0; i < pair_count; i++)
    {
        if (locked[pairs[i].winner][pairs[i].loser] == true)
        {
            locked_count++;
        }
    }

    printf("locked_count: %i\n", locked_count);

    // locked が Trueの場合
    for (int i = 0; i < locked_count; i++)
    {
        bool locked_loser[locked_count];

        // for (int j = 0; j < locked_count; j++)
        // {
        //     locked_loser[i] = false;
        // }

        for (int j = 0; j < locked_count; j++)
        {
            if (pairs[i].winner == pairs[j].loser)
            {
                locked_loser[i] = true;
                break;
            }
        }

        if (!locked_loser[i])
        {
            printf("winner: %i\n", pairs[i].winner);
            printf("winner: %s\n", candidates[pairs[i].winner]);
            break;
        }

    }



    return;
}

