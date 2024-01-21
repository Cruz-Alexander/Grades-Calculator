#include <stdio.h>
#include <math.h>
#define MAX_ASSIGNMENTS 50

/* Prototypes */
void compute_assignment_score(int score, int days_late, int penalty_per_day, int weight, double *adjusted_score, int assignment_dropped);
void drop_lowest_assignments(int num_assignments, int num_to_drop, int data[], double adjusted_scores[]);
double compute_mean(int num_assignments, int data[]);
double compute_standard_deviation(int num_assignments, int data[]);
void sort_data_by_assignment_nums(int num_assignments, int data[]);

/* Computes the adjusted score for an assignment based on late penalty and weight. 
   If assignment_dropped is set to 1, the score is set to 0. */
void compute_assignment_score(int score, int days_late, int penalty_per_day, int weight, double *adjusted_score, int assignment_dropped) {
    if (!assignment_dropped) {
        double late_penalty = penalty_per_day * days_late;
        *adjusted_score = (score - late_penalty < 0) ? 0 : score - late_penalty;
        *adjusted_score = (*adjusted_score * weight) / 100.0;
    } else {
        /* If the assignment is dropped, set the adjusted score to 0 */
        *adjusted_score = 0.0;
    }
}

/* Drops the lowest-valued assignments and updates the data array accordingly. 
   Also marks the dropped assignments in the assignment_dropped array. */
void drop_lowest_assignments(int num_assignments, int num_to_drop, int data[], double adjusted_scores[]) {
    int i, j;

    for (i = 0; i < num_to_drop; i++) {
        int min_index = i;
        for (j = i + 1; j < num_assignments; j++) {
            double value1 = adjusted_scores[min_index] * data[min_index * 4 + 2];
            double value2 = adjusted_scores[j] * data[j * 4 + 2];
            if (value2 < value1 /* || (value2 == value1 && data[j * 4] < data[min_index * 4]) */) {
                min_index = j;
            }
        }
        data[min_index * 4 + 2] = 0;

        /* Mark the assignment as dropped */
        adjusted_scores[min_index] = 0.0;
    }
}

/* Computes the mean of assignment scores (without late penalties) in the data array. */
double compute_mean(int num_assignments, int data[]) {
    double sum = 0;
    int i;

    for (i = 0; i < num_assignments; i++) {
        sum += (data[i * 4 + 1] - (data[i * 4 + 3] * 10));
    }
    return sum / num_assignments;
}

/* Computes the standard deviation of assignment scores (without late penalties) in the data array. */
double compute_standard_deviation(int num_assignments, int data[]) {
    double mean = compute_mean(num_assignments, data);
    double sum_squared_diffs = 0;
    int i;

    for (i = 0; i < num_assignments; i++) {
        double diff = (data[i * 4 + 1] - (data[i * 4 + 3] * 10)) - mean;
        sum_squared_diffs += diff * diff;
    }
    return sqrt(sum_squared_diffs / num_assignments);
}

/* Sorts the data array based on assignment numbers (first element in each set of four). */
void sort_data_by_assignment_nums(int num_assignments, int data[]) {
    int i, j, k;
    for (i = 0; i < num_assignments - 1; i++) {
        for (j = 0; j < num_assignments - i - 1; j++) {
            if (data[j * 4] > data[(j + 1) * 4]) {
                /* Swap the assignments */
                double temp;
                for (k = 0; k < 4; k++) {
                    temp = data[j * 4 + k];
                    data[j * 4 + k] = data[(j + 1) * 4 + k];
                    data[(j + 1) * 4 + k] = temp;
                }
            }
        }
    }
}

int main() {
    int penalty_per_day, num_to_drop, num_assignments, weight_sum, i;
    char stats_requested;
    int data[MAX_ASSIGNMENTS * 4] = {0}; /* Combined array for assignment data */
    double adjusted_scores[MAX_ASSIGNMENTS];
    /* Create a second copy */
    int data2[MAX_ASSIGNMENTS * 4] = {0};
    double numeric_score, mean, std_dev;
    int assignment_dropped[MAX_ASSIGNMENTS] = {0};
    double assignment_values[MAX_ASSIGNMENTS];

    /* Read input data */
    scanf("%d %d %c\n", &penalty_per_day, &num_to_drop, &stats_requested);

    scanf("%d\n", &num_assignments);

    /* Read assignment details into the combined array */
    for (i = 0; i < num_assignments; i++) {
        scanf("%d, %d, %d, %d", &data[i * 4], &data[i * 4 + 1], &data[i * 4 + 2], &data[i * 4 + 3]);
        assignment_dropped[i] = 0;
    }

    /* Sort data array based on assignment_nums */
    sort_data_by_assignment_nums(num_assignments, data);

    /* Check weight sum */
    weight_sum = 0;
    for (i = 0; i < num_assignments; i++) {
        weight_sum += data[i * 4 + 2];
    }
    if (weight_sum != 100) {
        printf("ERROR: Invalid values provided\n");
        return 1;
    }

    /* Copy elements from the original array to the new array */
    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        data2[i] = data[i];
    }

    /* Calculate the value of each assignment (score � weight) */
    for (i = 0; i < num_assignments; i++) {
        assignment_values[i] = data[i * 4 + 1] * data[i * 4 + 2];
    }

    /* Determine which assignment to drop based on value and assignment number */
    for (i = 0; i < num_to_drop; i++) {
        int min_index = -1, j;
        double min_value = assignment_values[0]; /* Initialize to a large value */
        for (j = 0; j < num_assignments; j++) {
            /* Check if the assignment is not already dropped */
            if (!assignment_dropped[j]) {
                /* Compare values */
                if (assignment_values[j] < min_value || (assignment_values[j] == min_value && data[j * 4] < data[min_index * 4])) {
                    min_index = j;
                    min_value = assignment_values[j];
                }
            }
        }

        /* Mark the assignment as dropped */
        assignment_dropped[min_index] = 1;
    }

    /* Compute adjusted scores */
    for (i = 0; i < num_assignments; i++) {
        compute_assignment_score(data[i * 4 + 1], data[i * 4 + 3], penalty_per_day, data[i * 4 + 2], &adjusted_scores[i], assignment_dropped[i]);
    }

    /* Drop the lowest-valued assignments */
    drop_lowest_assignments(num_assignments, num_to_drop, data, adjusted_scores);

    /* Compute numeric score */
    numeric_score = 0;
    for (i = 0; i < num_assignments; i++) {
        numeric_score += adjusted_scores[i];
    }

    /* Output results */
    printf("Numeric Score: %5.4f\n", numeric_score);
    printf("Points Penalty Per Day Late: %d\n", penalty_per_day);
    printf("Number of Assignments Dropped: %d\n", num_to_drop);
    printf("Values Provided:\n");
    printf("Assignment, Score, Weight, Days Late\n");
    for (i = 0; i < num_assignments; i++) {
        printf("%d, %d, %d, %d\n", data2[i * 4], data2[i * 4 + 1], data2[i * 4 + 2], data2[i * 4 + 3]);
    }

    if (stats_requested == 'Y' || stats_requested == 'y') {
        mean = compute_mean(num_assignments, data);
        std_dev = compute_standard_deviation(num_assignments, data);
        printf("Mean: %5.4f, Standard Deviation: %5.4f\n", mean, std_dev);
    }

    return 0;
}
